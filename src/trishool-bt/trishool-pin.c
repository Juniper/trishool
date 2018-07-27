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

#include "pin.H"
#include "trishool-bt-common.h"
#include "instrumentor-private.h"
#include "instr.h"
#include "db.h"
#include "util-string.h"
#include "trishool/util-log.h"

#include <iostream>

namespace WD {
    #include "Windows.h"
}

static void tri_pin_callback_call_instr(ADDRINT pc, ADDRINT target_addr)
{
    tri_instr_t instr;
    string str;

    memset(&instr, 0, sizeof(instr));

    instr.addr = pc;
    instr.type = TRI_INSTR_TYPE_CALL;
    instr.trace = 0;
    instr.bb = 0;
    instr.next = NULL;

    instr.call.target_addr = target_addr;

    tri_im_bt_callback(&instr);

    return;
}

static void tri_pin_callback_ret_instr(ADDRINT pc, ADDRINT target_addr)
{
    tri_instr_t instr;
    string str;

    memset(&instr, 0, sizeof(instr));

    instr.addr = pc;
    instr.type = TRI_INSTR_TYPE_RET;
    instr.trace = 0;
    instr.bb = 0;
    instr.next = NULL;

    instr.ret.target_addr = target_addr;

    tri_im_bt_callback(&instr);

    return;
}

static void tri_pin_callback_branch_instr(ADDRINT pc, ADDRINT target_addr,
                                          BOOL branch_taken)
{
    tri_instr_t instr;

    memset(&instr, 0, sizeof(instr));

    instr.addr = pc;
    instr.type = TRI_INSTR_TYPE_BRANCH;
    instr.trace = 0;
    instr.bb = 0;
    instr.next = NULL;

    instr.branch.target_addr = target_addr;

    tri_im_bt_callback(&instr);

    return;
}

static void tri_pin_callback_mem_rw_instr(ADDRINT pc,
                                          ADDRINT src_addr1, ADDRINT src_addr2,
                                          ADDRINT dst_addr)
{
    tri_instr_t instr;

    memset(&instr, 0, sizeof(instr));

    instr.addr = pc;
    instr.type = TRI_INSTR_TYPE_MEM_RW;
    instr.trace = 0;
    instr.bb = 0;
    instr.next = NULL;

    if (src_addr1 != 0) {
        instr.mem_rw.src_addr1 = src_addr1;
        //tri_im_track_memory(src_addr1);
    }
    if (src_addr2 != 0) {
        instr.mem_rw.src_addr2 = src_addr2;
        //tri_im_track_memory(src_addr2);
    }
    if (dst_addr != 0) {
        instr.mem_rw.dst_addr = dst_addr;
        //tri_im_track_memory(dst_addr);
    }

    tri_im_bt_callback(&instr);

    return;
}

static void tri_pin_callback_general_instr(ADDRINT pc)
{
    tri_instr_t instr;

    memset(&instr, 0, sizeof(instr));

    instr.addr = pc;
    instr.type = TRI_INSTR_TYPE_MEM_RW;
    instr.trace = 0;
    instr.bb = 0;
    instr.next = NULL;

    tri_im_bt_callback(&instr);

    return;
}

static void tri_pin_trace_instr_process_call(INS ins,
                                             uint32_t trace_addr,
                                             uint32_t bb_addr,
                                             uint32_t ins_addr,
                                             uint8_t *opcode,
                                             uint8_t opcode_len)
{
    tri_instr_t instr;

    memset(&instr, 0, sizeof(instr));

    instr.addr = ins_addr;
    instr.type = TRI_INSTR_TYPE_CALL;
    instr.trace = trace_addr;
    instr.bb = bb_addr;
    instr.next = NULL;
    memcpy(instr.opcode, opcode, opcode_len);

    INS_InsertCall(ins,
                   IPOINT_BEFORE,
                   (AFUNPTR)tri_pin_callback_call_instr,
                   IARG_INST_PTR,
                   IARG_BRANCH_TARGET_ADDR,
                   IARG_END);

    tri_im_bt_callback(&instr);
}

