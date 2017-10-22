#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "LFSortedList.h"
#include "Interlocked.h"
#include "Commons.h"

//lock free sorted list node
typedef struct lf_sortedlist_node {
  struct lf_sortedlist_node *next;  // markable pointer
  void     *val;
  uint32_t  key;
} __attribute__((aligned(sizeof(8)))) lf_sortedlist_node_t;

static lf_sortedlist_node_t *lfSortedListNodeCreate(uint32_t key_, void *val_);
static void lfSortedListNodeFree(lf_sortedlist_node_t *lf_sortedlist_node);

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

typedef struct lf_sorted_list {
  lf_sortedlist_node_t *head;
  lf_sortedlist_node_t *tail;
} lf_sorted_list_t;

static lf_sortedlist_node_t* search(lf_sorted_list_t *lst, uint32_t key,
                                    lf_sortedlist_node_t **lNode);

////////////////////////////////////////////////////////////////////////////
lf_sortedlist_node_t *lfSortedListNodeCreate(uint32_t key_, void *val_) {
  lf_sortedlist_node_t *res = (lf_sortedlist_node_t*) malloc(sizeof(lf_sortedlist_node_t));
  res->key = key_;
  res->val = val_;
  res->next = NULL;
  return res;
}

void lfSortedListNodeFree(lf_sortedlist_node_t *node) {
  if (node) free(node);
}
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

lf_sorted_list_t *LFSortedListCreate() {
  lf_sorted_list_t *res = (lf_sorted_list_t*) malloc(sizeof(lf_sorted_list_t));
  res->head = lfSortedListNodeCreate(-1, NULL);
  res->tail = lfSortedListNodeCreate(-2, NULL);
  res->head->next = res->tail;
  return res;
}

void LFSortedListFree(lf_sorted_list_t *lst) {
  if (lst) free(lst); //todo remove ALL elements
}
////////////////////////////////////////////////////////////////////////////

static lf_sortedlist_node_t *search(lf_sorted_list_t *lst,
                                    uint32_t key,
                                    lf_sortedlist_node_t **lNode) {
  lf_sortedlist_node_t *lNodeNext, *rNode;
  do {
    lf_sortedlist_node_t *tmp = lst->head;
    lf_sortedlist_node_t *tmpNext = lst->head->next;

    /* 1: Find left_node and right_node */
    do {
      if (!isMarkedPtr(tmpNext)) {
        *lNode = tmp;
        lNodeNext = tmpNext;
      }
      tmp = unmarkedPtr(tmpNext);
      if (tmp == lst->tail) break;
      tmpNext = tmp->next;
    } while (isMarkedPtr(tmpNext) || (tmp->key < key));
    rNode = tmp;

    /* 2: Check nodes are adjacent */
    if (lNodeNext == rNode) {
      if ((rNode != lst->tail) && isMarkedPtr(rNode->next)) { //already removed by another thread?
        continue; //search again
      } else {
        return rNode;
      }
    }

    /* 3: Remove one or more marked nodes */
    if (CASX64Ptr((volatile void**)&(*lNode)->next, (void**) &lNodeNext, rNode)) {
      if ((rNode != lst->tail) && isMarkedPtr(rNode->next)) { //already removed by another thread?
        continue; //search again
      } else {
        return rNode;
      }
    }
  } while (1);
}
////////////////////////////////////////////////////////////////////////////

uint8_t LFSortedListAdd(lf_sorted_list_t *lst,
                        uint32_t key,
                        void *val) {
  lf_sortedlist_node_t *nNode = lfSortedListNodeCreate(key, val);
  lf_sortedlist_node_t *rNode, *lNode;
  do {
    rNode = search(lst, key, &lNode);
    if ((rNode != lst->tail) && (rNode->key == key)) {
      free(nNode);
      return LFE_ALREADY_DONE;
    }
    nNode->next = rNode;
    if (CASX64Ptr((volatile void**)&lNode->next, (void**) &rNode, nNode))
      return LFE_SUCCESS;
  } while (1);
}
////////////////////////////////////////////////////////////////////////////

uint8_t LFSortedListRemove(lf_sorted_list_t *lst,
                           const uint32_t key) {
  lf_sortedlist_node_t *rNode, *rNodeNext, *lNode;

  do {
    rNode = search(lst, key, &lNode);
    if ((rNode == lst->tail) || (rNode->key != key))
      return LFE_FAILED;
    rNodeNext = rNode->next;
    if (isMarkedPtr(rNodeNext))
      continue;
    if (CASX64Ptr((volatile void**)&rNode->next, (void**)&rNodeNext, markedPtr(rNodeNext)))
      break;
  } while (1);

  if (!CASX64Ptr((volatile void**)&lNode->next, (void**)&rNode, rNodeNext))
    rNode = search(lst, rNode->key, &lNode);
  return LFE_SUCCESS;
}
////////////////////////////////////////////////////////////////////////////

uint8_t LFSortedListFind(lf_sorted_list_t *lst,
                         const uint32_t key) {
  lf_sortedlist_node_t *rNode, *lNode;
  rNode = search(lst, key, &lNode);
  return !((rNode == lst->tail) || (rNode->key != key));
}
////////////////////////////////////////////////////////////////////////////

void LFSortedListPrint(lf_sorted_list_t *lst) {
  lf_sortedlist_node_t *tmp ;
  for (tmp = lst->head->next; tmp != lst->tail; tmp = tmp->next) {
    printf("key : %u, val : %p\n", tmp->key, tmp->val);
  }
}
////////////////////////////////////////////////////////////////////////////
