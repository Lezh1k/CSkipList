#ifndef LFSKIPLIST_H
#define LFSKIPLIST_H

#include <stdint.h>

typedef struct lf_skip_list lf_skip_list_t;

lf_skip_list_t* LFSkipListCreate();
void LFSkipListFree(lf_skip_list_t *lst);

uint8_t LFSkipListAdd(lf_skip_list_t *lst, uint32_t key, void *val);
uint8_t LFSkipListRemove(lf_skip_list_t *lst, uint32_t key);
uint8_t LFSkipListFind(lf_skip_list_t *lst, uint32_t key);

void LFSkipListPrint(lf_skip_list_t *lst);

#endif // LFSKIPLIST_H
