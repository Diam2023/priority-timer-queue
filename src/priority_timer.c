/**
 * @file priority_timer.c
 * @author Diam (monoliths-uni@outlook.com)
 * @brief
 * @version 2.4
 * @date 2024-05-16
 *
 *
 * @copyright Copyright (c) 2022-2023 Diam. All rights reserved.
 * @copyright Copyright (c) 2024-2025 桦鸿科技（重庆）有限公司. All rights
 * reserved.
 */

#include "priority_timer.h"

/**
 * @brief 唯一id分配
 */
static MONO_NodeId_t MONO_g_ptn_auto_id = 1;

/************************** NODE **************************/

__attribute__((weak)) MONO_PriorityTimerNode_t *MONO_AllocNode() {
  return (MONO_PriorityTimerNode_t *)calloc(1, MONO_NODE_SIZE);
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
  node_dest_->_loop_timer = node_src_->_loop_timer;
  node_dest_->_loop = node_src_->_loop;
  node_dest_->_inner = node_src_->_inner;
  node_dest_->_id = node_src_->_id;
  node_dest_->_func = node_src_->_func;
  node_dest_->_enabled = node_src_->_enabled;
  node_dest_->_args = node_src_->_args;
}

// MONO_PriorityTimerNode_t *
// MONO_SetEnabled(uint8_t enabled_, MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT)
// {

//   node_->_enabled = enabled_;
//   return node_;
// }

// MONO_PriorityTimerNode_t *
// MONO_SetTimer(MONO_NodeTimer_t timer_,
//               MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT) {
//   node_->_timer = timer_;
//   return node_;
// }

// MONO_PriorityTimerNode_t *
// MONO_SetLoop(uint8_t loop_, MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT) {
//   node_->_loop = loop_;
//   // 若节点_loop_timer为0且_time不为0则设置_loop_timer的值为_timer的值
//   if (node_->_loop_timer == 0 && node_->_timer != 0) {
//     node_->_loop_timer = node_->_timer;
//   }
//   return node_;
// }

// MONO_PriorityTimerNode_t *
// MONO_SetLoopTimer(MONO_NodeTimer_t loop_timer_,
//                   MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT) {
//   node_->_loop_timer = loop_timer_;
//   return node_;
// }

// MONO_PriorityTimerNode_t *
// MONO_SetPriority(uint8_t priority_,
// MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT) {
//   node_->_priority = priority_;
//   return node_;
// }

// MONO_PriorityTimerNode_t *
// MONO_SetArgs(void *args_, MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT) {
//   node_->_args = args_;
//   return node_;
// }

// void MONO_RegisterResultPerformance(MONO_NodeFunction_t func_,
//                                     MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT)
//                                     {
//   node_->_performance_func = func_;
// }

MONO_PriorityTimerNode_t *MONO_CreateQueueNodeFull(
    MONO_NodeFunction_t node_func_, uint8_t inner_, uint8_t enabled_,
    MONO_NodeTimer_t timer_, uint8_t loop_, MONO_NodeTimer_t loop_timer_,
    uint8_t priority_, void *args_, MONO_NodeFunction_t performance_func_) {
  MONO_PriorityTimerNode_t *node = MONO_AllocNode();

  node->_id = MONO_g_ptn_auto_id++;

  node->_func = node_func_;

  node->_inner = inner_;

  node->_enabled = enabled_;

  node->_timer = timer_;

  node->_loop = loop_;

  node->_loop_timer = loop_timer_;

  node->_priority = priority_;

  node->_args = args_;

  node->_performance_func = performance_func_;

  node->_next = NULL;

  // #ifdef MONO_USE_FULL_PTN_MEMBER

  //   // 注册函数指针

  //   node->ExecuteNode = &MONO_ExecuteNode;

  //   node->DeallocNode = &MONO_DeallocNode;

  //   node->CopyNode = &MONO_CopyNode;

  //   node->RegisterResultPerformance = &MONO_RegisterResultPerformance;

  //   node->SetEnabled = &MONO_SetEnabled;
  //   node->SetTimer = &MONO_SetTimer;
  //   node->SetPriority = &MONO_SetPriority;
  //   node->SetLoop = &MONO_SetLoop;
  //   node->SetLoopTimer = &MONO_SetLoopTimer;
  //   node->SetArgs = &MONO_SetArgs;

  // #endif
  return node;
}

/************************** TIMER **************************/

