主要内容---内核工作原理解析
1, 内核的编译步骤
2, Kconfig和Makefile的使用
3, 内核的裁剪---make menuconfig
4, 内核的工作原理:
	a, 内核的内部构造
	b, 内核的启动步骤
	c, 内核中module_init是什么
	d, 挂载是什么意思
	e, 祖先进程init是如何被启动的
----------------------------------------------------
1, 内核的编译步骤
		1, 设置交叉工具链
			 195 ARCH            ?= arm
			 196 CROSS_COMPILE   ?= arm-none-linux-gnueabi-

		2, 选择当前的soc---s5pv210
			make s5pv210_defconfig

		3, make menuconfig --内核裁剪
			System Type  ---> 
				(0) S3C UART to use for low-level messages 
		
			注意: 执行make menuconfig 有可能缺少一个包:ncurse
						sudo apt-get install libncurses5-dev

		4, make zImage 或者 make zImage
		
	
	
	Kconfig和Makefile的使用
	
	
	Kconfig:arch/arm/Kconfig
			menu "Test Support"

		config MYTEST
				tristate "This is a simple test"
				help
				 you can choose y/n/m to build it


		choice
				prompt "Fruit Type"

		config APPLE
				bool "I have a apple"
				help
						i have a pen

		config BANANA
				bool "yellow banana"
				help
						give you

		config WATERMELON
				bool "big xigua"
				help
						xixixixxixigu


		endchoice


		endmenu

			
			

uImage=64字节头+zImage
			
	uboot: 启动内核---linux, bsd, vxwork, 但是启动方式不一样	
			linux: arm, x86, mips等
	
	启动zImage: 因为uboot做了移植, 默认认为zImage就是arm架构的linux内核
	64字节头包含了描述内核的信息:mkimage增加64字节信息的工具, mkimage是由uboot提供
	手动添加64字节头:
		farsight@ubuntu:/tftpboot$ ./mkimage