static void tri_pin_trace_instr_process_ret(INS ins,
                                            uint32_t trace_addr,
                                            uint32_t bb_addr,
                                            uint32_t ins_addr,
                                            uint8_t *opcode,
                                            uint8_t opcode_len)
{
    tri_instr_t instr;

    memset(&instr, 0, sizeof(instr));

    instr.addr = ins_addr;
    instr.type = TRI_INSTR_TYPE_RET;
    instr.trace = trace_addr;
    instr.bb = bb_addr;
    instr.next = NULL;
    memcpy(instr.opcode, opcode, opcode_len);

    INS_InsertCall(ins,
                   IPOINT_BEFORE,
                   (AFUNPTR)tri_pin_callback_ret_instr,
                   IARG_INST_PTR,
                   IARG_BRANCH_TARGET_ADDR,
                   IARG_END);

    tri_im_bt_callback(&instr);
}

static void tri_pin_trace_instr_process_branch(INS ins,
                                               uint32_t trace_addr,
                                               uint32_t bb_addr,
                                               uint32_t ins_addr,
                                               uint8_t *opcode,
                                               uint8_t opcode_len)
{
    tri_instr_t instr;

    memset(&instr, 0, sizeof(instr));

    instr.addr = ins_addr;
    instr.type = TRI_INSTR_TYPE_BRANCH;
    instr.trace = trace_addr;
    instr.bb = bb_addr;
    instr.next = NULL;
    memcpy(instr.opcode, opcode, opcode_len);

    INS_InsertCall(ins,
                   IPOINT_BEFORE,
                   (AFUNPTR)tri_pin_callback_branch_instr,
                   IARG_INST_PTR,
                   IARG_BRANCH_TARGET_ADDR,
                   IARG_BRANCH_TAKEN,
                   IARG_END);

    tri_im_bt_callback(&instr);
}

static void tri_pin_trace_instr_process_write_no_read(INS ins,
                                                      uint32_t trace_addr,
                                                      uint32_t bb_addr,
                                                      uint32_t ins_addr,
                                                      uint8_t *opcode,
                                                      uint8_t opcode_len)
{
    tri_instr_t instr;

    memset(&instr, 0, sizeof(instr));

    instr.addr = ins_addr;
    instr.type = TRI_INSTR_TYPE_MEM_RW;
    instr.trace = trace_addr;
    instr.bb = bb_addr;
    instr.next = NULL;
    memcpy(instr.opcode, opcode, opcode_len);

    INS_InsertCall(ins,
                   IPOINT_BEFORE,
                   (AFUNPTR)tri_pin_callback_mem_rw_instr,
                   IARG_INST_PTR,
                   IARG_UINT32, 0,
                   IARG_UINT32, 0,
                   IARG_MEMORYWRITE_EA,
                   IARG_END);

    tri_im_bt_callback(&instr);
}

static void tri_pin_trace_instr_process_read_no_write(INS ins,
                                                      uint32_t trace_addr,
                                                      uint32_t bb_addr,
                                                      uint32_t ins_addr,
                                                      uint8_t *opcode,
                                                      uint8_t opcode_len)
{
    tri_instr_t instr;

    memset(&instr, 0, sizeof(instr));

    instr.addr = ins_addr;
    instr.type = TRI_INSTR_TYPE_MEM_RW;
    instr.trace = trace_addr;
    instr.bb = bb_addr;
    instr.next = NULL;
    memcpy(instr.opcode, opcode, opcode_len);

    INS_InsertCall(ins,
                   IPOINT_BEFORE,
                   (AFUNPTR)tri_pin_callback_mem_rw_instr,
                   IARG_INST_PTR,
                   IARG_MEMORYREAD_EA,
                   IARG_UINT32, 0,
                   IARG_UINT32, 0,
                   IARG_END);

    tri_im_bt_callback(&instr);
}