__attribute__((weak)) MONO_PriorityTimerQueue_t *MONO_AllocTimerQueue() {
  return (MONO_PriorityTimerQueue_t *)calloc(1,
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

  // #ifdef MONO_USE_FULL_PTQ_MEMBER

  //   queue->IncreaseNodes = &MONO_IncreaseNodes;
  //   queue->DeallocateQueueMemory = &MONO_DeallocateQueueMemory;
  //   queue->RankTimer = &MONO_RankTimer;
  //   queue->FindNodeById = &MONO_FindNodeById;
  //   queue->PushNode = &MONO_PushNode;
  //   queue->PushNodeRelease = &MONO_PushNodeRelease;
  //   queue->TimerRunning = &MONO_TimerRunning;
  //   queue->PopNode = &MONO_PopNode;
  //   queue->PopNodeById = &MONO_PopNodeById;
  //   queue->EraseNode = &MONO_EraseNode;
  //   queue->RunTimerNode = &MONO_RunTimerNode;
  //   queue->RunInnerTimerNode = &MONO_RunInnerTimerNode;
  //   queue->Size = &MONO_Size;
  //   queue->EraseNodeByIndex = &MONO_EraseNodeByIndex;
  //   queue->PushNodeFullArguments = &MONO_PushNodeFullArguments;

  // #endif
  return queue;
}

void MONO_DestroyPriorityQueue(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT) {
  if (queue_ == NULL) {
    return;
  }

  if (queue_->_size <= 0) {
    MONO_DeallocTimerQueue(queue_);
    return;
  }

  MONO_PriorityTimerNode_t *tempNode = queue_->_header_node;
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

  MONO_DeallocTimerQueue(queue_);
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
    return node_->_id;
  }

  MONO_PriorityTimerNode_t *tempNode = *headerNode;
  MONO_PriorityTimerNode_t *prevNode = NULL;
  // 非头节点情况
  // 首先查找到合适的timer位置
  while (tempNode != NULL) {

    // TODO 减小缓存队列的开销
    if ((node_->_timer < tempNode->_timer) ||   // 时间小于这个节点
        ((node_->_timer == tempNode->_timer) && // 时间等于这个节点并且
         (node_->_priority < tempNode->_priority) // 优先级小于这个节点
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
      return node_->_id;
    }

    prevNode = tempNode;
    tempNode = tempNode->_next;
  }
  prevNode->_next = node_;
  node_->_next = NULL;
  queue_->_size++;
  return node_->_id;

  // #ifdef MONO_USE_FULL_PTN_MEMBER
  //   node_->CopyNode((queue_->nodes + queue_->size), node_);
  //   queue_->size++;
  // #else
  //   MONO_CopyNode((queue_->nodes + queue_->size), node_);
  //   queue_->size++;
  // #endif
}

static void MONO_TimerRunning(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT) {
  MONO_PriorityTimerNode_t *tempNode = queue_->_header_node;
  queue_->_timer_tick++;
  while (tempNode != NULL) {
    if (tempNode->_timer > 0) {
      tempNode->_timer--;
    } else {
      // TODO 检查节点有效性执行
    }
    tempNode = tempNode->_next;
  }
}

// /**
//  * @brief 弹出队列中优先级最高的（第一个）节点
//  * @param  queue_: 队列指针
//  * @deprecated
//  * @since v2.1
//  * @return MONO_PriorityTimerNode_t*: 节点的指针
//  */
// static MONO_PriorityTimerNode_t *
// MONO_PopNode(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT) {
//   (void)queue_;
//   return NULL;
//   // TODO 待实现
// }

/**
 * @brief 弹出指定id的节点
 * @param  queue_: 队列指针
 * @param  inner_: true为内部节点
 * @return MONO_PriorityTimerNode_t*: 节点指针
 */
static MONO_PriorityTimerNode_t *
MONO_PopRunableNode(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT, bool inner_) {
  if (queue_ == NULL) {
    return NULL;
  }
  if (queue_->_size == 0) {
    return NULL;
  }

  // 多节点处理
  MONO_PriorityTimerNode_t *tempNode = queue_->_header_node;
  MONO_PriorityTimerNode_t *prevNode = NULL;
  while ((tempNode != NULL) && (tempNode->_timer == 0)) {
    // TODO 可以进一步优化判断条件
    if (tempNode->_inner == inner_) {
      if (prevNode != NULL) {
        // 不为头节点
        prevNode->_next = tempNode->_next;
      } else {
        // 如果当前节点为头节点
        queue_->_header_node = tempNode->_next;
      }
      queue_->_size--;
      return tempNode;
    }
    prevNode = tempNode;
    tempNode = tempNode->_next;
  }

  return NULL;
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
  if (node_->_loop <= 0) {
    // 不要循环
    // 直接清理
    return false;
  } else {
    // 要循环
    if (node_->_loop != UINT8_MAX) {
      node_->_loop--;
    }

    // 这里判断是因为 如果结果处理函数中修改了timer则不更改
    if (node_->_timer == 0) {
      // reload
      node_->_timer = node_->_loop_timer;
    }
    return true;
  }
}

uint32_t MONO_TimerInnerHandler(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT) {
  if (!queue_->_run_status) {
    // 未开启
    return 0;
  }
  uint32_t resultCount = 0;
  // Lock
  if (!MONO_TryLockTimerQueue(queue_)) {
    return UINT32_MAX;
  }

  MONO_TimerRunning(queue_);

  MONO_PriorityTimerNode_t *runableNode = NULL;

  while ((runableNode = MONO_PopRunableNode(queue_, true)) != NULL) {
    if (MONO_RunNode(runableNode)) {
      MONO_PushNode(queue_, runableNode);
    } else {
      MONO_DeallocNode(runableNode);
    }
    resultCount++;
  }

  MONO_UnlockTimerQueue(queue_);

  return resultCount;
}

uint32_t MONO_TimerHandler(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT) {
  if (!queue_->_run_status) {
    // 未开启
    return 0;
  }
  // Lock
  if (!MONO_TryLockTimerQueue(queue_)) {
    return UINT32_MAX;
  }

  uint32_t resultCount = 0;

  MONO_PriorityTimerNode_t *runableNode = NULL;

  while ((runableNode = MONO_PopRunableNode(queue_, false)) != NULL) {
    if (MONO_RunNode(runableNode)) {
      MONO_PushNode(queue_, runableNode);
    } else {
      MONO_DeallocNode(runableNode);
    }
  }

  MONO_UnlockTimerQueue(queue_);

  return resultCount;
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
      return tempNode;
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
      return tempNode;
    }
    prevNode = tempNode;
    tempNode = tempNode->_next;
  }

  return NULL;
}

