/**
 * @file priority_timer.h
 *
 * @brief 定时优先级队列声明及其实现
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
 * @note version: 1.0
 * @description: 基础QUEUE方法.
 *
 * @note version: 1.1
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
 * @note version: 1.4
 * @description: 添加TimerTick计数器
 * @date 2025-05-15
 *
 * @note version: 2.1
 * @description: 修改项目结构
 * @date 2025-05-15
 *
 * @note version: 2.2
 * @description: 更改队列实现方式为链表 并预留用户分配空间接口
 * @date 2025-05-16
 *
 * @note version: 2.3
 * @description: 增加缓存队列来缓存关闭的节点
 * @date 2025-05-16
 *
 * @note version: 2.4
 * @description: 添加了调试信息接口
 * @date 2025-05-19
 *
 * @note version: 2.5
 * @description: 整理API 新增Timer设置API
 * @date 2025-06-06
 *
 * @note version: 3.0
 * @description: 升级定时器接口实现API
 * @date 2025-06-13
 *
 * *********************************************************************************
 */

#ifndef PRIORITY_TIMER_QUEUE_INC_PRIORITY_TIMER_QUEUE_H_
#define PRIORITY_TIMER_QUEUE_INC_PRIORITY_TIMER_QUEUE_H_

#ifdef __cplusplus
extern "C" {
#endif

#define MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT \
  MONO_PriorityTimerQueue_t *queue_

#define CreatePriorityTimerQueue() MONO_CreatePriorityQueue();

#include "priority_timer_node.h"

/**
 * @brief 定时优先级队列
 */
typedef struct MONO_PriorityTimerQueue_s {

  /**
   * @brief 头部节点
   */
  MONO_PriorityTimerNode_t *_header_node;

  /**
   * @brief 未启用的节点头节点
   *
   * @since v2.3
   */
  MONO_PriorityTimerNode_t *_disabled_header;

  /**
   * @brief 定时队列当前的计数
   */
  MONO_NodeTimer_t _timer_tick;

  /**
   * @brief 现有节点个数
   */
  uint32_t _size;

  /**
   * @brief 定时器打开状态
   */
  bool _run_status;

  /**
   * @brief 读写锁
   */
  volatile bool _lock;

} MONO_PriorityTimerQueue_t;

/// ***************************************** CORE

/**
 * @brief 分配内存
 *
 * @return MONO_PriorityTimerQueue_t* 指向内存的指针
 */
MONO_PriorityTimerQueue_t *MONO_AllocTimerQueue(void);

/**
 * @brief 回收内存
 *
 * @param queue_ 队列
 */
void MONO_DeallocTimerQueue(MONO_PriorityTimerQueue_t *queue_);

/**
 * @brief 初始化10个可容纳节点的数组
 * @return MONO_PriorityTimerQueue_t*: 队列指针
 */
MONO_PriorityTimerQueue_t *MONO_CreatePriorityQueue(void);

/**
 * @brief 销毁队列并释放内存
 * @param  queue_: 队列指针
 */
void MONO_DestroyPriorityQueue(MONO_PriorityTimerQueue_t **queue_);

/**
 * @brief 尝试锁定队列 非阻塞 用户可重载
 *
 * @return true 成功
 * @return false 失败
 */
bool MONO_TryLockTimerQueue(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT);

/**
 * @brief 解锁队列 用户可重载
 */
void MONO_UnlockTimerQueue(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT);

/**
 * @brief 设置下次到点的函数 用户需要实现该函数 到点后需要调用MONO_NextTimerAlarm
 *
 * @param timer_ 时间
 */
void MONO_SetNextAlarmTimer(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT, MONO_NodeTimer_t timer_);

/// ***************************************** USER

/**
 * @brief 设置定时器运行状态
 *
 * 调用该API 队列可能不会立即停止工作
 *
 * @since v2.3
 * @param status_ 状态
 */
void MONO_SetTimerQueueEnable(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT,
                              bool status_);

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
                            MONO_PriorityTimerNode_t *node_);

/**
 * @brief 增加定时器并 运行当前定时器周期数为零的所有节点中的函数
 *
 * @param queue_: 队列指针
 * @deprecated 将在3.2版本移除 使用MONO_TimerTickHandler替代
 * @return uint16_t 返回运行了的节点数量 如果为UINT32_MAX则是队列运行失败
 */
