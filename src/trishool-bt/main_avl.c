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
#include "avl.h"

typedef struct one_t {
    int addr;
    int touch;
    struct one_t *next;
} one_t;

int value_touch = 0;

void touch(void *data)
{
    if (data == NULL)
        return;

    one_t *user_data = (one_t *)data;
    user_data->touch = 1;
    value_touch += user_data->addr;
}

void touch_and_print(void *data)
{
    if (data == NULL)
        return;

    one_t *user_data = (one_t *)data;
    user_data->touch = 1;
    printf("%p\n", (void *)user_data->addr);
}

int cmp(void *a_, void *b_)
{
    one_t *a = (one_t *)a_;
    one_t *b = (one_t *)b_;

    return (a->addr - b->addr);

}

int main()
{
    tri_avl_t *tree = NULL;
    one_t *tmp = NULL;
    one_t *head = NULL;
    uint32_t a[] = { 0x00100000,
                     0x00200000,
                     0x00300000,
                     0x00400000,
                     0x00500000,
                     0x00600000,
                     0x00700000,
                     0x00800000,
                     0x00900000,
                     0x00A00000,
                     0x00B00000,
                     0x00C00000,
                     0x00D00000,
                     0x00E00000,
                     0x00F00000,
                     0x00001000,
                     0x00002000,
                     0x00003000,
                     0x00004000,
                     0x00005000,
                     0x00006000,
                     0x00007000,
                     0x00008000,
                     0x00009000,
                     0x0000A000,
                     0x0000B000,
                     0x0000C000,
                     0x0000D000,
                     0x0000E000,
                     0x0000F000,
                     0x01000000,
                     0x02000000,
                     0x03000000,
                     0x04000000,
                     0x05000000,
                     0x06000000,
                     0x07000000,
                     0x08000000,
                     0x09000000,
                     0x0A000000,
                     0x0B000000,
                     0x0C000000,
                     0x0D000000,
                     0x0E000000,
                     0x0F000000 };
    int len = sizeof(a) / sizeof(a[0]);
    int i = 0;
    void *data_existing;

    tri_avl_init(&tree, cmp);

    i = 0;
    int added_values = 0;
    int value = 0;
    while (added_values < 0x01000000) {
        //i = rand() % len;
        tmp = malloc(sizeof(*tmp));
        memset(tmp, 0, sizeof(tmp));
        /* *NOTE* rand() slows the program massively */
        tmp->addr = rand();
        //tmp->addr = ++i;

        data_existing = NULL;
        if (tri_avl_add_entry(tree, tmp, &data_existing) != TRI_STATUS_OK) {
            if (data_existing != NULL) {
                free(tmp);
            }
        } else {
            if (head == NULL) {
                head = tmp;
            } else {
                tmp->next = head;
                head = tmp;
            }
            added_values++;
            value += tmp->addr;
        }

        //a[i]++;
        i++;
    }
    printf("added values: %x\n", added_values);

#if 0
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
#endif

    //tri_avl_parse(tree, (tri_avl_touch_user_data_func_t)touch_and_print);
    tri_avl_parse(tree, (tri_avl_touch_user_data_func_t)touch);

    tmp = head;
    int not_touched = 0;
    int touched = 0;
    while (tmp != NULL) {
        if (tmp->touch == 0) {
            not_touched++;
        } else {
            touched++;
        }
        tmp = tmp->next;
    }
    printf("touched: %x not_touched: %x\n", touched, not_touched);
    printf("value: %x value_touched: %x\n", value, value_touch);

    return 0;
}
