# PriorityTimerQueue

## 项目用于定时任务

## 部署方法
1. 根据需要重写Alloc Dealloc
2. 多线程情况重写Lock Unlock
3. 使用用户外部实现定时功能

## 内存检测工具
valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all 
