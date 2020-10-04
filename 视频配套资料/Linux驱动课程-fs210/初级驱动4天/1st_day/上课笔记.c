 
1,内核源码的编译:
	a,获取源码----E:\Linux驱动课程-george\source\linux-3.0.8.tar.bz2
		www.kernel.org
	
		 tar -xvf linux-3.0.8.tar.bz2
	
	b, 内核编译步骤:参考E:\Linux驱动课程-george\初级驱动\第一天\实验\系统环境搭建移植文档.doc
		1, 设置交叉工具链
			 195 ARCH            ?= arm
			 196 CROSS_COMPILE   ?= arm-none-linux-gnueabi-

		2, 选择当前的soc---s5pv210
			make s5pv210_defconfig

		3, make menuconfig 
			System Type  ---> 
				(0) S3C UART to use for low-level messages 
		
			注意: 执行make menuconfig 有可能缺少一个包:ncurse
						sudo apt-get install libncurses5-dev

		4, make zImage 或者 make zImage

	c, 移植内核---网卡dm9000驱动
		cp -raf arch/arm/boot/zImage  /tftpboot/	
	

2,编写驱动
	

	a,头文件
		#include <linux/init.h>
		#include <linux/module.h>

	b,声明驱动模块的装载和卸载函数
		module_init(hello_init);
		module_exit(hello_exit);
	c,实现驱动模块的装载和卸载函数
		static int __init hello_init(void)
		{
				printk("-------^_^ %s-------\n", __FUNCTION__);

				return 0;
		}


		static void __exit hello_exit(void)
		{

				printk("-------^_^ %s-------\n", __FUNCTION__);

		}

	d,添加GPL认证
		MODULE_LICENSE("GPL");

	Makefile---结论:如果要编译驱动模块就必须先编译内核源码
		#内核源码路径---
		KERNEL_DIR = /home/farsight/linux_system/kernel/linux-3.0.8　

		CUR_DIR = $(shell pwd)

		all :
				#表示进入到内核目录,并且告诉内核要将当前的源码编译成模块
				make -C $(KERNEL_DIR) M=$(CUR_DIR) modules

		clean :
				make -C $(KERNEL_DIR) M=$(CUR_DIR) clean



		#指定编译哪个源文件
		obj-m = hello_drv.o


3, 运行开发板---前提,内核已经移植好
	
	下载启动内核: tftp
		pc端:
			 cp -raf arch/arm/boot/zImage   /tftpboot
		
		uboot中:
			FS210 # set serverip 192.168.7.3  #指定tftp服务器的ip
			FS210 # set ipaddr 192.168.7.7		#指定开发板的ip
			FS210 # set bootcmd tftp  0x40008000 zImage \; bootm 0x40008000
			FS210 # save


	挂载根文件系统:  nfs共享
		
		ubuntu pc机端
				E:\Linux驱动课程-george\source\rootfs_fs210.tgz--->ubuntu的/opt
			 sudo tar -xvf rootfs_fs210.tgz
		
			配置nfs服务器: /opt/rootfs可以被挂载
				sudo vim /etc/exports
					/opt/rootfs   *(subtree_check,rw,no_root_squash,async)
				
				重启:
					sudo service nfs-kernel-server restart
		

		uboot中:
			FS210 # set bootargs console=ttySAC0,115200 root=/dev/nfs nfsroot=192.168.7.3:/opt/rootfs ip=192.168.7.7 init=/linuxrc
			FS210 # save

			
			
	

			解释: bootargs是一个变量,传递给内核
				 console=ttySAC0,115200 //  console=xxx表示从哪个设备打印信息
				root=/dev/nfs  //告诉内核要通过nfs方式去挂载根文件系统
				nfsroot=ip:path // 具体挂载的路径
				ip=xxx //指定开发板的ip
	

4, 运行/装载驱动模块
		cp -raf hello_drv.ko  /opt/rootfs


		在开发板上:
			[root@farsight /]# insmod hello_drv.ko
			-------^_^ hello_init-------
			[root@farsight /]# rmmod hello_drv
			-------^_^ hello_exit-------
			[root@farsight /]# insmod hello_drv.ko
			-------^_^ hello_init-------
			[root@farsight /]# lsmod
			hello_drv 743 0 - Live 0x7f004000
			[root@farsight /]#
	
