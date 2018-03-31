#include <time.h>
#include "hash_tree.h"
#include "linklist.h"

#ifndef DISPLAY
#define DISPLAY 

clock_t begin;
clock_t end;
double spent;
void print_ary (int *ary, int len);
void print_item_list (item_list *start);
void print_tableList (table_list *tableL);
void calc_time (const char *msg);

#endif /* ifndef DISPLAY */
