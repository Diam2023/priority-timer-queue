# PriorityTimerQueue

## 项目适用于单片机内需要定时处理不同的任务的情况

pthread例程参考测试文件[./test/test_for_c_main.c](./test/test_for_c_main.c)

## 使用前
1. 根据需要重写Alloc Dealloc
2. 多线程情况重写Lock Unlock
3. 根据选择的使用方法配置运行逻辑

## 使用方法


### 引入项目
将项目源码在你的项目下, 例如third_party下, 然后在你的cmake内添加如下两行
```
target_link_libraries(${PROJECT_NAME} PRIVATE libmono)
add_subdirectory(third_party/priority-timer-queue)
```


### #1 定时调用 `TimerTickHandler` 实现
#### 适用场景
适用于不定时要获取实时Tick的情况

#### 缺点
1. 定时时间是周期只能是的倍数
2. 在单核MCU运行的情况下周期过短会降低实时性

#### 使用方式

```cpp

// 声明一个定时器处理任务队列
static MONO_PriorityTimerQueue_t *s_pMainTaskQueue = NULL;

//// ****************** 定时器任务

// 声明Led1任务的id
static MONO_NodeId_t s_led1TaskId;
// LED1定时任务回调函数
void *LED1_BlinkTask(void*)

// 声明Led2任务的id
static MONO_NodeId_t s_led2TaskId;
// LED2定时任务回调函数
void *LED2_BlinkTask(void*)

//// ****************** 定时器任务结束

// 定时器中断函数
void TIM10_IRQHandler(void)
{
    // 定时队列一个周期到达的回调
    TimerTickHandler(s_pMainTaskQueue);

    // 重载定时器
    TIM10_Reload();
}

int main()
{
    // 初始化定时器10
    // 以每1ms中断一次的方式运行
    TIM10_Init();

    // 实例化一个定时器任务队列
    s_pMainTaskQueue = NewTimerQueue();

    // 新建一个默认优先级的任务以1000个定时周期运行一次
    // 在该例程中为1s
    s_led1TaskId = NewTimerTask(s_pMainTaskQueue, LED1_BlinkTask, 1000, NULL);
    // 新建一个默认优先级的任务以1500个定时周期运行一次
    // 在该例程中为1.5s
    s_led2TaskId = NewTimerTask(s_pMainTaskQueue, LED2_BlinkTask, 1500, NULL);

    // 开启定时器
    EnableTimerQueue(s_pMainTaskQueue);
    // 运行Tim10定时器 并打开中断
    TIM10_Start();

    while(true)
    {
        // Write Your Logic
    }
}

// 重载lock逻辑
TryLockOverride {
    UNUSED(queue_);
    return true;
}

// 重载unlock逻辑
UnlockOverride {
    UNUSED(queue_);
}

// LED1定时任务回调函数
void *LED1_BlinkTask(void*)
{
    // 反转LED1电平
    HAL_GPIO_TogglePin(...);
}

// LED2定时任务回调函数
void *LED2_BlinkTask(void*)
{
    // 反转LED2电平
    HAL_GPIO_TogglePin(...);
}

```

#### 实验现象

1. LED1以1s亮一次
2. LED2以1.5s亮一次

### #2 重载 `SetNextAlarmTimer` 方式实现

#### 适用场景
任务精确定时的方式

通常应该配置单片机的闹钟方式来实现

#### 缺点
1. Tick不是实时反应

#### 使用方式

```cpp

static MONO_NodeTimer_t s_lastTimer;

// 闹钟A中断函数
void ALARM_A_IRQHandler(void)
{
    // 定时队列的s_lastTimer时间到达的回调
    TimerTickStep(s_pMainTaskQueue, s_lastTimer);
}

SetNextAlarmTimer {
    s_lastTimer = timer_;
    // 设置闹钟A在timer_时间后中断
    AlarmA_ClockConfig(timer_);
}

// ...
// 省略LED1 LED2 TASK
// 参考使用方法#1

int main()
{
    // 初始化闹钟
    AlarmA_Init();

    // 实例化一个定时器任务队列
    s_pMainTaskQueue = NewTimerQueue();

    // 新建一个默认优先级的任务以1000时间运行一次
    // 在该例程中为1s
    s_led1TaskId = NewTimerTask(s_pMainTaskQueue, LED1_BlinkTask, 1000, NULL);
    // 新建一个默认优先级的任务以1500时间运行一次
    // 在该例程中为1.5s
    s_led2TaskId = NewTimerTask(s_pMainTaskQueue, LED2_BlinkTask, 1500, NULL);

    // 开启定时器
    EnableTimerQueue(s_pMainTaskQueue);

    while(true)
    {
        // Write Your Logic
    }
}

```

#### 实验现象

1. LED1以1s亮一次
2. LED2以1.5s亮一次


## 内存检测工具
valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all 

```bash
❯ valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./build/test/libmono_test
==100973== Memcheck, a memory error detector
==100973== Copyright (C) 2002-2024, and GNU GPL'd, by Julian Seward et al.
==100973== Using Valgrind-3.23.0 and LibVEX; rerun with -h for copyright info
==100973== Command: ./build/test/libmono_test
==100973== 
Test Start 
设置下次唤醒 2000
重置唤醒 2000 to 1000

----------------  Task Info  -----------------
size   status  tick  
3      run     0     

------------------- Major --------------------
id    enabled  timer  loop  reload 
2     true     1000   255   1000   
4     true     1123   255   1123   

------------------- Cache --------------------
3     false    1400   255   1400   
End 
f 1
设置下次唤醒 123
f 3
设置下次唤醒 877
f 1
设置下次唤醒 246
f 3
设置下次唤醒 754
f 1
设置下次唤醒 369
f 3
设置下次唤醒 631
13 run end 

----------------  Task Info  -----------------
size   status  tick  
3      run     3369  

------------------- Major --------------------
id    enabled  timer  loop  reload 
2     true     631    255   1000   
4     true     1123   255   1123   

------------------- Cache --------------------
3     false    1400   255   1400   
End 
重置唤醒 334 to 631
f 1
设置下次唤醒 769
f 2
设置下次唤醒 231
f 1
设置下次唤醒 1000
f 1
设置下次唤醒 169
12 run end 

----------------  Task Info  -----------------
size   status  tick  
3      run     6000  

------------------- Major --------------------
id    enabled  timer  loop  reload 
3     true     169    255   1400   
2     true     1000   255   1000   

------------------- Cache --------------------
4     false    1123   255   1123   
End 
重置唤醒 92 to 169
f 2
设置下次唤醒 831
f 1
设置下次唤醒 123
f 3
设置下次唤醒 446
f 2
设置下次唤醒 431
123 run end 
重置唤醒 190 to 431

----------------  Task Info  -----------------
size   status  tick  
3      run     7569  

------------------- Major --------------------
id    enabled  timer  loop  reload 
2     true     431    255   1000   
4     true     677    255   1123   

------------------- Cache --------------------
3     false    1400   255   1400   
End 
定时队列退出
Test End 
==100973== 
==100973== HEAP SUMMARY:
==100973==     in use at exit: 0 bytes in 0 blocks
==100973==   total heap usage: 8 allocs, 8 frees, 1,856 bytes allocated
==100973== 
==100973== All heap blocks were freed -- no leaks are possible
==100973== 
==100973== For lists of detected and suppressed errors, rerun with: -s
==100973== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```
