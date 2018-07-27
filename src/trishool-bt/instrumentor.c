/**
 * Copyright(C) 2018, Juniper Networks, Inc.
 * All rights reserved
 *
 * This SOFTWARE is licensed under the license provided in the LICENSE.txt
 * file.  By downloading, installing, copying, or otherwise using the
 * SOFTWARE, you agree to be bound by the terms of that license.  This
 * SOFTWARE is not an official Juniper product.
 *
 * Third-Party Code: This SOFTWARE may depend on other components under
 * separate copyright notice and license terms.  Your use of the source
 * code for those components is subject to the term and conditions of
 * the respective license as noted in the Third-Party source code.
 */

/**
 * @author Anoop Saldanha
 */

#include "trishool-bt-common.h"
#include "trishool/status.h"
#include "trishool/avl.h"
#include "trishool/util-log.h"
#include "trishool/align.h"
#include "instrumentor.h"
#include "instr.h"
#include "db.h"
#include "mmap.h"
#include "image.h"
#include "util-platform.h"
#include "block.h"
#include "scan-engine.h"

typedef struct tri_im_module_t {
    tri_im_callback_t func;
    void *arg;
    struct tri_im_module_t *next;
} tri_im_module_t;

typedef enum tri_im_module_type_t {
    TRI_IM_MODULE_TYPE_INSTRUCTION = 0,
    TRI_IM_MODULE_TYPE_ROUTINE,
    TRI_IM_MODULE_TYPE_MODULE_LOAD,
    TRI_IM_MODULE_TYPE_MODULE_UNLOAD,
    TRI_IM_MODULE_TYPE_LOOP,
    TRI_IM_MODULE_TYPE_YARA_HIT,
    TRI_IM_MODULE_TYPE_MAX,
} tri_im_module_type_t;

static tri_im_module_t *g_tri_im_modules_root[TRI_IM_MODULE_TYPE_MAX];

static tri_avl_t *g_tree_instrs_all = NULL;
static tri_mmap_tree_t *g_tree_mmap = NULL;

static bool g_flag_block_enter_seen = true;
static bool g_flag_block_is_sub = false;
static uint32_t g_block_target_addr;
static char g_block_target_name[2048];

static bool g_flag_block_exit_seen = false;

static int calls = 0;
static int calls_win32 = 0;
static int block_enters = 0;

static tri_instr_t *tri_im_instr_alloc(tri_instr_type_t type, uint32_t addr)
{
    tri_instr_t *instr;

    instr = (tri_instr_t *)malloc(sizeof(*instr));
    if (instr == NULL) {
        tri_log_error("malloc() failure.");
        goto return_status;
    }
    memset(instr, 0, sizeof(*instr));
    instr->type = type;
    instr->addr = addr;

 return_status:
    return instr;
}

static void tri_im_instr_free(void *instr)
{
    free(instr);
}

tri_status_t tri_im_track_memory(uint32_t addr)
{
    return tri_mmap_track_memory(g_tree_mmap, addr);
}

tri_status_t tri_im_track_memory_range(tri_mmap_tree_t *mmap_tree)
{
    return tri_mmap_track_memory_range(g_tree_mmap, mmap_tree);
}

tri_status_t tri_im_scan_engine_run(uint32_t addr)
{
    return TRI_STATUS_OK;
}

tri_status_t tri_im_track_image(char *image_name,
                                tri_image_type_t type,
                                tri_mmap_tree_t *mmap_tree_image,
                                bool is_main_exe,
                                tri_image_event_t event)
{
    if (event == TRI_IMAGE_EVENT_LOAD) {
        tri_log_debug("Image Loaded: %s", image_name);
        if (is_main_exe) {
            tri_im_track_memory_range(mmap_tree_image);
        }
    } else {
        tri_log_debug("Image Unloaded - %s", image_name);
    }

    return TRI_STATUS_OK;
}

