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

#ifndef __INSTRUMENTOR__H__
#define __INSTRUMENTOR__H__

#include "trishool-bt-common.h"
#include "trishool/status.h"

typedef enum tri_im_instr_type_t {
    TRI_IM_INSTR_TYPE_CALL = 0,
    TRI_IM_INSTR_TYPE_BRANCH,
    TRI_IM_INSTR_TYPE_MEM_READ2,
    TRI_IM_INSTR_TYPE_GENERAL,
} tri_im_instr_type_t;

typedef tri_status_t (*tri_im_callback_t)(void *user_data);

tri_status_t tri_im_add_instruction_callback(tri_im_callback_t func,
                                             void *user_data);
tri_status_t tri_im_add_routine_callback(tri_im_callback_t func,
                                         void *user_data);
tri_status_t tri_im_add_module_load_callback(tri_im_callback_t func,
                                             void *user_data);
tri_status_t tri_im_add_module_unload_callback(tri_im_callback_t func,
                                               void *user_data);
tri_status_t tri_im_add_loop_callback(tri_im_callback_t func,
                                      void *user_data);
tri_status_t tri_im_add_yara_hit_callback(tri_im_callback_t func,
                                          void *user_data,
                                          const char *rule_dir);

#endif /* __INSTRUMENTOR__H__ */
