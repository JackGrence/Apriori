#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>
#include "version.h"
#include "hash_tree.h"
#include "linklist.h"
#include "display.h"

//#define MIN_SUP 5
//#define FILE_NAME "./T15I7N0.5KD1K.data"

bool subset_belong_L (int *ary, int ary_size, item_list *large_item_list);
bool guess_C_isCorrect (int *ary, int ary_size, ht_node *node, int item_size, int *prefix_ary);
ht_node * c_init (FILE *f);
int gen_L1_and_C2(FILE *f, ht_node **C2);
int get_transactions (FILE *f, int **ary);
void apriori_gen (ht_node *L_copy, ht_node *large_item_set, int item_size, ht_node *result_node);
void L_combination (ht_node *large_item_set, ht_node *region_node, int item_size, ht_node *result_node);
void gen_largeitemset (ht_node *node, int item_size, ht_node *result_node);
void test();
void count_C (ht_node *C, int C_itemSize);

static char *FILE_NAME;
static int MIN_SUP;
static int item_num;

int main (int argc, char *argv[])
{
    //FILE_NAME = argv[1];
    //MIN_SUP = atoi (argv[2]);
    //test();
    //char *a[1];
    //gets(a);

    FILE *f;
    begin = clock();
    int candidate_size;
    ht_node *large_item_set;
    ht_node *candidate;

    FILE_NAME = argv[1];
    MIN_SUP = atoi (argv[2]);

    printf ("start initialize c\n");
    f = fopen (FILE_NAME, "rb");
    if (f == NULL)
    {
        printf ("Open file error.\nexit...\n");
        exit(1);
    }

    item_num = gen_L1_and_C2 (f, &candidate);
    printf ("L1 size: %d\n", item_num);
    candidate_size = 2;

    fclose (f);

    calc_time ("Start count C2");
    /* start count C2 */
    count_C (candidate, candidate_size);

    //for (candidate_size = NUM_OF_INIT_CANDIDATE + 1; !ht_is_empty (candidate) && (candidate_size <= 3); candidate_size++)
    for (candidate_size = candidate_size + 1; !ht_is_empty (candidate); candidate_size++)
    {
        //if (candidate_size > 0)
        //{
        //    printf ("==================\n");
        //    print_ht_tree (candidate);
        //    printf ("==================\n");
        //    printf ("%d\n", candidate_size);
        //}


        calc_time ("Start generate L");

        large_item_set = create_ht_node (0, HASH_FUNC_MOD);
        gen_largeitemset (candidate, candidate_size - 1, large_item_set);
        free (candidate);
        if (ht_is_empty (large_item_set))
            break;
        else
        {
            printf ("++++++++++++++++++\n");
            item_num += show_items_from_ht (large_item_set, candidate_size - 1);
            printf ("%d\n", item_num);
            printf ("++++++++++++++++++\n");
        }

        printf("%d ", candidate_size);
        calc_time ("Start apriori generation");

        candidate = create_ht_node (0, HASH_FUNC_MOD);
        apriori_gen (large_item_set, large_item_set, candidate_size - 1, candidate);

        calc_time ("Start count_C");

        count_C (candidate, candidate_size);

        calc_time ("End loop cycle");
    }

    calc_time ("Total spent time");
    printf ("frq: %d\n", item_num);
    return 0;
}


int
get_transactions (FILE *f, int **ary)
{
    long long int tid;
    int len;
    int read_size;
    if ((read_size = fread (&tid, sizeof (tid), 1, f)) == 0)    // read tid
    {
        if (feof (f))
        {
            printf ("End read.\n");
            return -1;
        }
    }
    fread (&len, sizeof (len), 1, f);             // read len
    *ary = (int *) malloc (len * sizeof (int));
    read_size = fread (*ary, sizeof (int), len, f);
    if (len != read_size)
    {
        printf ("read error\n");
        exit (1);
    }
    //printf ("%d\n", len);
    //print_ary (*ary, len);
    return len;
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
            //for (i = 0; i < len; i++)
            //{
            //    ht_insert (&t_ary[i], 1, root, true);
            //}
        }
        if (t_ary != NULL && len != -1)
            free (t_ary);
    }
    return root;
}

