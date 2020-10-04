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
��Ҫ����-----�������ļ�ϵͳ
1, Android��ϵͳ����
2, AndroidԴ��ı���
3, �������ļ�ϵͳ
4, ����Androidϵͳ
5, ��Androidϵͳ��������---Android.mk
--------------------------------------------

androidϵͳԴ��ı���:
1,����uboot
	tar -xvf uboot-fs210_V5.tar.bz2
		a,�޸Ľ��湤����
			 150 CROSS_COMPILE = /opt/toolchain/toolchain-4.3.2-farsight/bin/arm-none-linux-gnueabi-
		b,ѡ��һ��board������
			make fs210_nand_config
				Configuring for fs210_nand board...

		c,����
			make -j2
			
		����u-boot.bin
	
	arm-none-linux-gnueabi-ld -Bstatic -T /home/george/src_210/uboot-fs210_V5/board/samsung/fs210/u-boot.lds  -Ttext 0x23e00000 $UNDEF_SYM cpu/s5pc11x/start.o \
                        --start-group lib_generic/libgeneric.a cpu/s5pc11x/libs5pc11x.a cpu/s5pc11x/s5pc110/libs5pc110.a lib_arm/libarm.a fs/cramfs/libcramfs.a fs/fat/libfat.a fs/fdos/libfdos.a fs/jffs2/libjffs2.a fs/reiserfs/libreiserfs.a fs/ext2/libext2fs.a fs/ext4/libext4fs.a net/libnet.a disk/libdisk.a drivers/bios_emulator/libatibiosemu.a drivers/block/libblock.a drivers/dma/libdma.a drivers/hwmon/libhwmon.a drivers/i2c/libi2c.a drivers/input/libinput.a drivers/misc/libmisc.a drivers/mmc/libmmc.a drivers/mtd/libmtd.a drivers/mtd/nand/libnand.a drivers/mtd/nand_legacy/libnand_legacy.a drivers/mtd/onenand/libonenand.a drivers/mtd/ubi/libubi.a drivers/mtd/spi/libspi_flash.a drivers/net/libnet.a drivers/net/sk98lin/libsk98lin.a drivers/pci/libpci.a drivers/pcmcia/libpcmcia.a drivers/spi/libspi.a drivers/rtc/librtc.a drivers/serial/libserial.a drivers/usb/libusb.a common/libcommon.a libfdt/libfdt.a api/libapi.a post/libpost.a board/samsung/fs210/libfs210.a --end-group -L /opt/toolchain/toolchain-4.3.2-farsight/bin/../lib/gcc/arm-none-linux-gnueabi/4.3.2 -lgcc \
                        -Map u-boot.map -o u-boot
	
	arm-none-linux-gnueabi-objcopy --gap-fill=0xff -O binary u-boot u-boot.bin
	arm-none-linux-gnueabi-objdump -d u-boot > u-boot.dis
2,�����ں�:
	tar -xvf linux-3.0.8-FS210.tar.bz2
		a,�޸Ľ��湤����	
			ARCH            ?= arm
			CROSS_COMPILE   ?= /opt/toolchain/toolchain-4.5.1-farsight/bin/arm-none-linux-gnueabi-
		b,ѡ��һ��soc
			cp config_for_FS210_Android_v1.0 .config
		
		c,make meuconfig
		
		
			vim arch/arm/mach-s5pv210/mach-fs210.c
				static struct dm9000_plat_data fs210_dm9000_platdata = {
						.flags          = DM9000_PLATF_16BITONLY | DM9000_PLATF_NO_EEPROM,
						.dev_addr       = { 0x00, 0xbd, 0x2b, 0x2b, 0x2b, 0x22 },
				};

		
		d,make uImage 

