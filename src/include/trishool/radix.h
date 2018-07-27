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

#ifndef __RADIX__H__
#define __RADIX__H__

#include "trishool/trishool-common.h"
#include "trishool/status.h"

typedef struct tri_radix_t tri_radix_t;
typedef void (*tri_radix_touch_user_data_func_t)(void *data);

tri_status_t tri_radix_init(tri_radix_t **root);

tri_status_t tri_radix_add_entry(tri_radix_t **root,
                                 uint32_t addr,
                                 void *data, void **data_existing);

void *tri_radix_get(tri_radix_t **root, uint32_t addr);

void tri_parse(tri_radix_t *node, tri_radix_touch_user_data_func_t touch_func);

#endif /* __RADIX__H__ */
