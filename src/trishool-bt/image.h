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

#ifndef __IMAGE__H__
#define __IMAGE__H__

#include "trishool-bt-common.h"
#include "trishool/status.h"

typedef enum tri_image_type_t {
    TRI_IMAGE_TYPE_SHAREDLIB = 0,
    TRI_IMAGE_TYPE_SHARED,
    TRI_IMAGE_TYPE_STATIC,
    TRI_IMAGE_TYPE_RELOCATABLE,
    TRI_IMAGE_TYPE_DYNAMIC_CODE,
    TRI_IMAGE_TYPE_MAX,
} tri_image_type_t;

typedef enum tri_image_event_t {
    TRI_IMAGE_EVENT_LOAD = 0,
    TRI_IMAGE_EVENT_UNLOAD = 1,
    TRI_IMAGE_EVENT_MAX,
} tri_image_event_t;

bool tri_image_address_is_win32(uint32_t addr);

#endif /* __IMAGE__H__  */