Usage: ./mkimage -l image
          -l ==> list image header information
       ./mkimage [-x] -A arch -O os -T type -C comp -a addr -e ep -n name -d data_file[:data_file...] image
          -A ==> set architecture to 'arch'//设定架构:arm, mips, x86
          -O ==> set operating system to 'os'//操作系统类型:linux, bsd, vxwork
          -T ==> set image type to 'type'//镜像类型: kernel,  ramdisk
          -C ==> set compression type 'comp'//压缩类型:gzip2, bzip2, none
          -a ==> set load address to 'addr' (hex)//内核被加载到内存的地址(参考值): 0x20008000
          -e ==> set entry point to 'ep' (hex)//内核被执行的地址: 0x20008000
          -n ==> set image name to 'name'// 描述语句:自定义
          -d ==> use image data from 'datafile'//指定增加头部信息的文件---zImage
        

	./mkimage -A arm -O linux -T kernel -C none -a 0x20008000 -e 0x20008000 -n "Linux 3.0.8-jihq" -d zImage  uImage
	Image Name:   Linux 3.0.8-jihq
	Created:      Tue Dec 20 17:49:19 2016
	Image Type:   ARM Linux Kernel Image (uncompressed)
	Data Size:    1949140 Bytes = 1903.46 kB = 1.86 MB
	Load Address: 0x20008000
	Entry Point:  0x20008000
	
	
	如果要启动uImage
	FS210 # set bootcmd tftp 0x40008000 uImage \; bootm 0x40008000
	FS210 # save

	
	基本的lds脚本:
	OUTPUT_ARCH(arm)
	ENTRY(_start)
	SECTIONS
	SECTIONS
	{
			. = 0x30008000;
			.text : {
					start.o (.text)
					ledc.o (.text)
					* (.text)
			}

			.rodata : {
					* (.rodata)
			}

			.data : {
					* (.data)
			}

			__bss_start = .;
			.bss : {
					* (.bss)
			}
			__bss_end = .;


	}


		
	内核的内部构造:
	 559 all: vmlinux	
			
	 730 # Build vmlinux
	 731 # ---------------------------------------------------------------------------
	 732 # vmlinux is built from the objects selected by $(vmlinux-init) and
	 733 # $(vmlinux-main). Most are built-in.o files from top-level directories
	 734 # in the kernel tree, others are specified in arch/$(ARCH)/Makefile.
	 735 # Ordering when linking is important, and $(vmlinux-init) must be first.
	 736 #
	 737 # vmlinux
	 738 #   ^
	 739 #   |
	 740 #   +-< $(vmlinux-init)
	 741 #   |   +--< init/version.o + more
	 742 #   |
	 743 #   +--< $(vmlinux-main)
	 744 #   |    +--< driver/built-in.o mm/built-in.o + more
	 745 #   |
	 746 #   +-< kallsyms.o (see description in CONFIG_KALLSYMS section)
	 
	 757 vmlinux-init := $(head-y) $(init-y)
	 758 vmlinux-main := $(core-y) $(libs-y) $(drivers-y) $(net-y)
	 759 vmlinux-all  := $(vmlinux-init) $(vmlinux-main)
	 760 vmlinux-lds  := arch/arm/kernel/vmlinux.lds
	 
	 
	 arch/arm/kernel/vmlinux.lds:
		
	OUTPUT_ARCH(arm)
	ENTRY(stext)
 
	 SECTIONS
{
 . = 0x80000000 + 0x00008000;
 .init : { /* Init code and data                */
  _stext = .;
  _sinittext = .;
   *(.head.text)
   *(.init.text) 
   *(.cpuinit.text) 
   *(.meminit.text)

  _einittext = .;
  
  __proc_info_begin = .; 
  *(.proc.info.init) // 处理器信息段
  __proc_info_end = .;
  
  __arch_info_begin = .;
   *(.arch.info.init) // 对象对象段,机器信息段--MACHINE_START
  __arch_info_end = .;
  
  __tagtable_begin = .;
   *(.taglist.init) //uboot传递bootargs数据的处理段
  __tagtable_end = .;
  
  __setup_start = .; 
  *(.init.setup) //  bootargs中各个参数的处理段
  __setup_end = .;

  __initcall_start = .; 
  *(.initcallearly.init) 

  __early_initcall_end = .;//驱动函数执行段
  *(.initcall0.init) *(.initcall0s.init) 
  *(.initcall1.init) *(.initcall1s.init)
  *(.initcall2.init) *(.initcall2s.init) //这个段全部都是函数指针
  *(.initcall3.init) *(.initcall3s.init) //如果批量执行这里的所有函数指针,驱动的入口函数就会被执行
  *(.initcall4.init) *(.initcall4s.init) //subsys_initcall 
  *(.initcall5.init) *(.initcall5s.init) 
  *(.initcallrootfs.init) 
  *(.initcall6.init) *(.initcall6s.init) // module_init
  *(.initcall7.init) *(.initcall7s.init) 
  __initcall_end = .;
	 
	 
	 
	 
	 






module_init(led_drv_init); //是一个函数指针,指针指向参数

typedef int (*initcall_t)(void);
#define module_init(x)	__initcall(x);
#define __initcall(fn) device_initcall(fn)
#define device_initcall(fn)		__define_initcall("6",fn,6)
#define __define_initcall(level,fn,id) \
	static initcall_t __initcall_##fn##id __used \
	__attribute__((__section__(".initcall" level ".init"))) = fn



module_init(led_drv_init);
==>
	static initcall_t 	__initcall_led_drv_init6 
		__used __attribute__((__section__(".initcall6.init"))) = led_drv_init



#define pure_initcall(fn)		__define_initcall("0",fn,0)

#define core_initcall(fn)		__define_initcall("1",fn,1)
#define core_initcall_sync(fn)		__define_initcall("1s",fn,1s)
#define postcore_initcall(fn)		__define_initcall("2",fn,2)
#define postcore_initcall_sync(fn)	__define_initcall("2s",fn,2s)
#define arch_initcall(fn)		__define_initcall("3",fn,3)
#define arch_initcall_sync(fn)		__define_initcall("3s",fn,3s)
#define subsys_initcall(fn)		__define_initcall("4",fn,4)
#define subsys_initcall_sync(fn)	__define_initcall("4s",fn,4s)
#define fs_initcall(fn)			__define_initcall("5",fn,5)
#define fs_initcall_sync(fn)		__define_initcall("5s",fn,5s)
#define rootfs_initcall(fn)		__define_initcall("rootfs",fn,rootfs)
#define device_initcall(fn)		__define_initcall("6",fn,6)
#define device_initcall_sync(fn)	__define_initcall("6s",fn,6s)
#define late_initcall(fn)		__define_initcall("7",fn,7)
#define late_initcall_sync(fn)		__define_initcall("7s",fn,7s)

	 
	 
	 
MACHINE_START(SMDKV210, "SMDKV210")
	/* Maintainer: Kukjin Kim <kgene.kim@samsung.com> */
	.boot_params	= S5P_PA_SDRAM + 0x100,
	.init_irq	= s5pv210_init_irq,
	.map_io		= smdkv210_map_io,
	.init_machine	= smdkv210_machine_init,
	.timer		= &s5p_timer,
MACHINE_END
	 
#define MACHINE_START(_type,_name)			\
static const struct machine_desc __mach_desc_##_type	\
 __used							\
 __attribute__((__section__(".arch.info.init"))) = {	\
	.nr		= MACH_TYPE_##_type,		\
	.name		= _name,

#define MACHINE_END				\
};	
	 
MACHINE_START(SMDKV210, "SMDKV210")
==>
	static const struct machine_desc  __mach_desc_SMDKV210  
	__attribute__((__section__(".arch.info.init"))) = {
		.nr		= MACH_TYPE_SMDKV210, // 2456
		.name		= "SMDKV210",
		.boot_params	= S5P_PA_SDRAM + 0x100,//  0x20000100
		.init_irq	= s5pv210_init_irq,
		.map_io		= smdkv210_map_io,
		.init_machine	= smdkv210_machine_init,
		.timer		= &s5p_timer,
	};	
	  
	

farsight@ubuntu:~/linux_system/kernel/linux-3.0.8$ grep -rHn 'MACH_TYPE_SMDKV210' ./include/
./include/generated/mach-types.h:417:#define MACH_TYPE_SMDKV210             2456
	
	
static int __init init_setup(char *str)
{
	unsigned int i;

	execute_command = str;
	/*
	 * In case LILO is going to boot us with default command line,
	 * it prepends "auto" before the whole cmdline which makes
	 * the shell think it should execute a script with such name.
	 * So we ignore all arguments entered _before_ init=... [MJ]
	 */
	for (i = 1; i < MAX_INIT_ARGS; i++)
		argv_init[i] = NULL;
	return 1;
}
__setup("init=", init_setup);	
	
#define __setup(str, fn)					\
	__setup_param(str, fn, fn, 0)
	
#define __setup_param(str, unique_id, fn, early)			\
	static const char __setup_str_##unique_id[] __initconst	\
		__aligned(1) = str; \
	static struct obs_kernel_param __setup_##unique_id	\
		__used __section(.init.setup)			\
		__attribute__((aligned((sizeof(long)))))	\
		= { __setup_str_##unique_id, fn, early }	
		
==>

	static const char __setup_str_init_setup[] __section(.init.rodata) = "init=";
	
__setup("init=", init_setup);
	static struct obs_kernel_param  
	__setup_init_setup 
	__used __section(.init.setup)
		= { 
				"init=" //__setup_str_init_setup, 
				init_setup, 
				0 
		};
	
	
struct obs_kernel_param {
	const char *str;
	int (*setup_func)(char *);
	int early;
};	
	
	
	
启动流程:
第一个阶段: arch/arm/kernel/head.S
	__HEAD
ENTRY(stext)
	setmode	PSR_F_BIT | PSR_I_BIT | SVC_MODE, r9 @ ensure svc mode
						@ and irqs disabled
	mrc	p15, 0, r9, c0, c0		@ get processor id
	bl	__lookup_processor_type		@ r5=procinfo r9=cpuid
	
	
	bl	__create_page_tables
	
	ldr	r13, =__mmap_switched		@ address to jump to after
						@ mmu has been enabled
	adr	lr, BSYM(1f)			@ return (PIC) address
	mov	r8, r4				@ set TTBR1 to swapper_pg_dir
	 ARM(	add	pc, r10, #PROCINFO_INITFUNC	)
	 THUMB(	add	r12, r10, #PROCINFO_INITFUNC	)
	 THUMB(	mov	pc, r12				)
	1:	b	__enable_mmu
	
	
__mmap_switched:// head-common.S
	adr	r3, __mmap_switched_data

	ldmia	r3!, {r4, r5, r6, r7}
	cmp	r4, r5				@ Copy data segment if needed
1:	cmpne	r5, r6
	ldrne	fp, [r4], #4
	strne	fp, [r5], #4
	bne	1b

	mov	fp, #0				@ Clear BSS (and zero fp)
1:	cmp	r6, r7
	strcc	fp, [r6],#4
	bcc	1b

 ARM(	ldmia	r3, {r4, r5, r6, r7, sp})
 THUMB(	ldmia	r3, {r4, r5, r6, r7}	)
 THUMB(	ldr	sp, [r3, #16]		)
	str	r9, [r4]			@ Save processor ID
	str	r1, [r5]			@ Save machine type
	str	r2, [r6]			@ Save atags pointer
	bic	r4, r0, #CR_A			@ Clear 'A' bit
	stmia	r7, {r0, r4}			@ Save control register values
	b	start_kernel
		
	
init/main.c

asmlinkage void __init start_kernel(void)	
	|
	printk(KERN_NOTICE "%s", linux_banner);
	setup_arch(&command_line);//初始化cpu和机器的代码,找到机器描述对象
		|//通过2456,找到对应的struct machine_desc
		|//从0x20000100中去获取到了uboot传递过来的参数
		struct machine_desc *mdesc;
		
		mdesc = setup_machine_tags(machine_arch_type);//machine_arch_type=2456
				|
				struct machine_desc *mdesc = NULL, *p;
				for (p = __arch_info_begin; p < __arch_info_end; p++)
				if (nr == p->nr) {// 比对2456
					printk("Machine: %s\n", p->name);
					mdesc = p;
				return mdesc;
		
		machine_desc = mdesc;//将遍历到的machine_desc赋值给machine_desc
		
		
	printk(KERN_NOTICE "Kernel command line: %s\n", boot_command_line);
	parse_early_param();
	//分析bootargs中的所有参数
	//console=ttySAC0,115200 root=/dev/nfs nfsroot=192.168.7.3:/opt/rootfs ip=192.168.7.7 init=/linuxrc
	parse_args("Booting kernel", static_command_line, __start___param,
		   __stop___param - __start___param,
		   &unknown_bootoption);
		   //有一个应用:　在bootargs中可以自定义传递参数
		   //  set bootargs myname=ruhua myage=250
		   /*
				内核中需要拿到ruhua和250
				就必须在任何一个文件中添加如下内容:
				init/main.c
				static int __init myname_setup(char *str)
				{
					
					printk("---------> name = %s\n", str);

					return 1;
				}
				__setup("myname=", myname_setup);


				static int __init myage_setup(char *str)
				{
					
					int  age = simple_strtoul(str, NULL, 10);
					printk("--------->age = %d\n", age/10);

					return 1;
				}
				__setup("myage=", myage_setup);
				
		   
		   */
		   
	rest_init();
		|
		kernel_thread(kernel_init, NULL, CLONE_FS | CLONE_SIGHAND);
				|
				do_basic_setup();//执行所有驱动的入口函数
					|
					do_initcalls();
						|
						initcall_t *fn;

						for (fn = __early_initcall_end; fn < __initcall_end; fn++)
							do_one_initcall(*fn);
								|
								ret = fn();//批量的执行了initcallX.init段中所有函数指针
									//也就执行了所有驱动的入口函数
				prepare_namespace();//挂载根文件系统
				init_post();//执行祖先init进程
					|
					if (execute_command) {//  init=/linuxrc, execute_command="/linuxrc"
						//优先执行bootage中指定的祖先进程
						run_init_process(execute_command);
						printk(KERN_WARNING "Failed to execute %s.  Attempting "
									"defaults...\n", execute_command);
					}
					//如果没有执行,就执行备胎
					run_init_process("/sbin/init");
					run_init_process("/etc/init");
					run_init_process("/bin/init");
					run_init_process("/bin/sh");

					panic("No init found.  Try passing init= option to kernel. "
						  "See Linux Documentation/init.txt for guidance.");
					
		pid = kernel_thread(kthreadd, NULL, CLONE_FS | CLONE_FILES);
							|
							create_kthread(create);//创建其他线程
		schedule();//让出调度权
	
	
	
do_basic_setup();
	|
	do_initcalls();
		|
		initcall_t *fn;

		for (fn = __early_initcall_end; fn < __initcall_end; fn++)
			do_one_initcall(*fn);
				|
				ret = fn();//批量的执行了initcallX.init段中所有函数指针
					//也就执行了所有驱动的入口函数
			
			
farsight@ubuntu:~/linux_system/kernel/linux-3.0.8$ grep -rHn '>init_machine' ./arch/arm/
./arch/arm/kernel/setup.c:732:  if (machine_desc->init_machine)
./arch/arm/kernel/setup.c:733:          machine_desc->init_machine();


static int __init customize_machine(void)
{
	/* customizes platform devices, or adds new ones */
	if (machine_desc->init_machine)
		machine_desc->init_machine();
	return 0;
}
arch_initcall(customize_machine);//do_basic_setup()中调用的
			