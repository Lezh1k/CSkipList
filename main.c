#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

#include "LFSortedList.h"
#include "LFSkipList.h"
////////////////////////////////////////////////////////////////////////////

lf_sorted_list_t *lst = NULL;

#define STEP 20
#define SLEEP 300

void* sortedListAddT1(void* t);
void* sortedListAddT2(void* t);
void* sortedListAddT3(void* t);

void* sortedListRemoveT1(void* t);
void* sortedListRemoveT2(void* t);
void* sortedListRemoveT3(void* t);

void* sortedListAddT1(void* t) {
  uint32_t i;
  (void)t;
  for (i = 0; i < STEP; ++i) {
    LFSortedListAdd(lst, i, NULL);
    usleep(SLEEP);
  }
  return NULL;
}

void* sortedListAddT2(void* t) {
  uint32_t i;
  (void)t;
  for (i = STEP; i < STEP*2; ++i) {
    LFSortedListAdd(lst, i, NULL);
    usleep(SLEEP);
  }
  return NULL;
}

void* sortedListAddT3(void* t) {
  uint32_t i;
  (void)t;
  for (i = STEP*2; i < STEP*3; ++i) {
    LFSortedListAdd(lst, i, NULL);
    usleep(SLEEP);
  }
  return NULL;
}
////////////////////////////////////////////////////////////////////////////

void* sortedListRemoveT1(void* t) {
  uint32_t i;
  (void)t;
  for (i = 0; i < STEP; i += 2) {
    LFSortedListRemove(lst, i);
    usleep(SLEEP);
  }
  return NULL;
}
////////////////////////////////////////////////////////////////////////////

void* sortedListRemoveT2(void* t) {
  uint32_t i;
  (void)t;
  for (i = STEP; i < STEP*2; i += 2) {
    LFSortedListRemove(lst, i);
    usleep(SLEEP);
  }
  return NULL;
}
////////////////////////////////////////////////////////////////////////////

void* sortedListRemoveT3(void* t) {
  uint32_t i;
  (void)t;
  for (i = STEP*2; i < STEP*3; i += 2) {
    LFSortedListRemove(lst, i);
    usleep(SLEEP);
  }
  return NULL;
}

void testSortedList() {
  pthread_t thAdd[3];
  pthread_attr_t thAttrAdd[3];
  void *(*pfAddTest[3])(void*) = {sortedListAddT1, sortedListAddT2, sortedListAddT3};

  pthread_t thRemove[3];
  pthread_attr_t thAttrRemove[3];
  void *(*pfRemoveTest[3])(void*) = {sortedListRemoveT1, sortedListRemoveT2, sortedListRemoveT3};

  int32_t i;
  lst = LFSortedListCreate();
  if (lst == NULL) {
    printf("Couldn't create LFSorteList\n");
    return;
  }

  /*test add*/
  for (i = 0; i < 3; ++i) {
    pthread_attr_init(&thAttrAdd[i]);
    pthread_create(&thAdd[i], &thAttrAdd[i], pfAddTest[i], NULL);
  }

  for (i = 0; i < 3; ++i) {
    pthread_join(thAdd[i], NULL);
  }
  LFSortedListPrint(lst);
  printf("\n*****************\n");

  /*test remove*/

  for (i = 0; i < 3; ++i) {
    pthread_attr_init(&thAttrRemove[i]);
    pthread_create(&thRemove[i], &thAttrRemove[i], pfRemoveTest[i], NULL);
  }

  for (i = 0; i < 3; ++i) {
    pthread_join(thRemove[i], NULL);
  }
  LFSortedListPrint(lst);
  printf("\n*****************\n");

  /*test both*/

  for (i = 0; i < 3; ++i) {
    pthread_attr_init(&thAttrAdd[i]);
    pthread_attr_init(&thAttrRemove[i]);
    pthread_create(&thAdd[i], &thAttrAdd[i], pfAddTest[i], NULL);
    pthread_create(&thRemove[i], &thAttrRemove[i], pfRemoveTest[i], NULL);
  }

  for (i = 0; i < 3; ++i) {
    pthread_join(thAdd[i], NULL);
    pthread_join(thRemove[i], NULL);
  }
  LFSortedListPrint(lst);
  printf("\n*****************\n");

}
////////////////////////////////////////////////////////////////////////////

#define MAX_LEVEL 16
#define TEST_VAL 30
int main() {
  int i;
  lf_skip_list_t *lstSkip = LFSkipListCreate();
  if (lstSkip == NULL)
    return 1;
  srand(time(NULL));

  for (i = 0; i < TEST_VAL; ++i)
    LFSkipListAdd(lstSkip, i, NULL);

  LFSkipListPrint(lstSkip);
  return 0;
}
////////////////////////////////////////////////////////////////////////////
