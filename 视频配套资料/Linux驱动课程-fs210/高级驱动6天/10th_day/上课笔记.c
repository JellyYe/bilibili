��Ҫ����---�ں˹���ԭ�����
1, �ں˵ı��벽��
2, Kconfig��Makefile��ʹ��
3, �ں˵Ĳü�---make menuconfig
4, �ں˵Ĺ���ԭ��:
	a, �ں˵��ڲ�����
	b, �ں˵���������
	c, �ں���module_init��ʲô
	d, ������ʲô��˼
	e, ���Ƚ���init����α�������
----------------------------------------------------
1, �ں˵ı��벽��
		1, ���ý��湤����
			 195 ARCH            ?= arm
			 196 CROSS_COMPILE   ?= arm-none-linux-gnueabi-

		2, ѡ��ǰ��soc---s5pv210
			make s5pv210_defconfig

		3, make menuconfig --�ں˲ü�
			System Type  ---> 
				(0) S3C UART to use for low-level messages 
		
			ע��: ִ��make menuconfig �п���ȱ��һ����:ncurse
						sudo apt-get install libncurses5-dev

		4, make zImage ���� make zImage
		
	
	
	Kconfig��Makefile��ʹ��
	
	
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

			
			

uImage=64�ֽ�ͷ+zImage
			
	uboot: �����ں�---linux, bsd, vxwork, ����������ʽ��һ��	
			linux: arm, x86, mips��
	
	����zImage: ��Ϊuboot������ֲ, Ĭ����ΪzImage����arm�ܹ���linux�ں�
	64�ֽ�ͷ�����������ں˵���Ϣ:mkimage����64�ֽ���Ϣ�Ĺ���, mkimage����uboot�ṩ
	�ֶ����64�ֽ�ͷ:
		farsight@ubuntu:/tftpboot$ ./mkimage
Usage: ./mkimage -l image
          -l ==> list image header information
       ./mkimage [-x] -A arch -O os -T type -C comp -a addr -e ep -n name -d data_file[:data_file...] image
          -A ==> set architecture to 'arch'//�趨�ܹ�:arm, mips, x86
          -O ==> set operating system to 'os'//����ϵͳ����:linux, bsd, vxwork
          -T ==> set image type to 'type'//��������: kernel,  ramdisk
          -C ==> set compression type 'comp'//ѹ������:gzip2, bzip2, none
          -a ==> set load address to 'addr' (hex)//�ں˱����ص��ڴ�ĵ�ַ(�ο�ֵ): 0x20008000
          -e ==> set entry point to 'ep' (hex)//�ں˱�ִ�еĵ�ַ: 0x20008000
          -n ==> set image name to 'name'// �������:�Զ���
          -d ==> use image data from 'datafile'//ָ������ͷ����Ϣ���ļ�---zImage
        

	./mkimage -A arm -O linux -T kernel -C none -a 0x20008000 -e 0x20008000 -n "Linux 3.0.8-jihq" -d zImage  uImage
	Image Name:   Linux 3.0.8-jihq
	Created:      Tue Dec 20 17:49:19 2016
	Image Type:   ARM Linux Kernel Image (uncompressed)
	Data Size:    1949140 Bytes = 1903.46 kB = 1.86 MB
	Load Address: 0x20008000
	Entry Point:  0x20008000
	
	
	���Ҫ����uImage
	FS210 # set bootcmd tftp 0x40008000 uImage \; bootm 0x40008000
	FS210 # save

	
	������lds�ű�:
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


		
	�ں˵��ڲ�����:
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
  *(.proc.info.init) // ��������Ϣ��
  __proc_info_end = .;
  
  __arch_info_begin = .;
   *(.arch.info.init) // ��������,������Ϣ��--MACHINE_START
  __arch_info_end = .;
  
  __tagtable_begin = .;
   *(.taglist.init) //uboot����bootargs���ݵĴ����
  __tagtable_end = .;
  
  __setup_start = .; 
  *(.init.setup) //  bootargs�и��������Ĵ����
  __setup_end = .;

  __initcall_start = .; 
  *(.initcallearly.init) 

  __early_initcall_end = .;//��������ִ�ж�
  *(.initcall0.init) *(.initcall0s.init) 
  *(.initcall1.init) *(.initcall1s.init)
  *(.initcall2.init) *(.initcall2s.init) //�����ȫ�����Ǻ���ָ��
  *(.initcall3.init) *(.initcall3s.init) //�������ִ����������к���ָ��,��������ں����ͻᱻִ��
  *(.initcall4.init) *(.initcall4s.init) //subsys_initcall 
  *(.initcall5.init) *(.initcall5s.init) 
  *(.initcallrootfs.init) 
  *(.initcall6.init) *(.initcall6s.init) // module_init
  *(.initcall7.init) *(.initcall7s.init) 
  __initcall_end = .;
	 
	 
	 
	 
	 






