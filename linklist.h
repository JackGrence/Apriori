#include "hash_tree.h"

#ifndef LINKLIST
#define LINKLIST

typedef struct _item_list
{
    item_set *item;
    struct _item_list *next_item_list;
} item_list;

void combination_and_create_list (int *ary, int ary_size, int len, int index,
                                  int *data, item_list *list);
void list_add (item_list *list, item_set *item);
void gen_ht_item_list (ht_node *node, item_list *large_item_list);
void free_item_list (item_list *start);
void free_item_list_and_item (item_list *start);
void free_item (item_set *item);

#endif
