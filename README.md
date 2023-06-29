# Neng-Log

## 概述

Neng-Log是一个通用的C语言日志库，可以将应用程序的日志是输出到文件、系统日志。

特色功能：

1. 模块级的日志级别控制

支持模块(mod)级别的日志级别设置，不同的模块设置不同的日志级别；

2. 日志输出支持标签(tag)

tag表示日志的用途，作为日志过滤的一个条件，不同模块可以输出同一tag的日志，便于输出到同一个目的地（文件/系统日志）

3. 支持日志过滤输出
    - 支持设定不同的模块(mod), 标签(tag), 级别(level) 日志输出到不同的文件；
    - 支持设定不同的模块(mod), 标签(tag), 级别(level) 日志使用不同的facility输出到系统日志。

5. 支持异步日志输出

6. 框架支持定制appender；自带文件(file),系统日志(syslog)两个appender。

## 编译

```
# 配置cmake；DCMAKE_BUILD_TYPE后选值：Debug, Release, RelWithDebInfo, MinSizeRel
cmake  -DCMAKE_BUILD_TYPE=Debug . -B ./build/
# 编译, VERBOSE=1, 显示编译命令
cmake --build ./build -- -j4 VERBOSE=0
```

## 安装

1. 头文件目录: ./include/

2. 库文件目录: ./objs/lib/

## 使用说明

### 接口说明

请看透文件./include/tang-log/log.h的注释

### 使用示例

1. 请看./test/main.c文件；

```
root@dev2210:/var/log# cat test-1.log
2023-06-27 14:03:30.562 dev2210 test 29826 Notice tid:29826 mod:10 tag:10 <main.c@main:78> :this is mod-20 tag-10 notice log.
root@dev2210:/var/log# cat test-2.log
2023-06-27 14:03:30.562 dev2210 test 29826 Notice tid:29826 mod:10 tag:10 <main.c@main:78> :this is mod-20 tag-10 notice log.
2023-06-27 14:03:30.562 dev2210 test 29826 Info   tid:29826 mod:0 tag:0 <main.c@main:79> :this is test info log
2023-06-27 14:03:30.562 dev2210 test 29826 Error  tid:29826 mod:0 tag:0 <main.c@main:80> :this is test error log
```

## 修改历史
2023/06/27 v1.0 版本发布；

## 附录

