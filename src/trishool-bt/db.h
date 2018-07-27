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

#ifndef __DB__H__
#define __DB__H__

#include "trishool-bt-common.h"
#include "trishool/status.h"

tri_status_t tri_db_init();

#if 0
tri_status_t tri_db_insert_call_graph(uint32_t caller_trace_addr,
                                      uint32_t caller_bb_addr,
                                      uint32_t caller_ins_addr,
                                      uint32_t callee_trace_addr,
                                      uint32_t callee_bb_addr,
                                      uint32_t callee_ins_addr);
#endif

tri_status_t tri_db_insert_call_graph(uint32_t caller_block_addr,
                                      uint32_t caller_ins_addr,
                                      uint32_t callee_block_addr,
                                      uint32_t callee_ins_addr);

tri_status_t tri_db_insert_block(uint32_t block_addr, char *block_name);

tri_status_t tri_db_print_call_graph();
tri_status_t tri_db_print_blocks();

#endif /* __DB__H__ */
