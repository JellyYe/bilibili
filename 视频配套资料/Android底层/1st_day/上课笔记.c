��Ҫ����---���ļ�ϵͳ����
1, ���ļ�ϵͳĿ¼�ṹ
2, ͨ��busybox�������ļ�ϵͳ
3, ��¼����---��Ʒģʽ
------------------------------------
�ں���ֲ��nand֮������һ��������:
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


1, ����cramfs����:
	mkfs.cramfs  myrootfs/  myrootfs.cramfs

	
	��¼:
		cp -raf myrootfs.cramfs  /tftpboot/
		
	��uboot��:
		FS210 # tftp 0x40008000 myrootfs.cramfs
		FS210 # nand erase 0x500000 0x1000000
		FS210 # nand write 0x40008000 0x500000 0x1000000
		
	����:
		FS210 # set bootargs console=ttySAC0,115200 root=/dev/mtdblock2  rootfstype=cramfs
		FS210 # save

	
2, ����jffs2����:E:\Android�ײ�ϵͳ����\1st_���ļ�ϵͳ����\�ļ�ϵͳok\mkfs.jffs2
	mkfs.jffs2��������뵽/usr/bin
			-->sudo mv  mkfs.jffs2 /usr/bin/
	
	mkfs.jffs2 -r myrootfs -o myrootfs.jffs2 -n -e 0x20000
	����:
		mkfs.jffs2 -r myrootfs -o myrootfs.jffs2 -n -e 0x20000 --pad=0x1000000
	
		
	��¼:
		cp -raf myrootfs.jffs2  /tftpboot/

		
	��uboot��:
		FS210 # tftp 0x40008000 myrootfs.jffs2
		FS210 # nand erase 0x1500000 0x1000000
		FS210 # nand write 0x40008000 0x1500000 0x1000000
		
	����:
		FS210 # set bootargs console=ttySAC0,115200 root=/dev/mtdblock3  rootfstype=jffs2
		FS210 # save
	

-----------------------------------------------------------------------------
3, �����¼
	vim etc/init.d/rcS
		 16 /sbin/mdev -s
		 17
		 18 echo "------ready to mount /dev/mtdblock3 to /home"
		 19 /bin/mount -t jffs2  /dev/mtdblock3  /home

	��������cramfs����:
		mkfs.cramfs  myrootfs/  myrootfs.cramfs

	
	��¼:
		cp -raf myrootfs.cramfs  /tftpboot/
		
	��uboot��:
		FS210 # tftp 0x40008000 myrootfs.cramfs
		FS210 # nand erase 0x500000 0x1000000
		FS210 # nand write 0x40008000 0x500000 0x1000000
	����:
		FS210 # set bootargs console=ttySAC0,115200 root=/dev/mtdblock2  rootfstype=cramfs
			����:
				set bootargs console=ttySAC0,115200 root=31:02  rootfstype=cramfs
				set bootargs console=ttySAC0,115200 root=1f02  rootfstype=cramfs
		FS210 # save
			
	========================================================================
	����һ���յ�jffs2����:
		mkdir usrdata
		mkfs.jffs2 -r usrdata/ -o usrdata.jffs2 -n -e 0x20000 --pad=0x1000000
	
		��¼:
		cp -raf usrdata.jffs2  /tftpboot/

		
		��uboot��:
			FS210 # tftp 0x40008000 usrdata.jffs2
			FS210 # nand erase 0x1500000 0x1000000
			FS210 # nand write 0x40008000 0x1500000 0x1000000
			
			
			
4, ��¼�ں�:
				
		��uboot��:
			FS210 # tftp 0x40008000 zImage
			FS210 # nand erase 0x100000 0x400000
			FS210 # nand write 0x40008000 0x100000 0x400000
		
			FS210 # set bootcmd  nand read 0x40008000 0x100000 0x400000 \; bootm 0x40008000
			FS210 # save
			
		
		
	
		
		
		
		
		
		
		
		
		
			