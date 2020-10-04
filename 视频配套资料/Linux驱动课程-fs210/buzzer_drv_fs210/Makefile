KERNEL_DIR  = /home/lpf/1609/linux_system/kernel/linux-3.0.8

CUR_DIR = $(shell pwd)

all: 
	    make -C $(KERNEL_DIR) M=$(CUR_DIR) modules
	    arm-none-linux-gnueabi-gcc buzzer_app.c -o buzzer_app

clean:
	    make -C $(KERNEL_DIR) M=$(CUR_DIR) clean

install:
	   sudo cp -raf *ko buzzer_app /opt/s5pv210/rootfs/drv_module/buzzer_drv

obj-m=buzzer_drv.o
