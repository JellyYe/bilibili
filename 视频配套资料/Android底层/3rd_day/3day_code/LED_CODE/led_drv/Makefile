ifeq ($(KERNELRELEASE),)
	KERNELDIR = /home/george/src_210/linux-3.0.8-FS210
	PWD =$(shell pwd)
modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	#arm-none-linux-gnueabi-gcc hello_test.c -o hello_test
	#cp hello_test  hello.ko /opt/filesystem/s5pv210
modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install
clean:
	rm -rf *_test  *.so *.o *.ko  .tmp_versions *.mod.c *.order *.symvers 
else
	obj-m :=s5pv210_led.o
endif