static void tri_pin_trace_instr_process_read2_no_write(INS ins,
                                                       uint32_t trace_addr,
                                                       uint32_t bb_addr,
                                                       uint32_t ins_addr,
                                                       uint8_t *opcode,
                                                       uint8_t opcode_len)
{
    tri_instr_t instr;

    memset(&instr, 0, sizeof(instr));

    instr.addr = ins_addr;
    instr.type = TRI_INSTR_TYPE_MEM_RW;
    instr.trace = trace_addr;
    instr.bb = bb_addr;
    instr.next = NULL;
    memcpy(instr.opcode, opcode, opcode_len);

    INS_InsertCall(ins,
                   IPOINT_BEFORE,
                   (AFUNPTR)tri_pin_callback_mem_rw_instr,
                   IARG_INST_PTR,
                   IARG_MEMORYREAD_EA,
                   IARG_MEMORYREAD2_EA,
                   IARG_UINT32, 0,
                   IARG_END);

    tri_im_bt_callback(&instr);
}

static void tri_pin_trace_instr_process_read_write(INS ins,
                                                   uint32_t trace_addr,
                                                   uint32_t bb_addr,
                                                   uint32_t ins_addr,
                                                   uint8_t *opcode,
                                                   uint8_t opcode_len)
{
    tri_instr_t instr;

    memset(&instr, 0, sizeof(instr));

    instr.addr = ins_addr;
    instr.type = TRI_INSTR_TYPE_MEM_RW;
    instr.trace = trace_addr;
    instr.bb = bb_addr;
    instr.next = NULL;
    memcpy(instr.opcode, opcode, opcode_len);

    INS_InsertCall(ins,
                   IPOINT_BEFORE,
                   (AFUNPTR)tri_pin_callback_mem_rw_instr,
                   IARG_INST_PTR,
                   IARG_MEMORYREAD_EA,
                   IARG_UINT32, 0,
                   IARG_MEMORYWRITE_EA,
                   IARG_END);

    tri_im_bt_callback(&instr);
}

static void tri_pin_trace_instr_process_read2_write(INS ins,
                                                    uint32_t trace_addr,
                                                    uint32_t bb_addr,
                                                    uint32_t ins_addr,
                                                    uint8_t *opcode,
                                                    uint8_t opcode_len)
{
    tri_instr_t instr;

    memset(&instr, 0, sizeof(instr));

    instr.addr = ins_addr;
    instr.type = TRI_INSTR_TYPE_MEM_RW;
    instr.trace = trace_addr;
    instr.bb = bb_addr;
    instr.next = NULL;
    memcpy(instr.opcode, opcode, opcode_len);

    INS_InsertCall(ins,
                   IPOINT_BEFORE,
                   (AFUNPTR)tri_pin_callback_mem_rw_instr,
                   IARG_INST_PTR,
                   IARG_MEMORYREAD_EA,
                   IARG_MEMORYREAD2_EA,
                   IARG_MEMORYWRITE_EA,
                   IARG_END);

    tri_im_bt_callback(&instr);
}

static void tri_pin_trace_instr_process_general(INS ins,
                                                uint32_t trace_addr,
                                                uint32_t bb_addr,
                                                uint32_t ins_addr,
                                                uint8_t *opcode,
                                                uint8_t opcode_len)
{
    tri_instr_t instr;

    memset(&instr, 0, sizeof(instr));

    instr.addr = ins_addr;
    instr.type = TRI_INSTR_TYPE_GENERAL;
    instr.trace = trace_addr;
    instr.bb = bb_addr;
    instr.next = NULL;
    memcpy(instr.opcode, opcode, opcode_len);

    INS_InsertCall(ins,
                   IPOINT_BEFORE,
                   (AFUNPTR)tri_pin_callback_general_instr,
                   IARG_INST_PTR,
                   IARG_END);

    tri_im_bt_callback(&instr);
}