3, ����AndroidԴ��:
	1, ��ʼ�����뻷��,ִ�нű�---���ý��湤����, �ṩ��������:lunch
		
		george@George-JI:~/src_210/android4.0-fs210_v2$ source ./build/envsetup.sh
			including device/farsight/fs210/vendorsetup.sh
			including device/moto/stingray/vendorsetup.sh
			including device/moto/wingray/vendorsetup.sh
			including sdk/bash_completion/adb.bash
		
	2,ѡ��һ����Ʒ
		george@George-JI:~/src_210/android4.0-fs210_v2$ lunch //����lunch full_fs210-eng

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
		
		
		ע��:ÿ�δ�һ���µ��ն˶�Ҫִ��������������

	3,����
		make 
		
	
	
	����һ�����ļ�ϵͳ:��ע:out/target/product/fs210/
	���ĵ���������: ֻ����ʾ
		 cp -raf out/target/product/fs210/root/*  myrootfs/	                         */
		 cp -raf out/target/product/fs210/data/  myrootfs/
		 cp -raf out/target/product/fs210/system  myrootfs/
		
		george@George-JI:~/src_210/android4.0-fs210_v2$ ls myrootfs/
			data : //Ӧ�û�����ϵͳ������  /data/data/��/files/       

			dev 
			proc  
			sys		
			sbin
			
			init  // android���Ƚ���, set  bootargs  init=/init
			init.rc //�����ű�,������/etc/init.d/rcS
			init.fs210.usb.rc  
			init.fs210.rc     
			init.goldfish.rc      

			ueventd.rc //�Զ������豸�ڵ�Ľű�---����������mdev -s
			ueventd.goldfish.rc
			ueventd.fs210.rc  	
			
			default.prop   //�����ļ�   
			
			system
				|
				app // ϵͳ�Դ���apk, ������ʱ����Զ���װ 			
				bin  
				lib
				usr
				xbin
				etc
				
				build.prop  //�����ļ�  
				 
				fonts  //�����
				framework // framework���java�������֮��Ķ������ļ�class�ļ�, �����Զ�ִ��  
				media // �������� 
				tts   //����֧�ְ�
				vendor  //�����ṩ�Ķ������ļ�

	�����������ļ�ϵͳ:��һ���ű���fs210_build.sh
		���޸ģ�
			33 #       make -j$CPU_JOB_NUM
			80         chmod -R 777 rootfs_dir/system/vendor
			
		ִ�У�
			./fs210_build.sh
			���գ����� out/target/product/fs210/rootfs_dir/
			
	
���У��������ϵͳ��tftp + nfs
		1, ��¼uboot.bin
		
		2, tftp�����ں�;
			cp -raf arch/arm/boot/uImage  /tftpboot/
			
		3, nfs����
			cp -raf out/target/product/fs210/rootfs_dir/  /opt/
			
			sudo vim /etc/exports
				/opt/rootfs_dir            *(subtree_check,rw,no_root_squash,async)
				
			sudo service nfs-kernel-server restart
	
	
		��uboot��:
			FS210 # set serverip 192.168.7.5
			FS210 # set ipaddr 192.168.7.7
			FS210 # set bootcmd tftp 0x40008000 uImage \; bootm 0x40008000
			FS210 # set bootargs console=ttySAC0,115200  init=/init root=/dev/nfs nfsroot=192.168.7.5:/opt/rootfs_dir ip=192.168.7.7
			FS210 # save
			
			
�鿴ͼ�ν���:
		E:\Android�ײ�ϵͳ����\��������������\����LCD��ʾAndroid����
		
		�ڿ�����������:
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

			
			
		window������vnc viewer:
			
			192.168.7.7:5901
			

--------------------------------------------------------------------			
��Androidϵͳ��������---Android.mk
c/c++��Ӧ�ó���:
	
	
	1, ��д����---������AndroidԴ��Ŀ¼�²���
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

	
	2, �������---Android.mk
		Ŀ��:����
			���뷽��
		
		��д����---��ģ��
		ģ��:hardware/ril/rild/Android.mk
		
		#��ʾ��ȡ��ǰ·��
		LOCAL_PATH:= $(call my-dir)
		#�����LOCAL_XX�ı���, LOCAL_PATH����
		include $(CLEAR_VARS)
		
		#���κ��������������ģ��
		LOCAL_MODULE_TAGS := optional
		
		#ָ��Դ�ļ�
		LOCAL_SRC_FILES:= \
				hello.c
		#ָ��Ŀ���ļ�
		LOCAL_MODULE:= hello_arm_elf
		#����ɿ�ִ�г���
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
	make:����Ŀ¼'/home/george/src_210/android4.0-fs210_v2'
	target thumb C: hello_arm_elf <= mytest/c_test/hello.c
	target Executable: hello_arm_elf (out/target/product/fs210/obj/EXECUTABLES/hello_arm_elf_intermediates/LINKED/hello_arm_elf)
	target Symbolic: hello_arm_elf (out/target/product/fs210/symbols/system/bin/hello_arm_elf)
	target Strip: hello_arm_elf (out/target/product/fs210/obj/EXECUTABLES/hello_arm_elf_intermediates/hello_arm_elf)
	Install: out/target/product/fs210/system/bin/hello_arm_elf
	
		
	3, ���д���--�ڿ�����
		cp -raf  out/target/product/fs210/system/bin/hello_arm_elf   /opt/rootfs_dir
		
		
		root@android:/ # ./hello_arm_elf                                                                                                 hello android world
			open: No such file or directory

	4, ���Դ���



















			


