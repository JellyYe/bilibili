 
1,�ں�Դ��ı���:
	a,��ȡԴ��----E:\Linux�����γ�-george\source\linux-3.0.8.tar.bz2
		www.kernel.org
	
		 tar -xvf linux-3.0.8.tar.bz2
	
	b, �ں˱��벽��:�ο�E:\Linux�����γ�-george\��������\��һ��\ʵ��\ϵͳ�������ֲ�ĵ�.doc
		1, ���ý��湤����
			 195 ARCH            ?= arm
			 196 CROSS_COMPILE   ?= arm-none-linux-gnueabi-

		2, ѡ��ǰ��soc---s5pv210
			make s5pv210_defconfig

		3, make menuconfig 
			System Type  ---> 
				(0) S3C UART to use for low-level messages 
		
			ע��: ִ��make menuconfig �п���ȱ��һ����:ncurse
						sudo apt-get install libncurses5-dev

		4, make zImage ���� make zImage

	c, ��ֲ�ں�---����dm9000����
		cp -raf arch/arm/boot/zImage  /tftpboot/	
	

2,��д����
	

	a,ͷ�ļ�
		#include <linux/init.h>
		#include <linux/module.h>

	b,��������ģ���װ�غ�ж�غ���
		module_init(hello_init);
		module_exit(hello_exit);
	c,ʵ������ģ���װ�غ�ж�غ���
		static int __init hello_init(void)
		{
				printk("-------^_^ %s-------\n", __FUNCTION__);

				return 0;
		}


		static void __exit hello_exit(void)
		{

				printk("-------^_^ %s-------\n", __FUNCTION__);

		}

	d,���GPL��֤
		MODULE_LICENSE("GPL");

	Makefile---����:���Ҫ��������ģ��ͱ����ȱ����ں�Դ��
		#�ں�Դ��·��---
		KERNEL_DIR = /home/farsight/linux_system/kernel/linux-3.0.8��

		CUR_DIR = $(shell pwd)

		all :
				#��ʾ���뵽�ں�Ŀ¼,���Ҹ����ں�Ҫ����ǰ��Դ������ģ��
				make -C $(KERNEL_DIR) M=$(CUR_DIR) modules

		clean :
				make -C $(KERNEL_DIR) M=$(CUR_DIR) clean



		#ָ�������ĸ�Դ�ļ�
		obj-m = hello_drv.o


3, ���п�����---ǰ��,�ں��Ѿ���ֲ��
	
	���������ں�: tftp
		pc��:
			 cp -raf arch/arm/boot/zImage   /tftpboot
		
		uboot��:
			FS210 # set serverip 192.168.7.3  #ָ��tftp��������ip
			FS210 # set ipaddr 192.168.7.7		#ָ���������ip
			FS210 # set bootcmd tftp  0x40008000 zImage \; bootm 0x40008000
			FS210 # save


	���ظ��ļ�ϵͳ:  nfs����
		
		ubuntu pc����
				E:\Linux�����γ�-george\source\rootfs_fs210.tgz--->ubuntu��/opt
			 sudo tar -xvf rootfs_fs210.tgz
		
			����nfs������: /opt/rootfs���Ա�����
				sudo vim /etc/exports
					/opt/rootfs   *(subtree_check,rw,no_root_squash,async)
				
				����:
					sudo service nfs-kernel-server restart
		

		uboot��:
			FS210 # set bootargs console=ttySAC0,115200 root=/dev/nfs nfsroot=192.168.7.3:/opt/rootfs ip=192.168.7.7 init=/linuxrc
			FS210 # save

			
			
	

			����: bootargs��һ������,���ݸ��ں�
				 console=ttySAC0,115200 //  console=xxx��ʾ���ĸ��豸��ӡ��Ϣ
				root=/dev/nfs  //�����ں�Ҫͨ��nfs��ʽȥ���ظ��ļ�ϵͳ
				nfsroot=ip:path // ������ص�·��
				ip=xxx //ָ���������ip
	