static void tri_pin_trace_instr_process(INS ins,
                                        uint32_t trace_addr,
                                        uint32_t bb_addr,
                                        uint32_t ins_addr)
{
    uint8_t opcode[TRI_INSTR_OPCODE_MAX_LENGTH];
    uint32_t opcode_len;

    string str;
    uint32_t str_len;
    char str_array[2048];

    str = INS_Disassemble(ins);
    str_len = str.length();
    if (tri_string_to_char_array(str, str_array) != TRI_STATUS_OK) {
        tri_log_error("Error converting image name string to char array.");
        BUG_ON(1);
    }
    tri_log_debug("INSTR TRACE %x: %s", ins_addr, str_array);
    fflush(stdout);

    opcode_len = INS_Size(ins);
    BUG_ON(opcode_len > TRI_INSTR_OPCODE_MAX_LENGTH);

    PIN_SafeCopy(opcode, (void *)ins_addr, opcode_len);

    if (INS_IsCall(ins)) {
        tri_pin_trace_instr_process_call(ins,
                                         trace_addr,
                                         bb_addr,
                                         ins_addr,
                                         opcode, opcode_len);
    } else if (INS_IsRet(ins)) {
        tri_pin_trace_instr_process_ret(ins,
                                        trace_addr,
                                        bb_addr,
                                        ins_addr,
                                        opcode, opcode_len);
    } else if (INS_IsBranch(ins)) {
        tri_pin_trace_instr_process_branch(ins,
                                           trace_addr,
                                           bb_addr,
                                           ins_addr,
                                           opcode, opcode_len);
    } else if (INS_IsMemoryWrite(ins) && !INS_IsMemoryRead(ins)) {
        tri_pin_trace_instr_process_write_no_read(ins,
                                                  trace_addr,
                                                  bb_addr,
                                                  ins_addr,
                                                  opcode, opcode_len);
    } else if (!INS_IsMemoryWrite(ins) && INS_IsMemoryRead(ins)) {
        if (!INS_HasMemoryRead2(ins)) {
            tri_pin_trace_instr_process_read_no_write(ins,
                                                      trace_addr,
                                                      bb_addr,
                                                      ins_addr,
                                                      opcode, opcode_len);
        } else {
            tri_pin_trace_instr_process_read2_no_write(ins,
                                                       trace_addr,
                                                       bb_addr,
                                                       ins_addr,
                                                       opcode, opcode_len);
        }
    } else if (INS_IsMemoryWrite(ins) && INS_IsMemoryRead(ins)) {
        if (!INS_HasMemoryRead2(ins)) {
            tri_pin_trace_instr_process_read_write(ins,
                                                   trace_addr,
                                                   bb_addr,
                                                   ins_addr,
                                                   opcode, opcode_len);
        } else {
            tri_pin_trace_instr_process_read2_write(ins,
                                                    trace_addr,
                                                    bb_addr,
                                                    ins_addr,
                                                    opcode, opcode_len);
        }
    } else {
        tri_pin_trace_instr_process_general(ins,
                                            trace_addr,
                                            bb_addr,
                                            ins_addr,
                                            opcode, opcode_len);
    }

    return;
}

