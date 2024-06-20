/**
 * @file priority_timer.c
 *
 * @brief 具体实现
 *
 * @author Diam (monoliths-uni@outlook.com)
 * @version 3.0
 * @date 2024-06-13
 *
 *
 * @copyright Copyright (c) 2022-2023 Diam. All rights reserved.
 * @copyright Copyright (c) 2024-2025 桦鸿科技（重庆）有限公司. All rights reserved.
 *
 * *********************************************************************************
 *
 * @note version: 2.4
 * @date 2024-05-16
 *
 * @note version: 2.5
 * @description: 整理API
 * @date 2025-06-06
 *
 * @note version: 3.0
 * @description: 升级定时器接口实现API
 * @date 2024-06-13
 *
 * @note version: 3.1
 * @description: 新增接口 MONO_SetNextAlarmTimer MONO_TimerTickHandler MONO_TimerTickStep 用于增强设置时间
 * @date 2024-06-20
 *
 * @note version: 3.2
 * @description: 修正了Lock逻辑导致的锁死BUG
 * @date 2024-06-20
 */

#include "priority_timer.h"


#ifdef MONO_PTQ_DEBUG

#include <stdio.h>

#endif

/**
 * @brief 唯一id分配
 */
static MONO_NodeId_t MONO_g_ptn_auto_id = 1;

/************************** NODE **************************/

__attribute__((weak)) MONO_PriorityTimerNode_t *MONO_AllocNode() {
  return (MONO_PriorityTimerNode_t *) calloc(1, MONO_NODE_SIZE);
}

__attribute__((weak)) void
MONO_DeallocNode(MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT) {
  free(node_);
}

void MONO_CopyNode(MONO_PriorityTimerNode_t *node_dest_,
                   const MONO_PriorityTimerNode_t *node_src_) {
  node_dest_->_timer = node_src_->_timer;
  node_dest_->_priority = node_src_->_priority;
  node_dest_->_performance_func = node_src_->_performance_func;
  node_dest_->_loop_counter = node_src_->_loop_counter;
  node_dest_->_reload = node_src_->_reload;
  node_dest_->_id = node_src_->_id;
  node_dest_->_func = node_src_->_func;
  node_dest_->_enabled = node_src_->_enabled;
  node_dest_->_args = node_src_->_args;
}

MONO_PriorityTimerNode_t *MONO_CreateQueueNodeFull(
        MONO_NodeFunction_t node_func_, uint8_t enabled_,
        MONO_NodeTimer_t timer_, uint8_t loop_counter_, MONO_NodeTimer_t reload_,
        uint8_t priority_, void *args_, MONO_NodeFunction_t performance_func_) {
  MONO_PriorityTimerNode_t *node = MONO_AllocNode();

  node->_id = MONO_g_ptn_auto_id++;

  node->_func = node_func_;

  node->_enabled = enabled_;

  node->_timer = timer_;

  node->_loop_counter = loop_counter_;

  node->_reload = reload_;

  node->_priority = priority_;

  node->_args = args_;

  node->_performance_func = performance_func_;

  node->_next = NULL;

  return node;
}

/************************** TIMER **************************/

__attribute__((weak)) MONO_PriorityTimerQueue_t *MONO_AllocTimerQueue() {
  return (MONO_PriorityTimerQueue_t *) calloc(1,
                                              sizeof(MONO_PriorityTimerQueue_t));
}

__attribute__((weak)) void
MONO_DeallocTimerQueue(MONO_PriorityTimerQueue_t *queue_) {
  free(queue_);
}

__attribute__((weak)) bool
MONO_TryLockTimerQueue(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT) {
  if (!queue_->_lock) {
    queue_->_lock = true;
    return true;
  }
  return false;
}

__attribute__((weak)) void
MONO_UnlockTimerQueue(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT) {
  queue_->_lock = false;
}

__attribute__((weak)) void MONO_SetNextAlarmTimer(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT, MONO_NodeTimer_t timer_) {
  // 默认无实现
}

void MONO_SetTimerQueueEnable(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT,
                              bool status_) {
  queue_->_run_status = status_;
}

MONO_PriorityTimerQueue_t *MONO_CreatePriorityQueue() {
  MONO_PriorityTimerQueue_t *queue = MONO_AllocTimerQueue();
  queue->_header_node = NULL;
  queue->_disabled_header = NULL;
  queue->_lock = false;
  queue->_run_status = false;
  queue->_size = 0;
  queue->_timer_tick = 0;

  return queue;
}

