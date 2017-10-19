#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "LFSkipList.h"
#include "LFSortedList.h"
#include "Interlocked.h"

//lock free skip list node
typedef struct lfss_node {
  struct lfss_node *next;
  struct lfss_node *nextLevel;
  void *data;
  uint32_t key;
} __attribute__((aligned(sizeof(8)))) lfss_node_t;

lfss_node_t *lfssNodeCreate(uint32_t key, void *data);
void lfssNodeFree(lfss_node_t *node);

lfss_node_t *lfssNodeCreate(uint32_t key, void *data) {
  lfss_node_t *res = (lfss_node_t*) malloc(sizeof(lfss_node_t));
  if (res == NULL) return NULL;
  res->next = NULL;
  res->nextLevel = NULL;
  res->data = data;
  res->key = key;
  return res;
}
////////////////////////////////////////////////////////////////////////////

void lfssNodeFree(lfss_node_t *node) {
  if (node) free(node);
}
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

typedef struct lf_skip_list {
  lfss_node_t **lstHead;
  lfss_node_t **lstTail;
  int32_t maxLevel;
} lf_skip_list_t;

static lfss_node_t *search(lf_sorted_list_t *lst, uint32_t key,
                           lfss_node_t **leftNode);


lf_skip_list_t* LFSkipListCreate(int32_t maxLevel) {
  assert(maxLevel);

  lf_skip_list_t *res = (lf_skip_list_t*) malloc(sizeof(lf_skip_list_t));
  if (res == NULL)
    return NULL;

  res->maxLevel = maxLevel;
  if (!(res->lstHead = (lfss_node_t**) malloc(sizeof(lfss_node_t*) * maxLevel)))
    return NULL;
  memset(res->lstHead, 0, sizeof(lfss_node_t*) * maxLevel);

  if (!(res->lstTail = (lfss_node_t**) malloc(sizeof(lfss_node_t*) * maxLevel))) {
    free(res->lstHead);
    return NULL;
  }
  memset(res->lstTail, 0, sizeof(lfss_node_t*) * maxLevel);


  while (--maxLevel >= 0) {
    res->lstHead[maxLevel] = lfssNodeCreate(-1, NULL);
    res->lstTail[maxLevel] = lfssNodeCreate(-2, NULL);

    if (!(res->lstHead[maxLevel] && res->lstTail[maxLevel]))
      break;

    res->lstHead[maxLevel]->next = res->lstTail[maxLevel];
    res->lstHead[maxLevel]->nextLevel = maxLevel ? res->lstHead[maxLevel-1] : NULL;
    res->lstTail[maxLevel]->nextLevel = maxLevel ? res->lstTail[maxLevel-1] : NULL;
  }

  if (maxLevel) { //we failed to create head or tail array.
    for (--maxLevel; maxLevel < res->maxLevel; ++maxLevel) { //free  allocated memory at all
      if (res->lstHead[maxLevel])
        free(res->lstHead[maxLevel]);
      if (res->lstTail[maxLevel])
        free(res->lstTail[maxLevel]);
    }
    free(res->lstHead);
    free(res->lstTail);
    return NULL;
  }

  return res;
}
////////////////////////////////////////////////////////////////////////////

void LFSkipListFree(lf_skip_list_t *lst) {
  //todo free memory
}
////////////////////////////////////////////////////////////////////////////

lfss_node_t* search(lf_sorted_list_t *lst, uint32_t key,
                    lfss_node_t **leftNode) {
  return NULL;
}
////////////////////////////////////////////////////////////////////////////

uint8_t LFSkipListAdd(lf_skip_list_t *lst,
                      uint32_t key,
                      void *val) {
  return 0;
}
////////////////////////////////////////////////////////////////////////////

uint8_t LFSkipListRemove(lf_skip_list_t *lst,
                         uint32_t key) {
  return 0;
}
////////////////////////////////////////////////////////////////////////////

uint8_t LFSkipListFind(lf_skip_list_t *lst,
                       uint32_t key) {
  return 0;
}
////////////////////////////////////////////////////////////////////////////

void LFSkipListPrint(lf_skip_list_t *lst) {
  return;
}