static void tri_pin_callback_instruction(INS ins, VOID *v)
{
    if (INS_IsCall(ins)) {
        INS_InsertCall(ins,
                       IPOINT_BEFORE,
                       (AFUNPTR)tri_pin_callback_call_instr,
                       IARG_INST_PTR,
                       IARG_BRANCH_TARGET_ADDR,
                       IARG_END);
    } else if (INS_IsBranch(ins)) {
        INS_InsertCall(ins,
                       IPOINT_BEFORE,
                       (AFUNPTR)tri_pin_callback_branch_instr,
                       IARG_INST_PTR,
                       IARG_BRANCH_TARGET_ADDR,
                       IARG_BRANCH_TAKEN,
                       IARG_END);
    } else if (INS_IsMemoryRead(ins) || INS_IsMemoryWrite(ins)) {
        if (!INS_IsMemoryWrite(ins)) {
            if (INS_HasMemoryRead2(ins)) {
                INS_InsertCall(ins,
                               IPOINT_BEFORE,
                               (AFUNPTR)tri_pin_callback_mem_rw_instr,
                               IARG_INST_PTR,
                               IARG_MEMORYREAD_EA,
                               IARG_MEMORYREAD2_EA,
                               IARG_UINT32, 0,
                               IARG_END);
            } else {
                INS_InsertCall(ins,
                               IPOINT_BEFORE,
                               (AFUNPTR)tri_pin_callback_mem_rw_instr,
                               IARG_INST_PTR,
                               IARG_MEMORYREAD_EA,
                               IARG_UINT32, 0,
                               IARG_UINT32, 0,
                               IARG_END);
            }
        } else {
            if (INS_HasMemoryRead2(ins)) {
                INS_InsertCall(ins,
                               IPOINT_BEFORE,
                               (AFUNPTR)tri_pin_callback_mem_rw_instr,
                               IARG_INST_PTR,
                               IARG_MEMORYREAD_EA,
                               IARG_MEMORYREAD2_EA,
                               IARG_MEMORYWRITE_EA,
                               IARG_END);
            } else if (INS_IsMemoryRead(ins)) {
                INS_InsertCall(ins,
                               IPOINT_BEFORE,
                               (AFUNPTR)tri_pin_callback_mem_rw_instr,
                               IARG_INST_PTR,
                               IARG_MEMORYREAD_EA,
                               IARG_UINT32, 0,
                               IARG_MEMORYWRITE_EA,
                               IARG_END);
            } else {
                INS_InsertCall(ins,
                               IPOINT_BEFORE,
                               (AFUNPTR)tri_pin_callback_mem_rw_instr,
                               IARG_INST_PTR,
                               IARG_UINT32, 0,
                               IARG_UINT32, 0,
                               IARG_MEMORYWRITE_EA,
                               IARG_END);
            }
        }
    } else {
        INS_InsertCall(ins,
                       IPOINT_BEFORE,
                       (AFUNPTR)tri_pin_callback_general_instr,
                       IARG_INST_PTR,
                       IARG_END);
    }

    return;
}

static void tri_pin_callback_fini(INT32 code, VOID *v)
{
    tri_im_deinit();
    tri_log_info("Trishool instrumentation done.");

    return;
}

static void tri_pin_callback_trace(TRACE trace, VOID *v)
{
    BBL bb;
    INS ins;
    ADDRINT trace_addr;
    ADDRINT bb_addr;
    ADDRINT ins_addr;

    trace_addr = TRACE_Address(trace);
    //tri_im_track_memory(trace_addr);
    //tri_im_track_memory(trace_addr + TRACE_Size(trace));

    for (bb = TRACE_BblHead(trace); BBL_Valid(bb); bb = BBL_Next(bb)) {
        bb_addr = BBL_Address(bb);
        for (ins = BBL_InsHead(bb); INS_Valid(ins); ins = INS_Next(ins)) {
            ins_addr = INS_Address(ins);
            tri_pin_trace_instr_process(ins, trace_addr, bb_addr, ins_addr);
        }
    }

    return;
}

