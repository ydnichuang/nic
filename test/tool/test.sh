###
 # @Author: Xing Wang (wangx_ddpt@yusur.tech)
 # @date: 2023-06-28 11:21:59
 # @last_author: Xing Wang (wangx_ddpt@yusur.tech)
 # @last_edit_time: 2023-06-28 11:24:55
### 

#! /bin/bash

rmmod hados_doe
insmod ../../modules/hados_doe.ko

make

./doe_tool function