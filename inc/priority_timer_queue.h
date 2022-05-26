/**
 * @file priority_timer_node.h
 *
 * @brief
 *
 * @author monoliths (monoliths-uni@outlook.com)
 * @version 1.3
 * @date 2022-05-23
 *
 * *********************************************************************************
 *
 * @note version:1.0
 * @description: 基础QUEUE方法.
 *
 * @note version:1.1
 * @description: 功能性扩展
 * * @details 添加了处理返回值的回调函数.
 *
 * @note version: 1.2
 * @description: bug
 * * @details 解决不能实时更改timer问题
 *
 * @note version: 1.3
 * @description: 重构代码
 * @date 2022-05-23 10:06:31
 *
 * *********************************************************************************
 */

#ifndef PRIORITY_TIMER_QUEUE_INC_PRIORITY_TIMER_QUEUE_H_
#define PRIORITY_TIMER_QUEUE_INC_PRIORITY_TIMER_QUEUE_H_

#ifdef __cplusplus
extern "C" {
#endif

#define MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT                             \
  MONO_PriorityTimerQueue *queue_
#define MONO_ALLOC_QUEUE()                                                     \
  (MONO_PriorityTimerQueue *)calloc(1, sizeof(MONO_PriorityTimerQueue))

#define CreatePriorityTimerQueue() MONO_CreatePriorityQueue();

#include "priority_timer_node.h"

typedef struct MONO_PriorityTimerQueue_s {

  /**
   * @brief 节点储存的数组
   */
  MONO_PriorityTimerNode *nodes;

  /**
   * @brief 目前储存的节点个数
   */
  size_t size;

  /**
   * @brief 目前数组能储存的节点数
   */
  size_t capacity;

#ifdef MONO_USE_FULL_PTQ_MEMBER
  void (*IncreaseNodes)(struct MONO_PriorityTimerQueue_s *);
  void (*DeallocateQueueMemory)(struct MONO_PriorityTimerQueue_s *);
  void (*RankTimer)(struct MONO_PriorityTimerQueue_s *);
  MONO_PriorityTimerNode *(*FindNodeById)(struct MONO_PriorityTimerQueue_s *,
                                          uint16_t);
  uint16_t (*PushNode)(struct MONO_PriorityTimerQueue_s *,
                       MONO_PriorityTimerNode *);
  uint16_t (*PushNodeRelease)(struct MONO_PriorityTimerQueue_s *,
                              MONO_PriorityTimerNode *);
  void (*TimerRunning)(struct MONO_PriorityTimerQueue_s *);
  MONO_PriorityTimerNode *(*PopNode)(struct MONO_PriorityTimerQueue_s *);
  MONO_PriorityTimerNode *(*PopNodeById)(struct MONO_PriorityTimerQueue_s *,
                                         uint16_t);
  MONO_PriorityTimerNode *(*EraseNode)(struct MONO_PriorityTimerQueue_s *,
                                       MONO_PriorityTimerNode *);
  MONO_PriorityTimerNode *(*EraseNodeByIndex)(
      struct MONO_PriorityTimerQueue_s *, size_t);
  void (*RunTimerNode)(struct MONO_PriorityTimerQueue_s *);
  void (*RunInnerTimerNode)(struct MONO_PriorityTimerQueue_s *);
  uint8_t (*Size)(struct MONO_PriorityTimerQueue_s *);
  uint16_t (*PushNodeFullArguments)(struct MONO_PriorityTimerQueue_s *,
                                    MONO_NodeFunction_t, uint8_t, uint8_t,
                                    MONO_NodeTimer_t, uint8_t, MONO_NodeTimer_t,
                                    uint8_t, void *, MONO_NodeFunction_t);
#endif
} MONO_PriorityTimerQueue;

/**
 * @brief 初始化10个可容纳节点的数组
 * @return MONO_PriorityTimerQueue*: 队列指针
 */
MONO_PriorityTimerQueue *MONO_CreatePriorityQueue();

/**
 * @brief 增加可容纳的节点数量
 * @param  queue_: 队列指针
 */
void MONO_IncreaseNodes(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT);

/**
 * @brief 销毁队列并释放内存
 * @param  queue_: 队列指针
 */
void MONO_DeallocateQueueMemory(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT);

/**
 * @brief 按照每个节点的定时器周期数进行排序
 * @param  queue_: 队列指针
 */
void MONO_RankTimer(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT);

/**
 * @brief 使用函数指针找到队列中的节点
 * @param  queue_: 队列指针
 * @param  id_   : 结点id
 */
MONO_PriorityTimerNode *
MONO_FindNodeById(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT, uint16_t id_);

/**
 * @brief 复制节点node_到队列中
 * @param  queue_: 队列指针
 * @param  node_: 节点指针
 * @return uint16_t: 节点id
 */
uint16_t MONO_PushNode(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT,
                       MONO_PriorityTimerNode *node_);

/**
 * @brief 复制节点node_到queue_中，并释放node_的内存
 * @param  queue_: 队列指针
 * @param  node_: 节点指针
 * @return uint16_t: 节点id
 */
uint16_t MONO_PushNodeRelease(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT,
                              MONO_PriorityTimerNode *node_);

/**
 * @brief 当其定时器周期数不为零时使队列中所有的节点周期数+1
 * @param  queue_: 队列指针
 */
void MONO_TimerRunning(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT);

/**
 * @brief 弹出队列中优先级最高的（第一个）节点
 * @param  queue_: 队列指针
 * @return MONO_PriorityTimerNode*: 节点的指针
 */
MONO_PriorityTimerNode *
    MONO_PopNode(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT);

/**
 * @brief 弹出指定id的节点
 * @param  queue_: 队列指针
 * @param  id_: 节点id
 * @return MONO_PriorityTimerNode*: 节点指针
 */
MONO_PriorityTimerNode *
MONO_PopNodeById(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT, uint16_t id_);

/**
 * @brief 如果队列中存在node_，则从队列中删除它
 * @param queue_: 队列指针
 * @param node_:            指向node_的指针
 * @return MONO_PriorityTimerNode*: 节点指针
 */
MONO_PriorityTimerNode *
MONO_EraseNode(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT,
               MONO_PriorityTimerNode *node_);

/**
 * @brief 从队列中移除指定index节点
 * @param queue_: 队列指针
 * @param id_: 待移除节点的id
 * @return MONO_PriorityTimerNode*: 节点指针
 */
MONO_PriorityTimerNode *
MONO_EraseNodeByIndex(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT, size_t index);

/**
 * @brief 运行当前定时器周期数为零的所有节点中的函数
 * @param queue_: 队列指针
 */
void MONO_RunTimerNode(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT);

/**
 * @brief 运行当前定时周期数为零且为中断内部执行的所有节点中的函数
 * @param queue_: 队列指针
 */
void MONO_RunInnerTimerNode(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT);

/**
 * @brief 判断当前队列时候为空
 * @param queue_: 队列指针
 * @return 如果当前队列为空则返回0
 */
uint8_t MONO_Size(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT);

/**
 * @brief 创建node以便加入到队列
 * @param queue_: 队列指针
 * @param  node_func_       节点函数指针
 * @param  inner_           0为异步执行，即在调用run_timer_node()函数时执行
 * @param  enabled_         0为关闭节点执行会跳过排序
 * @param  timer_           经过timer_个时钟周期后执行
 * @param  loop_
 * 如果启用计数循环，则该参数为第一次运行后循环运行的次数，0为不循环，最大值为无限循环
 * @param  loop_timer_      计数循环的时钟中断周期数
 * @param  priority_ 该节点的函数在相同timer时执行的有限级，0为优先级最高。
 * @param  args_            等待执行函数的参数。
 * @param  performance_func_            结果处理函数
 * @return MONO_PriorityTimerNode*
 * 返回创建好的MONO_PriorityTimerNode_t类型的指针。
 */
uint16_t MONO_PushNodeFullArguments(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT,
                                    MONO_NodeFunction_t node_func_,
                                    uint8_t inner_, uint8_t enabled_,
                                    MONO_NodeTimer_t timer_, uint8_t loop_,
                                    MONO_NodeTimer_t loop_timer_,
                                    uint8_t priority_, void *args_,
                                    MONO_NodeFunction_t performance_func_);

// TODO 增加debug选项
// #ifdef MONO_PTQ_DEBUG
// /**
//  * @brief 格式化队列
//  * @return char*: 
//  */
// static const char* MONO_GetQueueFormat(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT)
// {
//   if (queue_->size > 0)
//   {

//   } else {
//     return "empty";
//   }
// }
// #endif

#ifdef __cplusplus
}
#endif

#endif // PRIORITY_TIMER_QUEUE_INC_PRIORITY_TIMER_QUEUE_H_