void MONO_DestroyPriorityQueue(MONO_PriorityTimerQueue_t **queue_) {
  if (queue_ == NULL) {
    return;
  }
  if (*queue_ == NULL) {
    return;
  }

  if ((*queue_)->_size <= 0) {
    MONO_DeallocTimerQueue(*queue_);
    return;
  }

  MONO_PriorityTimerNode_t *tempNode = (*queue_)->_header_node;
  MONO_PriorityTimerNode_t *nextNode;
  // 回收节点内存
  while (tempNode != NULL) {
    if (tempNode->_next != NULL) {
      nextNode = tempNode->_next;
      MONO_DeallocNode(tempNode);
      tempNode = nextNode;
    } else {
      MONO_DeallocNode(tempNode);
      tempNode = NULL;
    }
  }

  MONO_DeallocTimerQueue(*queue_);
  *queue_ = NULL;
}

/**
 * @brief  找到合适的位置，将节点node_到队列中
 *
 * timer_参数越小越靠近头节点 priority参数越小越靠近头节点
 *
 * @param  queue_:        队列指针
 * @param  node_:         节点指针
 * @return MONO_NodeId_t: 节点id
 */
MONO_NodeId_t MONO_PushNode(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT,
                            MONO_PriorityTimerNode_t *node_) {
  // 上锁
  if (!MONO_TryLockTimerQueue(queue_)) {
    printf("Err Lock");
    return false;
  }

  MONO_NodeId_t result = node_->_id;

  do {

    MONO_PriorityTimerNode_t **headerNode;
    // 这里分是否为启用节点的情况
    if (node_->_enabled) {
      // 主队列
      headerNode = &queue_->_header_node;
    } else {
      // 缓存队列
      headerNode = &queue_->_disabled_header;
    }
    // 没有头节点的情况
    if ((*headerNode) == NULL) {
      *headerNode = node_;
      queue_->_size++;
      break;
    }

    MONO_PriorityTimerNode_t *tempNode = *headerNode;
    MONO_PriorityTimerNode_t *prevNode = NULL;
    // 非头节点情况
    // 首先查找到合适的timer位置
    bool insertFlag = false;
    while (tempNode != NULL) {

      // TODO 减小缓存队列的开销
      if ((node_->_timer < tempNode->_timer) ||    // 时间小于这个节点
          ((node_->_timer == tempNode->_timer) &&  // 时间等于这个节点并且
           (node_->_priority < tempNode->_priority)// 优先级小于这个节点
           )) {
        // 插入到这个节点的前面
        if (prevNode == NULL) {
          // 插入到头节点
          *headerNode = node_;
          node_->_next = tempNode;
        } else {
          // 非头节点
          node_->_next = prevNode->_next;
          prevNode->_next = node_;
        }
        queue_->_size++;
        insertFlag = true;
        break;
      }

      prevNode = tempNode;
      tempNode = tempNode->_next;
    }

    if (insertFlag) {
      break;
    }
    prevNode->_next = node_;
    node_->_next = NULL;
    queue_->_size++;

  } while (false);

  MONO_UnlockTimerQueue(queue_);
  return result;
}

/**
 * @brief  弹出队列首并且可运行的节点
 * @param  queue_: 队列指针
 * @return MONO_PriorityTimerNode_t*: 节点指针
 */
static MONO_PriorityTimerNode_t *
MONO_PopRunableNode(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT) {
  if (queue_ == NULL) {
    return NULL;
  }
  if (queue_->_size == 0) {
    return NULL;
  }

  if (!MONO_TryLockTimerQueue(queue_)) {
    return NULL;
  }

  MONO_PriorityTimerNode_t *result = NULL;

  do {
    // 多节点处理
    MONO_PriorityTimerNode_t *tempNode = queue_->_header_node;
    if ((tempNode != NULL) && (tempNode->_timer == 0)) {
      // 如果当前节点为头节点
      queue_->_header_node = tempNode->_next;
      queue_->_size--;
      result = tempNode;
      break;
    }

  } while (false);

  MONO_UnlockTimerQueue(queue_);

  return result;
}

/**
 * @brief 运行节点
 *
 * @return true  还要循环
 * @return false 下次不循环了
 */
static bool MONO_RunNode(MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT) {
  // 运行节点逻辑
  if (node_->_performance_func != NULL) {
    node_->_performance_func(node_->_func(node_->_args));
  } else {
    node_->_func(node_->_args);
  }
  // 检查是否需要reload
  // TODO 优化释放
  if (node_->_loop_counter <= 0) {
    // 不要循环
    // 直接清理
    return false;
  } else {
    // 要循环
    if (node_->_loop_counter != UINT8_MAX) {
      node_->_loop_counter--;
    }

    // 这里判断是因为 如果结果处理函数中修改了timer则不更改
    if (node_->_timer == 0) {
      // reload
      node_->_timer = node_->_reload;
    }
    return true;
  }
}

uint8_t MONO_Size(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT) {
  return queue_->_size;
}