static tri_status_t tri_im_add_callback(tri_im_module_type_t im_mod_type,
                                        tri_im_callback_t func,
                                        void *arg)
{
    tri_im_module_t *mod;
    tri_status_t status;

    if ((mod = (tri_im_module_t *)malloc(sizeof(*mod))) == NULL) {
        status = TRI_STATUS_ERROR;
        goto return_status;
    }
    memset(mod, 0, sizeof(*mod));

    mod->func = func;
    mod->arg = arg;

    if (g_tri_im_modules_root[im_mod_type] == NULL) {
        g_tri_im_modules_root[im_mod_type] = mod;
    } else {
        mod->next = g_tri_im_modules_root[im_mod_type];
        g_tri_im_modules_root[im_mod_type] = mod;
    }

    status = TRI_STATUS_OK;
 return_status:
    return status;
}

tri_status_t tri_im_add_instruction_callback(tri_im_callback_t func, void *arg)
{
    return tri_im_add_callback(TRI_IM_MODULE_TYPE_INSTRUCTION, func, arg);
}

tri_status_t tri_im_add_routine_callback(tri_im_callback_t func, void *arg)
{
    return tri_im_add_callback(TRI_IM_MODULE_TYPE_ROUTINE, func, arg);
}

tri_status_t tri_im_add_module_load_callback(tri_im_callback_t func, void *arg)
{
    return tri_im_add_callback(TRI_IM_MODULE_TYPE_MODULE_LOAD, func, arg);
}

tri_status_t tri_im_add_module_unload_callback(tri_im_callback_t func, void *arg)
{
    return tri_im_add_callback(TRI_IM_MODULE_TYPE_MODULE_UNLOAD, func, arg);
}

tri_status_t tri_im_add_loop_callback(tri_im_callback_t func, void *arg)
{
    return tri_im_add_callback(TRI_IM_MODULE_TYPE_LOOP, func, arg);
}

tri_status_t tri_im_add_yara_hit_callback(tri_im_callback_t func, void *arg,
                                          const char *rule_dir)
{
    return tri_im_add_callback(TRI_IM_MODULE_TYPE_YARA_HIT, func, arg);
}

/* BT Platform Callbacks */

static void tri_im_instr_touch(void *instr_, void *arg)
{
    tri_instr_t *instr = (tri_instr_t *)instr_;

    return;
}

static int tri_im_instr_cmp_by_addr(void *instr1_, void *instr2_)
{
    tri_instr_t *instr1 = (tri_instr_t *)instr1_;
    tri_instr_t *instr2 = (tri_instr_t *)instr2_;

    return (instr1->addr - instr2->addr);
}

