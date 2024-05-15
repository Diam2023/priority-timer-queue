#include "priority_timer.h"

/************************** NODE **************************/

void MONO_ExecuteNode(MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT) {
    if (node_ != NULL) {
        // 通过节点储存的参数数据来执行节点函数
        void *result = NULL;
        result = node_->_func(node_->_args);

        if (node_->_performance_func != NULL) {
            node_->_performance_func(result);
        }
    }
}

void MONO_DeallocNode(MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT) {
    memset(node_, 0, sizeof(MONO_PriorityTimerNode));
    free(node_);
}

void MONO_CopyNode(MONO_PriorityTimerNode *node_dest_,
                   MONO_PriorityTimerNode *node_src_) {
    memmove(node_dest_, node_src_, sizeof(MONO_PriorityTimerNode));
}

MONO_PriorityTimerNode *
MONO_SetTimer(MONO_NodeTimer_t timer_,
              MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT) {
    node_->_timer = timer_;
    return node_;
}


MONO_PriorityTimerNode *
MONO_SetEnabled(uint8_t enabled_, MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT) {
    node_->_enabled = enabled_;
    return node_;
}


MONO_PriorityTimerNode *
MONO_SetLoop(uint8_t loop_, MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT) {
    node_->_loop = loop_;
    // 若节点_loop_timer为0且_time不为0则设置_loop_timer的值为_timer的值
    if (node_->_loop_timer == 0 && node_->_timer != 0) {
        node_->_loop_timer = node_->_timer;
    }
    return node_;
}

MONO_PriorityTimerNode *
MONO_SetLoopTimer(MONO_NodeTimer_t loop_timer_,
                  MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT) {
    node_->_loop_timer = loop_timer_;
    return node_;
}

MONO_PriorityTimerNode *
MONO_SetPriority(uint8_t priority_, MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT) {
    node_->_priority = priority_;
    return node_;
}

MONO_PriorityTimerNode *
MONO_SetArgs(void *args_, MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT);

void MONO_RegisterResultPerformance(MONO_NodeFunction_t func_,
                                    MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT) {
    node_->_performance_func = func_;
}

MONO_PriorityTimerNode *
MONO_CreateQueueNodeFull(MONO_NodeFunction_t node_func_, uint8_t inner_,
                         uint8_t enabled_, MONO_NodeTimer_t timer_, uint8_t loop_,
                         MONO_NodeTimer_t loop_timer_, uint8_t priority_,
                         void *args_, MONO_NodeFunction_t performance_func_) {
    MONO_PriorityTimerNode *node = MONO_ALLOC_NODE();

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

#ifdef MONO_USE_FULL_PTN_MEMBER

    // 注册函数指针

    node->ExecuteNode = &MONO_ExecuteNode;

    node->DeallocNode = &MONO_DeallocNode;

    node->CopyNode = &MONO_CopyNode;

    node->RegisterResultPerformance = &MONO_RegisterResultPerformance;

    node->SetEnabled = &MONO_SetEnabled;
    node->SetTimer = &MONO_SetTimer;
    node->SetPriority = &MONO_SetPriority;
    node->SetLoop = &MONO_SetLoop;
    node->SetLoopTimer = &MONO_SetLoopTimer;
    node->SetArgs = &MONO_SetArgs;

#endif
    return node;
}

/************************** TIMER **************************/

/**
 * @brief TimerTick计数
 * @since v1.4
 */
static uint32_t s_timerTick = 0;

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
    queue->EraseNodeByIndex = &MONO_EraseNodeByIndex;
    queue->PushNodeFullArguments = &MONO_PushNodeFullArguments;

#endif
    return queue;
}

