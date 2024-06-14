# PriorityTimerQueue

## 项目用于定时任务

### 使用时必须重写Alloc、Dealloc、Lock、UnLock接口!


## TODO
* [x] 更换优先级队列实现方式为LinkedList
* [ ] 用户API整理
* [ ] Lock优化
* [ ] 自增ID从队列结构体实现

## 部署方法
1. 重写Alloc Dealloc Lock Unlock接口
2. 使用用户外部实现定时功能


### 非多线程情况无需重写Lock Unlock



## 内存检测工具
valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all 