static void tri_pin_callback_image(IMG img, void *v)
{
    string img_name_str;
    char *img_name;
    uint32_t img_len;
    tri_image_type_t img_type;

    SEC sec;
    RTN rtn;
    string sec_name_str;
    char sec_name[1024];
    string rtn_name_str;
    char rtn_name[1024];

    tri_mmap_tree_t *mmap_tree = NULL;
    uint32_t num_regions;
    uint32_t i;
    uint32_t region_low_addr;
    uint32_t region_high_addr;
    uint32_t addr;

    tri_mmap_init(&mmap_tree);

    img_name_str = IMG_Name(img);
    /* The + 1 to account for the terminating NULL char */
    img_len = img_name_str.length() + 1;

    if ((img_name = (char *)malloc(img_len)) == NULL) {
        tri_log_error("malloc() error allocating memory for image name.");
        goto return_status;
    }
    if (tri_string_to_char_array(img_name_str, img_name) != TRI_STATUS_OK) {
        tri_log_error("Error converting image name string to char array.");
        goto return_status;
    }

    switch (IMG_Type(img)) {
        case IMG_TYPE_STATIC:
            img_type = TRI_IMAGE_TYPE_STATIC;
            break;
        case IMG_TYPE_SHARED:
            img_type = TRI_IMAGE_TYPE_SHARED;
            break;
        case IMG_TYPE_SHAREDLIB:
            img_type = TRI_IMAGE_TYPE_SHAREDLIB;
            break;
        case IMG_TYPE_RELOCATABLE:
            img_type = TRI_IMAGE_TYPE_RELOCATABLE;
            break;
        case IMG_TYPE_DYNAMIC_CODE:
            img_type = TRI_IMAGE_TYPE_DYNAMIC_CODE;
            break;
    }

#if 0
    for (sec = IMG_SecHead(img) ; SEC_Valid(sec); sec = SEC_Next(sec)) {
        sec_name_str = SEC_Name(sec);
        if (tri_string_to_char_array(sec_name_str, sec_name) != TRI_STATUS_OK) {
            tri_log_error("Error converting sec name string to char array.");
            break;
        }
        printf("SEC: %s\n", sec_name);
        for (rtn = SEC_RtnHead(sec); RTN_Valid(rtn); rtn = RTN_Next(rtn)) {
            rtn_name_str = RTN_Name(rtn);
            if (tri_string_to_char_array(rtn_name_str, rtn_name) != TRI_STATUS_OK) {
                tri_log_error("Error converting rtn name string to char array.");
                break;
            }
            printf("RTN: %s %x\n", rtn_name, RTN_Address(rtn));
        }
        fflush(stdout);
    }
#endif

    num_regions = IMG_NumRegions(img);
    for (i = 0; i < num_regions; i++) {
        region_low_addr = IMG_RegionLowAddress(img, i);
        region_high_addr = IMG_RegionHighAddress(img, i);

        for (addr = region_low_addr; addr < region_high_addr; addr++) {
            tri_mmap_track_memory(mmap_tree, addr);
        }
    }

    tri_im_track_image(img_name, img_type,
                       mmap_tree, IMG_IsMainExecutable(img),
                       TRI_IMAGE_EVENT_LOAD);

 return_status:
    tri_mmap_deinit(mmap_tree);
    return;
}

static void tri_pin_callback_image_unload(IMG img, void *v)
{
    string img_name_str;
    char *img_name;
    uint32_t img_len;
    tri_image_type_t img_type;

    img_name_str = IMG_Name(img);
    /* The + 1 to account for the terminating NULL char */
    img_len = img_name_str.length() + 1;

    if ((img_name = (char *)malloc(img_len)) == NULL) {
        tri_log_error("malloc() error allocating memory for image name.");
        goto return_status;
    }
    if (tri_string_to_char_array(img_name_str, img_name) != TRI_STATUS_OK) {
        tri_log_error("Error converting image name string to char array.");
        goto return_status;
    }

    switch (IMG_Type(img)) {
        case IMG_TYPE_STATIC:
            img_type = TRI_IMAGE_TYPE_STATIC;
            break;
        case IMG_TYPE_SHARED:
            img_type = TRI_IMAGE_TYPE_SHARED;
            break;
        case IMG_TYPE_SHAREDLIB:
            img_type = TRI_IMAGE_TYPE_SHAREDLIB;
            break;
        case IMG_TYPE_RELOCATABLE:
            img_type = TRI_IMAGE_TYPE_RELOCATABLE;
            break;
        case IMG_TYPE_DYNAMIC_CODE:
            img_type = TRI_IMAGE_TYPE_DYNAMIC_CODE;
            break;
    }

    tri_im_track_image(img_name, img_type,
                       NULL, IMG_IsMainExecutable(img),
                       TRI_IMAGE_EVENT_UNLOAD);

 return_status:
    return;
}