static tri_status_t tri_im_bt_callback_call(tri_instr_t *instr_bt,
                                            bool in_trace)
{
    tri_instr_t *instr;
    tri_instr_t *instr_existing;
    char rtn_name[2048];
    uint32_t rtn_name_len;
    tri_status_t status;

    if (in_trace) {
        instr = tri_im_instr_alloc(TRI_INSTR_TYPE_CALL, instr_bt->addr);
        if (instr == NULL) {
            tri_log_error("Error allocating memory for an instruction.");
            status = TRI_STATUS_ERROR;
        }

        tri_avl_add_entry(g_tree_instrs_all, instr, (void **)&instr_existing);

        status = TRI_STATUS_OK;
        goto return_status;
    }

    if (!tri_image_address_is_win32(instr_bt->call.target_addr)) {
        tri_im_track_memory(instr_bt->call.target_addr);
    }

    calls++;

#if 0
    if (tri_image_address_is_win32(instr_bt->addr)) {
        status = TRI_STATUS_OK;
        goto return_status;
    }

    if (tri_db_insert_call_graph(tri_block_curr_addr(),
                                 instr_bt->addr,
                                 instr_bt->call.target_addr,
                                 instr_bt->call.target_addr) != TRI_STATUS_OK)
    {
        tri_log_error("Error inserting call graph into db");
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    g_flag_block_enter_seen = true;
    g_block_target_addr = instr_bt->call.target_addr;

    /* Check if call target is a win32 routine and if so get the name
     * of the win32 function */
    /* \todo Can also get the name of a routine in a 3rd party dll, but we
     * will skip that for now */
    if (tri_image_address_is_win32(instr_bt->call.target_addr)) {
        rtn_name_len = sizeof(rtn_name);
        if (tri_platform_rtn_name_from_addr(instr_bt->call.target_addr,
                                            rtn_name,
                                            &rtn_name_len) == TRI_STATUS_OK)
        {
            snprintf(g_block_target_name, sizeof(g_block_target_name),
                     "%s", rtn_name);
        }
    }
#endif

    /* Becase of the wrong data and the crash commenting it out */
    if (tri_db_insert_call_graph(tri_block_curr_addr(),
                                 instr_bt->addr,
                                 instr_bt->call.target_addr,
                                 instr_bt->call.target_addr) != TRI_STATUS_OK)
    {
        tri_log_error("Error inserting call graph into db");
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    g_flag_block_enter_seen = true;
    g_flag_block_is_sub = true;
    g_block_target_addr = instr_bt->call.target_addr;

    /* Check if call target is a win32 routine and if so get the name
     * of the win32 function */
    /* \todo Can also get the name of a routine in a 3rd party dll, but we
     * will skip that for now */
    if (tri_image_address_is_win32(instr_bt->call.target_addr)) {
        rtn_name_len = sizeof(rtn_name);
        if (tri_platform_rtn_name_from_addr(instr_bt->call.target_addr,
                                            rtn_name,
                                            &rtn_name_len) == TRI_STATUS_OK)
        {
#if BT_PLATFORM_PIN
            snprintf(g_block_target_name, sizeof(g_block_target_name),
                     "%s", rtn_name);
#elif BT_PLATFORM_DRIO
            _snprintf_s(g_block_target_name, sizeof(g_block_target_name),
                        _TRUNCATE, "%s", rtn_name);
#endif
        }
        /* This can happen in case of an error in the previous call
         * or if it is not an error, but rtn_name is 0 in lenght */
        if (strlen(rtn_name) == 0) {
            g_block_target_name[0] = '\0';
        }
    }

    status = TRI_STATUS_OK;
 return_status:
    return status;
}

static tri_status_t tri_im_bt_callback_ret(tri_instr_t *instr_bt,
                                            bool in_trace)
{
    tri_instr_t *instr;
    tri_instr_t *instr_existing;
    tri_status_t status;

    if (in_trace) {
        instr = tri_im_instr_alloc(TRI_INSTR_TYPE_RET, instr_bt->addr);
        if (instr == NULL) {
            tri_log_error("Error allocating memory for an instruction.");
            status = TRI_STATUS_ERROR;
        }

        tri_avl_add_entry(g_tree_instrs_all, instr, (void **)&instr_existing);

        status = TRI_STATUS_OK;
        goto return_status;
    }

    if (!tri_image_address_is_win32(instr_bt->ret.target_addr)) {
        tri_im_track_memory(instr_bt->ret.target_addr);
    }

    g_flag_block_exit_seen = true;

    status = TRI_STATUS_OK;
 return_status:
    return status;
}

static tri_status_t tri_im_bt_callback_branch(tri_instr_t *instr_bt,
                                              bool in_trace)
{
    tri_instr_t *instr;
    tri_instr_t *instr_existing;
    tri_status_t status;

    if (in_trace) {
        instr = tri_im_instr_alloc(TRI_INSTR_TYPE_BRANCH, instr_bt->addr);
        if (instr == NULL) {
            tri_log_error("Error allocating memory for an instruction.");
            status = TRI_STATUS_ERROR;
        }

        tri_avl_add_entry(g_tree_instrs_all, instr, (void **)&instr_existing);

        status = TRI_STATUS_OK;
        goto return_status;
    }

    if (!tri_image_address_is_win32(instr_bt->branch.target_addr)) {
        tri_im_track_memory(instr_bt->branch.target_addr);
    }

    status = TRI_STATUS_OK;
 return_status:
    return status;
}

static tri_status_t tri_im_bt_callback_mem_rw(tri_instr_t *instr_bt,
                                              bool in_trace)
{
    tri_instr_t *instr;
    tri_instr_t *instr_existing;
    tri_status_t status;
    uint8_t buf[4096];
    uint32_t copy_len;
    uint32_t page_start_addr;
    uint32_t instr_start_addr;

    if (in_trace) {
        instr = tri_im_instr_alloc(TRI_INSTR_TYPE_MEM_RW, instr_bt->addr);
        if (instr == NULL) {
            tri_log_error("Error allocating memory for an instruction.");
            status = TRI_STATUS_ERROR;
        }

        tri_avl_add_entry(g_tree_instrs_all, instr, (void **)&instr_existing);

        status = TRI_STATUS_OK;
        goto return_status;
    }

    if (instr_bt->mem_rw.src_addr1 != 0) {
        if (!tri_image_address_is_win32(instr_bt->mem_rw.src_addr1)) {
            tri_im_track_memory(instr_bt->mem_rw.src_addr1);
        }
    }
    if (instr_bt->mem_rw.src_addr2 != 0) {
        if (!tri_image_address_is_win32(instr_bt->mem_rw.src_addr2)) {
            tri_im_track_memory(instr_bt->mem_rw.src_addr2);
        }
    }
    if (instr_bt->mem_rw.dst_addr != 0) {
        tri_im_track_memory(instr_bt->mem_rw.dst_addr);

        page_start_addr = instr_bt->mem_rw.dst_addr;
        ALIGN_DOWN(page_start_addr, 4096);
        copy_len = 4096;
        tri_platform_memcpy(buf, page_start_addr, &copy_len);
        tri_log_debug("Platform memcopied %u bytes at addr %x(%x)\n",
                      copy_len, page_start_addr, instr_bt->mem_rw.dst_addr);
        if (tri_scan_engine_run(buf, copy_len) != 0) {
            tri_log_info("SCAN ENGINE match");
#if   0
            tri_log_info("SCAN ENGINE match - "
                         "instr_page_addr(%x) instr_addr(%x) mem_page_addr(%x) "
                         "mem_addr(%x)",
                         page_start_addr,
                         
                         instr_bt->addr, );
#endif
        }
    }

    status = TRI_STATUS_OK;
 return_status:
    return status;
}

static tri_status_t tri_im_bt_callback_general(tri_instr_t *instr_bt,
                                               bool in_trace)
{
    tri_instr_t *instr;
    tri_instr_t *instr_existing;
    tri_status_t status;

    if (in_trace) {
        instr = tri_im_instr_alloc(TRI_INSTR_TYPE_GENERAL, instr_bt->addr);
        if (instr == NULL) {
            tri_log_error("Error allocating memory for an instruction.");
            status = TRI_STATUS_ERROR;
        }

        tri_avl_add_entry(g_tree_instrs_all, instr, (void **)&instr_existing);

        status = TRI_STATUS_OK;
        goto return_status;
    }

    status = TRI_STATUS_OK;
 return_status:
    return status;
}

tri_status_t tri_im_bt_callback(tri_instr_t *instr)
{
    bool in_trace;
    tri_status_t status;

    if (instr->trace == 0 || instr->bb == 0) {
        in_trace = false;
    } else {
        in_trace = true;
    }

    if (!in_trace) {
        BUG_ON(g_flag_block_enter_seen &&  g_flag_block_exit_seen);

        tri_log_debug("INSTR: %x", instr->addr);

        /* Needs  to handle this scenario
         * ENTRY_POINT first instruction
         * INSTR TRACE 77f189d8: mov esp, esi
         * INSTR TRACE 77f189da: pop ebx
         * INSTR TRACE 77f189db: pop edi
         * INSTR TRACE 77f189dc: pop esi
         * INSTR TRACE 77f189dd: pop ebp
         * INSTR TRACE 77f189de: ret 0x10
         * INS: 77f189d8
         * INS: 77f189da
         * INS: 77f189db
         * INS: 77f189dc
         * INS: 77f189dd
         * INS: 77f189de
         * INSTR TRACE 77f25c71: mov byte ptr [ebp-0x19], al
         * INSTR TRACE 77f25c74: mov dword ptr [ebp-0x4], ebx
         * INSTR TRACE 77f25c77: call 0x77f25ca0
         * INS: 77f25c71
         * INS: 77f25c74
         * So we need that we have a RET(block exit at 0x77f189de which
         * we catch and track in 0x77f25c71 and then again we have a
         * first instruction ENTRY_POINT as well which is again
         * track in 0x77f25c71 */
        if (g_flag_block_exit_seen) {
            tri_block_exit();
            g_flag_block_exit_seen = false;
        }

        if (tri_block_curr_addr() == 0x00000000) {
            block_enters++;
            tri_block_enter(instr->addr, "ENTRY_POINT", false);
        } else if (g_flag_block_enter_seen) {
            /* Except for the case where we track the first block */
            if (g_block_target_addr != 0x00000000) {
                BUG_ON(g_block_target_addr != instr->addr);
            }

            block_enters++;
            tri_block_enter(instr->addr, g_block_target_name,
                            g_flag_block_is_sub);

            /* Reset the flag and the block name*/
            g_flag_block_enter_seen = false;
            g_flag_block_is_sub = false;
            g_block_target_addr = 0x00000000;
            g_block_target_name[0] = '\0';
        }
    } else {
        if (!tri_image_address_is_win32(instr->addr)) {
            tri_im_track_memory(instr->addr);
        }
    }

    switch (instr->type) {
        case TRI_INSTR_TYPE_CALL:
            if (tri_im_bt_callback_call(instr, in_trace) != TRI_STATUS_OK) {
                status = TRI_STATUS_ERROR;
                goto return_status;
            }
            break;
        case TRI_INSTR_TYPE_RET:
            if (tri_im_bt_callback_ret(instr, in_trace) != TRI_STATUS_OK) {
                status = TRI_STATUS_ERROR;
                goto return_status;
            }
            break;
        case TRI_INSTR_TYPE_BRANCH:
            if (tri_im_bt_callback_branch(instr, in_trace) != TRI_STATUS_OK) {
                status = TRI_STATUS_ERROR;
                goto return_status;
            }
            break;
        case TRI_INSTR_TYPE_MEM_RW:
            if (tri_im_bt_callback_mem_rw(instr, in_trace) != TRI_STATUS_OK) {
                status = TRI_STATUS_ERROR;
                goto return_status;
            }
            break;
        case TRI_INSTR_TYPE_GENERAL:
            if (tri_im_bt_callback_general(instr, in_trace) != TRI_STATUS_OK) {
                status = TRI_STATUS_ERROR;
                goto return_status;
            }
            break;
        default:
            BUG_ON(1);
    }

    status = TRI_STATUS_OK;
 return_status:
    return status;
}

tri_status_t tri_im_init()
{
    tri_status_t status;

    /* We use this technique of setting it as true so that we can track and
     * store the first instruction addresss as the first block address which
     " is the ENTRY_POINT block. */
    g_flag_block_enter_seen = false;
    g_flag_block_exit_seen = false;
    g_block_target_addr = 0x00000000;

    if (tri_avl_init(&g_tree_instrs_all,
                     tri_im_instr_cmp_by_addr,
                     tri_im_instr_free) != TRI_STATUS_OK)
    {
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    if (tri_mmap_init(&g_tree_mmap) != TRI_STATUS_OK) {
        tri_log_error("Error initializing mmap tree.");
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    if (tri_scan_engine_init() != TRI_STATUS_OK) {
        tri_log_error("Error initializing scan engine.");
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    status = TRI_STATUS_OK;
 return_status:
    return status;
}

tri_status_t tri_im_deinit()
{
    tri_status_t status;

    tri_avl_parse(g_tree_instrs_all, tri_im_instr_touch, NULL);
    tri_log_info("Total %"PRIu32" instructions instrumented.",
                 tri_avl_count(g_tree_instrs_all));
    tri_log_info("Total %"PRIu32" mmaps memory pages tracked.",
                 tri_mmap_count(g_tree_mmap));
    tri_log_info("Total calls encountered - %d %d", calls, calls_win32);
    tri_log_info("Total block enters - %d", block_enters);
    tri_mmap_print(g_tree_mmap);

    tri_mmap_deinit(g_tree_mmap);
    tri_avl_deinit(g_tree_instrs_all);

    //tri_db_print_call_graph();
    //tri_db_print_blocks();

    status = TRI_STATUS_OK;
 return_status:
    return status;
}
