
#内核源码路径
KERNEL_DIR = /home/farsight/linux_system/kernel/linux-3.0.8 

CUR_DIR = $(shell pwd)

all :
	#表示进入到内核目录,并且告诉内核要将当前的源码编译成模块
	make -C $(KERNEL_DIR) M=$(CUR_DIR) modules 

clean : 
	make -C $(KERNEL_DIR) M=$(CUR_DIR) clean
	
install:
	cp -raf  *.ko  /opt/rootfs/drv_module/


#指定编译哪个源文件
obj-m = hello_drv.o


