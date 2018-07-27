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

#ifndef __INSTRUMENTOR_PRIVATE__H__
#define __INSTRUMENTOR_PRIVATE__H__

#include "trishool-bt-common.h"
#include "instr.h"
#include "image.h"
#include "mmap.h"
#include "trishool/status.h"

tri_status_t tri_im_bt_callback(tri_instr_t *instr);
tri_status_t tri_im_track_image(char *image_name,
                                tri_image_type_t type,
                                tri_mmap_tree_t *mmap_tree_image,
                                bool is_main_exe,
                                tri_image_event_t event);
tri_status_t tri_im_track_memory(uint32_t addr);
tri_status_t tri_im_track_memory_range(tri_mmap_tree_t *mmap_tree);
tri_status_t tri_im_init();
tri_status_t tri_im_deinit();

#endif /* __INSTRUMENTOR_PRIVATE__H__ */
