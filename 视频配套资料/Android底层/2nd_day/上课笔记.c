sudo vim /etc/samba/smb.conf
[george]
path = /home/george
available = yes
browseable = yes
;public = yes
valid users = george
writable = yes

[opt]
path = /opt
available = yes
browseable = yes
public = yes
writable = yes

[tftpboot]
path = /tftpboot
available = yes
browseable = yes
public = yes
writable = yes

george@George-JI:~$ sudo smbpasswd -a george
New SMB password:
Retype new SMB password:


sudo service smbd restart
----------------------------------------------------------------
主要内容-----制作根文件系统
1, Android的系统构成
2, Android源码的编译
3, 制作根文件系统
4, 运行Android系统
5, 在Android系统开发程序---Android.mk
--------------------------------------------

android系统源码的编译:
1,编译uboot
	tar -xvf uboot-fs210_V5.tar.bz2
		a,修改交叉工具链
			 150 CROSS_COMPILE = /opt/toolchain/toolchain-4.3.2-farsight/bin/arm-none-linux-gnueabi-
		b,选择一个board开发板
			make fs210_nand_config
				Configuring for fs210_nand board...

		c,编译
			make -j2
			
		生成u-boot.bin
	
	arm-none-linux-gnueabi-ld -Bstatic -T /home/george/src_210/uboot-fs210_V5/board/samsung/fs210/u-boot.lds  -Ttext 0x23e00000 $UNDEF_SYM cpu/s5pc11x/start.o \
                        --start-group lib_generic/libgeneric.a cpu/s5pc11x/libs5pc11x.a cpu/s5pc11x/s5pc110/libs5pc110.a lib_arm/libarm.a fs/cramfs/libcramfs.a fs/fat/libfat.a fs/fdos/libfdos.a fs/jffs2/libjffs2.a fs/reiserfs/libreiserfs.a fs/ext2/libext2fs.a fs/ext4/libext4fs.a net/libnet.a disk/libdisk.a drivers/bios_emulator/libatibiosemu.a drivers/block/libblock.a drivers/dma/libdma.a drivers/hwmon/libhwmon.a drivers/i2c/libi2c.a drivers/input/libinput.a drivers/misc/libmisc.a drivers/mmc/libmmc.a drivers/mtd/libmtd.a drivers/mtd/nand/libnand.a drivers/mtd/nand_legacy/libnand_legacy.a drivers/mtd/onenand/libonenand.a drivers/mtd/ubi/libubi.a drivers/mtd/spi/libspi_flash.a drivers/net/libnet.a drivers/net/sk98lin/libsk98lin.a drivers/pci/libpci.a drivers/pcmcia/libpcmcia.a drivers/spi/libspi.a drivers/rtc/librtc.a drivers/serial/libserial.a drivers/usb/libusb.a common/libcommon.a libfdt/libfdt.a api/libapi.a post/libpost.a board/samsung/fs210/libfs210.a --end-group -L /opt/toolchain/toolchain-4.3.2-farsight/bin/../lib/gcc/arm-none-linux-gnueabi/4.3.2 -lgcc \
                        -Map u-boot.map -o u-boot
	
	arm-none-linux-gnueabi-objcopy --gap-fill=0xff -O binary u-boot u-boot.bin
	arm-none-linux-gnueabi-objdump -d u-boot > u-boot.dis
2,编译内核:
	tar -xvf linux-3.0.8-FS210.tar.bz2
		a,修改交叉工具链	
			ARCH            ?= arm
			CROSS_COMPILE   ?= /opt/toolchain/toolchain-4.5.1-farsight/bin/arm-none-linux-gnueabi-
		b,选择一个soc
			cp config_for_FS210_Android_v1.0 .config
		
		c,make meuconfig
		
		
			vim arch/arm/mach-s5pv210/mach-fs210.c
				static struct dm9000_plat_data fs210_dm9000_platdata = {
						.flags          = DM9000_PLATF_16BITONLY | DM9000_PLATF_NO_EEPROM,
						.dev_addr       = { 0x00, 0xbd, 0x2b, 0x2b, 0x2b, 0x22 },
				};

		
		d,make uImage 