void
test ()
{
    //FILE *f;
    //ht_node *root;
    //root = create_ht_node (0, 0);
    //f = fopen (FILE_NAME, "rb");
    //if (f == NULL)
    //{
    //    printf ("Open file error.\nexit...\n%s\n", strerror (errno));
    //    exit(1);
    //}
    //gen_L1_and_C2 (f, root);
    //fclose (f);
    //printf ("Test ht_insert\n");
    HASH_FUNC_MOD = 10;
    int ary[] = {0, 1, 3};
    ht_node *root;
    root = create_ht_node (0, HASH_FUNC_MOD);
    ht_insert (ary, 3, root, false);
    print_ht_tree (root, 3);
    //item_set *item;
    //item = create_item_set (ary, 3);
    //leaf = create_leaf_node (item, 3);
    //int i;
    //for (i = 0; i < 10; i++)
    //{
    //    ary[2] += 5;
    //    item = create_item_set (ary, 3);
    //    append_item_set (item, leaf);
    //}
    //ary[1] += 5;
    //for (i = 0; i < 10; i++)
    //{
    //    ary[2] += 5;
    //    item = create_item_set (ary, 3);
    //    append_item_set (item, leaf);
    //}
    //int prefix[3];
    //int i;
    //ht_node *root;
    //root = create_ht_node (0, 0);
    //for (i = 0; i < 5; i++)
    //{
    //    ht_insert (ary, 3, root, false);
    //    ary[0] += 5;
    //}
    //ary[0] = 0;
    //ary[1] = 2;
    //for (i = 0; i < 5; i++)
    //{
    //    ht_insert (ary, 3, root, false);
    //    ary[2] += 5;
    //}
    //ary[2] = 124;
    //ht_insert (ary, 3, root, false);
    //ht_count (ary2, 10, root, 3, prefix);
    //print_ht_tree (root);
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
gen_largeitemset (ht_node *node, int item_size, ht_node *result_node)
{
    int i;
    ht_node **nodes_ary;
    item_set **item_set_ary;
    if (node->depth != item_size - 1)
    {
        nodes_ary = (ht_node **) node->nodes;
        for (i = 0; i < node->len; i++)
        {
            if (nodes_ary[i] != NULL)
            {
                gen_largeitemset (nodes_ary[i], item_size, result_node);
                free (nodes_ary[i]);
            }
        }
    }
    else
    {
        item_set_ary = (item_set **) node->nodes;
        for (i = 0; i < node->len; i++)
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
            printf ("impossible\n");
            gen_largeitemset (node->next_leaf, item_size, result_node);
            free (node->next_leaf);
        }
    }
    free (node->nodes);
}


bool
guess_C_isCorrect (int *ary, int ary_size, ht_node *node, int item_size, int *prefix_ary)
{
    ht_node *childnode;
    int hash_index;
    int i;
    int item_index;
    int prefix_size;
    int tableL_val;
    int match_cnt;
    item_set **item_set_ary;

    prefix_size = node->depth;
    table_list *remain_aryTableL;

    if (node->len == 0) /* is interior node */
    {
        for (i = 0; i <= ary_size - item_size + prefix_size; i++)
        {
            prefix_ary[prefix_size] = ary[i];
            hash_index = ary[i];
            childnode = (ht_node *) node->nodes[hash_index];
            if (childnode != NULL)
            {
                return guess_C_isCorrect (&ary[i + 1], ary_size - i - 1, childnode, item_size, prefix_ary);
            }
        }
    }
    else  /* is leaf node */
    {
        /* search item */
        remain_aryTableL = create_tableList();
        for (i = 0; i < ary_size; i++)
            tableL_insert (ary[i], remain_aryTableL, true);

        match_cnt = 0;
        while (node != NULL)
        {
            item_set_ary = (item_set **) node->nodes;
            for (i = 0; i < node->len; i++)
            {
                if (item_set_ary[i] == NULL)
                    continue;
                if (!memcmp(item_set_ary[i]->items, prefix_ary, sizeof (int) * prefix_size))
                {
                    if (item_size == prefix_size) /* prefix size == item_size */
                    {
                        match_cnt++;
                        break;
                    }
                    else
                    {
                        for (item_index = prefix_size; item_index < item_size; item_index++)
                        {
                            tableL_val = get_tableL_val (item_set_ary[i]->items[item_index], remain_aryTableL);
                            if (tableL_val == 0)
                                break;
                        }
                        match_cnt += tableL_val;
                    }
                }
            }
            node = node->next_leaf;
        }
        free_tableList (remain_aryTableL);
        if (match_cnt != item_size - prefix_size + 1)
            return false;
    }
    return true;
}


