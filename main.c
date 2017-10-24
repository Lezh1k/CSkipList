#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

#include "LFSortedList.h"
#include "LFSkipList.h"
////////////////////////////////////////////////////////////////////////////

#define STEP 10
#define SLEEP 100

typedef struct sort_list_test_arg {
  lf_sorted_list_t *lst;
  int32_t start, end;
} sort_list_test_arg_t;

void* sortListAddTest(void* t);
void* sortListRemoveTest(void* t);

void* sortListAddTest(void* t) {
  sort_list_test_arg_t *arg =  (sort_list_test_arg_t*)t;
  while(++arg->start < arg->end) {
    LFSortedListAdd(arg->lst, arg->start, NULL);
    usleep(SLEEP);
  }
  return NULL;
}

void* sortListRemoveTest(void* t) {
  sort_list_test_arg_t *arg =  (sort_list_test_arg_t*)t;
  while(++arg->start < arg->end) {
    LFSortedListRemove(arg->lst, arg->start);
    usleep(SLEEP);
  }
  return NULL;
}
////////////////////////////////////////////////////////////////////////////

void testSortedList() {
  pthread_t thAdd[3];
  pthread_attr_t thAttrAdd[3];
  sort_list_test_arg_t thAddArg[3];

  pthread_t thRemove[3];
  pthread_attr_t thAttrRemove[3];
  sort_list_test_arg_t thRemoveArg[3];

  int32_t i;
  lf_sorted_list_t *lstSorted = LFSortedListCreate();
  if (lstSorted == NULL) {
    printf("Couldn't create LFsortList\n");
    return;
  }

  /*test add*/
  for (i = 0; i < 3; ++i) {
    thAddArg[i].start = STEP * i;
    thAddArg[i].end = STEP * (i + 1);
    thAddArg[i].lst = lstSorted;

    pthread_attr_init(&thAttrAdd[i]);
    pthread_create(&thAdd[i], &thAttrAdd[i], sortListAddTest, &thAddArg[i]);
  }

  for (i = 0; i < 3; ++i) {
    pthread_join(thAdd[i], NULL);
  }
  LFSortedListPrint(lstSorted);
  printf("\n*************************************************************\n");

  /*test remove*/

  for (i = 0; i < 3; ++i) {
    thRemoveArg[i].start = STEP * i;
    thRemoveArg[i].end = STEP * (i + 1);
    thRemoveArg[i].lst = lstSorted;

    pthread_attr_init(&thAttrRemove[i]);
    pthread_create(&thRemove[i], &thAttrRemove[i], sortListRemoveTest, &thRemoveArg[i]);
  }

  for (i = 0; i < 3; ++i) {
    pthread_join(thRemove[i], NULL);
  }
  LFSortedListPrint(lstSorted);
  printf("\n*************************************************************\n");
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


typedef struct skip_list_test_arg {
  lf_skip_list_t *lst;
  int32_t start, end;
} skip_list_test_arg_t;

void* skipListAddTest(void* t);
void* skipListRemoveTest(void* t);

void* skipListAddTest(void* t) {
  skip_list_test_arg_t *arg =  (skip_list_test_arg_t*)t;
  while(++arg->start < arg->end) {
    LFSkipListAdd(arg->lst, arg->start, NULL);
    usleep(SLEEP);
  }
  return NULL;
}

void* skipListRemoveTest(void* t) {
  skip_list_test_arg_t *arg =  (skip_list_test_arg_t*)t;
  while(++arg->start < arg->end) {
    LFSkipListRemove(arg->lst, arg->start);
    usleep(SLEEP);
  }
  return NULL;
}
////////////////////////////////////////////////////////////////////////////

void testSkipList() {
  pthread_t thAdd[3];
  pthread_attr_t thAttrAdd[3];
  skip_list_test_arg_t thAddArg[3];

  pthread_t thRemove[3];
  pthread_attr_t thAttrRemove[3];
  skip_list_test_arg_t thRemoveArg[3];

  int32_t i;
  lf_skip_list_t *lstSkip = LFSkipListCreate();
  if (lstSkip == NULL) {
    printf("Couldn't create LFSkipList\n");
    return;
  }

  /*test add*/
  for (i = 0; i < 3; ++i) {
    thAddArg[i].start = STEP * i;
    thAddArg[i].end = STEP * (i + 1);
    thAddArg[i].lst = lstSkip;

    pthread_attr_init(&thAttrAdd[i]);
    pthread_create(&thAdd[i], &thAttrAdd[i], skipListAddTest, &thAddArg[i]);
  }

  for (i = 0; i < 3; ++i) {
    pthread_join(thAdd[i], NULL);
  }
  LFSkipListPrint(lstSkip);
  printf("\n*************************************************************\n");

  /*test remove*/

  for (i = 0; i < 3; ++i) {
    thRemoveArg[i].start = STEP * i;
    thRemoveArg[i].end = STEP * (i + 1);
    thRemoveArg[i].lst = lstSkip;

    pthread_attr_init(&thAttrRemove[i]);
    pthread_create(&thRemove[i], &thAttrRemove[i], skipListRemoveTest, &thRemoveArg[i]);
  }

  for (i = 0; i < 3; ++i) {
    pthread_join(thRemove[i], NULL);
  }
  LFSkipListPrint(lstSkip);
  printf("\n*************************************************************\n");  
}
////////////////////////////////////////////////////////////////////////////

int main() {
  srand(time(NULL));
//  testSkipList();

  int32_t i;
  lf_skip_list_t *lstSkip = LFSkipListCreate();

  for (i = 0; i < 10; ++i) {
    LFSkipListAdd(lstSkip, i, NULL);
  }
  LFSkipListPrint(lstSkip);
  printf("\n****************************************\n");


  for (i = 0; i < 10; i+=2) {
    LFSkipListRemove(lstSkip, i);
  }
  LFSkipListPrint(lstSkip);
  printf("\n****************************************\n");


  return 0;
}
////////////////////////////////////////////////////////////////////////////
