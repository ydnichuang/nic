ifneq ($(KERNELRELEASE),)

KBUILD_EXTRA_SYMBOLS += /home/zhoumc/generic_nic_framwork/linux_kernel/Module.symvers
EXTRA_CFLAGS += -I/opt/yusur/hados/include
# object files to build
#obj-m += doe_kernel_test.o
obj-m += array_test.o
obj-m += hash_test.o
#doe_kernel_test-y += kernel_call_test.o
array_test-y += array_delay_test.o
hash_test-y += hash_delay_test.o

else

ifneq ($(KERNEL_SRC),)
# alternatively to variable KDIR accept variable KERNEL_SRC as used in
# PetaLinux/Yocto for example
KDIR ?= $(KERNEL_SRC)
endif

# Uesd for rpm-build install
DESTDIR =
SDKDIR  = /opt/yusur/hados
KDIR ?= /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) M=$(shell pwd) modules

help modules modules_install clean:
	$(MAKE) -C $(KDIR) M=$(shell pwd) $@

endif
