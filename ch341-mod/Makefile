PWD         := $(shell pwd)
KVERSION    := $(shell uname -r)
KERNEL_DIR   = /lib/modules/$(KVERSION)/build

obj-m       := ch341-uart.o

ch341-uart-objs := ch341.o

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules
clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