uint32_t MONO_TimerInnerHandler(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT);

/**
 * @brief 获取队列中的数量
 * @param queue_: 队列指针
 * @return 如果当前队列为空则返回0
 */
uint8_t MONO_Size(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT);

/**
 * @brief 使用函数指针找到队列中的节点
 * @param  queue_: 队列指针
 * @param  id_   : 节点id
 */
MONO_PriorityTimerNode_t *
MONO_FindNodeById(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT,
                  MONO_NodeId_t id_);

/**
 * @brief 设置指定节点的运行状态
 *
 * @param queue_: 队列指针
 * @param id_ 定时器队列任务启用状态
 * @param enable_ 是否启用
 * @return true 成功
 * @return false 失败
 */
bool MONO_SetTimerNodeEnable(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT,
                             MONO_NodeTimer_t id_, bool enable_);

/**
 * @brief 设置指定节点下次运行时间
 *
 * @since 2.5
 * @param queue_: 队列指针
 * @param id_ 定时器队列任务启用状态
 * @param timer_ 时间
 * @return true 成功
 * @return false 失败
 */
bool MONO_SetTimerNodeTimer(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT,
                            MONO_NodeTimer_t id_,
                            const MONO_NodeTimer_t timer_);

/**
 * @brief 设置节点运行循环时间
 *
 * @since 2.5
 * @param queue_: 队列指针
 * @param id_ 定时器队列任务启用状态
 * @param timer_ 时间
 * @return true 成功
 * @return false 失败
 */
bool MONO_SetTimerNodeLoopTimer(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT,
                                MONO_NodeTimer_t id_,
                                const MONO_NodeTimer_t timer_);

/**
 * @brief 节点是否存在于队列中
 *
 * @since 2.5
 * @param queue_: 队列指针
 * @param id_ 定时器队列任务启用状态
 * @return true 成功
 * @return false 失败
 */
bool MONO_IsTimerNodeExist(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT,
                           MONO_NodeTimer_t id_);

/**
 * @brief  创建node以便加入到队列
 * @param  queue_            队列指针
 * @param  node_func_        节点函数指针
 * @param  enabled_          0为关闭节点执行会跳过排序
 * @param  timer_            经过timer_个时钟周期后执行
 * @param  loop_             如果启用计数循环，
 *                           则该参数为第一次运行后循环运行的次数，
 *                           0为不循环，最大值为无限循环
 * @param  loop_counter_     计数循环的时钟中断周期数
 * @param  reload_           该节点的函数在相同timer时执行的有限级，
 *                           0为优先级最高。
 * @param  args_             等待执行函数的参数。
 * @param  performance_func_ 结果处理函数
 * @return MONO_PriorityTimerNode_t* 返回创建好的MONO_PriorityTimerNode_t类型的指针。
 */
uint16_t MONO_PushNodeFullArguments(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT,
                                    MONO_NodeFunction_t node_func_,
                                    uint8_t enabled_,
                                    MONO_NodeTimer_t timer_, uint8_t loop_counter_,
                                    MONO_NodeTimer_t reload_,
                                    uint8_t priority_, void *args_,
                                    MONO_NodeFunction_t performance_func_);

/**
 * @brief 获取队列的TimeTick
 *
 * @since v2.2
 * @return uint32_t TimeTick
 */
uint32_t MONO_GetTimerTick(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT);

/**
 * @brief 补偿1点TimerTick 并运行当前定时器周期数为零的所有节点中的函数
 *
 * @since v3.1
 * @return uint16_t 返回运行了的节点数量 如果为UINT32_MAX则是队列运行失败
 */
uint32_t MONO_TimerTickHandler(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT);

/**
 * @brief 补偿step_点TimeTick 并运行可运行的节点
 *
 * @since v3.1
 * @param uint32_t-step_ 步长
 */
uint32_t MONO_TimerTickStep(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT, uint32_t step_);

#ifdef MONO_PTQ_DEBUG
/**
 * @brief 输出调试信息
 *
 * @return uint32_t
 */
void MONO_QueueTaskInfo(MONO_PRIORITY_TIMER_QUEUE_POINTER_ARGUMENT);

#endif

#ifdef __cplusplus
}
#endif

#endif// PRIORITY_TIMER_QUEUE_INC_PRIORITY_TIMER_QUEUE_H_
