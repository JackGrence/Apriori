#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "hash_tree.h"
#include "linklist.h"
#include "display.h"

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

    if (leaf->depth + 1 == new_items->size)
    {
        add_deepest_leaf (new_items, leaf);
        return;
    }

    if (leaf->len == MAX_LEAF_SIZE)
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
                if (leaf->depth + 2 == new_items->size)
                    add_deepest_leaf (enum_item, new_nodes[hash_index]);
                else
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
            if (leaf->depth + 2 == new_items->size)
                add_deepest_leaf (new_items, new_nodes[hash_index]);
            else
                append_item_set (new_items, new_nodes[hash_index]);
        else
            new_nodes[hash_index] =
                create_leaf_node (new_items, leaf->depth + 1);

        free (leaf->nodes);
        leaf->nodes = (void **) new_nodes;
        leaf->len = 0;
    }
    else
    {
        leaf->nodes[leaf->len] = new_items;
        leaf->len++;
    }
}

void
add_deepest_leaf (item_set *new_items, ht_node *leaf)
{
    int cmp_size;
    int *cmp_ary;
    int depth;
    int match;
    item_set **item_set_ary;
    ht_node *prev_leaf;

    depth = leaf->depth;
    cmp_size = new_items->size - 1;
    cmp_ary = (int *) malloc (sizeof (int) * cmp_size);
    prev_leaf = NULL;
    while (leaf != NULL)
    {
        item_set_ary = (item_set **) leaf->nodes;
        memcpy (cmp_ary, item_set_ary[0]->items, sizeof (int) * cmp_size);
        match = memcmp (cmp_ary, new_items->items, sizeof (int) * cmp_size);
        /* leaf's prefix ary vs new_items prefix ary */
        if (match == 0)
        { /* match */
            if (leaf->len == MAX_LEAF_SIZE)
            {
                prev_leaf = leaf;
                leaf = leaf->next_leaf;
            }
            else
            {
                leaf->nodes[leaf->len] = new_items;
                leaf->len++;
                return;
            }
        }
        else if (match > 0)
        { /* grater than */
            if (prev_leaf != NULL)
            {
                prev_leaf->next_leaf = create_leaf_node (new_items, depth);
                prev_leaf->next_leaf->next_leaf = leaf;
            }
            else
            { /* copy leaf */
                //prev_leaf = create_ht_node (0, 0);
                //memcpy (prev_leaf, leaf, sizeof (ht_node));
                prev_leaf = ht_node_cpy (leaf);
                leaf->nodes[0] = new_items;
                leaf->len = 1;
                leaf->next_leaf = prev_leaf;
                printf ("why?\n");
                //exit (1);
            }
            return;
        }
        else
        {
            prev_leaf = leaf;
            leaf = leaf->next_leaf;
        }
    }
    prev_leaf->next_leaf = create_leaf_node (new_items, depth);
}

ht_node *
ht_node_cpy (ht_node *src)
{
    ht_node *new_node;
    int i;

    new_node = create_ht_node (src->depth, src->len);
    new_node->next_leaf = src->next_leaf;
    for (i = 0; i < HASH_FUNC_MOD; i++)
    {
        new_node->nodes[i] = src->nodes[i];
    }
    return new_node;
}

