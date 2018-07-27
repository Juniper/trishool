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
#include "mmap.h"

#define TRI_MMAP_PAGE_SIZE 4096

typedef struct tri_mmap_t {
    uint32_t page_start_addr;
    uint32_t page_end_addr;
} tri_mmap_t;

typedef struct tri_mmap_tree_t {
    tri_avl_t *mmap_tree_avl;
} tri_mmap_tree_t;

static tri_mmap_t *tri_mmap_alloc(uint32_t page_start_addr,
                                  uint32_t page_end_addr)
{
    tri_mmap_t *mmap;

    mmap = (tri_mmap_t *)malloc(sizeof(*mmap));
    if (mmap == NULL) {
        tri_log_error("malloc() failure.");
        goto return_status;
    }
    memset(mmap, 0, sizeof(*mmap));
    mmap->page_start_addr = page_start_addr;
    mmap->page_end_addr = page_end_addr;

 return_status:
    return mmap;
}

static void tri_mmap_free(tri_mmap_t *mmap)
{
    free(mmap);
    return;
}

static int tri_mmap_cmp(void *mmap1_, void *mmap2_)
{
    tri_mmap_t *mmap1 = (tri_mmap_t *)mmap1_;
    tri_mmap_t *mmap2 = (tri_mmap_t *)mmap2_;

    return (mmap1->page_start_addr - mmap2->page_start_addr);
}

tri_status_t tri_mmap_track_memory(tri_mmap_tree_t *mmap_tree,
                                   uint32_t addr)
{
    tri_mmap_t *mmap;
    tri_mmap_t *mmap_existing;
    uint32_t page_start_addr;
    uint32_t page_end_addr;
    tri_status_t status;

    page_start_addr = addr;
    ALIGN_DOWN(page_start_addr, TRI_MMAP_PAGE_SIZE);
    page_end_addr = page_start_addr + (TRI_MMAP_PAGE_SIZE - 1);

    mmap = tri_mmap_alloc(page_start_addr, page_end_addr);
    if (mmap == NULL) {
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    if (tri_avl_add_entry(mmap_tree->mmap_tree_avl, mmap,
                          (void **)&mmap_existing) != TRI_STATUS_OK)
    {
        tri_mmap_free(mmap);

        if (mmap_existing == NULL) {
            status = TRI_STATUS_ERROR;
            goto return_status;
        }
    }

    status = TRI_STATUS_OK;
 return_status:
    return status;
}

static void tri_mmap_track_memory_range_(void *mmap_, void *mmap_tree_dst)
{
    tri_mmap_t *mmap = (tri_mmap_t *)mmap_;

    tri_mmap_track_memory((tri_mmap_tree_t *)mmap_tree_dst,
                          mmap->page_start_addr);

    return;
}

tri_status_t tri_mmap_track_memory_range(tri_mmap_tree_t *mmap_tree_dst,
                                         tri_mmap_tree_t *mmap_tree_src)
{
    tri_avl_parse(mmap_tree_src->mmap_tree_avl,
                  tri_mmap_track_memory_range_, mmap_tree_dst);

    return TRI_STATUS_OK;
}

uint32_t tri_mmap_count(tri_mmap_tree_t *mmap_tree)
{
    return tri_avl_count(mmap_tree->mmap_tree_avl);
}

static void tri_mmap_print_page(void *mmap_, void *arg)
{
    tri_mmap_t *mmap = (tri_mmap_t *)mmap_;

    tri_log_debug("Page: %x %x", mmap->page_start_addr, mmap->page_end_addr);

    return;
}

void tri_mmap_print(tri_mmap_tree_t *mmap_tree)
{
    tri_log_debug("Mmap:");

    tri_avl_parse(mmap_tree->mmap_tree_avl, tri_mmap_print_page, NULL);

    return;
}

tri_status_t tri_mmap_init(tri_mmap_tree_t **mmap_tree_)
{
    tri_mmap_tree_t *mmap_tree = NULL;
    tri_status_t status;

    *mmap_tree_ = NULL;

    if ((mmap_tree = (tri_mmap_tree_t *)malloc(sizeof(*mmap_tree))) == NULL) {
        tri_log_error("malloc() error allocating memory for mmap tree.");
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    if (tri_avl_init(&mmap_tree->mmap_tree_avl,
             tri_mmap_cmp,
             (tri_avl_free_user_data_func_t)tri_mmap_free) != TRI_STATUS_OK)
    {
        tri_log_error("Error initializing avl tree for mmap.");
        status = TRI_STATUS_ERROR;
        goto return_status;
    }

    *mmap_tree_ = mmap_tree;

    status = TRI_STATUS_OK;
 return_status:
    return status;
}

tri_status_t tri_mmap_deinit(tri_mmap_tree_t *mmap_tree)
{
    tri_avl_deinit(mmap_tree->mmap_tree_avl);
    free(mmap_tree);

    return TRI_STATUS_OK;
}