bool MONO_SetTimerNodeEnable(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT,
                             MONO_NodeTimer_t id_, bool enable_) {
  // 跳过无效参数
  if ((id_ == UINT32_MAX) || (id_ == 0)) {
    return false;
  }

  // 开锁
  if (!MONO_TryLockTimerQueue(queue_)) {
    return false;
  }

  MONO_PriorityTimerNode_t *findNode = MONO_FindNodeById(queue_, id_);
  if (findNode == NULL) {
    return false;
  }
  // 判断是否不需要操作
  if (findNode->_enabled == enable_) {
    return true;
  }

  // 需要操作
  MONO_PriorityTimerNode_t *popNode = MONO_PopNodeById(queue_, id_);
  if (popNode == NULL) {
    return false;
  }
  popNode->_enabled = enable_;
  bool result = (MONO_PushNode(queue_, popNode) != UINT16_MAX);

  // 解锁
  MONO_UnlockTimerQueue(queue_);
  return result;
}

uint16_t MONO_PushNodeFullArguments(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT,
                                    MONO_NodeFunction_t node_func_,
                                    uint8_t inner_, uint8_t enabled_,
                                    MONO_NodeTimer_t timer_, uint8_t loop_,
                                    MONO_NodeTimer_t loop_timer_,
                                    uint8_t priority_, void *args_,
                                    MONO_NodeFunction_t performance_func_) {
  MONO_PriorityTimerNode_t node = {
      // #ifdef MONO_USE_FULL_PTN_MEMBER
      //       .CopyNode = MONO_CopyNode,
      //       .DeallocNode = MONO_DeallocNode,
      //       .ExecuteNode = MONO_ExecuteNode,
      //       .RegisterResultPerformance = MONO_RegisterResultPerformance,
      // #endif
      ._args = args_,
      ._enabled = enabled_,
      ._func = node_func_,
      ._id = MONO_g_ptn_auto_id++,
      ._inner = inner_,
      ._loop = loop_,
      ._loop_timer = loop_timer_,
      ._performance_func = performance_func_,
      ._priority = priority_,
      ._timer = timer_};

  MONO_PriorityTimerNode_t *newNode = MONO_AllocNode();
  MONO_CopyNode(newNode, &node);

  return MONO_PushNode(queue_, newNode);

  // #ifdef MONO_USE_FULL_PTQ_MEMBER
  //   return queue_->PushNode(queue_, &node);
  // #else
  //   return MONO_PushNode(queue_, &node);
  // #endif
}

uint32_t MONO_GetTimeTick(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT) {
  return queue_->_timer_tick;
}

#ifdef MONO_PTQ_DEBUG

#include <stdio.h>
static void PrintNodeHeader() {
  printf("%-4s  %-7s  %-5s  %-4s  %-6s  %-6s \n", "id", "enabled", "timer",
         "loop", "reload", "where");
}
static void PrintNode(MONO_PriorityTimerNode_t *temp_node_) {
  printf("%-4d  %-7s  %-5d  %-4d  %-6d  %-6s \n", temp_node_->_id,
         temp_node_->_enabled ? "true" : "false", temp_node_->_timer,
         temp_node_->_loop, temp_node_->_loop_timer,
         temp_node_->_inner ? "inner" : "outside");
}

void MONO_QueueTaskInfo(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT) {
  if (queue_ == NULL) {
    printf("Error Queue NULL!\n");
    return;
  }

  printf("\n");
  printf("----------------  Task Info  -----------------\n");
  printf("%-5s  %-6s  %-5s \n", "size", "status", "tick");
  printf("%-5d  %-6s  %-5d \n", queue_->_size,
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
  printf("Empty \n");
}

#endif
