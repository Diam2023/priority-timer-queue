/**
 * @file priority_timer_node.h
 * 
 * @brief 节点结构 
 * 
 * @author monoliths (monoliths-uni@outlook.com)
 * @version 1.3
 * @date 2022-05-23
 * 
 * *********************************************************************************
 */
#ifndef PRIORITY_TIMER_QUEUE_INC_PRIORITY_TIMER_NODE_H_
#define PRIORITY_TIMER_QUEUE_INC_PRIORITY_TIMER_NODE_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 数据初始长度
 */
#define MONO_DEFAULT_LENGTH 10

/**
 * @brief 设置数据增长步长
 */
#define MONO_DEFAULT_INCREASED_CAPACITY 10

#define MONO_TOP_NODE_INDEX 0
#define MONO_DEFAULT_SIZE_ZERO 0

#define MONO_NODE_SIZE sizeof(MONO_PriorityTimerNode)
#define ARGS_SIZE(args__) sizeof(*args__)

#define MONO_ALLOC_NODE() (MONO_PriorityTimerNode *)calloc(1, MONO_NODE_SIZE)
#define MONO_ALLOC_NODES(size__)                                               \
  (MONO_PriorityTimerNode *)calloc(size__, MONO_NODE_SIZE)
#ifndef UINT8_MAX
#define UINT8_MAX (255)
#endif

#define MONO_QUEUE_TIMER_TYPE_MAX UINT32_MAX

#define MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT MONO_PriorityTimerNode *node_

// 创建一个只运行一次且默认优先级的定时node

/**
 * @brief 创建一个默认优先级的定时node
 * @param  f_            节点函数指针
 * @param  i_            若为0则在中断函数外部执行
 * @param  t_            定时器周期数
 * @param  v_            等待执行函数的参数
 * @return MONO_PriorityTimerNode* 返回创建的指针
 */
#define MONO_CreateQueueNode(f_, i_, t_, v_) MONO_CreateQueueNodeFull(f_, i_, 1, t_, 0, t_, UINT8_MAX, v_, NULL)

/**
 * @brief 创建一个运行指定次数的Node
 * @param  f_            节点函数指针
 * @param  i_            若为0则在中断函数外部执行
 * @param  t_            定时器周期数
 * @param  c_            循环次数 UINT8_MAX则无限循环
 * @param  v_            等待执行函数的参数
 * @return MONO_PriorityTimerNode* 返回创建的指针
 */
#define MONO_CreateQueueNodeCount(f_, i_, t_, c_, v_) MONO_CreateQueueNodeFull(f_, i_, 1, t_, c_, t_, UINT8_MAX, v_, NULL)

/**
 * @brief 节点标志的基本类型
 */
typedef unsigned char uint8_t;

/**
 * @brief 队列中储存节点数量的类型
 */
// typedef unsigned long size_t;

/**
 * @brief 储存定时器周期数的数据类型
 */
typedef unsigned int MONO_NodeTimer_t;

/**
 * @brief 节点函数类型的指针
 */
typedef void *(*MONO_NodeFunction_t)(void *);

/**
 * @brief 唯一id分配
 */
static uint16_t MONO_g_ptn_auto_id = 1;

/**
 * @brief 队列节点的类型
 */
typedef struct MONO_PriorityTimerNode_s {
  uint16_t _id; // 节点的唯一id

  /**
   * @brief 等待timer个时钟周期后执行_function
   */
  MONO_NodeTimer_t _timer; // sizeof 8

  /**
   * @brief 定时执行的函数指针
   */
  MONO_NodeFunction_t _func; // sizeof 4

  /**
   * @brief 如果不为零则为使能
   */
  uint8_t _enabled;

  /**
   * @brief 该值为循环次数，如果为0则不循环
   * 循环计数是否启用取决于_count_loop的值时候不为0
   * 当_loop为uint8最大值时无限循环
   */
  uint8_t _loop;

  /**
   * @brief
   * 如果_loop为不为0且_count_loop不为零，则_loop_timer为下一次循环的初始等待周期数
   */
  MONO_NodeTimer_t _loop_timer;

  /**
   * @brief 定实执行函数的参数
   */
  void *_args;

  /**
   * @brief 优先级，优先级越靠近0则在_timer相等的情况下节点约靠近队首 默认为0xFF
   */
  uint8_t _priority;

  /**
   * @brief 如果为1则在中断内部执行
   */
  uint8_t _inner;

  /**
   * @brief 注册的结果指针处理函数
   * @sense: 1.1
   * @details 使用#register_result_performance函数将该属性添加到节点。
   */
  MONO_NodeFunction_t _performance_func;

#ifdef MONO_USE_FULL_PTN_MEMBER

  void (*ExecuteNode)(struct MONO_PriorityTimerNode_s *);

  // 指向销毁节点函数的函数指针
  void (*DeallocNode)(struct MONO_PriorityTimerNode_s *);

  void (*CopyNode)(struct MONO_PriorityTimerNode_s *,
                   struct MONO_PriorityTimerNode_s *);

  void (*RegisterResultPerformance)(MONO_NodeFunction_t,
                                    struct MONO_PriorityTimerNode_s *);

  struct MONO_PriorityTimerNode_s* (*SetEnabled)(uint8_t, struct MONO_PriorityTimerNode_s *);
  struct MONO_PriorityTimerNode_s* (*SetTimer)(MONO_NodeTimer_t, struct MONO_PriorityTimerNode_s *);
  struct MONO_PriorityTimerNode_s* (*SetLoop)(uint8_t, struct MONO_PriorityTimerNode_s *);
  struct MONO_PriorityTimerNode_s* (*SetLoopTimer)(MONO_NodeTimer_t, struct MONO_PriorityTimerNode_s *);

  struct MONO_PriorityTimerNode_s* (*SetPriority)(uint8_t, struct MONO_PriorityTimerNode_s *);
  struct MONO_PriorityTimerNode_s* (*SetArgs)(void *, struct MONO_PriorityTimerNode_s *);
  // struct MONO_PriorityTimerNode_s* (*RegisterResultPerformance)(MONO_NodeFunction_t, struct MONO_PriorityTimerNode_s *);
#endif

} MONO_PriorityTimerNode;

