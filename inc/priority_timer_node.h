/**
 * @file priority_timer_node.h
 *
 * @brief 节点结构
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
 * @note version: 2.2
 * @description: 更改队列实现方式为链表 并预留用户分配空间接口
 * @date 2025-05-16
 *
 * @note version: 2.5
 * @description: 移除了PTN选项
 * @date 2025-06-06
 *
 * @note version: 3.0
 * @description: 修改了部分参数变量名字 以及移除了 inner选项
 * * @details loop_timer -> reload
 * * @details loop -> loop_counter
 * @date 2025-06-13
 *
 * @note version: 3.4
 * @description: 增加Clear API
 * @date 2024-06-20
 *
 * *********************************************************************************
 */

#ifndef PRIORITY_TIMER_QUEUE_INC_PRIORITY_TIMER_NODE_H_
#define PRIORITY_TIMER_QUEUE_INC_PRIORITY_TIMER_NODE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief 数据初始长度
 */
#define MONO_DEFAULT_LENGTH             10

/**
 * @brief 设置数据增长步长
 */
#define MONO_DEFAULT_INCREASED_CAPACITY 10

#define MONO_TOP_NODE_INDEX             0
#define MONO_DEFAULT_SIZE_ZERO          0

#define MONO_NODE_SIZE                  sizeof(MONO_PriorityTimerNode_t)
#define ARGS_SIZE(args__)               sizeof(*args__)

#ifndef UINT8_MAX
#define UINT8_MAX (255)
#endif

#define MONO_QUEUE_TIMER_TYPE_MAX UINT32_MAX

#define MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT \
  MONO_PriorityTimerNode_t *node_

// 创建一个只运行一次且默认优先级的定时node

/**
 * @brief 创建一个默认优先级且一直运行的定时node
 * @param  function       节点函数指针
 * @param  timer          定时器周期数
 * @param  arg            等待执行函数的参数
 * @return MONO_PriorityTimerNode_t* 返回创建的指针
 */
#define MONO_CreateQueueNode(function, timer, arg) \
  MONO_CreateQueueNodeFull(function, true, timer, UINT8_MAX, timer, UINT8_MAX, arg, NULL)

/**
 * @brief 创建一个默认优先级且只运行一次的定时node
 * @param  function       节点函数指针
 * @param  timer          定时器周期数
 * @param  arg            等待执行函数的参数
 * @return MONO_PriorityTimerNode_t* 返回创建的指针
 */
#define MONO_CreateQueueNodeOnce(function, timer, arg) \
  MONO_CreateQueueNodeFull(function, true, timer, 0, timer, UINT8_MAX, arg, NULL)

/**
 * @brief 创建一个运行指定次数的Node
 * @param  f_            节点函数指针
 * @param  t_            定时器周期数
 * @param  c_            循环次数 UINT8_MAX则无限循环
 * @param  v_            等待执行函数的参数
 * @return MONO_PriorityTimerNode_t* 返回创建的指针
 */
#define MONO_CreateQueueNodeCount(f_, t_, c_, v_) \
  MONO_CreateQueueNodeFull(f_, true, t_, c_, t_, UINT8_MAX, v_, NULL)

// -----> Basic Type

// -----> Node Type

/**
 * @brief 储存定时器周期数的数据类型
 */
typedef uint32_t MONO_NodeTimer_t;

/**
 * @brief 节点ID
 */
typedef uint16_t MONO_NodeId_t;

/**
 * @brief 队列节点的类型
 */
struct MONO_PriorityTimerNode_s {

  /**
   * @brief 节点的唯一id
   *
   */
  MONO_NodeId_t _id;

  /**
   * @brief 定时执行的函数指针
   */
  void *(*_func)(void *);

  /**
   * @brief 等待timer个时钟周期后执行_function
   *
   * sizeof 8
   */
  MONO_NodeTimer_t _timer;

  /**
   * @brief 如果不为零则为使能
   *
   */
  bool _enabled;

  /**
   * @brief 该值为循环次数，如果为0则只运行一次
   * 当_loop_counter为uint8最大值时无限循环
   */
  uint8_t _loop_counter;

  /**
   * @brief _reload为任务重载时间
   */
  MONO_NodeTimer_t _reload;

  /**
   * @brief 定实执行函数的参数
   */
  void *_args;

  /**
   * @brief 优先级，优先级越靠近0则在_timer相等的情况下节点约靠近队首 默认为0xFF
   */
  uint8_t _priority;

  /**
   * @brief 注册的结果处理函数回调
   */
  void *(*_performance_func)(void *);

  /**
   * @brief 下一个节点
   */
  struct MONO_PriorityTimerNode_s *_next;

} __attribute__((aligned(4)));

/**
 * @brief 节点函数类型的指针
 */
typedef void *(*MONO_NodeFunction_t)(void *);

/**
 * @brief 定义类型
 */
typedef struct MONO_PriorityTimerNode_s MONO_PriorityTimerNode_t;

/**
 * @brief 运行节点中的回调函数以及结果处理回调
 * @param node_: 节点指针
 */
void MONO_ExecuteNode(MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT);

/**
 * @brief 分配空节点
 *
 * @return MONO_PriorityTimerNode_t* 空节点
 *
 * @details 根据用户需要可被用户重载
 */
MONO_PriorityTimerNode_t *MONO_AllocNode();

/**
 * @brief 清理node
 * @param  node_            节点
 *
 * @details 根据用户需要可被用户重载
 */
void MONO_DeallocNode(MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT);

/**
 * @brief 复制节点
 * @param  node_dst_  copy到的节点
 * @param  node_src_  被copy的节点
 */
void MONO_CopyNode(MONO_PriorityTimerNode_t *node_dest_,
                   const MONO_PriorityTimerNode_t *const node_src_);

/**
 * @brief 注册一个处理节点运行后返回值的函数.
 * @param  func_            节点结果处理函数指针
 * @param  node_            节点的指针
 * @sense: 1.1
 */
void MONO_RegisterResultPerformance(MONO_NodeFunction_t func_,
                                    MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT);

/**
 * @brief 创建node以便加入到队列
 * @param  node_func_           节点函数指针
 * @param  enabled_             0为关闭节点执行会跳过排序
 * @param  timer_               经过timer_个时钟周期后执行
 * @param  loop_counter_        如果启用计数循环，则该参数为第一次运行后循环运行的次数
 *                              0为不循环，UINT8_MAX为无限循环
 * @param  reload_              计数循环的时钟中断周期时间/重载时间
 * @param  priority_            该节点的函数在相同timer时执行的优先级，0为优先级最高。
 * @param  args_                等待执行函数的参数。
 * @param  performance_func_    结果处理回调函数
 * @return MONO_PriorityTimerNode_t*
 * 返回创建好的MONO_PriorityTimerNode_t类型的指针。
 */
MONO_PriorityTimerNode_t *MONO_CreateQueueNodeFull(
        MONO_NodeFunction_t node_func_, uint8_t enabled_,
        MONO_NodeTimer_t timer_, uint8_t loop_counter_, MONO_NodeTimer_t reload_,
        uint8_t priority_, void *args_, MONO_NodeFunction_t performance_func_);

#ifdef __cplusplus
}
#endif

#endif// PRIORITY_TIMER_QUEUE_INC_PRIORITY_TIMER_NODE_H_
