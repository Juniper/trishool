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
#include "trishool/status.h"

uint32_t tri_string_length(string &s)
{
    return s.length();
}

tri_status_t tri_string_to_char_array(const std::string &s, char *a)
{
    uint32_t len = s.length();

    memcpy(a, s.c_str(), len);
    a[len] = '\0';

    return TRI_STATUS_OK;
}
