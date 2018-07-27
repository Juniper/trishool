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

void tri_token_breakup_line_inplace(char *line,
                                    int symbol,
                                    char *tokens[], uint32_t tokens_buf_size,
                                    uint32_t *tokens_returned)
{
    char *tmp1 = line;
    char *sub;

    *tokens_returned = 0;

    while ((sub = strchr(tmp1, symbol)) != NULL) {
        BUG_ON(*tokens_returned == tokens_buf_size);
        tokens[(*tokens_returned)++] = tmp1;
        sub[0] = '\0';
        tmp1 = sub + 1;
    }
    BUG_ON(*tokens_returned == tokens_buf_size);
    tokens[(*tokens_returned)++] = tmp1;
}