void MONO_IncreaseNodes(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT) {
    // 增加最大容纳大小
    queue_->capacity += MONO_DEFAULT_INCREASED_CAPACITY;

    // 重新分配队列的大小，使其增加原来的加上步长个空间
    MONO_PriorityTimerNode *nodes_ = (MONO_PriorityTimerNode *) reallocarray(
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

        for (size_t i = (queue_->size - 1); i > 0; i--) {
            // 排序指标为节点的剩余时钟周期数_timer
            if (queue_->nodes[i]._timer < queue_->nodes[i - 1]._timer) {
                memmove(temp, (queue_->nodes + (i - 1)), MONO_NODE_SIZE);
                memmove((queue_->nodes + (i - 1)), (queue_->nodes + i), MONO_NODE_SIZE);
                memmove((queue_->nodes + i), temp, MONO_NODE_SIZE);
            }

            if ((queue_->nodes[i]._timer == queue_->nodes[i - 1]._timer) &&
                (queue_->nodes[i]._priority != queue_->nodes[i - 1]._priority)) {
                if (queue_->nodes[i]._priority >
                    queue_->nodes[i - 1]._priority)// 优先级大于
                {
                    memmove(temp, (queue_->nodes + (i - 1)), MONO_NODE_SIZE);
                    memmove((queue_->nodes + (i - 1)), (queue_->nodes + i),
                            MONO_NODE_SIZE);
                    memmove((queue_->nodes + i), temp, MONO_NODE_SIZE);
                }
            }
        }
// 释放临时内存
#ifdef MONO_USE_FULL_PTN_MEMBER
        queue_->nodes->DeallocNode(temp);
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
#ifdef MONO_USE_FULL_PTN_MEMBER
    node_->CopyNode((queue_->nodes + queue_->size), node_);
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
#else
    id = MONO_PushNode(queue_, node_);
#endif
#ifdef MONO_USE_FULL_PTN_MEMBER
    node_->DeallocNode(node_);
#else
    MONO_DeallocNode(node_);
#endif
    return id;
}

void MONO_TimerRunning(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT) {
    s_timerTick++;
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
    for (size_t i = 0; i < queue_->size; i++) {
        if ((queue_->nodes + i)->_id == id_) {
#ifdef MONO_USE_FULL_PTQ_MEMBER
            return queue_->EraseNodeByIndex(queue_, i);
#else
            return MONO_EraseNodeByIndex(queue_, i);
#endif
        }
    }
    return NULL;
}

MONO_PriorityTimerNode *
MONO_EraseNode(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT,
               MONO_PriorityTimerNode *node_) {
    for (size_t i = 0; i < queue_->size; i++) {
        if ((queue_->nodes + i)->_id == node_->_id) {
#ifdef MONO_USE_FULL_PTQ_MEMBER
            return queue_->EraseNodeByIndex(queue_, i);
#else
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
            if (queue_->nodes[position_index]._id == 0) {
                continue;
            }
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
#else
                node = MONO_PopNodeById(queue_, queue_->nodes->_id);
#endif
#ifdef MONO_USE_FULL_PTN_MEMBER
                node->ExecuteNode(node);
#else
                MONO_ExecuteNode(node);
#endif
                if (node->_loop < UINT8_MAX && node->_loop > 0) {
                    node->_loop--;
                }
                if (node->_timer <= 0) {
                    node->_timer = node->_loop_timer;
                }
                if (node->_loop > 0) {
#ifdef MONO_USE_FULL_PTQ_MEMBER
                    queue_->PushNodeRelease(queue_, node);
#else
                    MONO_PushNodeRelease(queue_, node);
#endif
                }
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
            if (queue_->nodes[position_index]._id == 0) {
                continue;
            }
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
#else
                node = MONO_PopNodeById(queue_, queue_->nodes->_id);
#endif
#ifdef MONO_USE_FULL_PTN_MEMBER
                node->ExecuteNode(node);
#else
                MONO_ExecuteNode(node);
#endif
                if (node->_loop < UINT8_MAX && node->_loop > 0) {
                    node->_loop--;
                }
                if (node->_timer <= 0) {
                    node->_timer = node->_loop_timer;
                }
                if (node->_loop > 0) {
#ifdef MONO_USE_FULL_PTQ_MEMBER
                    queue_->PushNodeRelease(queue_, node);
#else
                    MONO_PushNodeRelease(queue_, node);
#endif
                }
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

#ifdef MONO_USE_FULL_PTN_MEMBER
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

uint32_t MONO_GetTimeTick(void) {
    return s_timerTick;
}
