#include "priority_timer_queue.h"

MONO_PriorityTimerQueue *MONO_CreatePriorityQueue() {
  MONO_PriorityTimerQueue *queue = MONO_ALLOC_QUEUE();
  queue->nodes = MONO_ALLOC_NODES(MONO_DEFAULT_LENGTH);
  queue->capacity = MONO_DEFAULT_LENGTH;
  queue->size = 0;

#ifdef MONO_USE_FULL_PTQ_MEMBER

  queue->IncreaseNodes = &MONO_IncreaseNodes;
  queue->DeallocateQueueMemory = &MONO_DeallocateQueueMemory;
  queue->RankTimer = &MONO_RankTimer;
  queue->FindNodeById = &MONO_FindNodeById;
  queue->PushNode = &MONO_PushNode;
  queue->PushNodeRelease = &MONO_PushNodeRelease;
  queue->TimerRunning = &MONO_TimerRunning;
  queue->PopNode = &MONO_PopNode;
  queue->PopNodeById = &MONO_PopNodeById;
  queue->EraseNode = &MONO_EraseNode;
  queue->RunTimerNode = &MONO_RunTimerNode;
  queue->RunInnerTimerNode = &MONO_RunInnerTimerNode;
  queue->Size = &MONO_Size;
  queue->PushNodeFullArguments = &MONO_PushNodeFullArguments;

#endif
  return queue;
}

void MONO_IncreaseNodes(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT) {
  // 增加最大容纳大小
  queue_->capacity += MONO_DEFAULT_INCREASED_CAPACITY;

  // 重新分配队列的大小，使其增加原来的加上步长个空间
  MONO_PriorityTimerNode *nodes_ = (MONO_PriorityTimerNode *)reallocarray(
      queue_->nodes, MONO_NODE_SIZE, queue_->capacity);
  if (nodes_ == NULL) {
    // 需要的空间大于所剩余的空间
    // 错误处理
  } else {
    // 使queue_timer_nodes指向queue_timer_nodes_
    queue_->nodes = nodes_;
  }
  // 解除指针引用
  nodes_ = NULL;
}

void MONO_DeallocateQueueMemory(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT) {
  memset(queue_->nodes, 0, queue_->capacity * MONO_NODE_SIZE);
  free(queue_->nodes);
  queue_->nodes = NULL;

  memset(queue_, 0, sizeof(MONO_PriorityTimerQueue));
  free(queue_);
  queue_ = NULL;
}

void MONO_RankTimer(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT) {
  if (queue_->size > 0) {
    MONO_PriorityTimerNode *temp;
    // 分配临时内存用于交换内存数据
    temp = MONO_ALLOC_NODE();

    // node_timer_t temp_timer = QUEUE_TIMER_TYPE_MAX;

    for (size_t i = (queue_->size - 1); i > 0; i--) {
      // 排序指标为节点的剩余时钟周期数_timer
      if (queue_->nodes[i]._timer < queue_->nodes[i - 1]._timer) {
        memmove(temp, (queue_->nodes + (i - 1)), MONO_NODE_SIZE);
        memmove((queue_->nodes + (i - 1)), (queue_->nodes + i), MONO_NODE_SIZE);
        memmove((queue_->nodes + i), temp, MONO_NODE_SIZE);
        memset(temp, 0, MONO_NODE_SIZE);
      }

      if ((queue_->nodes[i]._timer == queue_->nodes[i - 1]._timer) &&
          (queue_->nodes[i]._priority != queue_->nodes[i - 1]._priority)) {
        if (queue_->nodes[i]._priority >
            queue_->nodes[i - 1]._priority) // 优先级大于
        {
          memmove(temp, (queue_->nodes + (i - 1)), MONO_NODE_SIZE);
          memmove((queue_->nodes + (i - 1)), (queue_->nodes + i),
                  MONO_NODE_SIZE);
          memmove((queue_->nodes + i), temp, MONO_NODE_SIZE);
          memset(temp, 0, MONO_NODE_SIZE);
        }
      }
    }

// 释放临时内存
#ifdef MONO_USE_FULL_PTQ_MEMBER
    temp->DeallocNode(temp);
#else

    MONO_DeallocNode(temp);
#endif
  }
}

