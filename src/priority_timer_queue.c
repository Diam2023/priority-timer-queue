#include "priority_timer_queue.h"

// void init_priority_queue()
// {
//     // 初始化为空指针
//     queue_timer_nodes = NULL;

//     // 分配默认内存
//     queue_timer_nodes = ALLOC_NODES(DEFAULT_INCREASED_LENGTH);

//     // 判断是否为空指针
//     if (queue_timer_nodes == NULL)
//     {
//         // 可用堆大小不足
//     }
//     else
//     {
//         // 初始化队列节点个数以及分配的大小
//         queue_timer_node_size = DEFAULT_SIZE_ZERO;
//         queue_timer_node_length = DEFAULT_LENGTH;
//     }
// }
// void increase_nodes()
// {
//     // 增加最大容纳大小
//     queue_timer_node_length += DEFAULT_INCREASED_LENGTH;

//     // 重新分配队列的大小，使其增加原来的加上步长个空间
//     priority_timer_node_t *queue_timer_nodes_ = (priority_timer_node_t *)
//         reallocarray(queue_timer_nodes, MONO_NODE_SIZE,
//         queue_timer_node_length);
//     if (queue_timer_nodes_ == NULL)
//     {
//         // 需要的空间大于所剩余的空间
//         // 错误处理
//     }
//     else
//     {
//         // 使queue_timer_nodes指向queue_timer_nodes_
//         queue_timer_nodes = queue_timer_nodes_;
//     }
//     // 解除指针引用
//     queue_timer_nodes_ = NULL;
// }

// void deallocate_node(priority_timer_node_t *node_)
// {
//     // 回收缓存数据的内存
//     memset(node_, 0, MONO_NODE_SIZE);
//     free(node_);
//     node_ = NULL;
// }

// void deallocate_queue_memory()
// {
//     // 回收elements的内存
//     memset(queue_timer_nodes, 0, queue_timer_node_length * MONO_NODE_SIZE);
//     free(queue_timer_nodes);
//     queue_timer_nodes = NULL;

//     // 记零
//     queue_timer_node_size = 0;
//     queue_timer_node_length = 0;
// }

// void rank_timer()
// {
//     if (size() > 1)
//     {
//         priority_timer_node_t *temp;
//         // 分配临时内存用于交换内存数据
//         temp = ALLOC_NODE();

//         // node_timer_t temp_timer = QUEUE_TIMER_TYPE_MAX;

//         for (size_t i = (queue_timer_node_size - 1); i > 0; i--)
//         {
//             // 排序指标为节点的剩余时钟周期数_timer
//             if (queue_timer_nodes[i]._timer < queue_timer_nodes[i -
//             1]._timer)
//             {
//                 memmove(temp, (queue_timer_nodes + (i - 1)), MONO_NODE_SIZE);
//                 memmove((queue_timer_nodes + (i - 1)), (queue_timer_nodes +
//                 i), MONO_NODE_SIZE); memmove((queue_timer_nodes + i), temp,
//                 MONO_NODE_SIZE); memset(temp, 0, MONO_NODE_SIZE);
//             }

//             if ((queue_timer_nodes[i]._timer == queue_timer_nodes[i -
//             1]._timer) && (queue_timer_nodes[i]._priority !=
//             queue_timer_nodes[i - 1]._priority))
//             {
//                 if (queue_timer_nodes[i]._priority > queue_timer_nodes[i -
//                 1]._priority) // 优先级大于
//                 {
//                     memmove(temp, (queue_timer_nodes + (i - 1)),
//                     MONO_NODE_SIZE); memmove((queue_timer_nodes + (i - 1)),
//                     (queue_timer_nodes
//                     + i), MONO_NODE_SIZE); memmove((queue_timer_nodes + i),
//                     temp, MONO_NODE_SIZE); memset(temp, 0, MONO_NODE_SIZE);
//                 }
//             }
//         }

//         // 释放临时内存
//         deallocate_node(temp);
//     }
// }

