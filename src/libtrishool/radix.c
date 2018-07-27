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

#include "trishool/trishool-common.h"
#include "trishool/status.h"
#include "trishool/radix.h"

typedef struct tri_radix_t {
    /* node_leg[0] if bit is 0 and node_leg[1] if bit is 1 */
    struct tri_radix_t *node_leg[2];
    void *data;
} tri_radix_t;

tri_radix_t *tri_radix_node_alloc(void *data)
{
    tri_radix_t *node;
    tri_status_t status;

    if ((node = (tri_radix_t *)malloc(sizeof(*node))) == NULL) {
        status = TRI_STATUS_ERROR;
        goto return_status;
    }
    memset(node, 0, sizeof(*node));
    node->data = data;

    status = TRI_STATUS_OK;
 return_status:
    return (status == TRI_STATUS_OK) ? node : NULL;
}

void *tri_radix_get(tri_radix_t **root, uint32_t addr)
{
#define BIT_AT_INDEX_IS_0(v_, idx_) (((v_ >> (31 - idx_)) & 0x00000001))

    tri_radix_t *tmp;
    uint32_t bit;
    uint32_t i;
    void *data;

    /* For 0x00000001, the index starts at 0 from left to right */
    tmp = *root;
    for (i = 0; i < 32; i++) {
        bit = BIT_AT_INDEX_IS_0(addr, i);
        if (tmp->node_leg[bit] == NULL) {
            data = NULL;
            goto return_status;
        }

        tmp = tmp->node_leg[bit];
    }

    data = tmp->data;
 return_status:
    return data;
}

tri_status_t tri_radix_add_entry(tri_radix_t **root,
                                 uint32_t addr,
                                 void *data, void **data_existing)
{
#define BIT_AT_INDEX_IS_0(v_, idx_) (((v_ >> (31 - idx_)) & 0x00000001))

    tri_radix_t *tmp;
    uint32_t bit;
    uint32_t i;
    void *data_tmp;
    tri_status_t status;

    *data_existing = NULL;

    /* For 0x00000001, the index starts at 0 from left to right */
    tmp = *root;
    for (i = 0; i < 32; i++) {
        bit = BIT_AT_INDEX_IS_0(addr, i);
        if (tmp->node_leg[bit] == NULL) {
            if (i == 31) {
                data_tmp = data;
            } else {
                data_tmp = NULL;
            }
            if ((tmp->node_leg[bit] = tri_radix_node_alloc(data_tmp)) == NULL) {
                status = TRI_STATUS_ERROR;
                goto return_status;
            }
        } else {
            /* looks like we already have an entry */
            if (i == 31) {
                *data_existing = tmp->node_leg[bit];
                break;
            }
        }
        tmp = tmp->node_leg[bit];
    }

    status = TRI_STATUS_OK;
 return_status:
    return status;
}

tri_status_t tri_radix_init(tri_radix_t **root)
{
    tri_status_t status;

    if ((*root = tri_radix_node_alloc(NULL)) == NULL) {
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    status = TRI_STATUS_OK;
 return_status:
    return status;
}

void tri_parse(tri_radix_t *node, tri_radix_touch_user_data_func_t touch_func)
{
    if (node == NULL)
        return;

    tri_parse(node->node_leg[0], touch_func);
    tri_parse(node->node_leg[1], touch_func);

    if (node->data != NULL) {
        (*touch_func)(node->data);
    }

    return;
}
