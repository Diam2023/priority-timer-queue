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

#include <stdint.h>
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


/// ***************************************************8

// Mutex
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

TryLockOverride {
  // return pthread_mutex_trylock(&mutex);
  return pthread_mutex_lock(&mutex) == 0;
}

UnlockOverride {
  pthread_mutex_unlock(&mutex);
}

/// ***************************************************8

// 模拟定时器
uint32_t sleepTime = 0;

uint32_t tempStep = 0;

void *timer(void *arg) {
  (void) arg;
  usleep(1000000);

  while (true) {
    if (sleepTime != 0) {
      while (sleepTime > 0) {
        // 1ms 一次
        usleep(1000);
        sleepTime--;
        if (sleepTime <= 0) {
          TimerTickStep(queue, tempStep);
        }
      }
    }
    // MONO_TimerTickHandler(queue);
    if (queue == NULL) {
      printf("定时队列退出\n");
      break;
    }
    // usleep(1000);
  }

  return NULL;
}

SetNextAlarmTimer {
  if (sleepTime == 0) {
    printf("设置下次唤醒 %u\n", timer_);
  } else {
    if (timer_ != sleepTime) {
      printf("重置唤醒 %u to %u\n", sleepTime, timer_);
    }
  }
  sleepTime = timer_;
  tempStep = timer_;
}

void *test(void *arg) {
  printf("Test Start \n");

  NewTimerTask(queue, func_3, 2000, NULL);

  // 清空队列
  ClearTimerQueue(queue);

  testTaskId1 = NewTimerTask(queue, func_1, 1000, NULL);
  testTaskId2 = NewTimerTask(queue, func_2, 1400, NULL);
  testTaskId3 = NewTimerTask(queue, func_3, 1123, NULL);
  ClearTimerQueue(queue);

  testTaskId1 = NewTimerTask(queue, func_1, 1000, NULL);
  testTaskId2 = NewTimerTask(queue, func_2, 1400, NULL);
  testTaskId3 = NewTimerTask(queue, func_3, 1123, NULL);

  DisableTimerTask(queue, testTaskId2);
  MONO_QueueTaskInfo(queue);

  sleep(5);
  printf("13 run end \n");
  MONO_QueueTaskInfo(queue);
  DisableTimerTask(queue, testTaskId3);
  EnableTimerTask(queue, testTaskId2);
  sleep(3);
  printf("12 run end \n");
  MONO_QueueTaskInfo(queue);
  EnableTimerTask(queue, testTaskId3);
  sleep(2);
  printf("123 run end \n");

  DisableTimerTask(queue, testTaskId2);
  MONO_QueueTaskInfo(queue);
  // 回收内存
  DeleteTimerQueue(queue);

  pthread_join(timerThread, NULL);

  printf("Test End \n");

  return NULL;
}

void init() {
  queue = NewTimerQueue();

  pthread_create(&timerThread, NULL, timer, NULL);
  pthread_create(&testThread, NULL, test, NULL);
  EnableTimerQueue(queue);
}

int main() {
  init();

  pthread_join(testThread, NULL);

  pthread_mutex_destroy(&mutex);

  return 0;
}
