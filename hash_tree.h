#include <stdio.h>
#include <stdbool.h>

#ifndef HASH_TREE
#define HASH_TREE

#define NUM_OF_INIT_CANDIDATE 1

int HASH_FUNC_MOD;
int MAX_LEAF_SIZE;

typedef struct hash_tree_node
{
    /* if len == 0 represent that is interior node,
     * else it's number of leaf.
     * nodes point to a node array or items.
     * if len > MAX_LEAF_SIZE and in max depth,
     * next_leaf point to extend leaf.
     */
    int depth;
    int len;
    void **nodes;
    struct hash_tree_node *next_leaf;
} ht_node;

typedef struct _item_set
{
    int count;
    int size;
    int *items;
} item_set;


void ht_insert (int *insert_item, int item_size,
                ht_node *node, bool needcount);
item_set *create_item_set (int *item, int item_size);
ht_node *create_ht_node (int depth, int len);
ht_node *create_leaf_node (item_set *items, int depth);
void append_item_set (item_set *new_items, ht_node *leaf);
void print_ht_tree (ht_node *node);
void combination_and_insert_ht (int *ary, int ary_size, int len, int index,
                                int *data, ht_node *root, bool needcount);
void ht_count (int *ary, int ary_size, ht_node *node, int item_size, int *prefix_ary);
bool all_in_ary (int *ary1, int ary1_size, int *ary2, int ary2_size);
bool ht_is_empty (ht_node *node);
int show_items_from_ht (ht_node *node);
void free_leaf_node (ht_node *leaf);
void cate_deepest_leaf (ht_node *leaf, int cmp_size);
void add_deepest_leaf (item_set *new_items, ht_node *leaf);
ht_node *ht_node_cpy (ht_node *src);

#endif /* ifndef HASH_TREE */
