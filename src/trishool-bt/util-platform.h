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

#ifndef __UTIL_PLATFORM__H__
#define __UTIL_PLATFORM__H__

#include "trishool-bt-common.h"
#include "trishool/status.h"

tri_status_t tri_platform_img_name_from_addr(uint32_t addr,
                                             char *img_name,
                                             uint32_t *img_name_len);
tri_status_t tri_platform_rtn_name_from_addr(uint32_t addr,
                                             char *rtn_name,
                                             uint32_t *rtn_name_len);
void tri_platform_memcpy(uint8_t *buf, uint32_t addr, uint32_t *len);

void tri_sleep(uint32_t time);

void tri_delete_file(char *path);


#endif /* __UTIL_PLATFORM__H__ */
