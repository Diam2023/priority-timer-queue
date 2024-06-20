# PriorityTimerQueue

## 项目用于定时任务

## 部署方法
1. 根据需要重写Alloc Dealloc
2. 多线程情况重写Lock Unlock
3. 使用用户外部实现定时功能

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
