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
#include "trishool-common.h"
#include "radix.h"

typedef struct one_t {
    int touch;
    struct one_t *next;
} one_t;

void touch(void *data)
{
    if (data == NULL)
        return;

    one_t *user_data = (one_t *)data;
    user_data->touch = 1;
}

int main()
{
    tri_radix_t *root = NULL;
    tri_radix_t *tmpr = NULL;
    one_t *tmp = NULL;
    one_t *head = NULL;
    uint32_t a = 0x00400000;
    void *data_existing;

    tri_radix_init(&root);

    while (a < 0x00800000) {
        tmp = malloc(sizeof(*tmp));
        memset(tmp, 0, sizeof(tmp));

        if (head == NULL) {
            head = tmp;
        } else {
            tmp->next = head;
            head = tmp;

        }

        tri_radix_add_entry(&root, a, tmp, &data_existing);
        a++;
    }

    tri_radix_add_entry(&root, a, 1, &data_existing);
    if (data_existing == NULL) {
        printf("No existing data\n");
    } else {
        printf("Yes existing data\n");
    }
    tri_radix_add_entry(&root, a, 2, &data_existing);
    if (data_existing == NULL) {
        printf("No existing data\n");
    } else {
        printf("Yes existing data\n");
    }
    fflush(stdout);
    exit(0);

    tri_parse(root, (tri_radix_touch_user_data_func_t)touch);

    tmp = head;
    while (tmp->next != NULL) {
        if (tmp->touch == 0) {
            printf("not touched\n");
            exit(EXIT_FAILURE);
        }
        tmp = tmp->next;
    }
    printf("all touched\n");
    return 0;
}
