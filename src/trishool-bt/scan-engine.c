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
#include "trishool/util-log.h"
#include "trishool/status.h"
#include "util-platform.h"

#define TRI_SCAN_ENGINE_PATTERNS_FILE "tri-scan-engine-patterns.txt"

static char **g_tri_patterns;
static uint32_t g_tri_patterns_count = 0;

uint32_t tri_scan_engine_run(uint8_t *membuf, uint32_t membuf_len)
{
#if 0
    uint32_t i = 0;
    uint8_t *pat = (uint8_t *)"hahalala";
    uint32_t patlen = strlen((char *)pat);
    uint32_t max = buf_len - patlen;

    for (i = 0; i < max; i++) {
        if (memcmp(buf + i, pat, patlen) == 0) {
            return i;
        }
    }

    return 0;
#endif
    uint32_t match_count = 0;
    char *pat;
    uint32_t patlen;
    uint32_t maxlen;
    uint32_t pat_idx = 0;
    uint32_t i;

    for (pat_idx = 0; pat_idx < g_tri_patterns_count; pat_idx++) {
        pat = g_tri_patterns[pat_idx];
        patlen = strlen(pat);
        maxlen = membuf_len - patlen;
        fflush(stdout);
        for (i = 0; i < maxlen; i++) {
            if (memcmp(membuf + i, pat, patlen) == 0) {
                match_count++;
            }
        }
    }

    tri_log_debug("Scanning done");

 return_status:
    return match_count;
}

tri_status_t tri_scan_engine_init()
{
    FILE *fp = NULL;
    char buf[200];
    uint32_t count;
    uint32_t len;
    uint32_t i;
    tri_status_t status;

    if ((fp = fopen(TRI_SCAN_ENGINE_PATTERNS_FILE, "r")) == NULL) {
        tri_log_error("Error opening scan engine patterns file \"%s\".",
                      TRI_SCAN_ENGINE_PATTERNS_FILE);
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    while (fgets(buf, sizeof(buf), fp) != NULL) {
        count++;
    }

    g_tri_patterns = (char **)malloc(count * sizeof(*g_tri_patterns));
    if (g_tri_patterns == NULL) {
        status = TRI_STATUS_ERROR;
        goto return_status;
    }
    memset(g_tri_patterns, 0, count * sizeof(char *));

    fseek(fp, 0, SEEK_SET);

    while (fgets(buf, sizeof(buf), fp) != NULL) {
        BUG_ON(count == g_tri_patterns_count);

        len = strlen(buf);
        if (len < 2)
            continue;

        if (buf[len - 2] == '\r' && buf[len - 1] == '\n') {
            buf[len - 2] = '\0';
        } else if (buf[len - 1] == '\n') {
            buf[len - 1] = '\0';
        }

        if ((g_tri_patterns[g_tri_patterns_count] = strdup(buf)) == NULL) {
            status = TRI_STATUS_ERROR;
            goto return_status;
        }
        tri_log_debug("Scan engine added pattern \"%s\".",
                      g_tri_patterns[g_tri_patterns_count]);
        g_tri_patterns_count++;
    }
    tri_log_debug("Scan engine added %"PRIu32" patterns.",
                  g_tri_patterns_count);

    status = TRI_STATUS_OK;
 return_status:
    if (status == TRI_STATUS_ERROR) {
        if (g_tri_patterns != NULL) {
            for (i = 0; i < g_tri_patterns_count; i++) {
                free(g_tri_patterns[i]);
            }
            free(g_tri_patterns);
        }
    }
    if (fp != NULL) {
        fclose(fp);
    }
    return status;
}