3, 编译Android源码:
	1, 初始化编译环境,执行脚本---设置交叉工具链, 提供编译命令:lunch
		
		george@George-JI:~/src_210/android4.0-fs210_v2$ source ./build/envsetup.sh
			including device/farsight/fs210/vendorsetup.sh
			including device/moto/stingray/vendorsetup.sh
			including device/moto/wingray/vendorsetup.sh
			including sdk/bash_completion/adb.bash
		
	2,选择一个产品
		george@George-JI:~/src_210/android4.0-fs210_v2$ lunch //或者lunch full_fs210-eng

		You're building on Linux

		Lunch menu... pick a combo:
			 1. full-eng
			 2. full_x86-eng
			 3. vbox_x86-eng
			 4. full_fs210-userdebug
			 5. full_fs210-eng
			 6. full_stingray-userdebug
			 7. full_wingray-userdebug

		Which would you like? [full-eng] 5
		============================================
		PLATFORM_VERSION_CODENAME=REL
		PLATFORM_VERSION=4.0.4
		TARGET_PRODUCT=full_fs210
		TARGET_BUILD_VARIANT=eng
		TARGET_BUILD_TYPE=release
		TARGET_BUILD_APPS=
		TARGET_ARCH=arm
		TARGET_ARCH_VARIANT=armv7-a-neon
		HOST_ARCH=x86
		HOST_OS=linux
		HOST_BUILD_TYPE=release
		BUILD_ID=IMM76I
		
		
		注意:每次打开一个新的终端都要执行以上两个步骤

	3,编译
		make 
		
	
	
	制作一个根文件系统:关注:out/target/product/fs210/
	核心的三个步骤: 只是演示
		 cp -raf out/target/product/fs210/root/*  myrootfs/	                         */
		 cp -raf out/target/product/fs210/data/  myrootfs/
		 cp -raf out/target/product/fs210/system  myrootfs/
		
		george@George-JI:~/src_210/android4.0-fs210_v2$ ls myrootfs/
			data : //应用或者是系统的数据  /data/data/包/files/       

			dev 
			proc  
			sys		
			sbin
			
			init  // android祖先进程, set  bootargs  init=/init
			init.rc //启动脚本,类似与/etc/init.d/rcS
			init.fs210.usb.rc  
			init.fs210.rc     
			init.goldfish.rc      

			ueventd.rc //自动创建设备节点的脚本---功能类似与mdev -s
			ueventd.goldfish.rc
			ueventd.fs210.rc  	
			
			default.prop   //属性文件   
			
			system
				|
				app // 系统自带的apk, 开机的时候会自动安装 			
				bin  
				lib
				usr
				xbin
				etc
				
				build.prop  //属性文件  
				 
				fonts  //字体库
				framework // framework层的java代码编译之后的二进制文件class文件, 开机自动执行  
				media // 开机动画 
				tts   //语言支持包
				vendor  //厂商提供的二进制文件

	真正制作根文件系统:用一个脚本：fs210_build.sh
		先修改：
			33 #       make -j$CPU_JOB_NUM
			80         chmod -R 777 rootfs_dir/system/vendor
			
		执行：
			./fs210_build.sh
			最终：生成 out/target/product/fs210/rootfs_dir/
			
	
运行Ａｎｄｒｏｉｄ系统：tftp + nfs
		1, 烧录uboot.bin
		
		2, tftp启动内核;
			cp -raf arch/arm/boot/uImage  /tftpboot/
			
		3, nfs挂载
			cp -raf out/target/product/fs210/rootfs_dir/  /opt/
			
			sudo vim /etc/exports
				/opt/rootfs_dir            *(subtree_check,rw,no_root_squash,async)
				
			sudo service nfs-kernel-server restart
	
	
		在uboot中:
			FS210 # set serverip 192.168.7.5
			FS210 # set ipaddr 192.168.7.7
			FS210 # set bootcmd tftp 0x40008000 uImage \; bootm 0x40008000
			FS210 # set bootargs console=ttySAC0,115200  init=/init root=/dev/nfs nfsroot=192.168.7.5:/opt/rootfs_dir ip=192.168.7.7
			FS210 # save
			
			
