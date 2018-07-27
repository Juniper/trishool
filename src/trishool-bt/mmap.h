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

#ifndef __MMAP__H__
#define __MMAP__H__

#include "trishool-bt-common.h"
#include "trishool/status.h"

typedef struct tri_mmap_tree_t tri_mmap_tree_t;

tri_status_t tri_mmap_init(tri_mmap_tree_t **mmap_tree);
tri_status_t tri_mmap_deinit(tri_mmap_tree_t *mmap_tree);
tri_status_t tri_mmap_track_memory(tri_mmap_tree_t *mmap_tree,
                                   uint32_t addr);
tri_status_t tri_mmap_track_memory_range(tri_mmap_tree_t *mmap_tree_dst,
                                         tri_mmap_tree_t *mmap_tree_src);
uint32_t tri_mmap_count(tri_mmap_tree_t *mmap_tree);

void tri_mmap_print(tri_mmap_tree_t *mmap_tree);

#endif /* __MMAP__H__ */