void
cate_deepest_leaf (ht_node *leaf, int cmp_size)
{
    ht_node *cur_leaf_node;
    ht_node *new_leaf_node;
    ht_node *orig_next_leaf;
    int *cmp_ary;
    int i;
    int orig_leaf_len;
    item_set **item_set_ary;

    cmp_ary = (int *) malloc (sizeof (int) * cmp_size);
    item_set_ary = (item_set **) leaf->nodes;
    memcpy (cmp_ary, item_set_ary[0]->items, sizeof (int) * cmp_size);

    orig_next_leaf = leaf->next_leaf;
    orig_leaf_len = leaf->len;
    cur_leaf_node = NULL;

    for (i = 1; i < orig_leaf_len; i++)
    {
        if (memcmp (cmp_ary, item_set_ary[i]->items, sizeof (int) * cmp_size))
        { /* no match */
            new_leaf_node = create_leaf_node (item_set_ary[i], leaf->depth);
            if (cur_leaf_node == NULL)
            {
                leaf->next_leaf = new_leaf_node;
                leaf->len = i;
            }
            else
                cur_leaf_node->next_leaf = new_leaf_node;
            cur_leaf_node = new_leaf_node;
            memcpy (cmp_ary, item_set_ary[i]->items, sizeof (int) * cmp_size);
        }
        else
        {
            if (cur_leaf_node != NULL)
            {
                if (cur_leaf_node->len == MAX_LEAF_SIZE)
                {
                    cur_leaf_node->next_leaf =
                        create_leaf_node(item_set_ary[i], cur_leaf_node->depth);
                    cur_leaf_node = cur_leaf_node->next_leaf;
                }
                else
                {
                    cur_leaf_node->nodes[cur_leaf_node->len] = item_set_ary[i];
                    cur_leaf_node->len++;
                }
            }
        }
    }
    if (cur_leaf_node != NULL)
        cur_leaf_node->next_leaf = orig_next_leaf;
    if (orig_next_leaf != NULL)
        cate_deepest_leaf (orig_next_leaf, cmp_size);
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
            for (i = 0; i < current_node->len; i++)
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
    ht_node *childnode;
    int hash_index;
    int i;
    int item_index;
    int prefix_size;
    int tableL_val;
    int match;
    item_set **item_set_ary;

    prefix_size = node->depth;
    table_list *remain_aryTableL;

    if (node->len == 0) /* is interior node */
    {
        for (i = 0; i <= ary_size - item_size + prefix_size; i++)
        {
            prefix_ary[prefix_size] = ary[i];
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
        remain_aryTableL = create_tableList();
        for (i = 0; i < ary_size; i++)
            tableL_insert (ary[i], remain_aryTableL, true);

        item_set_ary = (item_set **) node->nodes;
        for (i = 0; i < node->len; i++)
        {
            if (item_set_ary[i] == NULL)
                continue;

            match = memcmp (item_set_ary[i]->items, prefix_ary, sizeof (int) * (prefix_size));
            if (item_size - 1 == prefix_size) /* at deepest leaf */
            {
                //if (match != 0)
                //    break;
                if (match < 0)
                { /* less than */
                    //printf ("next_leaf\n");
                    break;
                }
                else if (match > 0)
                {
                    //printf ("YAY, return\n");
                    return;
                }
            }

            if (match == 0)
            {
                //if (item_size - 1 == prefix_size) /* prefix size == item_size */
                //{
                //    item_set_ary[i]->count++;
                //    break;
                //}
                //else
                //{
                    for (item_index = prefix_size; item_index < item_size; item_index++)
                    {
                        tableL_val = get_tableL_val (item_set_ary[i]->items[item_index], remain_aryTableL);
                        if (tableL_val == 0)
                            break;
                    }
                    item_set_ary[i]->count += tableL_val;
                //}
            }
        }
        free_tableList (remain_aryTableL);
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
    int match;
    void **null_str;
    null_str = (void **) malloc(sizeof (void *) * HASH_FUNC_MOD);
    memset (null_str, 0, sizeof (void *) * HASH_FUNC_MOD);
    match = !memcmp (null_str, node->nodes, HASH_FUNC_MOD * sizeof (void *));
    free (null_str);
    return match;
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

void
free_leaf_node (ht_node *leaf)
{ /* first leaf won't free */
    int i;
    for (i = 0; i < leaf->len; i++)
        free (leaf->nodes[i]);
    free (leaf->nodes);
    if (leaf->next_leaf != NULL)
    {
        free_leaf_node (leaf->next_leaf);
        free (leaf->next_leaf);
    }
}