// TODO: 增加选择是否释放节点选项
/**
 * @brief 执行并清理node
 * @param node_: 节点指针
 */
static void MONO_ExecuteNode(MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT) {
  if (node_ != NULL) {
    // 通过节点储存的参数数据来执行节点函数
    void *result = NULL;
    result = node_->_func(node_->_args);

    if (node_->_performance_func != NULL) {
      node_->_performance_func(result);
    }
  }
}

/**
 * @brief 清理node
 * @param  node_    :        待执行node
 */
static void MONO_DeallocNode(MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT) {
  memset(node_, 0, sizeof(MONO_PriorityTimerNode));
  free(node_);
}

/**
 * @brief 复制节点
 * @param  node_dst_: copy到的节点
 * @param  node_src_: 被copy的节点
 */
static void MONO_CopyNode(MONO_PriorityTimerNode *node_dest_,
                          MONO_PriorityTimerNode *node_src_) {
  memmove(node_dest_, node_src_, sizeof(MONO_PriorityTimerNode));
}

/**
 * @brief 设置node_是否启用
 * @param  enabled_         若为0则不启用，1为启用
 * @param  node_            节点的指针
 * @return MONO_PriorityTimerNode* 返回操作节点的指针
 */
static MONO_PriorityTimerNode *
MONO_SetEnabled(uint8_t enabled_, MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT) {
  node_->_enabled = enabled_;
  return node_;
}

/**
 * @brief 设置定时时间
 * @param  timer_           timer_ MONO_NodeTimer_t类型的值
 * @param  node_            节点的指针
 * @return MONO_PriorityTimerNode* 返回操作节点的指针
 */
static MONO_PriorityTimerNode *
MONO_SetTimer(MONO_NodeTimer_t timer_,
              MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT) {
  node_->_timer = timer_;
  return node_;
}

/**
 * @brief 设置定时时间
 * @param  loop_            设置循环次数，0为关闭循环 UINT8_MAX为无限循环
 若节点_loop_timer为0且_time不为0则设置_loop_timer的值为_timer的值
 * @param  node_            节点的指针
 * @return MONO_PriorityTimerNode* 返回操作节点的指针
 */
static MONO_PriorityTimerNode *
MONO_SetLoop(uint8_t loop_, MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT) {
  node_->_loop = loop_;
  // 若节点_loop_timer为0且_time不为0则设置_loop_timer的值为_timer的值
  if (node_->_loop_timer == 0 && node_->_timer != 0) {
    node_->_loop_timer = node_->_timer;
  }
  return node_;
}

/**
 * @brief 设置定时时间
 * @param  loop_timer_      timer_ MONO_NodeTimer_t类型的值
 * @param  node_            节点的指针
 * @return MONO_PriorityTimerNode* 返回操作节点的指针
 */
static MONO_PriorityTimerNode *
MONO_SetLoopTimer(MONO_NodeTimer_t loop_timer_,
                  MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT) {
  node_->_loop_timer = loop_timer_;
  return node_;
}

/**
 * @brief 设置优先级
 * @param  priority_        0的优先最高
 * @param  node_            节点的指针
 * @return MONO_PriorityTimerNode* 返回操作的节点的指针
 */
static MONO_PriorityTimerNode *
MONO_SetPriority(uint8_t priority_, MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT) {
  node_->_priority = priority_;
  return node_;
}

/**
 * @brief 设置优先级
 * @param  args_             节点函数的参数
 * @param  node_            节点的指针
 * @return MONO_PriorityTimerNode* 返回操作的节点的指针
 */
static MONO_PriorityTimerNode *
MONO_SetArgs(void *args_, MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT) {
  node_->_args = args_;
  return node_;
}

/**
 * @brief 注册一个处理节点运行后返回值的函数.
 * @param  func_            节点结果处理函数指针
 * @param  node_            节点的指针
 * @sense: 1.1
 */
static void
MONO_RegisterResultPerformance(MONO_NodeFunction_t func_,
                               MONO_PRIORITY_TIMER_NODE_POINTER_ARGUMENT) {
  node_->_performance_func = func_;
}

/**
 * @brief 创建node以便加入到队列
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
static MONO_PriorityTimerNode *
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

#ifdef __cplusplus
}
#endif

#endif // PRIORITY_TIMER_QUEUE_INC_PRIORITY_TIMER_NODE_H_
