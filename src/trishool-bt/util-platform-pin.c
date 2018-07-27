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
#include "pin.H"
#include "util-platform.h"
#include "util-string.h"
#include "trishool/util-log.h"
#include "trishool/status.h"

namespace WD {
    #include "Windows.h"
}

tri_status_t tri_platform_img_name_from_addr(uint32_t addr,
                                             char *img_name,
                                             uint32_t *img_name_len)
{
    string img_name_str;
    uint32_t img_name_str_len;
    tri_status_t status;
    IMG img;

    if (*img_name_len == 0) {
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    img_name[0] = '\0';

    PIN_LockClient();
    img = IMG_FindByAddress(addr);
    PIN_UnlockClient();
    if (!IMG_Valid(img)) {
        tri_log_debug("No image found to contain the address %x.", addr);
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    img_name_str = IMG_Name(img);
    /* The + 1 to account for the terminating NULL char */
    img_name_str_len = img_name_str.length() + 1;

    if (img_name_str_len > *img_name_len) {
        tri_log_error("Buffer not long enough to hold image name.  "
                      "Need buf which is %"PRIu32" bytes long.",
                      img_name_str_len);
        *img_name_len = img_name_str_len;
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    if (tri_string_to_char_array(img_name_str, img_name) != TRI_STATUS_OK) {
        tri_log_error("Error converting image name string to char array.");
        status = TRI_STATUS_ERROR;
        goto return_status;
    }
    *img_name_len = img_name_str_len;

    status = TRI_STATUS_OK;
 return_status:
    return status;
}

tri_status_t tri_platform_rtn_name_from_addr(uint32_t addr,
                                             char *rtn_name,
                                             uint32_t *rtn_name_len)
{
    string rtn_name_str;
    uint32_t rtn_name_str_len;
    tri_status_t status;

    if (*rtn_name_len == 0) {
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    rtn_name[0] = '\0';

    rtn_name_str = RTN_FindNameByAddress(addr);
    if (rtn_name_str.length() == 0) {
        tri_log_debug("No routine found for the address %x.", addr);
        status = TRI_STATUS_ERROR;
        goto return_status;
    }
    /* The + 1 to account for the terminating NULL char */
    rtn_name_str_len = rtn_name_str.length() + 1;

    if (rtn_name_str_len > *rtn_name_len) {
        tri_log_error("Buffer not long enough to hold routine name.  "
                      "Need buf which is %"PRIu32" bytes long.",
                      rtn_name_str_len);
        *rtn_name_len = rtn_name_str_len;
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    if (tri_string_to_char_array(rtn_name_str, rtn_name) != TRI_STATUS_OK) {
        tri_log_error("Error converting routine name string to char array.");
        status = TRI_STATUS_ERROR;
        goto return_status;
    }
    *rtn_name_len = rtn_name_str_len;

    status = TRI_STATUS_OK;
 return_status:
    return status;
}

void tri_platform_memcpy(uint8_t *buf, uint32_t addr, uint32_t *len)
{
    *len = PIN_SafeCopy(buf, (VOID *)addr, *len);

    return;
}

void tri_sleep(uint32_t time)
{
    WD::Sleep(time);
}

void tri_delete_file(char *path)
{
    WD::DeleteFileA(path);
}
