obj-m += my_module.o

KDIR := /lib/modules/$(shell uname -r)/build
ccflags-y := -I$(PWD)/../include

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
