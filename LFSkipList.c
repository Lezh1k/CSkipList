#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "LFSkipList.h"
#include "LFSortedList.h"
#include "Interlocked.h"
#include "Commons.h"

//lock free skip list node
#define MAX_LEVEL 8
typedef struct lf_skiplist_node {
  struct lf_skiplist_node *next[MAX_LEVEL];
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
  memset(res->next, 0, sizeof(lf_skiplist_node_t*) * MAX_LEVEL);
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
  lf_skiplist_node_t *head;
  lf_skiplist_node_t *tail;
} lf_skip_list_t;

static lf_skiplist_node_t *searchOnLevel(lf_skip_list_t *lst,
                                         int32_t level,
                                         lf_skiplist_node_t *startNode,
                                         uint32_t key,
                                         lf_skiplist_node_t **lNode);

lf_skip_list_t* LFSkipListCreate() {
  register int32_t ml = MAX_LEVEL;
  lf_skip_list_t *lst = (lf_skip_list_t*) malloc(sizeof(lf_skip_list_t));
  if (lst == NULL)
    return NULL;
  lst->head = lfSkipListNodeCreate(-1, NULL);
  if (lst->head == NULL) {
    free(lst);
    return NULL;
  }

  lst->tail = lfSkipListNodeCreate(-2, NULL);
  if (lst->tail == NULL) {
    free(lst->head);
    free(lst->tail);
    return NULL;
  }

  while(ml--)
    lst->head->next[ml] = lst->tail;
  return lst;
}
////////////////////////////////////////////////////////////////////////////

void LFSkipListFree(lf_skip_list_t *lst) {
  if (lst)
    free(lst);
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
    lf_skiplist_node_t *tmpNext = startNode->next[level];

    /* 1: Find left_node and right_node */
    do {
      if (!IsMarkedPtr(tmpNext)) {
        *lNode = tmp;
        lNodeNext = tmpNext;
      }
      tmp = UnmarkedPtr(tmpNext);
      if (tmp == lst->tail) break;
      tmpNext = tmp->next[level];
    } while (IsMarkedPtr(tmpNext) || (tmp->key < key));
    rNode = tmp;

    /* 2: Check nodes are adjacent */
    if (lNodeNext == rNode) {
      if ((rNode != lst->tail) && IsMarkedPtr(rNode->next[level])) { //already removed by another thread?
        continue; //search again
      } else {
        return rNode;
      }
    }

    /* 3: Remove one or more marked nodes */
    if (CASPtr((volatile void**)&(*lNode)->next[level], (void**) &lNodeNext, rNode)) {
      if ((rNode != lst->tail) && IsMarkedPtr(rNode->next[level])) { //already removed by another thread?
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
  int32_t cl, lvl ;
  lf_skiplist_node_t *lNode, *rNode, *nNode, *startNode;
  lf_skiplist_node_t **prev = (lf_skiplist_node_t**) malloc(MAX_LEVEL*sizeof(lf_skiplist_node_t*));
  if (prev == NULL)
    return LFE_FAILED;

  nNode = lfSkipListNodeCreate(key, val);
  lvl = GenRandomBase2(MAX_LEVEL);

  do {
    startNode = lst->head;
    for (cl = MAX_LEVEL; cl--;) {
      rNode = searchOnLevel(lst, cl, startNode, key, &lNode);
      if ((rNode != lst->tail) && (rNode->key == key)) {
        free(nNode);
        free(prev);
        return LFE_ALREADY_DONE;
      }
      prev[cl] = lNode;
      nNode->next[cl] = rNode;
      startNode = lNode;
    }
    assert(cl == -1);

    if (!CASPtr((volatile void**)&lNode->next[0], (void**) &rNode, nNode))
      continue;

    //inserted on 0 level

    for (cl = 1; cl <= lvl; ++cl) {
      if (CASPtr((volatile void**)&prev[cl]->next[cl],
                 (void**) &nNode->next[cl], nNode)) {
        continue;
      }

      do {
        rNode = searchOnLevel(lst, cl, lst->head, key, &lNode);
        nNode->next[cl] = rNode;
        if (CASPtr((volatile void**)&lNode->next[cl], (void**) &rNode, nNode)) {
          break;
        }
      } while(1);
    } //for cl <= lvl

    free(prev);
    return LFE_SUCCESS;

  } while (1);
}
////////////////////////////////////////////////////////////////////////////

uint8_t LFSkipListRemove(lf_skip_list_t *lst,
                         uint32_t key) {
  UNUSED_ARG(lst);
  UNUSED_ARG(key);
  return LFE_FAILED;
}
////////////////////////////////////////////////////////////////////////////

uint8_t LFSkipListFind(lf_skip_list_t *lst,
                       uint32_t key) {
  return 0;
}
////////////////////////////////////////////////////////////////////////////

void LFSkipListPrint(lf_skip_list_t *lst) {
  lf_skiplist_node_t *tmp;
  int32_t i;

  for (tmp = lst->head->next[0]; tmp != lst->tail; tmp = tmp->next[0]) {
    printf("key : %d\n", tmp->key);
    for (i = 0; i < MAX_LEVEL; ++i) {
      printf("%d\t", tmp->next[i]->key);
    }
    printf("\n************\n");
  }
}
