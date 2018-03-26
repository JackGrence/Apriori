#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define NUM_OF_PRODUCT 500
#define HASH_FUNC_MOD 23
#define MAX_LEAF_SIZE 10
#define MIN_SUP 30000
#define NUM_OF_INIT_CANDIDATE 1
#define FILE_NAME "./T15I7N0.5KD1000K.data"


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

typedef struct _item_list
{
    item_set *item;
    struct _item_list *next_item_list;
} item_list;

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
void combination_and_create_list (int *ary, int ary_size, int len, int index,
                                  int *data, item_list *list);
void gen_largeitemset (ht_node *node, ht_node *result_node);

void apriori_gen (ht_node *large_item_set, ht_node *result_node);
int get_transactions (FILE *f, int **ary);
void list_add (item_list *list, item_set *item);
void gen_ht_item_list (ht_node *node, item_list *large_item_list);
bool subset_belong_L (int *ary, int ary_size, item_list *large_item_list);
void ht_count (int *ary, int ary_size, ht_node *node, int item_size, int *prefix_ary);
bool all_in_ary (int *ary1, int ary1_size, int *ary2, int ary2_size);
bool ht_is_empty (ht_node *node);

void print_ary (int *ary, int len);
void test();


int main (int argc, char *argv[])
{
    //char a[10];
    //test();
    //gets(a);
    FILE *f;
    int candidate_size;
    ht_node *large_item_set;
    ht_node *candidate;

    printf ("start initialize c\n");
    f = fopen (FILE_NAME, "rb");
    if (f == NULL)
    {
        printf ("Open file error.\nexit...\n");
        exit(1);
    }

    clock_t begin = clock();
    candidate = c_init (f);
    candidate_size = NUM_OF_INIT_CANDIDATE;
    //print_ht_tree (large_item_set);

    fclose (f);

    for (candidate_size = NUM_OF_INIT_CANDIDATE + 1; !ht_is_empty (candidate); candidate_size++)
    {
        printf ("++++++++++++++++++\n");
        print_ht_tree (candidate);
        printf ("++++++++++++++++++\n");
        large_item_set = create_ht_node (0, 0);
        gen_largeitemset (candidate, large_item_set);
        free (candidate);
        if (ht_is_empty (large_item_set))
            break;
        //else
        //{
        //    printf ("++++++++++++++++++\n");
        //    print_ht_tree (large_item_set);
        //    printf ("++++++++++++++++++\n");
        //}
        candidate = create_ht_node (0, 0);
        apriori_gen (large_item_set, candidate);
        f = fopen (FILE_NAME, "rb");
        if (f == NULL)
        {
            printf ("Open file error.\nexit...\n");
            exit(1);
        }
        int len = 0;
        int *t_ary;
        int *ht_count_prefix_ary;
        ht_count_prefix_ary = (int *) malloc (sizeof (int) * candidate_size);
        while (len != -1)
        {
            len = get_transactions (f, &t_ary);
            if (len >= candidate_size)
            {
                ht_count (t_ary, len, candidate, candidate_size, ht_count_prefix_ary);
            }
            if (t_ary != NULL && len != -1)
                free (t_ary);
        }
        fclose (f);
        free (ht_count_prefix_ary);
    }

    clock_t end = clock();
    double spent = (double) (end - begin) / CLOCKS_PER_SEC;
    printf ("Spent time: %fs\n", spent);
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
        if (t_ary != NULL && len != -1)
            free (t_ary);
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
        printf ("Isn't leaf node.\nreturn...\n");
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
    int ary[] = {0, 3};
    int ary2[] = {0, 1, 2, 3, 4};
    int prefix[3];
    int i;
    ht_node *root;
    root = create_ht_node (0, 0);
    ht_insert (ary, 2, root, false);
    ary[1] = 2;
    ht_insert (ary, 2, root, false);
    ht_count (ary2, 5, root, 2, prefix);
    ht_count (ary2, 5, root, 2, prefix);
    print_ht_tree (root);
    //for (i = 0; i < 10; i++)
    //    ht_insert (ary, 3, root, false);
    //printf ("count: %d\n", ((item_set *) ((ht_node *) root->nodes[0])->nodes[0])->count);

    //printf ("Test ht_insert2\n");
    //for (i = 0; i <= 23 * 33; i += 23)
    //{
    //    ary[0] = i;
    //    ht_insert (ary, 3, root, true);
    //}
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
                    ht_insert (item_set_ary[i]->items, item_set_ary[i]->size,
                               result_node, false);
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


void
apriori_gen (ht_node *large_item_set, ht_node *result_node)
{
    item_list *large_item_list;
    large_item_list = (item_list *) malloc (sizeof (item_list));
    memset (large_item_list, 0, sizeof (item_list));
    gen_ht_item_list (large_item_set, large_item_list);

    item_list *list_i, *list_j;
    int cmp_size;
    int *candidate;
    cmp_size = large_item_list->next_item_list->item->size - 1;
    candidate = (int *) malloc (sizeof (int) * (cmp_size + 2));
    for (list_i = large_item_list->next_item_list; list_i != NULL;
         list_i = list_i->next_item_list)
    {
        for (list_j = large_item_list->next_item_list; list_j != NULL;
             list_j = list_j->next_item_list)
        {
            if (!memcmp (list_i->item->items, list_j->item->items, cmp_size * sizeof (int)))
            { /* match */
                if (list_i->item->items[cmp_size] < list_j->item->items[cmp_size])
                { /* judge large list */
                    memcpy (candidate, list_i->item->items, sizeof (int) * cmp_size);
                    candidate[cmp_size] = list_i->item->items[cmp_size];
                    candidate[cmp_size + 1] = list_j->item->items[cmp_size];

                    if (subset_belong_L (candidate, cmp_size + 2, large_item_list))
                    {
                        ht_insert(candidate, cmp_size + 2, result_node, false);
                    }
                }
            }
        }
    }
}

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

bool
subset_belong_L (int *ary, int ary_size, item_list *large_item_list)
{
    int *data;
    item_list *list;
    list = (item_list *) malloc (sizeof (item_list));
    list->next_item_list = NULL;
    data = (int *) malloc (sizeof (int) * ary_size - 1);
    combination_and_create_list (ary, ary_size, ary_size - 1, 0, data, list);
    item_list *list_i, *list_j;
    bool match;
    for (list_i = list->next_item_list; list_i != NULL;
         list_i = list_i->next_item_list)
    {
        match = false;
        for (list_j = large_item_list->next_item_list; list_j != NULL;
             list_j = list_j->next_item_list)
        {
            if (!memcmp (list_i->item->items, list_j->item->items, sizeof (int) * (ary_size - 1)))
            {
                match = true;
                break;
            }
        }
        if (!match)
            return false;
    }
    return true;
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
        for (i = 0; i < MAX_LEAF_SIZE; i++)
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
ht_count (int *ary, int ary_size, ht_node *node, int item_size, int *prefix_ary)
{
    int hash_index;
    ht_node *childnode;
    int i;
    item_set **item_set_ary;

    if (node->len == 0) /* is interior node */
    {
        for (i = 0; i <= ary_size - item_size; i++)
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
        for (i = 0; i < node->len; i++)
        {
            if (!memcmp(item_set_ary[i]->items, prefix_ary, sizeof (int) * node->depth))
            {
                if (all_in_ary (&item_set_ary[i]->items[node->depth],
                                item_set_ary[i]->size - node->depth,
                                ary, ary_size))
                {
                    item_set_ary[i]->count++;
                }
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
    char null_str[HASH_FUNC_MOD * sizeof (int)];
    memset (null_str, 0, HASH_FUNC_MOD * sizeof (int));
    return !memcmp (null_str, node->nodes, HASH_FUNC_MOD * sizeof (int));
}