module_init(led_drv_init); //��һ������ָ��,ָ��ָ�����

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
	
	
	
��������:
��һ���׶�: arch/arm/kernel/head.S
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
	setup_arch(&command_line);//��ʼ��cpu�ͻ����Ĵ���,�ҵ�������������
		|//ͨ��2456,�ҵ���Ӧ��struct machine_desc
		|//��0x20000100��ȥ��ȡ����uboot���ݹ����Ĳ���
		struct machine_desc *mdesc;
		
		mdesc = setup_machine_tags(machine_arch_type);//machine_arch_type=2456
				|
				struct machine_desc *mdesc = NULL, *p;
				for (p = __arch_info_begin; p < __arch_info_end; p++)
				if (nr == p->nr) {// �ȶ�2456
					printk("Machine: %s\n", p->name);
					mdesc = p;
				return mdesc;
		
		machine_desc = mdesc;//����������machine_desc��ֵ��machine_desc
		
		
	printk(KERN_NOTICE "Kernel command line: %s\n", boot_command_line);
	parse_early_param();
	//����bootargs�е����в���
	//console=ttySAC0,115200 root=/dev/nfs nfsroot=192.168.7.3:/opt/rootfs ip=192.168.7.7 init=/linuxrc
	parse_args("Booting kernel", static_command_line, __start___param,
		   __stop___param - __start___param,
		   &unknown_bootoption);
		   //��һ��Ӧ��:����bootargs�п����Զ��崫�ݲ���
		   //  set bootargs myname=ruhua myage=250
		   /*
				�ں�����Ҫ�õ�ruhua��250
				�ͱ������κ�һ���ļ��������������:
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
				do_basic_setup();//ִ��������������ں���
					|
					do_initcalls();
						|
						initcall_t *fn;

						for (fn = __early_initcall_end; fn < __initcall_end; fn++)
							do_one_initcall(*fn);
								|
								ret = fn();//������ִ����initcallX.init�������к���ָ��
									//Ҳ��ִ����������������ں���
				prepare_namespace();//���ظ��ļ�ϵͳ
				init_post();//ִ������init����
					|
					if (execute_command) {//  init=/linuxrc, execute_command="/linuxrc"
						//����ִ��bootage��ָ�������Ƚ���
						run_init_process(execute_command);
						printk(KERN_WARNING "Failed to execute %s.  Attempting "
									"defaults...\n", execute_command);
					}
					//���û��ִ��,��ִ�б�̥
					run_init_process("/sbin/init");
					run_init_process("/etc/init");
					run_init_process("/bin/init");
					run_init_process("/bin/sh");

					panic("No init found.  Try passing init= option to kernel. "
						  "See Linux Documentation/init.txt for guidance.");
					
		pid = kernel_thread(kthreadd, NULL, CLONE_FS | CLONE_FILES);
							|
							create_kthread(create);//���������߳�
		schedule();//�ó�����Ȩ
	
	
	
do_basic_setup();
	|
	do_initcalls();
		|
		initcall_t *fn;

		for (fn = __early_initcall_end; fn < __initcall_end; fn++)
			do_one_initcall(*fn);
				|
				ret = fn();//������ִ����initcallX.init�������к���ָ��
					//Ҳ��ִ����������������ں���
			
			
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
arch_initcall(customize_machine);//do_basic_setup()�е��õ�
			