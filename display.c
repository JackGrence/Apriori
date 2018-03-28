#include <stdio.h>
#include <stdlib.h>
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
