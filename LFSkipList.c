#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "LFSkipList.h"
#include "LFSortedList.h"
#include "Interlocked.h"
#include "Commons.h"

//lock free skip list node
typedef struct lf_skiplist_node {
  struct lf_skiplist_node *next;
  struct lf_skiplist_node *nextLevel;
  void *data;
  uint32_t key;
} __attribute__((aligned(sizeof(8)))) lf_skiplist_node_t;

static lf_skiplist_node_t *lfSkipListNodeCreate(uint32_t key,
                                          void *data);
static void lfSkipListNodeFree(lf_skiplist_node_t *node) __attribute_used__;

lf_skiplist_node_t *lfSkipListNodeCreate(uint32_t key,
                                   void *data) {
  lf_skiplist_node_t *res = (lf_skiplist_node_t*) malloc(sizeof(lf_skiplist_node_t));
  if (res == NULL) return NULL;
  res->next = NULL;
  res->nextLevel = NULL;
  res->data = data;
  res->key = key;
  return res;
}
////////////////////////////////////////////////////////////////////////////

void lfSkipListNodeFree(lf_skiplist_node_t *node) {
  UNUSED_ARG(node);
  if (node) free(node);
}
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

typedef struct lf_skip_list {
  lf_skiplist_node_t **lstHead;
  lf_skiplist_node_t **lstTail;
  int32_t maxLevel;
} lf_skip_list_t;

static lf_skiplist_node_t *searchOnLevel(lf_skip_list_t *lst, int32_t level, lf_skiplist_node_t *startNode, uint32_t key,
                                         lf_skiplist_node_t **lNode);

lf_skip_list_t* LFSkipListCreate(int32_t maxLevel) {
  assert(maxLevel);

  lf_skip_list_t *res = (lf_skip_list_t*) malloc(sizeof(lf_skip_list_t));
  if (res == NULL)
    return NULL;

  res->maxLevel = maxLevel;
  if (!(res->lstHead = (lf_skiplist_node_t**) malloc(sizeof(lf_skiplist_node_t*) * maxLevel)))
    return NULL;
  memset(res->lstHead, 0, sizeof(lf_skiplist_node_t*) * maxLevel);

  if (!(res->lstTail = (lf_skiplist_node_t**) malloc(sizeof(lf_skiplist_node_t*) * maxLevel))) {
    free(res->lstHead);
    return NULL;
  }
  memset(res->lstTail, 0, sizeof(lf_skiplist_node_t*) * maxLevel);

  --maxLevel;
  while (maxLevel--) {
    res->lstHead[maxLevel] = lfSkipListNodeCreate(-1, NULL);
    res->lstTail[maxLevel] = lfSkipListNodeCreate(-2, NULL);

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

lf_skiplist_node_t* searchOnLevel(lf_skip_list_t *lst,
                                  int32_t level,
                                  lf_skiplist_node_t *startNode,
                                  uint32_t key,
                                  lf_skiplist_node_t **lNode) {
  lf_skiplist_node_t *lNodeNext, *rNode;
  do {
    lf_skiplist_node_t *tmp = startNode;
    lf_skiplist_node_t *tmpNext = startNode->next;

    /* 1: Find left_node and right_node */
    do {
      if (!isMarkedPtr(tmpNext)) {
        *lNode = tmp;
        lNodeNext = tmpNext;
      }
      tmp = unmarkedPtr(tmpNext);
      if (tmp == lst->lstTail[level]) break;
      tmpNext = tmp->next;
    } while (isMarkedPtr(tmpNext) || (tmp->key < key));
    rNode = tmp;

    /* 2: Check nodes are adjacent */
    if (lNodeNext == rNode) {
      if ((rNode != lst->lstTail[level]) && isMarkedPtr(rNode->next)) { //already removed by another thread?
        continue; //search again
      } else {
        return rNode;
      }
    }

    /* 3: Remove one or more marked nodes */
    if (CompareAndSwapX64Ptr((volatile void**)&(*lNode)->next, (void**) &lNodeNext, rNode)) {
      if ((rNode != lst->lstTail[level]) && isMarkedPtr(rNode->next)) { //already removed by another thread?
        continue; //search again
      } else {
        return rNode;
      }
    }
  } while (1);
}
////////////////////////////////////////////////////////////////////////////

uint8_t LFSkipListAdd(lf_skip_list_t *lst,
                      uint32_t key,
                      void *val) {
  int32_t cl ;
  lf_skiplist_node_t **prev = (lf_skiplist_node_t**) malloc(lst->maxLevel*sizeof(lf_skiplist_node_t*));
  lf_skiplist_node_t *lNode, *rNode, *nNode, *startNode;
  if (prev == NULL)
    return LFE_FAILED;

  nNode = lfSkipListNodeCreate(key, val);

  do {
    cl = lst->maxLevel;
    startNode = lst->lstHead[cl-1];

    while(cl--) {
      rNode = searchOnLevel(lst, cl, startNode, key, &lNode);
      if ((rNode != lst->lstTail[cl]) && (rNode->key == key)) {
        free(nNode);
        free(prev);
        return LFE_ALREADY_DONE;
      }
      prev[cl] = lNode;
      startNode = lNode->nextLevel;
    }

    nNode->next = rNode;
    if (CompareAndSwapX64Ptr((volatile void**)&lNode->next, (void**) &rNode, nNode)) {
      free(prev);
      return LFE_SUCCESS;
    }
  } while (1);

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