MONO_PriorityTimerNode_t *
MONO_FindNodeById(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT,
                  MONO_NodeId_t id_) {
  if (queue_ == NULL || queue_->_size <= 0) {
    return NULL;
  }
  MONO_PriorityTimerNode_t *tempNode = queue_->_header_node;
  while (tempNode != NULL) {
    if (tempNode->_id == id_) {
      if (!tempNode->_enabled) {
        // 检查修正
        tempNode->_enabled = true;
      }
      return tempNode;
    }
    tempNode = tempNode->_next;
  }

  tempNode = queue_->_disabled_header;
  while (tempNode != NULL) {
    if (tempNode->_id == id_) {
      // 检查修正
      if (tempNode->_enabled) {
        tempNode->_enabled = false;
      }
      return tempNode;
    }
    tempNode = tempNode->_next;
  }

  return NULL;
}

/**
 * @brief 弹出队列指定节点
 *
 * @param  queue_ 队列指针
 * @param  id_    节点ID
 * @return MONO_PriorityTimerNode_t*
 */
static MONO_PriorityTimerNode_t *
MONO_PopNodeById(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT,
                 MONO_NodeId_t id_) {
  if (queue_ == NULL || queue_->_size <= 0) {
    return NULL;
  }

  // 上锁
  if (!MONO_TryLockTimerQueue(queue_)) {
    return NULL;
  }

  MONO_PriorityTimerNode_t *result = NULL;

  do {

    MONO_PriorityTimerNode_t *tempNode = queue_->_header_node;
    MONO_PriorityTimerNode_t *prevNode = NULL;
    while (tempNode != NULL) {
      if (tempNode->_id == id_) {
        // pop节点
        if (prevNode != NULL) {
          // 如果有上一个节点
          prevNode->_next = tempNode->_next;
        } else {
          // 如果为头节点
          queue_->_header_node = tempNode->_next;
        }
        queue_->_size--;
        result = tempNode;
        break;
      }
      prevNode = tempNode;
      tempNode = tempNode->_next;
    }

    tempNode = queue_->_disabled_header;
    prevNode = NULL;
    while (tempNode != NULL) {
      if (tempNode->_id == id_) {
        // pop节点
        if (prevNode != NULL) {
          // 如果有上一个节点
          prevNode->_next = tempNode->_next;
        } else {
          // 如果为头节点
          queue_->_disabled_header = tempNode->_next;
        }
        queue_->_size--;
        result = tempNode;
        break;
      }
      prevNode = tempNode;
      tempNode = tempNode->_next;
    }

  } while (false);

  // 解锁
  MONO_UnlockTimerQueue(queue_);
  return result;
}

bool MONO_SetTimerNodeEnable(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT,
                             MONO_NodeTimer_t id_, bool enable_) {
  // 跳过无效参数
  if ((id_ == UINT32_MAX) || (id_ == 0)) {
    printf("Err Id");
    return false;
  }

  MONO_PriorityTimerNode_t *findNode = MONO_FindNodeById(queue_, id_);
  if (findNode == NULL) {
    printf("Err Find ID");
    return false;
  }

  // 判断是否不需要操作
  if (findNode->_enabled == enable_) {
    printf("Enabel Skip");
    return true;
  }

  // 需要操作
  MONO_PriorityTimerNode_t *popNode = MONO_PopNodeById(queue_, id_);
  if (popNode == NULL) {
    printf("Pop Err");
    return false;
  }
  popNode->_next = NULL;
  popNode->_enabled = enable_;

  return (MONO_PushNode(queue_, popNode) != UINT16_MAX);
}

bool MONO_SetTimerNodeTimer(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT,
                            MONO_NodeTimer_t id_,
                            const MONO_NodeTimer_t timer_) {
  MONO_PriorityTimerNode_t *node = MONO_FindNodeById(queue_, id_);

  if (node != NULL) {
    node->_timer = timer_;
    return true;
  }

  return false;
}

bool MONO_SetTimerNodeLoopTimer(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT,
                                MONO_NodeTimer_t id_,
                                const MONO_NodeTimer_t timer_) {
  MONO_PriorityTimerNode_t *node = MONO_FindNodeById(queue_, id_);

  if (node != NULL) {
    node->_timer = timer_;
    return true;
  }

  return false;
}

bool MONO_IsTimerNodeExist(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT,
                           MONO_NodeTimer_t id_) {
  return !(MONO_FindNodeById(queue_, id_) == NULL);
}