void
L_combination (ht_node *large_item_set, ht_node *region_node, int item_size, ht_node *result_node)
{ /* C will not insert to tree with sorted order */
    int cmp_size;
    int scan_ind;
    int node_ind;
    int *guess_C;
    int *prefix_ary;
    ht_node **nodes_ary;
    item_set **item_set_ary;
    guess_C = (int *) malloc (sizeof (int) * (item_size + 1));
    prefix_ary = (int *) malloc (sizeof (int) * item_size);

    cmp_size = item_size - 1;

    item_set_ary = (item_set **) region_node->nodes;
    for (node_ind = 0; node_ind < region_node->len; node_ind++)
    {
        if (item_set_ary[node_ind] != NULL)
        {
            memcpy (guess_C, item_set_ary[node_ind]->items, sizeof (int) * item_size);
            for (scan_ind = node_ind + 1; scan_ind < region_node->len; scan_ind++)
            {
                if (item_set_ary[scan_ind] != NULL)
                {
                    guess_C[item_size] = item_set_ary[scan_ind]->items[item_size - 1];
                    ht_insert (guess_C, item_size + 1, result_node, false);
                }
            }
            free (item_set_ary[node_ind]);
        }
    }
    free (item_set_ary);
    free (prefix_ary);
    free (guess_C);
}


void
apriori_gen (ht_node *L_copy, ht_node *large_item_set, int item_size, ht_node *result_node)
{
    int node_ind;
    ht_node **nodes_ary;
    if (large_item_set->depth == item_size - 1)
    {
        L_combination (L_copy, large_item_set, item_size, result_node);
    }
    else
    {
        nodes_ary = (ht_node **) large_item_set->nodes;
        for (node_ind = 0; node_ind < large_item_set->len; node_ind++)
        {
            if (nodes_ary[node_ind] != NULL)
            {
                apriori_gen (L_copy, nodes_ary[node_ind], item_size, result_node);
                free (nodes_ary[node_ind]);
            }
        }
        free (nodes_ary);
    }
}


bool
subset_belong_L (int *ary, int ary_size, item_list *large_item_list)
{
    int *data;
    item_list *list;
    list = (item_list *) malloc (sizeof (item_list));
    list->item = NULL;
    list->next_item_list = NULL;
    data = (int *) malloc (sizeof (int) * (ary_size - 1));
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
            break;
    }
    free_item_list_and_item (list);
    free (data);
    return match;
}

int
gen_L1_and_C2(FILE *f, ht_node **C2)
{
    /* return L1 size */
    int *t_ary;
    int i;
    int len = 0;
    int cnt;
    table_list *C1;
    table_list *L1;
    table_list *cur_table;
    int L_ind;
    int C_item[2];
    int ind;
    int c2_num;
    int max_item = 0;

    /* generate C1 */
    C1 = create_tableList();
    while (len != -1)
    {
        len = get_transactions (f, &t_ary);
        for (i = 0; i < len; i++)
        {
            tableL_insert (t_ary[i], C1, true);
            if (max_item < t_ary[i])
                max_item = t_ary[i];
        }
        if (t_ary != NULL && len != -1)
            free (t_ary);
    }

    /* generate L1 */
    L1 = C1;
    cur_table = L1;
    len = 0;
    cnt = 0;
    ind = 0;
    while (L1 != NULL)
    {
        for (i = 0; i < TABLE_LIST_SIZE; i++)
        {
            if (L1->table[i] >= MIN_SUP)
            {
                cur_table->table[cnt] = ind;
                len++;
                cnt++;
                if (cnt >= TABLE_LIST_SIZE)
                {
                    cnt = 0;
                    cur_table = cur_table->next_table;
                }
            }
            ind++;
        }
        L1 = L1->next_table;
    }

    HASH_FUNC_MOD = max_item + 1;
    MAX_LEAF_SIZE = 1;
    printf ("Find HASH_FUNC_MOD: %d\n", HASH_FUNC_MOD);
    printf ("Find MAX_LEAF_SIZE: %d\n", MAX_LEAF_SIZE);
    *C2 = create_ht_node (0, HASH_FUNC_MOD);

    calc_time ("Start generate C2");
    /* generate C2 */
    c2_num = 0;
    L1 = C1;
    for (L_ind = 0; L_ind < len - 1; L_ind++)
    {
        C_item[0] = get_tableL_val (L_ind, L1);
        for (i = L_ind + 1; i < len; i++)
        {
            C_item[1] = get_tableL_val (i, L1);
            ht_insert (C_item, 2, *C2, false);
            c2_num++;
        }
    }
    printf ("C2 num: %d\n", c2_num);
    return len;
}

void
count_C (ht_node *C, int C_itemSize)
{
    FILE *f;
    int *prefix_ary;
    int len;
    int *t_ary;
    f = fopen (FILE_NAME, "rb");
    if (f == NULL)
    {
        printf ("Open file error.\nexit...\n");
        exit(1);
    }
    len = 0;
    prefix_ary = (int *) malloc (sizeof (int) * C_itemSize);
    while (len != -1)
    {
        len = get_transactions (f, &t_ary);
        if (len >= C_itemSize)
        {
            ht_count (t_ary, len, C, C_itemSize, prefix_ary);
        }
        if (t_ary != NULL && len != -1)
            free (t_ary);
    }
    fclose (f);
    free (prefix_ary);
}
