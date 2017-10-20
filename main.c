#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "LFSortedList.h"
////////////////////////////////////////////////////////////////////////////

lf_sorted_list_t *lst = NULL;

#define STEP 20
#define SLEEP 300

void* lfAddT1(void* t);
void* lfAddT2(void* t);
void* lfAddT3(void* t);

void* lfRemoveT1(void* t);
void* lfRemoveT2(void* t);
void* lfRemoveT3(void* t);

void* lfAddT1(void* t) {
  uint32_t i;
  (void)t;
  for (i = 0; i < STEP; ++i) {
    LFSortedListAdd(lst, i, NULL);
    usleep(SLEEP);
  }
  return NULL;
}

void* lfAddT2(void* t) {
  uint32_t i;
  (void)t;
  for (i = STEP; i < STEP*2; ++i) {
    LFSortedListAdd(lst, i, NULL);
    usleep(SLEEP);
  }
  return NULL;
}

void* lfAddT3(void* t) {
  uint32_t i;
  (void)t;
  for (i = STEP*2; i < STEP*3; ++i) {
    LFSortedListAdd(lst, i, NULL);
    usleep(SLEEP);
  }
  return NULL;
}
////////////////////////////////////////////////////////////////////////////

void* lfRemoveT1(void* t) {
  uint32_t i;
  (void)t;
  for (i = 0; i < STEP; i += 2) {
    LFSortedListRemove(lst, i);
    usleep(SLEEP);
  }
  return NULL;
}
////////////////////////////////////////////////////////////////////////////

void* lfRemoveT2(void* t) {
  uint32_t i;
  (void)t;
  for (i = STEP; i < STEP*2; i += 2) {
    LFSortedListRemove(lst, i);
    usleep(SLEEP);
  }
  return NULL;
}
////////////////////////////////////////////////////////////////////////////

void* lfRemoveT3(void* t) {
  uint32_t i;
  (void)t;
  for (i = STEP*2; i < STEP*3; i += 2) {
    LFSortedListRemove(lst, i);
    usleep(SLEEP);
  }
  return NULL;
}
////////////////////////////////////////////////////////////////////////////

int main() {
  pthread_t thAdd[3];
  pthread_attr_t thAttrAdd[3];
  void *(*pfAddTest[3])(void*) = {lfAddT1, lfAddT2, lfAddT3};

  pthread_t thRemove[3];
  pthread_attr_t thAttrRemove[3];
  void *(*pfRemoveTest[3])(void*) = {lfRemoveT1, lfRemoveT2, lfRemoveT3};

  int32_t i;

  lst = LFSortedListCreate();
  if (lst == NULL) {
    printf("Couldn't create LFSorteList\n");
    return -1;
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

  return 0;
}
////////////////////////////////////////////////////////////////////////////
