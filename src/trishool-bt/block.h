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

#ifndef __BLOCK__H__
#define __BLOCK__H__

#include "trishool-bt-common.h"
#include "trishool/status.h"

uint32_t tri_block_curr_addr();
tri_status_t tri_block_enter(uint32_t block_addr, char *tag, bool is_routine);
tri_status_t tri_block_exit();

#endif /* __BLOCK__H__ */
