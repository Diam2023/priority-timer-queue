/**
 * @file test_for_c_main.c
 * @author Diam (monoliths-uni@outlook.com)
 * @brief
 * @version 2.3
 * @date 2024-05-19
 *
 * @copyright Copyright (c) 2024-2025 桦鸿科技（重庆）有限公司. All rights
 * reserved.
 *
 */

#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>

#include <unistd.h>

#include "priority_timer.h"

MONO_PriorityTimerQueue_t *queue;

void func_say_2() { printf("outside 2\n"); }
void func_say_2_inner() { printf("inner 2\n"); }
void func_say_3() { printf("outside 3\n"); }
void func_say_3_inner() { printf("inner 3\n"); }
void func_say_4() { printf("outside 4\n"); }
void func_say_4_inner() { printf("inner 4\n"); }
void func_say_5() { printf("outside 5\n"); }
void func_say_5_inner() { printf("inner 5\n"); }

void test_func_logic() { printf("test_func_logic \n"); }

static MONO_NodeId_t testId;
static MONO_NodeId_t testId2;
static MONO_NodeId_t testId3;

/**
 * @brief 定时器
 *
 * @param arg
 * @return void*
 */
void *timer(void *arg) {
  (void)arg;

  while (true) {
    MONO_TimerInnerHandler(queue);
    // 1ms 一次
    usleep(1000);
  }

  return NULL;
}

/**
 * @brief 外部处理函数
 *
 * @param arg
 * @return void*
 */
void *outside(void *arg) {
  (void)arg;

  while (true) {
    MONO_TimerHandler(queue);
    // 10us一次
    usleep(10);
  }
  return NULL;
}

static MONO_NodeId_t testId4;
static MONO_NodeId_t testId5;
static MONO_NodeId_t testId6;

void test() {
  // MONO_SetTimerNodeEnable(queue, testId2, true);

  testId2 = MONO_PushNode(
      queue,
      MONO_CreateQueueNodeFull((MONO_NodeFunction_t)func_say_2, false, false,
                               1000, UINT8_MAX, 1000, UINT8_MAX, NULL, NULL));
  MONO_SetTimerNodeEnable(queue, testId2, true);
  testId3 = MONO_PushNode(
      queue, MONO_CreateQueueNodeFull((MONO_NodeFunction_t)func_say_3_inner,
                                      true, false, 500, UINT8_MAX, 500,
                                      UINT8_MAX, NULL, NULL));
  testId4 = MONO_PushNode(
      queue,
      MONO_CreateQueueNodeFull((MONO_NodeFunction_t)func_say_4, false, true,
                               500, UINT8_MAX, 500, UINT8_MAX, NULL, NULL));
  testId5 = MONO_PushNode(
      queue, MONO_CreateQueueNodeFull((MONO_NodeFunction_t)func_say_5_inner,
                                      true, true, 500, UINT8_MAX, 500,
                                      UINT8_MAX, NULL, NULL));
  MONO_QueueTaskInfo(queue);

  // MONO_PushNode(queue, MONO_CreateQueueNode((MONO_NodeFunction_t)func_say_3,
  // 1,
  //                                           95, NULL));
  sleep(1);

  MONO_QueueTaskInfo(queue);
  sleep(1);
  MONO_SetTimerNodeEnable(queue, testId4, false);
  MONO_QueueTaskInfo(queue);
  sleep(1);

  printf("Test Start");
  testId = MONO_PushNode(
      queue,
      MONO_CreateQueueNodeFull((MONO_NodeFunction_t)test_func_logic, 1, 1, 1500,
                               UINT8_MAX, 1000, UINT8_MAX, NULL, NULL));
  printf("#1 \n");
  MONO_QueueTaskInfo(queue);
  sleep(1);
  MONO_SetTimerNodeEnable(queue, testId, false);
  MONO_QueueTaskInfo(queue);
  printf("#2 \n");
  sleep(3);
  MONO_QueueTaskInfo(queue);
  MONO_SetTimerNodeEnable(queue, testId, true);
  MONO_QueueTaskInfo(queue);
  printf("#3 \n");
  sleep(2);

  MONO_SetTimerQueueEnable(queue, false);

  while (!MONO_SetTimerNodeEnable(queue, testId, false)) {
  };
  while (!MONO_SetTimerNodeEnable(queue, testId2, false)) {
  };
  while (!MONO_SetTimerNodeEnable(queue, testId3, false)) {
  };
  while (!MONO_SetTimerNodeEnable(queue, testId4, false)) {
  };
  while (!MONO_SetTimerNodeEnable(queue, testId5, false)) {
  };
  MONO_QueueTaskInfo(queue);
  printf("#4 \n");
  MONO_SetTimerQueueEnable(queue, true);
  sleep(1);
  MONO_SetTimerNodeEnable(queue, testId, true);
  MONO_SetTimerNodeEnable(queue, testId2, true);
  MONO_SetTimerNodeEnable(queue, testId3, true);
  MONO_SetTimerNodeEnable(queue, testId4, true);
  MONO_SetTimerNodeEnable(queue, testId5, true);
  MONO_QueueTaskInfo(queue);
  printf("#5 \n");
  sleep(5);
  // TODO 提供关闭API
  MONO_SetTimerNodeEnable(queue, testId, false);
  MONO_SetTimerNodeEnable(queue, testId2, false);
  MONO_SetTimerNodeEnable(queue, testId3, false);
  MONO_QueueTaskInfo(queue);
  printf("#6 \n");
  sleep(10);

  printf("END Test \n");
}

void init() {
  queue = CreatePriorityTimerQueue();

  pthread_t hand;
  pthread_t hand2;
  pthread_create(&hand, NULL, timer, NULL);
  pthread_create(&hand2, NULL, outside, NULL);
  MONO_SetTimerQueueEnable(queue, true);
}

int main() {
  init();

  test();

  return 0;
}
