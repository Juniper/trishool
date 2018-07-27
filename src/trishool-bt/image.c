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

#include "trishool-bt-common.h"
#include "trishool/avl.h"
#include "trishool/util-log.h"
#include "trishool/avl.h"
#include "trishool/align.h"
#include "trishool/status.h"
#include "util-platform.h"
#include "mmap.h"
#include "image.h"

static char *image_win32[] = {
    "kernelbase",
    "kernel32",
    "ntdll",
};

typedef struct tri_image_t {
    char *name;
    tri_image_type_t type;
} tri_image_t;

bool tri_image_address_is_win32(uint32_t addr)
{
    char img_name[1024];
    uint32_t img_name_len;
    tri_status_t status;
    bool found;
    int i;

    img_name_len = sizeof(img_name);
    if (tri_platform_img_name_from_addr(addr,
                                        img_name,
                                        &img_name_len) == TRI_STATUS_ERROR)
    {
        tri_log_debug("Error getting image name for addr %x.", addr);
        status = TRI_STATUS_ERROR;
        found = false;
        goto return_status;
    }

    for (i = 0; i < sizeof(image_win32) / sizeof(image_win32[0]); i++) {
        if (strstr(img_name, image_win32[i]) != NULL) {
            break;
        }
    }

    if (i < (sizeof(image_win32) / sizeof(image_win32[0]))) {
        found = true;
    } else {
        found = false;
    }

    status = TRI_STATUS_OK;
 return_status:
    return found;
}
