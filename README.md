# VirtualMeter

A general develop framework for smart meter.

▪Pure C language( C99 standard ).
▪Object-oriented.
▪MVC pattern: M(device) V(task) C(scheduler).
▪Time-based scheduler, non-preemptive, non-independent stack.
▪Rarely RAM(<16KB) & ROM(<64KB) usage.
▪Cross-platform capability, running on Windows and Linux.
▪Lua script language embedded to support integration testing & black box testing.


# 一个智能电表的通用开发框架。

▪纯C语言（C99标准）。
▪面向对象。
▪MVC分离：M（设备）V（任务）C（调度程序）。
▪基于时间的调度程序，非抢占，非独立堆栈。
▪很少的RAM（<16KB）和ROM（<64KB）占用。
▪跨平台功能，可运行在Windows和Linux系统上。
▪嵌入Lua脚本语言以支撑集成测试和黑盒测试。 


cmake -DCMAKE_TOOLCHAIN_FILE=STM32F0.cmake -DCMAKE_BUILD_TYPE=DEBUG -G "MinGW Makefiles" CMakeLists.txt