uint16_t MONO_PushNodeFullArguments(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT,
                                    MONO_NodeFunction_t node_func_,
                                    uint8_t enabled_,
                                    MONO_NodeTimer_t timer_, uint8_t loop_counter_,
                                    MONO_NodeTimer_t reload_,
                                    uint8_t priority_, void *args_,
                                    MONO_NodeFunction_t performance_func_) {
  MONO_PriorityTimerNode_t node = {._args = args_,
                                   ._enabled = enabled_,
                                   ._func = node_func_,
                                   ._id = MONO_g_ptn_auto_id++,
                                   ._loop_counter = loop_counter_,
                                   ._reload = reload_,
                                   ._performance_func = performance_func_,
                                   ._priority = priority_,
                                   ._timer = timer_};

  MONO_PriorityTimerNode_t *newNode = MONO_AllocNode();
  MONO_CopyNode(newNode, &node);

  return MONO_PushNode(queue_, newNode);
}

uint32_t MONO_GetTimerTick(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT) {
  return queue_->_timer_tick;
}

uint32_t MONO_TimerTickHandler(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT) {
  return MONO_TimerTickStep(queue_, 1);
}

uint32_t MONO_TimerTickStep(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT, uint32_t step_) {
  if (queue_ == NULL) {
    // 不存在实例
    return UINT32_MAX;
  }
  if (!queue_->_run_status) {
    // 未开启
    return UINT32_MAX;
  }
  // // Lock!
  // if (!MONO_TryLockTimerQueue(queue_)) {
  //   return UINT32_MAX;
  // }

  // 补偿定时时间
  {
    MONO_PriorityTimerNode_t *tempNode = queue_->_header_node;
    queue_->_timer_tick += step_;
    while (tempNode != NULL) {
      if (tempNode->_timer > 0) {
        if (tempNode->_timer >= step_) {
          tempNode->_timer -= step_;
        } else {
#ifdef MONO_PTQ_DEBUG
          printf("Task Id %d Timer Overflow %d", tempNode->_id, step_ - tempNode->_timer);
#endif
          // 时间补偿超过了定时时间就直接为0
          tempNode->_timer = 0;
        }
      } else {
        // TODO 检查节点有效性执行
      }
      tempNode = tempNode->_next;
    }
  }

  uint32_t resultCount = 0;
  // 运行可运行的节点
  {
    MONO_PriorityTimerNode_t *runableNode = NULL;
    while ((runableNode = MONO_PopRunableNode(queue_)) != NULL) {
      if (MONO_RunNode(runableNode)) {
        MONO_PushNode(queue_, runableNode);
      } else {
        MONO_DeallocNode(runableNode);
      }
      resultCount++;
    }
  }

  // 调用下次唤醒
  if (queue_->_header_node != NULL) {
    if (queue_->_header_node->_timer > 0) {
      MONO_SetNextAlarmTimer(queue_, queue_->_header_node->_timer);
    }
  }

  // MONO_UnlockTimerQueue(queue_);
  return resultCount;
}

uint32_t MONO_TimerInnerHandler(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT) {
  return MONO_TimerTickHandler(queue_);
}

#ifdef MONO_PTQ_DEBUG

static void PrintNodeHeader() {
  printf("%-4s  %-7s  %-5s  %-4s  %-6s \n", "id", "enabled", "timer",
         "loop", "reload");
}
static void PrintNode(MONO_PriorityTimerNode_t *temp_node_) {
  printf("%-4d  %-7s  %-5ld  %-4d  %-6ld \n", temp_node_->_id,
         temp_node_->_enabled ? "true" : "false", temp_node_->_timer,
         temp_node_->_loop_counter, temp_node_->_reload);
}

void MONO_QueueTaskInfo(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT) {
  if (queue_ == NULL) {
    printf("Error Queue NULL!\n");
    return;
  }

  printf("\n");
  printf("----------------  Task Info  -----------------\n");
  printf("%-5s  %-6s  %-5s \n", "size", "status", "tick");
  printf("%-5ld  %-6s  %-5ld \n", queue_->_size,
         queue_->_run_status ? "run" : "stop", queue_->_timer_tick);
  if (queue_->_size == 0) {
    printf("Empty \n");
    return;
  }

  int nowIndex = 0;

  MONO_PriorityTimerNode_t *tempNode = NULL;
  printf("\n");
  printf("------------------- Major --------------------\n");
  tempNode = queue_->_header_node;
  while (tempNode != NULL) {
    if (nowIndex == 0) {
      PrintNodeHeader();
      nowIndex++;
    }
    PrintNode(tempNode);
    tempNode = tempNode->_next;
  }
  printf("\n");
  printf("------------------- Cache --------------------\n");
  tempNode = queue_->_disabled_header;
  while (tempNode != NULL) {
    if (nowIndex == 0) {
      PrintNodeHeader();
      nowIndex++;
    }
    PrintNode(tempNode);
    tempNode = tempNode->_next;
  }
  printf("End \n");
}

#endif
