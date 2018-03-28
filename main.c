/* version: 1.1 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "hash_tree.h"
#include "linklist.h"
#include "display.h"

//#define MIN_SUP 5
//#define FILE_NAME "./T15I7N0.5KD1K.data"




ht_node * c_init (FILE *f);
void gen_largeitemset (ht_node *node, ht_node *result_node);
void apriori_gen (ht_node *large_item_set, ht_node *result_node);
int get_transactions (FILE *f, int **ary);
bool subset_belong_L (int *ary, int ary_size, item_list *large_item_list);

void test();

char *FILE_NAME;
int MIN_SUP;

int main (int argc, char *argv[])
{
    //char *a[1];
    //test();
    //gets(a);

    FILE *f;
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

    clock_t begin = clock();
    candidate = c_init (f);
    candidate_size = NUM_OF_INIT_CANDIDATE;
    int item_num = 0;

    fclose (f);

    /* start generate C2 */
    //for (candidate_size = NUM_OF_INIT_CANDIDATE + 1; !ht_is_empty (candidate) && (candidate_size <= 3); candidate_size++)
    for (candidate_size = NUM_OF_INIT_CANDIDATE + 1; !ht_is_empty (candidate); candidate_size++)
    {
        //printf ("==================\n");
        //print_ht_tree (candidate);
        //printf ("==================\n");
        //printf ("%d\n", candidate_size);
        large_item_set = create_ht_node (0, 0);
        gen_largeitemset (candidate, large_item_set);
        free (candidate);
        if (ht_is_empty (large_item_set))
            break;
        else
        {
            printf ("++++++++++++++++++\n");
            item_num += show_items_from_ht (large_item_set);
            printf ("%d\n", item_num);
            printf ("++++++++++++++++++\n");
        }
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
    int i;
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
    printf ("Test ht_insert\n");
    int ary[] = {0, 101, 103};
    int ary2[] = {0, 1, 2, 25, 40, 55, 95, 101, 103, 128};
    int prefix[3];
    int i;
    ht_node *root;
    root = create_ht_node (0, 0);
    for (i = 0; i < 5; i++)
    {
        ht_insert (ary, 3, root, false);
        ary[0] += 5;
    }
    ary[0] = 0;
    ary[1] = 2;
    for (i = 0; i < 5; i++)
    {
        ht_insert (ary, 3, root, false);
        ary[2] += 5;
    }
    ary[2] = 124;
    ht_insert (ary, 3, root, false);
    ht_count (ary2, 10, root, 3, prefix);
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

    free_item_list (large_item_list);
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








