#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

#include "LFSortedList.h"
#include "LFSkipList.h"
////////////////////////////////////////////////////////////////////////////

lf_sorted_list_t *lstSorted = NULL;
lf_skip_list_t *lstSkip = NULL;

#define STEP 10
#define SLEEP 100

void* sortedListAddT1(void* t);
void* sortedListAddT2(void* t);
void* sortedListAddT3(void* t);

void* sortedListRemoveT1(void* t);
void* sortedListRemoveT2(void* t);
void* sortedListRemoveT3(void* t);

void* skipListAddT1(void* t);
void* skipListAddT2(void* t);
void* skipListAddT3(void* t);

void* skipListRemoveT1(void* t);
void* skipListRemoveT2(void* t);
void* skipListRemoveT3(void* t);

void* sortedListAddT1(void* t) {
  uint32_t i;
  (void)t;
  for (i = 0; i < STEP; ++i) {
    LFSortedListAdd(lstSorted, i, NULL);
    usleep(SLEEP);
  }
  return NULL;
}

void* sortedListAddT2(void* t) {
  uint32_t i;
  (void)t;
  for (i = STEP; i < STEP*2; ++i) {
    LFSortedListAdd(lstSorted, i, NULL);
    usleep(SLEEP);
  }
  return NULL;
}

void* sortedListAddT3(void* t) {
  uint32_t i;
  (void)t;
  for (i = STEP*2; i < STEP*3; ++i) {
    LFSortedListAdd(lstSorted, i, NULL);
    usleep(SLEEP);
  }
  return NULL;
}
////////////////////////////////////////////////////////////////////////////

void* sortedListRemoveT1(void* t) {
  uint32_t i;
  (void)t;
  for (i = 0; i < STEP; i += 2) {
    LFSortedListRemove(lstSorted, i);
    usleep(SLEEP);
  }
  return NULL;
}
////////////////////////////////////////////////////////////////////////////

void* sortedListRemoveT2(void* t) {
  uint32_t i;
  (void)t;
  for (i = STEP; i < STEP*2; i += 2) {
    LFSortedListRemove(lstSorted, i);
    usleep(SLEEP);
  }
  return NULL;
}
////////////////////////////////////////////////////////////////////////////

void* sortedListRemoveT3(void* t) {
  uint32_t i;
  (void)t;
  for (i = STEP*2; i < STEP*3; i += 2) {
    LFSortedListRemove(lstSorted, i);
    usleep(SLEEP);
  }
  return NULL;
}
//////////////////////////////////////////////////////////////////////////

void testSortedList() {
  pthread_t thAdd[3];
  pthread_attr_t thAttrAdd[3];
  void *(*pfAddTest[3])(void*) = {sortedListAddT1, sortedListAddT2, sortedListAddT3};

  pthread_t thRemove[3];
  pthread_attr_t thAttrRemove[3];
  void *(*pfRemoveTest[3])(void*) = {sortedListRemoveT1, sortedListRemoveT2, sortedListRemoveT3};

  int32_t i;
  lstSorted = LFSortedListCreate();
  if (lstSorted == NULL) {
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
  LFSortedListPrint(lstSorted);
  printf("\n*****************\n");

  /*test remove*/

  for (i = 0; i < 3; ++i) {
    pthread_attr_init(&thAttrRemove[i]);
    pthread_create(&thRemove[i], &thAttrRemove[i], pfRemoveTest[i], NULL);
  }

  for (i = 0; i < 3; ++i) {
    pthread_join(thRemove[i], NULL);
  }
  LFSortedListPrint(lstSorted);
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
  LFSortedListPrint(lstSorted);
  printf("\n*****************\n");
}
////////////////////////////////////////////////////////////////////////////

void* skipListAddT1(void* t) {
  int32_t i;
  (void)t;
  for (i = STEP; i >= 0; --i) {
    LFSkipListAdd(lstSkip, i, NULL);
    usleep(SLEEP);
  }
  return NULL;
}

void* skipListAddT2(void* t) {
  int32_t i;
  (void)t;
  for (i = STEP; i < STEP*2; ++i) {
    LFSkipListAdd(lstSkip, i, NULL);
    usleep(SLEEP);
  }
  return NULL;
}

void* skipListAddT3(void* t) {
  int32_t i;
  (void)t;
  for (i = STEP*3; i >= STEP*2; --i) {
    LFSkipListAdd(lstSkip, i, NULL);
    usleep(SLEEP);
  }
  return NULL;
}
////////////////////////////////////////////////////////////////////////////

void* skipListRemoveT1(void* t) {
  int32_t i;
  (void)t;
  for (i = 0; i < STEP; i += 2) {
    LFSkipListRemove(lstSkip, i);
    usleep(SLEEP);
  }
  return NULL;
}
////////////////////////////////////////////////////////////////////////////

void* skipListRemoveT2(void* t) {
  int32_t i;
  (void)t;
  for (i = STEP; i < STEP*2; i += 2) {
    LFSkipListRemove(lstSkip, i);
    usleep(SLEEP);
  }
  return NULL;
}
////////////////////////////////////////////////////////////////////////////

void* skipListRemoveT3(void* t) {
  int32_t i;
  (void)t;
  for (i = STEP*2; i < STEP*3; i += 2) {
    LFSkipListRemove(lstSkip, i);
    usleep(SLEEP);
  }
  return NULL;
}
//////////////////////////////////////////////////////////////////////////

void testSkipList() {
  pthread_t thAdd[3];
  pthread_attr_t thAttrAdd[3];
  void *(*pfAddTest[3])(void*) = {skipListAddT1, skipListAddT2, skipListAddT3};

  pthread_t thRemove[3];
  pthread_attr_t thAttrRemove[3];
  void *(*pfRemoveTest[3])(void*) = {skipListRemoveT1, skipListRemoveT2, skipListRemoveT3};

  int32_t i;
  lstSkip = LFSkipListCreate();
  if (lstSkip == NULL) {
    printf("Couldn't create LFSkipList\n");
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
  LFSkipListPrint(lstSkip);
  printf("\n*************************************************************\n");

  /*test remove*/

  for (i = 0; i < 3; ++i) {
    pthread_attr_init(&thAttrRemove[i]);
    pthread_create(&thRemove[i], &thAttrRemove[i], pfRemoveTest[i], NULL);
  }

  for (i = 0; i < 3; ++i) {
    pthread_join(thRemove[i], NULL);
  }
  LFSkipListPrint(lstSkip);
  printf("\n*************************************************************\n");

  /*test both*/

//  for (i = 0; i < 3; ++i) {
//    pthread_attr_init(&thAttrAdd[i]);
//    pthread_attr_init(&thAttrRemove[i]);
//    pthread_create(&thAdd[i], &thAttrAdd[i], pfAddTest[i], NULL);
//    pthread_create(&thRemove[i], &thAttrRemove[i], pfRemoveTest[i], NULL);
//  }

//  for (i = 0; i < 3; ++i) {
//    pthread_join(thAdd[i], NULL);
//    pthread_join(thRemove[i], NULL);
//  }
//  LFSkipListPrint(lstSkip);
//  printf("\n*************************************************************\n");
}

int main() {
  srand(time(NULL));
  lstSkip = LFSkipListCreate();
  if (lstSkip == NULL)
    return 1;

  testSkipList();
  return 0;
}
////////////////////////////////////////////////////////////////////////////
