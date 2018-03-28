#include "hash_tree.h"

#ifndef LINKLIST
#define LINKLIST

#define TABLE_LIST_SIZE 500

typedef struct _table_list table_list;

struct _table_list
{
    int table[TABLE_LIST_SIZE];
    table_list *next_table;
};

typedef struct _item_list item_list;

struct _item_list
{
    item_set *item;
    item_list *next_item_list;
};

void combination_and_create_list (int *ary, int ary_size, int len, int index,
                                  int *data, item_list *list);
void list_add (item_list *list, item_set *item);
void gen_ht_item_list (ht_node *node, item_list *large_item_list);
void free_item_list (item_list *start);
void free_item_list_and_item (item_list *start);
void free_item (item_set *item);
table_list *create_tableList (void);
void free_tableList (table_list *victim);
void tableL_insert (int val, table_list *des, bool needcount);
int get_tableL_val (int ind, table_list *tableL);

#endif
