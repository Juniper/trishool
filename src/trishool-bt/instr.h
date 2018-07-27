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

#ifndef __INSTR__H__
#define __INSTR__H__

#include "trishool-bt-common.h"
#include "trishool/status.h"

#define TRI_INSTR_OPCODE_MAX_LENGTH 15

typedef enum tri_instr_type_t {
    TRI_INSTR_TYPE_CALL = 0,
    TRI_INSTR_TYPE_RET,
    TRI_INSTR_TYPE_BRANCH,
    TRI_INSTR_TYPE_MEM_RW,
    TRI_INSTR_TYPE_GENERAL,
    TRI_INSTR_TYPE_MAX,
} tri_instr_type_t;

typedef struct tri_instr_call_t {
    uint32_t target_addr;
} tri_instr_call_t;

typedef struct tri_instr_re_t {
    uint32_t target_addr;
} tri_instr_ret_t;

typedef struct tri_instr_branch_t {
    uint32_t target_addr;
} tri_instr_branch_t;

typedef struct tri_instr_mem_rw_t {
    uint32_t src_addr1;
    uint32_t src_addr2;
    uint32_t dst_addr;
} tri_instr_mem_rw_t;

typedef struct tri_instr_t {
    uint32_t addr;
    tri_instr_type_t type;
    uint32_t trace;
    uint32_t bb;
    struct tri_instr_t *next;
    uint8_t opcode[TRI_INSTR_OPCODE_MAX_LENGTH];
    union {
        tri_instr_call_t call;
        tri_instr_call_t ret;
        tri_instr_branch_t branch;
        tri_instr_mem_rw_t mem_rw;
    };
} tri_instr_t;

#endif /* __INSTR__H__ */
