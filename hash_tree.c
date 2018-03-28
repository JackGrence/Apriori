#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "hash_tree.h"

void
ht_insert (int *insert_item, int item_size, ht_node *node, bool needcount)
{
    int hash_index;
    ht_node *childnode;
    int i;
    int match;
    item_set *new_items;
    if (node->len == 0) /* is interior node */
    {
        hash_index = insert_item[node->depth] % HASH_FUNC_MOD;
        childnode = (ht_node *) node->nodes[hash_index];
        if (childnode == NULL)
        {
            /* create leaf node */
            new_items = create_item_set (insert_item, item_size);
            new_items->count = needcount ? 1 : 0;
            node->nodes[hash_index] =
                create_leaf_node (new_items, node->depth + 1);
        }
        else
        {
            ht_insert (insert_item, item_size, childnode, needcount);
        }
    }
    else  /* is leaf node */
    {
        /* search item */
        childnode = node;
        while (childnode != NULL)
        {
            for (i = 0; i < MAX_LEAF_SIZE; i++)
            {
                if (childnode->nodes[i] != NULL)
                    match = memcmp (insert_item,
                                    ((item_set *)childnode->nodes[i])->items,
                                    sizeof (int) * item_size);

                if (match == 0)
                    break;
            }

            if (match == 0)
            {
                /* count++ */
                if (needcount)
                    ((item_set *) childnode->nodes[i])->count++;
                break;
            }
            else if (childnode->next_leaf == NULL)
            {
                new_items = create_item_set (insert_item, item_size);
                new_items->count = needcount ? 1 : 0;
                append_item_set (new_items, childnode);
                break;
            }
            childnode = childnode->next_leaf;
        }
    }
}

ht_node *
create_ht_node (int depth, int len)
{
    ht_node *new_node;
    new_node = (ht_node *) malloc (sizeof (ht_node));
    memset (new_node, 0, sizeof (ht_node));
    new_node->depth = depth;
    new_node->len = len;
    new_node->nodes = (void **) malloc(sizeof (void *) * HASH_FUNC_MOD);
    memset (new_node->nodes, 0, sizeof (void *) * HASH_FUNC_MOD);
    return new_node;
}


item_set *
create_item_set (int *item, int item_size)
{
    item_set *new_set;
    new_set = (item_set *) malloc (sizeof (item_set));
    new_set->count = 0;
    new_set->size = item_size;
    new_set->items = (int *) malloc (sizeof (int) * item_size);
    memcpy (new_set->items, item, sizeof (int) * item_size);
    return new_set;
}

ht_node *
create_leaf_node (item_set *items, int depth)
{
    ht_node *new_leaf_node;
    new_leaf_node = create_ht_node (depth, 1);
    new_leaf_node->nodes[0] = items;
    return new_leaf_node;
}

void
append_item_set (item_set *new_items, ht_node *leaf)
{
    /* judge leaf size, split or extend. */
    ht_node **new_nodes;
    int i;
    int hash_index;
    item_set *enum_item;
    if (leaf->len <= 0)
    {
        printf ("Isn't leaf node.\nreturn...\n");
        exit(1);
        return;
    }

    if (leaf->len == MAX_LEAF_SIZE)
    {
        if (leaf->depth == new_items->size)
        {
            /* extend leaf */
            if (leaf->next_leaf == NULL)
            {
                leaf->next_leaf = create_leaf_node(new_items, leaf->depth);
            }
            else
                append_item_set(new_items, leaf->next_leaf);
        }
        else
        {
            /* split */
            new_nodes = (ht_node **) malloc(sizeof (void *) * HASH_FUNC_MOD);

            memset (new_nodes, 0, sizeof (void *) * HASH_FUNC_MOD);
            for (i = 0; i < MAX_LEAF_SIZE; i++)
            {
                enum_item = (item_set *) leaf->nodes[i];
                hash_index = enum_item->items[leaf->depth] % HASH_FUNC_MOD;
                if (new_nodes[hash_index] != NULL)
                {
                    append_item_set (enum_item, new_nodes[hash_index]);
                }
                else
                {
                    /* create leaf node */
                    new_nodes[hash_index] =
                        create_leaf_node (enum_item, leaf->depth + 1);
                }
            }

            hash_index = new_items->items[leaf->depth] % HASH_FUNC_MOD;
            if (new_nodes[hash_index] != NULL)
                append_item_set (new_items, new_nodes[hash_index]);
            else
                new_nodes[hash_index] =
                    create_leaf_node (new_items, leaf->depth + 1);

            free (leaf->nodes);
            leaf->nodes = (void **) new_nodes;
            leaf->len = 0;
        }
    }
    else
    {
        leaf->nodes[leaf->len] = new_items;
        leaf->len++;
    }
}