查看图形界面:
		E:\Android底层系统开发\各种问题解决方法\无需LCD显示Android界面
		
		在开发板上运行:
		root@android:/ # ./androidvncserver
			Initializing framebuffer device /dev/graphics/fb0...
			xres=800, yres=480, xresv=800, yresv=960, xoffs=0, yoffs=0, bpp=32
			Initializing keyboard device /dev/input/event0 ...
			Initializing touch device /dev/input/event3 ...
					xmin:  0
					xmax: 0
					ymin:  0
					ymax: 0
			Initializing VNC server:
					width:  800
					height: 480
					bpp:    32
					port:   5901
			Initializing server...
			23/12/2016 03:42:16 Listening for VNC connections on TCP port 5901

			
			
		window下启动vnc viewer:
			
			192.168.7.7:5901
			

--------------------------------------------------------------------			
在Android系统开发程序---Android.mk
c/c++的应用程序:
	
	
	1, 编写代码---必须在Android源码目录下操作
		mytest/c_test$ touch hello.c
	
		#include <stdio.h>
		#include <sys/types.h>
		#include <sys/stat.h>
		#include <fcntl.h>
		#include <stdlib.h>

		int main(int argc, char *argv[])
		{
				printf("hello android world\n");

				int fd = open("/dev/xxx", O_RDWR);
				if(fd < 0)
				{
						perror("open");
						exit(1);
				}

				return 0;
		}

	
	2, 编译代码---Android.mk
		目标:依赖
			编译方法
		
		编写方法---抄模板
		模板:hardware/ril/rild/Android.mk
		
		#表示获取当前路径
		LOCAL_PATH:= $(call my-dir)
		#清空以LOCAL_XX的变量, LOCAL_PATH除外
		include $(CLEAR_VARS)
		
		#在任何情况都会编译这个模块
		LOCAL_MODULE_TAGS := optional
		
		#指定源文件
		LOCAL_SRC_FILES:= \
				hello.c
		#指定目标文件
		LOCAL_MODULE:= hello_arm_elf
		#编译成可执行程序
		include $(BUILD_EXECUTABLE)

	george@George-JI:~/src_210/android4.0-fs210_v2$ mmm mytest/c_test/
	============================================
	PLATFORM_VERSION_CODENAME=REL
	PLATFORM_VERSION=4.0.4
	TARGET_PRODUCT=full_fs210
	TARGET_BUILD_VARIANT=eng
	TARGET_BUILD_TYPE=release
	TARGET_BUILD_APPS=
	TARGET_ARCH=arm
	TARGET_ARCH_VARIANT=armv7-a-neon
	HOST_ARCH=x86
	HOST_OS=linux
	HOST_BUILD_TYPE=release
	BUILD_ID=IMM76I
	============================================
	make:进入目录'/home/george/src_210/android4.0-fs210_v2'
	target thumb C: hello_arm_elf <= mytest/c_test/hello.c
	target Executable: hello_arm_elf (out/target/product/fs210/obj/EXECUTABLES/hello_arm_elf_intermediates/LINKED/hello_arm_elf)
	target Symbolic: hello_arm_elf (out/target/product/fs210/symbols/system/bin/hello_arm_elf)
	target Strip: hello_arm_elf (out/target/product/fs210/obj/EXECUTABLES/hello_arm_elf_intermediates/hello_arm_elf)
	Install: out/target/product/fs210/system/bin/hello_arm_elf
	
		
	3, 运行代码--在开发板
		cp -raf  out/target/product/fs210/system/bin/hello_arm_elf   /opt/rootfs_dir
		
		
		root@android:/ # ./hello_arm_elf                                                                                                 hello android world
			open: No such file or directory

	4, 调试代码



















			


