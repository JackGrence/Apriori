#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define NUM_OF_PRODUCT 500
#define HASH_FUNC_MOD 23
#define MAX_LEAF_SIZE 10
#define MIN_SUP 5
#define NUM_OF_INIT_CANDIDATE 2


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

ht_node *c_init (FILE *f);
void ht_insert (int *insert_item, int item_size,
                ht_node *node, bool needcount);
item_set *create_item_set (int *item, int item_size);
ht_node *create_ht_node (int depth, int len);
ht_node *create_leaf_node (item_set *items, int depth);
void append_item_set (item_set *new_items, ht_node *leaf);
void print_ht_tree (ht_node *node);
void combination_and_insert_ht (int *ary, int ary_size, int len, int index,
                                int *data, ht_node *root, bool needcount);
void gen_largeitemset (ht_node *node, ht_node *result_node);

int get_transactions (FILE *f, int **ary);
void print_ary (int *ary, int len);
void test();


int main (int argc, char *argv[])
{
    FILE *f;
    ht_node *large_item_set;
    ht_node *candidate;

    printf ("start initialize c\n"); 
    f = fopen ("./T15I7N0.5KD1K.data", "rb");
    if (f == NULL)
    {
        printf ("Open file error.\nexit...\n");
        exit(1);
    }

    clock_t begin = clock();
    candidate = c_init (f);
    large_item_set = create_leaf_node (0, NUM_OF_INIT_CANDIDATE);
    gen_largeitemset (candidate, large_item_set);
    //print_ht_tree (large_item_set);
    clock_t end = clock();
    double spent = (double) (end - begin) / CLOCKS_PER_SEC;
    printf ("Spent time: %fs\n", spent);

    fclose (f);
    return 0;
}


int
get_transactions (FILE *f, int **ary)
{
    long int tid;
    int len;
    if (fread (&tid, sizeof (tid), 1, f) == 0)    // read tid
    {
        printf ("End read.\n");
        return -1;
    }
    fread (&len, sizeof (len), 1, f);             // read len
    *ary = (int *) malloc (len * sizeof (int));
    fread (*ary, sizeof (int), len, f);
    return len;
}


void
print_ary (int *ary, int len)
{
    int i;
    for (i = 0; i < len; i++)
    {
        printf ("%d ", ary[i]);
    }
    printf ("\n");
}


ht_node *
c_init (FILE *f)
{
    ht_node *root;
    int len = 0;
    int *t_ary;
    root = create_ht_node (0, 0);
    int data[NUM_OF_INIT_CANDIDATE];
    while (len != -1)
    {
        len = get_transactions (f, &t_ary);
        if (len >= NUM_OF_INIT_CANDIDATE)
        {
            combination_and_insert_ht (t_ary, len, NUM_OF_INIT_CANDIDATE,
                                       0, data, root, true);
        }
    }
    //print_ht_tree (root);
    return root;
}

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
            for (i = 0; i < childnode->len; i++)
            {
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
        printf ("Isn't leaf node.\nreturn...");
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
            for (i = 0; i < leaf->len; i++)
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

ht_node *
create_leaf_node (item_set *items, int depth)
{
    ht_node *new_leaf_node;
    new_leaf_node = create_ht_node (depth, 1);
    new_leaf_node->nodes[0] = items;
    return new_leaf_node;
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
                printf ("Find interior node: %d\n", i);
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
        printf ("\n");
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
test ()
{
    printf ("Test ht_insert\n");
    int ary[] = {0, 1, 2};
    int i;
    ht_node *root;
    root = create_ht_node (0, 0);
    for (i = 0; i < 10; i++)
        ht_insert (ary, 3, root, false);
    printf ("count: %d\n", ((item_set *) ((ht_node *) root->nodes[0])->nodes[0])->count);

    printf ("Test ht_insert2\n");
    for (i = 0; i <= 23 * 33; i += 23)
    {
        ary[0] = i;
        ht_insert (ary, 3, root, true);
    }
    //print_ht_tree (root);
}


void
gen_largeitemset (ht_node *node, ht_node *result_node)
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
                gen_largeitemset (nodes_ary[i], result_node);
                free (nodes_ary[i]);
            }
        }
    }
    else /* leaf node */
    {
        item_set_ary = (item_set **) node->nodes;
        for (i = 0; i < MAX_LEAF_SIZE; i++)
        {
            if (item_set_ary[i] != NULL)
            {
                if (item_set_ary[i]->count >= MIN_SUP)
                    append_item_set (item_set_ary[i], result_node);
                else
                    free (item_set_ary[i]);
            }
        }
        if (node->next_leaf != NULL)
        {
            gen_largeitemset (node->next_leaf, result_node);
            free (node->next_leaf);
        }
    }
    free (node->nodes);
}
