主要内容---根文件系统制作
1, 根文件系统目录结构
2, 通过busybox制作根文件系统
3, 烧录镜像---产品模式
------------------------------------
内核移植好nand之后会出现一个分区表:
S3C NAND Driver, (c) 2008 Samsung Electronics
S3C NAND Driver is using hardware ECC.
ONFI flash detected
ONFI param page 0 valid
NAND device: Manufacturer ID: 0x01, Chip ID: 0xd3 (AMD S34ML08G1)
Creating 5 MTD partitions on "s5pv210-nand":
0x000000000000-0x000000100000 : "bootloader-uboot"
0x000000100000-0x000000500000 : "kernel"
0x000000500000-0x000001500000 : "rootfs"
0x000001500000-0x000002500000 : "userdata"
0x000002500000-0x000020000000 : "rest"


[root@farsight /]# cat /proc/partitions
major minor  #blocks  name

  31        0       1024 mtdblock0
  31        1       4096 mtdblock1
  31        2      16384 mtdblock2
  31        3      16384 mtdblock3
  31        4     486400 mtdblock4


1, 制作cramfs镜像:
	mkfs.cramfs  myrootfs/  myrootfs.cramfs

	
	烧录:
		cp -raf myrootfs.cramfs  /tftpboot/
		
	在uboot中:
		FS210 # tftp 0x40008000 myrootfs.cramfs
		FS210 # nand erase 0x500000 0x1000000
		FS210 # nand write 0x40008000 0x500000 0x1000000
		
	挂载:
		FS210 # set bootargs console=ttySAC0,115200 root=/dev/mtdblock2  rootfstype=cramfs
		FS210 # save

	
2, 制作jffs2镜像:E:\Android底层系统开发\1st_根文件系统制作\文件系统ok\mkfs.jffs2
	mkfs.jffs2将这个放入到/usr/bin
			-->sudo mv  mkfs.jffs2 /usr/bin/
	
	mkfs.jffs2 -r myrootfs -o myrootfs.jffs2 -n -e 0x20000
	或者:
		mkfs.jffs2 -r myrootfs -o myrootfs.jffs2 -n -e 0x20000 --pad=0x1000000
	
		
	烧录:
		cp -raf myrootfs.jffs2  /tftpboot/

		
	在uboot中:
		FS210 # tftp 0x40008000 myrootfs.jffs2
		FS210 # nand erase 0x1500000 0x1000000
		FS210 # nand write 0x40008000 0x1500000 0x1000000
		
	挂载:
		FS210 # set bootargs console=ttySAC0,115200 root=/dev/mtdblock3  rootfstype=jffs2
		FS210 # save
	

-----------------------------------------------------------------------------
3, 混合烧录
	vim etc/init.d/rcS
		 16 /sbin/mdev -s
		 17
		 18 echo "------ready to mount /dev/mtdblock3 to /home"
		 19 /bin/mount -t jffs2  /dev/mtdblock3  /home

	重新制作cramfs镜像:
		mkfs.cramfs  myrootfs/  myrootfs.cramfs

	
	烧录:
		cp -raf myrootfs.cramfs  /tftpboot/
		
	在uboot中:
		FS210 # tftp 0x40008000 myrootfs.cramfs
		FS210 # nand erase 0x500000 0x1000000
		FS210 # nand write 0x40008000 0x500000 0x1000000
	挂载:
		FS210 # set bootargs console=ttySAC0,115200 root=/dev/mtdblock2  rootfstype=cramfs
			或者:
				set bootargs console=ttySAC0,115200 root=31:02  rootfstype=cramfs
				set bootargs console=ttySAC0,115200 root=1f02  rootfstype=cramfs
		FS210 # save
			
	========================================================================
	制作一个空的jffs2镜像:
		mkdir usrdata
		mkfs.jffs2 -r usrdata/ -o usrdata.jffs2 -n -e 0x20000 --pad=0x1000000
	
		烧录:
		cp -raf usrdata.jffs2  /tftpboot/

		
		在uboot中:
			FS210 # tftp 0x40008000 usrdata.jffs2
			FS210 # nand erase 0x1500000 0x1000000
			FS210 # nand write 0x40008000 0x1500000 0x1000000
			
			
			
4, 烧录内核:
				
		在uboot中:
			FS210 # tftp 0x40008000 zImage
			FS210 # nand erase 0x100000 0x400000
			FS210 # nand write 0x40008000 0x100000 0x400000
		
			FS210 # set bootcmd  nand read 0x40008000 0x100000 0x400000 \; bootm 0x40008000
			FS210 # save
			
		
		
	
		
		
		
		
		
		
		
		
		
			