5---用source insight去写和看代码
	si_linux308-ori.tgz---> 内核源码中

	tar -xvf si_linux308-ori.tgz



6,  模块特性--ko
		模块传参
		模块调用


	MODULE_AUTHOR("wukong@qq.com");
	MODULE_DESCRIPTION("This is a simplest hello driver");

	模块传参
		insmod hello_drv.ko  number=250 name="ruhua"
	
	代码:
		module_param(参数名, 参数类型, 权限)
	
		例子:
			module_param(number, int, 0644);
			module_param(name, charp, 0644);

		
				[root@farsight /drv_module]# ls /sys/module/hello_drv/parameters/ -l
				total 0
				-rw-r--r--    1 0        0             4096 Jan  1 00:01 name
				-rw-r--r--    1 0        0             4096 Jan  1 00:01 number
	
	
	模块调用:
		A.ko					 B.ko
							int func()
	调用func()				{
								xxxx
							}	


	
	被调用函数所在的ko驱动代码: math.c
		#include <linux/init.h>
		#include <linux/module.h>


		int my_add(int a, int b)
		{

			return a+b;

		}
		EXPORT_SYMBOL(my_add);


		int my_sub(int a, int b)
		{

			return a-b;

		}
		EXPORT_SYMBOL(my_sub);
		MODULE_LICENSE("GPL");


	调用:
			#include "math.h"
			

			printk("a+b = %d\n", my_add(30, 8));
			printk("a-b = %d\n", my_sub(38, 8));

	装载:
		[root@farsight /drv_module]# insmod  math.ko
		[root@farsight /drv_module]# insmod hello_drv.ko
			-------^_^ hello_init-------
			number = 120
			name = Alibaba
			a+b = 38
			a-b = 30



7,  一个真正的设备驱动需要一些什么元素
	  a,需要一个设备号
		因为内核中有很多的设备驱动,所以需要一个设备号id来进行区分
		设备号分成两个部分:
			主设备号:某一类设备
			次设备号: 某类设备中某个设备设备
				比如:前置和后置摄像头都是camera这类设备
					
					前置 :  0
					后置 :  1
		在内核中:　dev_t来表示设备号, 32bit的整数
					高12bit: 主设备号
					低20bit:次设备号
			
		
		//参数1---指定一个号码也可以由系统分配,填0表示由系统分配
		//参数2--字符串--描述设备驱动信息--自定义
		//参数3---文件操作对象
		//返回值---如果是系统分配,返回分配之后的号码,否则返回负数为错误
		int register_chrdev(unsigned int major, const char *name,const struct file_operations *fops)


		//参数1---已经申请到的设备号
		//参数2--字符串--描述设备驱动信息--自定义
		void unregister_chrdev(unsigned int major, const char *name)

		
		
	b,需要一个设备文件
		linux中将所有的设备都看成是文件,操作设备其实就是操作文件
		设备文件称为设备节点

		ls -l /dev/
			crw--w----  1 root     tty       4,  10 Nov 23 00:41 tty10
			crw--w----  1 root     tty       4,  11 Nov 23 00:41 tty11
			crw--w----  1 root     tty       4,  12 Nov 23 00:41 tty12

		
		创建:　手动和自动创建
			手动：每次都要创建, /dev中所有的文件都是在内存中
				mknod　文件名   类型 主设备号  次设备号
				
				mknod /dev/hello c 254 0
		
			
			[root@farsight /drv_module]# ls /dev/hello -l
			crw-r--r--    1 0        0         254,   0 Jan  1 00:52 /dev/hello

	c,需要一个设备的操作方法
		
		int hello_drv_open(struct inode *inode, struct file *filp)
		{

			printk("-------^_^ %s-------\n", __FUNCTION__);
			return 0;

		}


		const struct file_operations hello_fops = {
			.open = hello_drv_open,

		};


	
	