#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "hash_tree.h"
#include "linklist.h"
#include "display.h"

void
print_item_list (item_list *start)
{
    print_ary (start->item->items, start->item->size);
    if (start->next_item_list != NULL)
        print_item_list (start->next_item_list);
}

void
print_ary (int *ary, int len)
{
    int i;
    printf ("[ ");
    for (i = 0; i < len; i++)
    {
        printf ("%d ", ary[i]);
    }
    printf ("]\n");
}

void
print_tableList (table_list *tableL)
{
    int cnt = 0;
    int i;
    while (tableL != NULL)
    {
        printf ("Find table: %d\n[ ", cnt);
        for (i = 0; i < TABLE_LIST_SIZE; i++)
        {
            printf ("%d ", tableL->table[i]);
        }
        printf ("]\n");
        tableL = tableL->next_table;
        cnt++;
    }
}

void
calc_time (const char *msg)
{
    end = clock();
    spent = (double) (end - begin) / CLOCKS_PER_SEC;
    printf ("%s, spent: %fs\n", msg, spent);
}