static void tri_print_usage()
{
    return;
}

static tri_status_t tri_init(int argc, char **argv)
{
    tri_status_t status;

    if (tri_im_init() != TRI_STATUS_OK) {
        tri_log_error("Error initializing IM.");
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    PIN_InitSymbols();

    if (PIN_Init(argc, argv) == TRUE) {
        tri_log_error("Command line parsing failure.");
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    if (tri_db_init() != TRI_STATUS_OK) {
        tri_log_error("DB init failure.");
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    status = TRI_STATUS_OK;
 return_status:
    return status;
}

void tri_smc_detected(ADDRINT start, ADDRINT end)
{
    tri_log_debug("SMC Detected.  Start address(%x) End address(%x).",
                  start, end);
    return;
}

static tri_status_t tri_register_pin_callbacks()
{
    PIN_SetSmcSupport(SMC_ENABLE);

    TRACE_AddInstrumentFunction(tri_pin_callback_trace, 0);
    /* We have moved away from a per instruction instrumentation to
     * trace level, so that we can better scope and build our
     * call graph at a trace/function level */
#if 0
    INS_AddInstrumentFunction(tri_pin_callback_instruction, 0);
#endif
    PIN_AddFiniFunction(tri_pin_callback_fini, 0);

    /* Note: We don't need this.  We added this because we couldn't get
     * pin working correctly for the smc case we wrote in test_52_smc.c.
     * So the only workaround we had was to set the trace size manually
     * to 1, and since th latest PIN deprecated the below APIs, we had
     * to use an older version of PIN, some 3.2.x which still supported
     * these APIs.
     * But then we figured out how to get smc working with the latest
     * version of PIN, and all was good.  We got it working by using
     * the -smc_strict flag.
     */
#if 0
    CODECACHE_BlockSize();
    CODECACHE_ChangeMaxInsPerTrace(1);
#endif

    /* Note: We don't need this.  We added this assuming it will help us
     * with smc.  But it didn't help.  All it gave was a notification, and
     * with the smc sample that we had with test_52_smc.c, it never gave
     * an alert for that particular smc code.  All in all, doesn't work
     * as expected, or maybe we didn't understand it enough.
     * Until we sent the -smc_strict flag */
#if 0
    TRACE_AddSmcDetectedFunction((SMC_CALLBACK)smc_detected, NULL);
#endif

    IMG_AddInstrumentFunction(tri_pin_callback_image, NULL);
    IMG_AddUnloadFunction(tri_pin_callback_image_unload, NULL);

    return TRI_STATUS_OK;
}

static tri_status_t tri_run()
{
    PIN_StartProgram();

    return TRI_STATUS_OK;
}

int main(int argc, char * argv[])
{
    tri_status_t status;

    if (tri_log_init(TRI_LOG_LEVEL_DEBUG,
                     "trishool_log.txt") != TRI_STATUS_OK)
    {
        tri_log_error("Error initializing logging library.");
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    if (tri_init(argc, argv) != TRI_STATUS_OK) {
        tri_log_error("Trishool initialization failure.");
        status = TRI_STATUS_ERROR;
        goto return_status;
    }
    tri_log_info("Trishool initialization success.");

    if (tri_register_pin_callbacks() != TRI_STATUS_OK) {
        tri_log_error("Trishool PIN callback registration failure.");
        status = TRI_STATUS_ERROR;
        goto return_status;
    }
    tri_log_info("Trishool PIN callback registration success.");

    if (tri_run() != TRI_STATUS_OK) {
        tri_log_error("Trishool instrumentation start failure.");
        status = TRI_STATUS_ERROR;
        goto return_status;
    }
    tri_log_info("Trishool instrumentation success.");

    status = TRI_STATUS_OK;
 return_status:
    return (status == TRI_STATUS_OK) ? 0 : -1;
}
