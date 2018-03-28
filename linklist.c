#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_tree.h"
#include "linklist.h"

void
combination_and_create_list (int *ary, int ary_size, int len, int index,
                             int *data, item_list *list)
{
    int i;
    if (len == 0)
    {
        list_add (list, create_item_set (data, index));
        return;
    }
    for (i = 0; i <= ary_size - len; i++)
    {
        data[index] = ary[i];
        combination_and_create_list (&ary[i + 1], ary_size - i - 1, len - 1,
                                     index + 1, data, list);
    }
}

void
gen_ht_item_list (ht_node *node, item_list *large_item_list)
{
    int i;
    ht_node **nodes_ary;
    item_set **item_set_ary;
    if (node->len == 0) /* is interior node */
    {
        nodes_ary = (ht_node **) node->nodes;
        for (i = 0; i < HASH_FUNC_MOD; i++)
        {
            if (nodes_ary[i] != NULL)
            {
                gen_ht_item_list (nodes_ary[i], large_item_list);
            }
        }
    }
    else /* leaf node */
    {
        item_set_ary = (item_set **) node->nodes;
        for (i = 0; i < node->len; i++)
        {
            if (item_set_ary[i] != NULL)
            {
                list_add (large_item_list, item_set_ary[i]);
            }
        }
        if (node->next_leaf != NULL)
        {
            gen_ht_item_list (node->next_leaf, large_item_list);
        }
    }

}

void
free_item (item_set *item)
{
    free (item->items);
    free (item);
}

void
free_item_list (item_list *start)
{
    if (start->next_item_list != NULL)
        free_item_list (start->next_item_list);
    free (start);
}

void
free_item_list_and_item (item_list *start)
{
    if (start->next_item_list != NULL)
        free_item_list_and_item (start->next_item_list);
    if (start->item != NULL)
        free (start->item);
    free (start);
}

void
list_add (item_list *list, item_set *item)
{
    item_list *new_list;
    new_list = (item_list *) malloc (sizeof (item_list));
    memset (new_list, 0, sizeof (item_list));
    new_list->item = item;
    new_list->next_item_list = list->next_item_list;
    list->next_item_list = new_list;
}

table_list *
create_tableList (void)
{
    table_list *new_table;
    new_table = (table_list *) malloc (sizeof (table_list));
    memset (new_table, 0, sizeof (table_list));
    return new_table;
}

void
free_tableList (table_list *victim)
{
    if (victim->next_table != NULL)
        free_tableList (victim->next_table);
    free (victim);
}

void
tableL_insert (int val, table_list *des, bool needcount)
{
    div_t val_pos;
    int i;

    val_pos = div (val, TABLE_LIST_SIZE);
    for (i = 0; i < val_pos.quot; i++)
    {
        if (des->next_table == NULL)
        {
            des->next_table = create_tableList();
        }
        des = des->next_table;
    }
    if (needcount)
        des->table[val_pos.rem]++;
}