// priority_timer_node_t *find_node_by_function(node_func_t func_)
// {
//     priority_timer_node_t *temp_node_;
//     temp_node_ = NULL;
//     if (!empty())
//     {
//         for (size_t i = 0; i < size(); i++)
//         {
//             if (queue_timer_nodes[i]._function == func_)
//             {
//                 temp_node_ = &queue_timer_nodes[i];
//             }
//         }
//     }

//     return temp_node_;
// }

// void push_node_release(priority_timer_node_t *node_)
// {
//     free(__push_node(node_));
// }

// priority_timer_node_t *__push_node(priority_timer_node_t *node_)
// {
//     // 增加一个节点到队列中
//     if ((queue_timer_node_size + 1) >= queue_timer_node_length)
//     {
//         // 但队列容纳不下的时候就扩容
//         increase_nodes();
//     }

//     // 将节点数据复制到队列中
//     memmove((queue_timer_nodes + queue_timer_node_size), node_,
//     MONO_NODE_SIZE); (queue_timer_nodes + queue_timer_node_size)->_args =
//     node_->_args;

//     queue_timer_node_size++;

//     // 排序
//     rank_timer();

//     return node_;
// }

// priority_timer_node_t *pop_node()
// {
//     return pop_index_node(TOP_NODE_INDEX);
// }

// priority_timer_node_t *pop_index_node(size_t index_)
// {
//     priority_timer_node_t *temp_node_;

//     temp_node_ = NULL;

//     if (!empty() && index_ < size())
//     {
//         temp_node_ = ALLOC_NODE();

//         memmove(temp_node_, (queue_timer_nodes + index_), MONO_NODE_SIZE);

//         memset((queue_timer_nodes + index_), 0, MONO_NODE_SIZE);

//         memmove((queue_timer_nodes + index_), (queue_timer_nodes + (index_ +
//         1)), ((queue_timer_node_size - index_) * MONO_NODE_SIZE));

//         memset((queue_timer_nodes + (queue_timer_node_size - 1)), 0,
//         MONO_NODE_SIZE);

//         queue_timer_node_size--;

//         rank_timer();
//     }

//     return temp_node_;
// }

// void erase_node(priority_timer_node_t *node_)
// {
//     if (!empty())
//     {
//         // index 用于标记是队列中的哪一个需要移除
//         int index = -1;
//         // 遍历队列
//         for (size_t i = 0; i < size(); i++)
//         {
//             if (&queue_timer_nodes[i] == node_)
//             {
//                 // 符合条件则返回下标
//                 index = i;
//                 break;
//             }
//         }

//         if (index != -1)
//         {
//             priority_timer_node_t *temp = pop_index_node(index);

//             deallocate_node(temp);
//         }
//     }
// }

// void execute_node(priority_timer_node_t *node_)
// {
//     // 通过节点储存的参数数据来执行节点函数
//     void *result = NULL;
//     result = node_->_function(node_->_args);

//     if (node_->_performance_func != NULL)
//     {
//         node_->_performance_func(result);
//     }
//     result = NULL;

//     if (node_->_loop != 0)
//     {
//         if (node_->_loop != UINT8_MAX) // 循环定时执行loop次
//         {
//             node_->_loop--; // 计数器减一
//         }
//         // 循环和执行时重置计数周期并加入到队列
//         if (node_->_timer == 0)
//         {
//             node_->_timer = node_->_loop_timer;
//         }
//         __push_node(node_);
//     }

//     // 满足清理条件

//     deallocate_node(node_);
// }

// void run_timer_node()
// {
//     if (!empty())
//     {
//         size_t position_index = 0;
//         size_t now_size = queue_timer_node_size;
//         for (size_t i = 0; i < now_size; i++)
//         {
//             // 如果未使能则跳过
//             if (queue_timer_nodes[position_index]._enabled == 0 ||
//             queue_timer_nodes[position_index]._inner != 0)
//             {
//                 position_index++;
//                 continue;
//             }
//             if (queue_timer_nodes[position_index]._timer <= 0)
//             {
//                 priority_timer_node_t *temp_node_;
//                 // 取队列中top0的节点
//                 temp_node_ = pop_index_node(position_index);

//                 execute_node(temp_node_);
//             }
//             else
//             {
//                 // 当定时器不为0时跳出循环
//                 break;
//             }
//         }
//     }
// }

