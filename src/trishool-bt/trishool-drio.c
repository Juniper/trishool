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

#include "trishool/status.h"
#include "trishool/util-log.h"
#include "dr_api.h"
#ifdef SHOW_SYMBOLS
# include "drsyms.h"
#endif
#include "instrumentor-private.h"

static void tri_event_exit(void)
{
#ifdef SHOW_SYMBOLS
    if (drsym_exit() != DRSYM_SUCCESS) {
        dr_log(NULL, DR_LOG_ALL, 1, "WARNING: error cleaning up symbol library\n");
    }
#endif
}

#ifdef WINDOWS
# define IF_WINDOWS(x) x
#else
# define IF_WINDOWS(x) /* nothing */
#endif

static void tri_event_instruction(app_pc pc)
{
    void *drcontext = dr_get_current_drcontext();

    printf("%x\n", pc);
    fflush(stdout);

    //dr_flush_region(pc, 1);
}

static dr_emit_flags_t tri_event_trace(void *dr_context, void *tag,
                                       instrlist_t *trace, bool for_trace,
                                       bool translating)
{
    instr_t *instr, *instr_next;
    uint32_t addr_first_instr = 0;

    for (instr = instrlist_first(trace); instr != NULL; instr = instr_next) {
        if (addr_first_instr == 0) {
            addr_first_instr = (uint32_t)instr_get_app_pc(instr);
        }
        instr_next = instr_get_next(instr);
#if 0
        dr_insert_clean_call(dr_context, trace, instr, tri_event_instruction,
                             false, 1,
                             OPND_CREATE_INTPTR(instr_get_app_pc(instr)));
#endif
        //printf("TRACE %x %x - ", addr_first_instr, instr_get_app_pc(instr));
        //fflush(stdout);
        //instr_disassemble(dr_context, instr, STDOUT);
        //dr_print_instr(dr_context, STDOUT, instr, "");
        //printf("\n");
        printf("%x\n", instr_get_app_pc(instr));
        fflush(stdout);
    }

    return DR_EMIT_DEFAULT;
}

static dr_emit_flags_t tri_event_bb(void *dr_context, void *tag,
                                    instrlist_t *bb, bool for_trace,
                                    bool translating)
{
    instr_t *instr, *instr_next;

    for (instr = instrlist_first(bb); instr != NULL; instr = instr_next) {
        instr_next = instr_get_next(instr);
#if 1
        dr_insert_clean_call(dr_context, bb, instr, tri_event_instruction,
                             false, 1,
                             OPND_CREATE_INTPTR(instr_get_app_pc(instr)));
#endif
        printf("BB %x - ", instr_get_app_pc(instr));
        fflush(stdout);
        instr_disassemble(dr_context, instr, STDOUT);
        printf("\n");
        fflush(stdout);
    }

    return DR_EMIT_DEFAULT;
}

DR_EXPORT void dr_client_main(client_id_t id, int argc, const char *argv[])
{
    client_id_t client_id;
    tri_status_t status;

    if (tri_log_init(TRI_LOG_LEVEL_DEBUG,
                     "trishool_log.txt") != TRI_STATUS_OK)
    {
        printf("Error initializing logging library.\n");
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    if (tri_im_init() != TRI_STATUS_OK) {
        tri_log_error("Error initializing IM.");
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    client_id = id;
    dr_set_client_name("Trishool", "http://github.com/issues");
    //dr_register_trace_event(tri_event_trace);
    dr_register_bb_event(tri_event_bb);
    dr_register_exit_event(tri_event_exit);

#ifdef SHOW_SYMBOLS
    if (drsym_init(0) != DRSYM_SUCCESS) {
        tri_log_error("Error initializing symbol translation.");
    }
#endif

 return_status:
    if (status == TRI_STATUS_ERROR) {
        exit(EXIT_FAILURE);
    }
    return;
}
