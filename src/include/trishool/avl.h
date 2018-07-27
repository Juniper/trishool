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

#ifndef __AVL__H__
#define __AVL__H__

#include "trishool/trishool-common.h"
#include "trishool/status.h"

typedef struct tri_avl_t tri_avl_t;
typedef void (*tri_avl_touch_user_data_func_t)(void *data, void *arg);
typedef void (*tri_avl_free_user_data_func_t)(void *data);
typedef int (*tri_avl_compare_func_t)(void *a, void *b);

tri_status_t tri_avl_init(tri_avl_t **tree,
                          tri_avl_compare_func_t compare_func,
                          tri_avl_free_user_data_func_t free_func);

tri_status_t tri_avl_deinit(tri_avl_t *tree);

tri_status_t tri_avl_add_entry(tri_avl_t *tree,
                               void *data, void **data_existing);

void *tri_avl_get(tri_avl_t *tree, void *data);

void tri_avl_parse(tri_avl_t *tree,
                   tri_avl_touch_user_data_func_t touch_func,
                   void *arg);

uint32_t tri_avl_count(tri_avl_t *tree);

#endif /* __AVL__H__ */