4, ����/װ������ģ��
		cp -raf hello_drv.ko  /opt/rootfs


		�ڿ�������:
			[root@farsight /]# insmod hello_drv.ko
			-------^_^ hello_init-------
			[root@farsight /]# rmmod hello_drv
			-------^_^ hello_exit-------
			[root@farsight /]# insmod hello_drv.ko
			-------^_^ hello_init-------
			[root@farsight /]# lsmod
			hello_drv 743 0 - Live 0x7f004000
			[root@farsight /]#
	
5---��source insightȥд�Ϳ�����
	si_linux308-ori.tgz---> �ں�Դ����

	tar -xvf si_linux308-ori.tgz



6,  ģ������--ko
		ģ�鴫��
		ģ�����


	MODULE_AUTHOR("wukong@qq.com");
	MODULE_DESCRIPTION("This is a simplest hello driver");

	ģ�鴫��
		insmod hello_drv.ko  number=250 name="ruhua"
	
	����:
		module_param(������, ��������, Ȩ��)
	
		����:
			module_param(number, int, 0644);
			module_param(name, charp, 0644);

		
				[root@farsight /drv_module]# ls /sys/module/hello_drv/parameters/ -l
				total 0
				-rw-r--r--    1 0        0             4096 Jan  1 00:01 name
				-rw-r--r--    1 0        0             4096 Jan  1 00:01 number
	
	
	ģ�����:
		A.ko					 B.ko
							int func()
	����func()				{
								xxxx
							}	


	
	�����ú������ڵ�ko��������: math.c
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


	����:
			#include "math.h"
			

			printk("a+b = %d\n", my_add(30, 8));
			printk("a-b = %d\n", my_sub(38, 8));

	װ��:
		[root@farsight /drv_module]# insmod  math.ko
		[root@farsight /drv_module]# insmod hello_drv.ko
			-------^_^ hello_init-------
			number = 120
			name = Alibaba
			a+b = 38
			a-b = 30



7,  һ���������豸������ҪһЩʲôԪ��
	  a,��Ҫһ���豸��
		��Ϊ�ں����кܶ���豸����,������Ҫһ���豸��id����������
		�豸�ŷֳ���������:
			���豸��:ĳһ���豸
			���豸��: ĳ���豸��ĳ���豸�豸
				����:ǰ�úͺ�������ͷ����camera�����豸
					
					ǰ�� :  0
					���� :  1
		���ں���:��dev_t����ʾ�豸��, 32bit������
					��12bit: ���豸��
					��20bit:���豸��
			
		
		//����1---ָ��һ������Ҳ������ϵͳ����,��0��ʾ��ϵͳ����
		//����2--�ַ���--�����豸������Ϣ--�Զ���
		//����3---�ļ���������
		//����ֵ---�����ϵͳ����,���ط���֮��ĺ���,���򷵻ظ���Ϊ����
		int register_chrdev(unsigned int major, const char *name,const struct file_operations *fops)


		//����1---�Ѿ����뵽���豸��
		//����2--�ַ���--�����豸������Ϣ--�Զ���
		void unregister_chrdev(unsigned int major, const char *name)

		
		
	b,��Ҫһ���豸�ļ�
		linux�н����е��豸���������ļ�,�����豸��ʵ���ǲ����ļ�
		�豸�ļ���Ϊ�豸�ڵ�

		ls -l /dev/
			crw--w----  1 root     tty       4,  10 Nov 23 00:41 tty10
			crw--w----  1 root     tty       4,  11 Nov 23 00:41 tty11
			crw--w----  1 root     tty       4,  12 Nov 23 00:41 tty12

		
		����:���ֶ����Զ�����
			�ֶ���ÿ�ζ�Ҫ����, /dev�����е��ļ��������ڴ���
				mknod���ļ���   ���� ���豸��  ���豸��
				
				mknod /dev/hello c 254 0
		
			
			[root@farsight /drv_module]# ls /dev/hello -l
			crw-r--r--    1 0        0         254,   0 Jan  1 00:52 /dev/hello

	c,��Ҫһ���豸�Ĳ�������
		
		int hello_drv_open(struct inode *inode, struct file *filp)
		{

			printk("-------^_^ %s-------\n", __FUNCTION__);
			return 0;

		}


		const struct file_operations hello_fops = {
			.open = hello_drv_open,

		};


	
	