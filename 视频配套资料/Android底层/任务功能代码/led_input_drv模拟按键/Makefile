
CROSS_COMPILE = /opt/toolchain/toolchain-4.5.1-farsight/bin/arm-none-linux-gnueabi-
CC = $(CROSS_COMPILE)gcc -static
APP_NAME = led_app
MODULE_NAME = s5pv210_led


#内核源码路径
#KERNEL_DIR = /home/farsight/linux_system/kernel/linux-3.0.8 
KERNEL_DIR = /home/george/src_210/linux-3.0.8-FS210

CUR_DIR = $(shell pwd)

all :
	make -C $(KERNEL_DIR) M=$(CUR_DIR) modules 
	$(CC) $(APP_NAME).c -o  $(APP_NAME)

clean : 
	make -C $(KERNEL_DIR) M=$(CUR_DIR) clean
	rm -rf $(APP_NAME)	


install:
	cp -raf  *.ko $(APP_NAME)  /opt/rootfs/drv_module/


#指定编译哪个源文件
obj-m = $(MODULE_NAME).o


