#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "LFSortedList.h"
#include "Interlocked.h"
#include "Commons.h"

//lock free sorted list node
typedef struct lfsl_node {
  struct lfsl_node *next;  // markable pointer
  void     *val;
  uint32_t  key;
} __attribute__((aligned(sizeof(8)))) lfsl_node_t;

lfsl_node_t *lfslNodeCreate(uint32_t key_, void *val_);
void freeNode(lfsl_node_t *lfsl_node);

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

typedef struct lf_sorted_list {
  lfsl_node_t *head;
  lfsl_node_t *tail;
} lf_sorted_list_t;

static lfsl_node_t* search(lf_sorted_list_t *lst, uint32_t key,
                           lfsl_node_t **leftNode);

////////////////////////////////////////////////////////////////////////////
lfsl_node_t *lfslNodeCreate(uint32_t key_, void *val_) {
  lfsl_node_t *res = (lfsl_node_t*) malloc(sizeof(lfsl_node_t));
  res->key = key_;
  res->val = val_;
  res->next = NULL;
  return res;
}

void freeNode(lfsl_node_t *node) {
  if (node) free(node);
}
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

lf_sorted_list_t *LFSortedListCreate() {
  lf_sorted_list_t *res = (lf_sorted_list_t*) malloc(sizeof(lf_sorted_list_t));
  res->head = lfslNodeCreate(-1, NULL);
  res->tail = lfslNodeCreate(-2, NULL);
  res->head->next = res->tail;
  return res;
}

void LFSortedListFree(lf_sorted_list_t *lst) {
  if (lst) free(lst); //todo remove ALL elements
}
////////////////////////////////////////////////////////////////////////////

lfsl_node_t *search(lf_sorted_list_t *lst,
                    uint32_t key,
                    lfsl_node_t **leftNode) {
  lfsl_node_t *lNodeNext, *rNode;
search_again:
  do {
    lfsl_node_t *tmp = lst->head;
    lfsl_node_t *tmpNext = lst->head->next;

    /* 1: Find left_node and right_node */
    do {
      if (!isMarkedPtr(tmpNext)) {
        *leftNode = tmp;
        lNodeNext = tmpNext;
      }
      tmp = unmarkedPtr(tmpNext);
      if (tmp == lst->tail) break;
      tmpNext = tmp->next;
    } while (isMarkedPtr(tmpNext) || (tmp->key < key));
    rNode = tmp;

    /* 2: Check nodes are adjacent */
    if (lNodeNext == rNode) {
      if ((rNode != lst->tail) && isMarkedPtr(rNode->next)) {
        goto search_again;
      } else {
        return rNode;
      }
    }

    /* 3: Remove one or more marked nodes */
    if (CompareAndSwapX64Ptr((volatile void**)&(*leftNode)->next, (void**) &lNodeNext, rNode)) {
      if ((rNode != lst->tail) && isMarkedPtr(rNode->next))
        goto search_again;
      else
        return rNode;
    }
  } while (1);
}
////////////////////////////////////////////////////////////////////////////

uint8_t LFSortedListAdd(lf_sorted_list_t *lst,
                        uint32_t key,
                        void *val) {
  lfsl_node_t *nNode = lfslNodeCreate(key, val);
  lfsl_node_t *rNode, *lNode;
  do {
    rNode = search(lst, key, &lNode);
    if ((rNode != lst->tail) && (rNode->key == key))
      return 0;
    nNode->next = rNode;
    if (CompareAndSwapX64Ptr((volatile void**)&lNode->next, (void**) &rNode, nNode))
      return 1;
  } while (1);
}
////////////////////////////////////////////////////////////////////////////

uint8_t LFSortedListRemove(lf_sorted_list_t *lst,
                           const uint32_t key) {
  lfsl_node_t *rNode, *rNodeNext, *lNode;

  do {
    rNode = search(lst, key, &lNode);
    if ((rNode == lst->tail) || (rNode->key != key))
      return 0;
    rNodeNext = rNode->next;
    if (isMarkedPtr(rNodeNext))
      continue;
    if (CompareAndSwapX64Ptr((volatile void**)&rNode->next, (void**)&rNodeNext, markedPtr(rNodeNext)))
      break;
  } while (1);

  if (!CompareAndSwapX64Ptr((volatile void**)&lNode->next, (void**)&rNode, rNodeNext))
    rNode = search(lst, rNode->key, &lNode);
  return 1;
}
////////////////////////////////////////////////////////////////////////////

uint8_t LFSortedListFind(lf_sorted_list_t *lst,
                         const uint32_t key) {
  lfsl_node_t *rNode, *lNode;
  rNode = search(lst, key, &lNode);
  return !((rNode == lst->tail) || (rNode->key != key));
}
////////////////////////////////////////////////////////////////////////////

void LFSortedListPrint(lf_sorted_list_t *lst) {
  lfsl_node_t *tmp ;
  for (tmp = lst->head->next; tmp != lst->tail; tmp = tmp->next) {
    printf("key : %u, val : %p\n", tmp->key, tmp->val);
  }
}
////////////////////////////////////////////////////////////////////////////