MONO_PriorityTimerNode *
MONO_FindNodeById(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT, uint16_t id_) {
  for (size_t i = 0; i < queue_->size; i++) {
    if ((queue_->nodes + i)->_id == id_) {
      return (queue_->nodes + i);
    }
  }
  return NULL;
}

uint16_t MONO_PushNode(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT,
                       MONO_PriorityTimerNode *node_) {
#ifdef MONO_USE_FULL_PTQ_MEMBER
  (queue_->nodes + queue_->size)
      ->CopyNode((queue_->nodes + queue_->size), node_);
  queue_->size++;
#else
  MONO_CopyNode((queue_->nodes + queue_->size), node_);
  queue_->size++;
#endif

  return node_->_id;
}

uint16_t MONO_PushNodeRelease(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT,
                              MONO_PriorityTimerNode *node_) {

  uint16_t id;
#ifdef MONO_USE_FULL_PTQ_MEMBER
  id = queue_->PushNode(queue_, node_);
  node_->DeallocNode(node_);
#else
  id = MONO_PushNode(queue_, node_);
  MONO_DeallocNode(node_);
#endif
  return id;
}

void MONO_TimerRunning(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT) {

#ifdef MONO_USE_FULL_PTQ_MEMBER
  if (queue_->Size(queue_) != 0) {
#else
  if (MONO_Size(queue_) != 0) {
#endif
    for (size_t i = 0; i < queue_->size; i++) {
      MONO_PriorityTimerNode *node = (queue_->nodes + i);
      if (node->_timer > 0) {
        node->_timer--;
      }
    }
#ifdef MONO_USE_FULL_PTQ_MEMBER
    queue_->RankTimer(queue_);
#else
    MONO_RankTimer(queue_);
#endif
  }
}

MONO_PriorityTimerNode *
MONO_PopNode(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT) {
#ifdef MONO_USE_FULL_PTQ_MEMBER
  return queue_->EraseNodeByIndex(queue_, 0);
#else
  return MONO_EraseNodeByIndex(queue_, 0);
#endif
}

MONO_PriorityTimerNode *
MONO_PopNodeById(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT, uint16_t id_) {
  MONO_PriorityTimerNode *node = NULL;
  for (size_t i = 0; i < queue_->size; i++) {
    if ((queue_->nodes + i)->_id == id_) {
      node = MONO_ALLOC_NODE();
#ifdef MONO_USE_FULL_PTQ_MEMBER
      node->CopyNode(node, queue_->nodes + i);
      return queue_->EraseNodeByIndex(queue_, i);
#else
      MONO_CopyNode(node, queue_->nodes + i);
      return MONO_EraseNodeByIndex(queue_, i);
#endif
    }
  }
  return node;
}

MONO_PriorityTimerNode *
MONO_EraseNode(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT,
               MONO_PriorityTimerNode *node_) {
  MONO_PriorityTimerNode *node = NULL;
  for (size_t i = 0; i < queue_->size; i++) {
    if ((queue_->nodes + i)->_id == node_->_id) {
      node = MONO_ALLOC_NODE();
#ifdef MONO_USE_FULL_PTQ_MEMBER
      node->CopyNode(node, queue_->nodes + i);
      return queue_->EraseNodeByIndex(queue_, i);
#else
      MONO_CopyNode(node, queue_->nodes + i);
      return MONO_EraseNodeByIndex(queue_, i);
#endif
    }
  }
  return NULL;
}

MONO_PriorityTimerNode *
MONO_EraseNodeByIndex(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT,
                      size_t index) {
#ifdef MONO_USE_FULL_PTQ_MEMBER
  if (queue_->Size(queue_) != 0) {
#else
  if (MONO_Size(queue_) != 0) {
#endif

    MONO_PriorityTimerNode *node = MONO_ALLOC_NODE();

    memmove(node, (queue_->nodes + index), MONO_NODE_SIZE);

    memset((queue_->nodes + index), 0, MONO_NODE_SIZE);

    memmove((queue_->nodes + index), (queue_->nodes + index + 1),
            ((queue_->size - index) * MONO_NODE_SIZE));

    memset((queue_->nodes + (queue_->size - 1)), 0, MONO_NODE_SIZE);

    queue_->size--;

#ifdef MONO_USE_FULL_PTQ_MEMBER
    queue_->RankTimer(queue_);
#else
    MONO_RankTimer(queue_);
#endif
  return node;
  }
  return NULL;
}
void MONO_RunTimerNode(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT) {
#ifdef MONO_USE_FULL_PTQ_MEMBER
  if (queue_->Size(queue_) != 0) {
#else
  if (MONO_Size(queue_) != 0) {
#endif

    size_t position_index = 0;
    size_t now_size = queue_->size;
    for (size_t i = 0; i < now_size; i++) {
      // 如果未使能则跳过
      if (queue_->nodes[position_index]._enabled == 0 ||
          queue_->nodes[position_index]._inner != 0) {
        position_index++;
        continue;
      }
      if (queue_->nodes[position_index]._timer <= 0) {
        MONO_PriorityTimerNode *node;
        // 取队列中top0的节点
#ifdef MONO_USE_FULL_PTQ_MEMBER
        node = queue_->PopNodeById(queue_, queue_->nodes->_id);
        node->ExecuteNode(node);
#else
        node = MONO_PopNodeById(queue_, queue_->nodes->_id);
        MONO_ExecuteNode(node);
#endif
      } else {
        // 当定时器不为0时跳出循环
        break;
      }
    }
#ifdef MONO_USE_FULL_PTQ_MEMBER
    queue_->RankTimer(queue_);
#else
    MONO_RankTimer(queue_);
#endif
  }
}

void MONO_RunInnerTimerNode(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT) {
#ifdef MONO_USE_FULL_PTQ_MEMBER
  if (queue_->Size(queue_) != 0) {
#else
  if (MONO_Size(queue_) != 0) {
#endif

    size_t position_index = 0;
    size_t now_size = queue_->size;
    for (size_t i = 0; i < now_size; i++) {
      // 如果未使能则跳过
      if (queue_->nodes[position_index]._enabled == 0 ||
          queue_->nodes[position_index]._inner == 0) {
        position_index++;
        continue;
      }
      if (queue_->nodes[position_index]._timer <= 0) {
        MONO_PriorityTimerNode *node;
        // 取队列中top0的节点
#ifdef MONO_USE_FULL_PTQ_MEMBER
        node = queue_->PopNodeById(queue_, queue_->nodes->_id);
        node->ExecuteNode(node);
#else
        node = MONO_PopNodeById(queue_, queue_->nodes->_id);
        MONO_ExecuteNode(node);
#endif
      } else {
        // 当定时器不为0时跳出循环
        break;
      }
    }
#ifdef MONO_USE_FULL_PTQ_MEMBER
    queue_->RankTimer(queue_);
#else
    MONO_RankTimer(queue_);
#endif
  }
}

uint8_t MONO_Size(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT) {
  return queue_->size;
}

uint16_t MONO_PushNodeFullArguments(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT,
                                    MONO_NodeFunction_t node_func_,
                                    uint8_t inner_, uint8_t enabled_,
                                    MONO_NodeTimer_t timer_, uint8_t loop_,
                                    MONO_NodeTimer_t loop_timer_,
                                    uint8_t priority_, void *args_,
                                    MONO_NodeFunction_t performance_func_) {
  MONO_PriorityTimerNode node = {

#ifdef MONO_USE_FULL_PTQ_MEMBER
      .CopyNode = MONO_CopyNode,
      .DeallocNode = MONO_DeallocNode,
      .ExecuteNode = MONO_ExecuteNode,
      .RegisterResultPerformance = MONO_RegisterResultPerformance,
#endif
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

#ifdef MONO_USE_FULL_PTQ_MEMBER
  return queue_->PushNode(queue_, &node);
#else
  return MONO_PushNode(queue_, &node);
#endif
}