// void run_inner_timer_node()
// {
//     if (!empty())
//     {
//         size_t position_index = 0;
//         size_t now_size = queue_timer_node_size;
//         for (size_t i = 0; i < now_size; i++)
//         {
//             // 如果未使能或者没开启内部执行则跳过
//             if (queue_timer_nodes[position_index]._inner == 0 ||
//             queue_timer_nodes[position_index]._enabled == 0)
//             {
//                 position_index++;
//                 continue;
//             }

//             if (queue_timer_nodes[position_index]._timer <= 0)
//             {
//                 // 取队列中top0的节点
//                 priority_timer_node_t *temp = pop_index_node(position_index);

//                 execute_node(temp);
//             }
//             else
//             {
//                 // 当定时器不为0时跳出循环
//                 break;
//             }
//         }
//     }
// }

// inline void timer_running()
// {
//     if (!empty())
//     {
//         for (size_t i = 0; i < size(); i++)
//         {
//             // 在使能与定时周期未结束时运行
//             if (queue_timer_nodes[i]._enabled != 0 &&
//             queue_timer_nodes[i]._timer > 0)
//             {
//                 queue_timer_nodes[i]._timer--;
//             }
//         }
//     }

//     rank_timer();
// }

// size_t size()
// {
//     return queue_timer_node_size;
// }

// uint8_t empty()
// {
//     return (uint8_t)(queue_timer_node_size == 0);
// }

// priority_timer_node_t *set_enabled(uint8_t enabled_, priority_timer_node_t
// *node_)
// {
//     node_->_enabled = enabled_;
//     return node_;
// }

// priority_timer_node_t *set_timer(node_timer_t timer_, priority_timer_node_t
// *node_)
// {
//     node_->_timer = timer_;
//     return node_;
// }

// priority_timer_node_t *set_loop(uint8_t loop_, priority_timer_node_t *node_)
// {
//     node_->_loop = loop_;
//     // 若节点_loop_timer为0且_time不为0则设置_loop_timer的值为_timer的值
//     if (node_->_loop_timer == 0 && node_->_timer != 0)
//     {
//         node_->_loop_timer = node_->_timer;
//     }
//     return node_;
// }

// priority_timer_node_t *set_loop_timer(node_timer_t loop_timer_,
// priority_timer_node_t *node_)
// {
//     node_->_loop_timer = loop_timer_;
//     return node_;
// }

// priority_timer_node_t *set_priority(uint8_t priority_, priority_timer_node_t
// *node_)
// {
//     node_->_priority = priority_;
//     return node_;
// }

// priority_timer_node_t *set_args(void *args_, priority_timer_node_t *node_)
// {
//     node_->_args = args_;
//     return node_;
// }

// void register_result_performance(node_func_t func_, priority_timer_node_t
// *node_)
// {
//     node_->_performance_func = func_;
// }

// priority_timer_node_t *__create_queue_node(node_func_t node_func_, uint8_t
// inner_, uint8_t enabled_, node_timer_t timer_, uint8_t loop_, node_timer_t
// loop_timer_, uint8_t priority_, void *args_, node_func_t performance_func_)
// {
//     priority_timer_node_t *temp_node_;

//     temp_node_ = ALLOC_NODE();

//     temp_node_->_function = node_func_;

//     temp_node_->_inner = inner_;

//     temp_node_->_enabled = enabled_;

//     temp_node_->_timer = timer_;

//     temp_node_->_loop = loop_;

//     temp_node_->_loop_timer = loop_timer_;

//     temp_node_->_priority = priority_;

//     temp_node_->_args = args_;

//     temp_node_->_performance_func = performance_func_;

//     return temp_node_;
// }

// priority_timer_node_t *create_timer_node(node_func_t node_func_, uint8_t
// inner_, node_timer_t timer_, void *args_)
// {
//     return __create_queue_node(
//         node_func_,
//         inner_,
//         1,
//         timer_,
//         0,
//         timer_,
//         UINT8_MAX,
//         args_,
//         NULL);
// }

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
  queue_->PushNode(queue_, &node);
#else
  MONO_PushNode(queue_, &node);
#endif
}
