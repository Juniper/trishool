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

#define _CRT_SECURE_NO_WARNINGS

#include "trishool/trishool-common.h"
#include "trishool/status.h"
#include "trishool/util-log.h"

FILE * g_tri_log_fp;
tri_log_level_t g_tri_log_level;

const char *tri_log_level_to_string(tri_log_level_t log_level)
{
    switch (log_level) {
        case TRI_LOG_LEVEL_EMERGENCY:
            return "Emerg";
        case TRI_LOG_LEVEL_ALERT:
            return "Alert";
        case TRI_LOG_LEVEL_CRITICAL:
            return "Critical";
        case TRI_LOG_LEVEL_ERROR:
            return "Error";
        case TRI_LOG_LEVEL_WARNING:
            return "Warning";
        case TRI_LOG_LEVEL_NOTICE:
            return "Notice";
        case TRI_LOG_LEVEL_INFO:
            return "Info";
        case TRI_LOG_LEVEL_DEBUG:
            return "Debug";
        default:
            exit(EXIT_FAILURE);
    }
}

tri_status_t tri_log_init(tri_log_level_t log_level, const char *path)
{
    tri_status_t status;

    if (log_level == TRI_LOG_LEVEL_NONE) {
        status = TRI_STATUS_OK;
        goto return_status;
    }

    g_tri_log_fp = NULL;
    g_tri_log_level = log_level;

    if (path != NULL) {
        g_tri_log_fp = fopen(path, "w");
        if (g_tri_log_fp == NULL) {
            tri_log_error("Error opening file path \"%s\" for debug logging "
                          "module.", path);
            status = TRI_STATUS_ERROR;
            goto return_status;
        }
    }

    status = TRI_STATUS_OK;
 return_status:
    return status;
}

tri_status_t tri_log_deinit()
{
    tri_status_t status;

    if (g_tri_log_fp != NULL) {
        fclose(g_tri_log_fp);
        g_tri_log_fp = NULL;
    }

    status = TRI_STATUS_OK;
    return status;
}
