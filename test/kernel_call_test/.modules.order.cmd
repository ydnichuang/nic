cmd_/home/wangyf/test/kernel_call_test/modules.order := {   echo /home/wangyf/test/kernel_call_test/array_test.ko;   echo /home/wangyf/test/kernel_call_test/hash_test.ko; :; } | awk '!x[$$0]++' - > /home/wangyf/test/kernel_call_test/modules.order
