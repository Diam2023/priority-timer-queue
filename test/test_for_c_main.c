/**
 * @file test_for_c_main.c
 * @author Diam (monoliths-uni@outlook.com)
 * @brief
 * @version 3.1
 * @date 2024-05-19
 *
 * @copyright Copyright (c) 2024-2025 桦鸿科技（重庆）有限公司. All rights
 * reserved.
 *
 */

#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <stdio.h>

#include <unistd.h>

#include "priority_timer.h"

static MONO_PriorityTimerQueue_t *queue;
static pthread_t timerThread;
static pthread_t testThread;

static void func_1() { printf("f 1\n"); }
static void func_2() { printf("f 2\n"); }
static void func_3() { printf("f 3\n"); }

static MONO_NodeId_t testTaskId1;
static MONO_NodeId_t testTaskId2;
static MONO_NodeId_t testTaskId3;


// Mutex
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

bool MONO_TryLockTimerQueue(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT) {
  // return pthread_mutex_trylock(&mutex);
  return pthread_mutex_lock(&mutex) == 0;
}

void MONO_UnlockTimerQueue(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT) {
  pthread_mutex_unlock(&mutex);
}

/**
 * @brief 定时器
 *
 * @param arg
 * @return void*
 */
void *timer(void *arg) {
  (void) arg;
  usleep(1000000);

  while (true) {
    MONO_TimerTickHandler(queue);
    if (queue == NULL) {
      printf("定时队列退出\n");
      break;
    }
    // 1ms 一次
    usleep(1000);
  }

  return NULL;
}

void *test(void *arg) {
  printf("Test Start \n");

  testTaskId1 = MONO_PushNode(queue, MONO_CreateQueueNode((MONO_NodeFunction_t) func_1, 1000, NULL));
  testTaskId2 = MONO_PushNode(queue, MONO_CreateQueueNode((MONO_NodeFunction_t) func_2, 1000, NULL));
  testTaskId3 = MONO_PushNode(queue, MONO_CreateQueueNode((MONO_NodeFunction_t) func_3, 1000, NULL));

  MONO_SetTimerNodeEnable(queue, testTaskId2, false);

  MONO_QueueTaskInfo(queue);
  sleep(5);
  printf("12 run end \n");
  MONO_SetTimerNodeEnable(queue, testTaskId3, false);
  MONO_SetTimerNodeEnable(queue, testTaskId2, true);
  MONO_QueueTaskInfo(queue);
  sleep(3);
  MONO_SetTimerNodeEnable(queue, testTaskId3, true);
  sleep(2);

  MONO_QueueTaskInfo(queue);
  // 回收内存
  MONO_DestroyPriorityQueue(&queue);

  pthread_join(timerThread, NULL);

  printf("Test End \n");

  return NULL;
}

void init() {
  // pthread_mutex_init(&mutex, NULL);
  queue = CreatePriorityTimerQueue();

  pthread_create(&timerThread, NULL, timer, NULL);
  pthread_create(&testThread, NULL, test, NULL);
  MONO_SetTimerQueueEnable(queue, true);
}

int main() {
  init();

  pthread_join(testThread, NULL);

  pthread_mutex_destroy(&mutex);

  return 0;
}
