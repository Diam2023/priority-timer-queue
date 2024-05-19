/**
 * @file priority_timer.c
 * @author Diam (monoliths-uni@outlook.com)
 * @brief
 * @version 2.2
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

MONO_PriorityTimerNode_t *
MONO_SetEnabled(uint8_t enabled_, MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT) {

  node_->_enabled = enabled_;
  return node_;
}

MONO_PriorityTimerNode_t *
MONO_SetTimer(MONO_NodeTimer_t timer_,
              MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT) {
  node_->_timer = timer_;
  return node_;
}

MONO_PriorityTimerNode_t *
MONO_SetLoop(uint8_t loop_, MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT) {
  node_->_loop = loop_;
  // 若节点_loop_timer为0且_time不为0则设置_loop_timer的值为_timer的值
  if (node_->_loop_timer == 0 && node_->_timer != 0) {
    node_->_loop_timer = node_->_timer;
  }
  return node_;
}

MONO_PriorityTimerNode_t *
MONO_SetLoopTimer(MONO_NodeTimer_t loop_timer_,
                  MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT) {
  node_->_loop_timer = loop_timer_;
  return node_;
}

MONO_PriorityTimerNode_t *
MONO_SetPriority(uint8_t priority_, MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT) {
  node_->_priority = priority_;
  return node_;
}

MONO_PriorityTimerNode_t *
MONO_SetArgs(void *args_, MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT) {
  node_->_args = args_;
  return node_;
}

void MONO_RegisterResultPerformance(MONO_NodeFunction_t func_,
                                    MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT) {
  node_->_performance_func = func_;
}

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

MONO_PriorityTimerQueue_t *MONO_CreatePriorityQueue() {
  MONO_PriorityTimerQueue_t *queue = MONO_AllocTimerQueue();
  queue->_header_node = NULL;

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

MONO_PriorityTimerNode_t *
MONO_FindNodeById(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT,
                  MONO_NodeId_t id_) {
  if (queue_ == NULL || queue_->_size <= 0) {
    return NULL;
  }
  MONO_PriorityTimerNode_t *tempNode = queue_->_header_node;
  while (tempNode != NULL) {
    if (tempNode->_id == id_) {
      return tempNode;
    }
    tempNode = tempNode->_next;
  }

  return NULL;
}

MONO_NodeId_t MONO_PushNode(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT,
                            MONO_PriorityTimerNode_t *node_) {
  MONO_PriorityTimerNode_t *tempNode = queue_->_header_node;
  MONO_PriorityTimerNode_t *prevNode = queue_->_header_node;

  // 没有头节点的情况
  if (tempNode == NULL) {
    queue_->_header_node = node_;
    queue_->_size = 1;
    return node_->_id;
  }

  // TODO 优化节点Push
  // 只有一个节点的情况
  if (tempNode->_next == NULL) {
    // 先比较执行时间
    if ((node_->_timer < tempNode->_timer) ||   // 时间小于上一个节点
        ((node_->_timer == tempNode->_timer) && // 时间等于上个节点并且
         (node_->_priority < tempNode->_priority) // 优先级小于上个节点
         )) {
      // 插入到这个节点的前面
      queue_->_header_node = node_;
      node_->_next = tempNode;
      tempNode->_next = NULL;
    } else {
      // 插入到这个节点的后面
      tempNode->_next = node_;
      node_->_next = NULL;
    }
    queue_->_size++;
    return node_->_id;
  }

  prevNode = tempNode;
  tempNode = tempNode->_next;
  // 其他
  // 首先查找到合适的timer位置
  while (tempNode != NULL) {

    if ((node_->_timer < tempNode->_timer) ||   // 时间小于这个节点
        ((node_->_timer == tempNode->_timer) && // 时间等于这个节点并且
         (node_->_priority < tempNode->_priority) // 优先级小于这个节点
         )) {
      // 插入到这个节点的前面

      // 将上一个节点的下一个节点值设为要插入的这个节点
      prevNode->_next = node_;
      // 要插入这个节点的下一个节点值设置为当前几点值
      node_->_next = tempNode;
      break;
    }

    prevNode = tempNode;
    tempNode = tempNode->_next;
  }
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
  queue_->_timer_tick++;
  MONO_PriorityTimerNode_t *tempNode = queue_->_header_node;
  while (tempNode != NULL) {
    if (tempNode->_timer > 0) {
      tempNode->_timer--;
    } else {
      // TODO 检查节点有效性执行
    }
    tempNode = tempNode->_next;
  }
}

MONO_PriorityTimerNode_t *
MONO_PopNode(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT) {
  (void)queue_;
  return NULL;
  // TODO 待实现
  // #ifdef MONO_USE_FULL_PTQ_MEMBER
  //   return queue_->EraseNodeByIndex(queue_, 0);
  // #else
  //   return MONO_EraseNodeByIndex(queue_, 0);
  // #endif
}

MONO_PriorityTimerNode_t *
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
    if ((tempNode->_inner == inner_) && (tempNode->_enabled)) {
      if (prevNode != NULL) {
        prevNode->_next = tempNode->_next;
      }
      if (tempNode == queue_->_header_node) {
        queue_->_header_node = NULL;
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
    node_->_performance_func(node_, node_->_func(node_, node_->_args));
  } else {
    node_->_func(node_, node_->_args);
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
  uint32_t resultCount = 0;
  // Lock
  if (!MONO_TryLockTimerQueue(queue_)) {
    return UINT32_MAX;
  }

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

  // #ifdef MONO_USE_FULL_PTQ_MEMBER
  //   return queue_->PushNode(queue_, &node);
  // #else
  //   return MONO_PushNode(queue_, &node);
  // #endif
}

uint32_t MONO_GetTimeTick(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT) {
  return queue_->_timer_tick;
}
