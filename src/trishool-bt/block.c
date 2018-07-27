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
#include "db.h"

typedef struct tri_block_t {
    uint32_t addr;
    char *name;
    bool is_routine;
    struct tri_block_t *next;
} tri_block_t;

/* This block head holds the last inserted element.
 * We basically are mimicing a stack.  So the head
 * holds the last callee or long jump address.
 * This has been devised to hold details for the call graph.
 * When the program starts the first instruction of the
 * application is going to be the first chunk/function/block
 * address. */
static tri_block_t *g_block_root = NULL;

static tri_block_t *tri_block_alloc(uint32_t addr,
                                    char *tag,
                                    bool is_routine)
{
    tri_block_t *b = NULL;
    char *block_name_format_for_sub = "sub_%.8x_%s";
    char *block_name_format_for_non_sub = "loc_%.8x_%s";
    char *block_name_format;
    uint32_t block_name_len;
    tri_status_t status;

    if ((b = (tri_block_t *)malloc(sizeof(*b))) == NULL) {
        tri_log_error("malloc() error allocting memory.");
        goto return_status;
    }
    memset(b, 0, sizeof(*b));
    b->addr = addr;
    b->is_routine = is_routine;

    if (is_routine) {
        block_name_format = block_name_format_for_sub;
    } else {
        block_name_format = block_name_format_for_non_sub;
    }
    /* 10 for the %.8x and in case 0x is added to it and 1 is for the
     * ending NULL character */
    block_name_len = strlen(block_name_format) + strlen(tag) + 10 + 1;

    b->name = (char *)malloc(block_name_len);
    if (b->name == NULL) {
        tri_log_debug("malloc() error allocating memory.");
        status = TRI_STATUS_ERROR;
        goto return_status;
    }
    /* The extra argument in case of block_name_format_without_routine_name being
     * used is ignored */
#if BT_PLATFORM_PIN
    snprintf(b->name, block_name_len, block_name_format, addr, tag);
#elif BT_PLATFORM_DRIO
    _snprintf_s(b->name, block_name_len, _TRUNCATE,
                block_name_format, addr, tag);
#endif

 return_status:
    if (status == TRI_STATUS_ERROR) {
        if (b != NULL) {
            if (b->name != NULL) {
                free(b->name);
            }
            free(b);
        }
        b = NULL;
    }
    return b;
}

static void tri_block_free(tri_block_t *b)
{
    if (b->name != NULL) {
        free(b->name);
    }
    free(b);

    return;
}

uint32_t tri_block_curr_addr()
{
    if (g_block_root == NULL)
        return 0x00000000;

    return g_block_root->addr;
}

tri_status_t tri_block_enter(uint32_t block_addr, char *tag, bool is_routine)
{
    tri_block_t *b;
    tri_status_t status;

    if ((b = tri_block_alloc(block_addr, tag, is_routine)) == NULL) {
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    b->next = g_block_root;
    g_block_root = b;

    tri_log_debug("Block Entered: "
                  "{ "
                  "\'block_addr\': \'%x\', "
                  "\'block_name\': \'%s\' "
                  "}",
                  b->addr, b->name);

    //tri_db_insert_block(block_addr, block_name);

    status = TRI_STATUS_OK;
 return_status:
    return status;
}

tri_status_t tri_block_exit()
{
    tri_block_t *b;

    BUG_ON(g_block_root == NULL);

    if (g_block_root->next == NULL) {
        tri_log_debug("Block exited: "
                      "{ "
                      "\'from_block_addr\': \'%.8x\', "
                      "\'from_block_name\': \'%s\', "
                      "\'to_block_addr\': \'%.8x\', "
                      "\'to_block_name\': \'%s\' "
                      "}",
                      g_block_root->addr,
                      g_block_root->name,
                      0x00000000,
                      "ROOT");
    } else {
        tri_log_debug("Block exited: "
                      "{ "
                      "\'from_block_addr\': \'%.8x\', "
                      "\'from_block_name\': \'%s\', "
                      "\'to_block_addr\': \'%.8x\', "
                      "\'to_block_name\': \'%s\' "
                      "}",
                      g_block_root->addr,
                      g_block_root->name,
                      g_block_root->next->addr,
                      g_block_root->next->name);
    }

    b = g_block_root;
    g_block_root = g_block_root->next;

    tri_block_free(b);

    return TRI_STATUS_OK;
}