void
print_ht_tree (ht_node *node)
{
    int i;
    int cnt;
    ht_node **nodes_ary;
    item_set **item_set_ary;
    ht_node *current_node;
    if (node->len == 0) /* is interior node */
    {
        nodes_ary = (ht_node **) node->nodes;
        for (i = 0; i < HASH_FUNC_MOD; i++)
        {
            if (nodes_ary[i] != NULL)
            {
                printf ("Find interior node: %d, depth: %d\n", i, node->depth);
                print_ht_tree (nodes_ary[i]);
            }
        }
    }
    else
    {
        current_node = node;
        while (current_node != NULL)
        {
            printf ("Find leaf node.\ncontent:\n");
            item_set_ary = (item_set **) current_node->nodes;
            for (i = 0; i < MAX_LEAF_SIZE; i++)
            {
                if (item_set_ary[i] != NULL)
                {
                    printf ("Find items: %d\n[", i);
                    for (cnt = 0; cnt < item_set_ary[i]->size; cnt++)
                    {
                        printf ("%d ", item_set_ary[i]->items[cnt]);
                    }
                    printf ("] count: %d\n", item_set_ary[i]->count);
                }
            }
            current_node = current_node->next_leaf;
        }
    }
}

void
combination_and_insert_ht (int *ary, int ary_size, int len, int index,
                           int *data, ht_node *root, bool needcount)
{
    int i;
    if (len == 0)
    {
        ht_insert (data, index, root, needcount);
        return;
    }
    for (i = 0; i <= ary_size - len; i++)
    {
        data[index] = ary[i];
        combination_and_insert_ht (&ary[i + 1], ary_size - i - 1, len - 1,
                                   index + 1, data, root, needcount);
    }
}

void
ht_count (int *ary, int ary_size, ht_node *node, int item_size, int *prefix_ary)
{
    int hash_index;
    ht_node *childnode;
    int i;
    item_set **item_set_ary;

    if (node->len == 0) /* is interior node */
    {
        for (i = 0; i <= ary_size - item_size + node->depth; i++)
        {
            prefix_ary[node->depth] = ary[i];
            hash_index = ary[i] % HASH_FUNC_MOD;
            childnode = (ht_node *) node->nodes[hash_index];
            if (childnode != NULL)
            {
                ht_count (&ary[i + 1], ary_size - i - 1, childnode, item_size, prefix_ary);
            }
        }
    }
    else  /* is leaf node */
    {
        /* search item */
        item_set_ary = (item_set **) node->nodes;
        for (i = 0; i < MAX_LEAF_SIZE; i++)
        {
            if (item_set_ary[i] == NULL)
                continue;
            if (!memcmp(item_set_ary[i]->items, prefix_ary, sizeof (int) * node->depth))
            {
                if (item_size - node->depth == 0)
                    item_set_ary[i]->count++;
                else if (all_in_ary (&item_set_ary[i]->items[node->depth], item_size - node->depth,
                                ary, ary_size))
                    item_set_ary[i]->count++;
            }
        }
        if (node->next_leaf != NULL)
            ht_count (ary, ary_size, node->next_leaf, item_size, prefix_ary);
    }
}

bool
all_in_ary (int *ary1, int ary1_size, int *ary2, int ary2_size)
{ /* ary1 all in ary2. ary2_size > ary1_size */
    int i, j;
    bool result;
    for (i = 0; i < ary1_size; i++)
    {
        result = false;
        for (j = 0; j < ary2_size; j++)
        {
            if (ary1[i] == ary2[j])
            {
                result = true;
                break;
            }
        }
        if (!result)
        {
            return false;
        }
    }
    return result;
}

bool
ht_is_empty (ht_node *node)
{
    //char null_str[HASH_FUNC_MOD * sizeof (int)];
    //memset (null_str, 0, HASH_FUNC_MOD * sizeof (int));
    void **null_str;
    null_str = (void **) malloc(sizeof (void *) * HASH_FUNC_MOD);
    memset (null_str, 0, sizeof (void *) * HASH_FUNC_MOD);
    return !memcmp (null_str, node->nodes, HASH_FUNC_MOD * sizeof (void *));
}

int
show_items_from_ht (ht_node *node)
{
    int item_num = 0;
    int i;
    ht_node **nodes_ary;
    item_set **item_set_ary;
    if (node->len == 0) /* interior node */
    {
        nodes_ary = (ht_node **) node->nodes;
        for (i = 0; i < HASH_FUNC_MOD; i++)
        {
            if (nodes_ary[i] != NULL)
            {
                item_num += show_items_from_ht (nodes_ary[i]);
            }
        }
    }
    else
    {
        item_set_ary = (item_set **) node->nodes;
        for (i = 0; i < MAX_LEAF_SIZE; i++)
        {
            if (item_set_ary[i] != NULL)
            {
                //print_ary (item_set_ary[i]->items, item_set_ary[i]->size);
                item_num++;
            }
        }
        if (node->next_leaf != NULL)
            item_num += show_items_from_ht (node->next_leaf);
    }
    return item_num;
}