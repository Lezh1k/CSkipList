#ifndef LFSORTEDLIST_H
#define LFSORTEDLIST_H

#include <stdint.h>

typedef struct lf_sorted_list lf_sorted_list_t;

lf_sorted_list_t *LFSortedListCreate();
void LFSortedListFree(lf_sorted_list_t* lst);

uint8_t LFSortedListAdd(lf_sorted_list_t *lst, uint32_t key, void *val);
uint8_t LFSortedListRemove(lf_sorted_list_t *lst, const uint32_t key);
uint8_t LFSortedListFind(lf_sorted_list_t *lst, const uint32_t key);

void LFSortedListPrint(lf_sorted_list_t *lst);

#endif // LFSORTEDLIST_H
