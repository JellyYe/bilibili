主要内容---平台总线
1, 平台总线的作用
2, 平台总线的编程
3, 平台总线中的自定义数据
4, 内核中的平台设备

-----------------------------------
linux字符设备驱动编程

	1，实现入口函数 xxx_init()和卸载函数 xxx_exit()
	2，申请主设备号  register_chrdev_region (与内核相关)
	3，注册字符设备驱动 cdev_alloc  cdev_init  cdev_add (与内核相关)
	4，利用udev/mdev机制创建设备文件(节点) class_create, device_create (与内核相关)
	5，硬件部分初始化
		io资源映射 ioremap,内核提供gpio库函数 (与硬件相关)
		注册中断(与硬件相关)
		初始化等待队列 (与内核相关)
	6，构建 file_operation结构 (与内核相关)
	7，实现操作硬件方法  xxx_open,xxx_read,xxxx_write...(与硬件相关)

	
	
	如果要实现一个驱动代码兼容多个设备
		分离: 将通用/相似的代码和差异化代码分离
		合并: 在实际运行时候,需要将差异化代码拿出来,进行操作
		
	很多的相似的硬件: 
		都有相似的操作方法
		有不同的操作地址
		
	
平台总线的作用:　用于ｓｏｃ的升级／ 驱动相似的设备


	soc:
		s3c2440                   s3c6410              s5pv210
		  arm9                      arm11               A8
		
		gpio                        gpio                gpio
		uart                         uart                uart
		i2c                         i2c                   i2c


	gpio控制器:
		1, 配置gpio的功能
		2, 给数据寄存器赋值
		
	uart: 
		1,设置波特率115200,8n1
		2,设置no fifo, no AFC
		3, 发送数据---写发送数据寄存器
			读取数据---读取接收数据寄存器
			
	不同soc中:
		操作逻辑/操作方法相似
		操作的硬件地址不一样
		
	如果要编写一个gpio驱动,能够用在不同的soc中,该怎么办
		在代码编程的时候:
			操作方法的代码(通用)---------分离---------硬件的资源(差异化代码)
									为了更好的代码维护
			
		在运行的时候:
			操作方法的代码(通用)---------合并---------硬件的资源(差异化代码)
					
			
			
			


	如果需要合并就需要总线:
		 bus
		 device
		 driver

	
2, 平台总线的编程
	平台总线:struct bus_type总线对象
		struct bus_type platform_bus_type = {
			.name		= "platform",
			.dev_attrs	= platform_dev_attrs,
			.match		= platform_match, //匹配方法
			.uevent		= platform_uevent,
			.pm		= &platform_dev_pm_ops,
		};
	pdev:
		struct platform_device {// 描述一个设备的信息
			const char	* name; // 名字,用于匹配
			int		id; // 表示不同寄存器组的编号, 一般可以填-1
			struct device	dev;//   父类
			u32		num_resources;//  资源的个数
			struct resource	* resource;// 资源的详细信息--描述中断和内存资源
		};
		
		struct resource {
			resource_size_t start;
			resource_size_t end;
			const char *name;
			unsigned long flags;
			struct resource *parent, *sibling, *child;
		};
		
		注册:
			int platform_device_register(struct platform_device *);
			void platform_device_unregister(struct platform_device *);


	pdrv:
		struct platform_driver {
			int (*probe)(struct platform_device *);//表示匹配之后的函数
			int (*remove)(struct platform_device *);
			struct device_driver driver;//   父类
			const struct platform_device_id *id_table;// 可以匹配列表
		};
		注册和注销:
			extern int platform_driver_register(struct platform_driver *);
			extern void platform_driver_unregister(struct platform_driver *);

		
		probe中要做事情:
			//为用户提供接口
			0, 实例化全局的设备对象-- kzalloc
			1,  申请主设备号---register_chrdev
			2, 自动创建设备节点---class_create, device_create
			3, 初始化硬件--ioremap
			4,实现 file_operation

			
			
			5,拿到pdev中的资源对硬件进行初始化




gpio操作方法:
		1, 直接操作gpio口对应的寄存器地址(看原理图---数据手册---地址--ioremap)
			*gpc0_conf &= ~(0xff<<12);
			*gpc0_conf |= (0x11<<12);
	
		2, gpio库函数的接口---只需要知道gpio口的号码即可
			gpio_request(unsigned gpio,const char * label)
			// 将某个gpio配置成输出功能,并且直接输出高低电平
			gpio_direction_output(unsigned gpio,int value)
			
			// 将某个gpio配置成输入功能
			gpio_direction_input(unsigned gpio)
			
			// 将某个gpio配置成特定功能
			s3c_gpio_cfgpin(unsigned int pin,unsigned int config)
			
			//将某个gpio口内部上拉或者下拉
			s3c_gpio_setpull(unsigned int pin,s3c_gpio_pull_t pull)
			
			//获取到gpio的值
			gpio_get_value
			//设置gpio的值
			gpio_set_value
			//通过gpio口获取到中断号码
			gpio_to_irq	
			gpio_free(unsigned gpio)


		3,内核提供接口:readl/writel---__raw_readl/__raw_writel
			主要用于操作地址
			
			__raw_readl(a)
			__raw_writel(d,ad)
		
		

3, 平台总线中的自定义数据
	pdev:
		resource: 地址 和中断号 (资源)
		定义其他类型的资源/数据
		struct led_platdata led_pdata = {
			.name = "gpc0_3/4",
			.shift = 3,
		};

		struct platform_device led_pdev = {
			.name = "s5pv210_led",
			.id = -1,
			.num_resources = ARRAY_SIZE(led_resource),
			.resource = led_resource,
			.dev = {
				.platform_data = &led_pdata,  // <------------

			},
		};

	pdrv: 获取到自定义数据
		led_dev->pdata = pdev->dev.platform_data;

4, 内核中的平台设备
	static struct platform_device *smdkv210_devices[] __initdata = {
		&s3c_device_adc,
		&s3c_device_cfcon,
		&s3c_device_fb,
		&s3c_device_hsmmc0,
		&s3c_device_hsmmc1,
		&s3c_device_hsmmc2,
		&s3c_device_hsmmc3,
		&s3c_device_i2c0,
		&s3c_device_i2c1,
		&s3c_device_i2c2,
		&s3c_device_rtc,
		&s3c_device_ts,
		&s3c_device_wdt,
		&s5pv210_device_ac97,
		&s5pv210_device_iis0,
		&s5pv210_device_spdif,
		&samsung_asoc_dma,
		&samsung_device_keypad,
		&smdkv210_dm9000,
		&smdkv210_lcd_lte480wv,
		&s3c_device_timer[3],
		&smdkv210_backlight_device,
	};
	arch/arm/mach-s5pv210/mach-smdkv210.c
		|
		smdkv210_machine_init(void)//在内核启动的时候会自动执行
			|
			platform_add_devices(smdkv210_devices, ARRAY_SIZE(smdkv210_devices));	
				|
					for (i = 0; i < num; i++) {
						ret = platform_device_register(devs[i]);//开机的时候批量的注册pdev	
		
		
		
	
	
	
	
	什么时候用平台总线
		1, 只要有设备的地址和中断都可以用平台总线
		2, 如果写的驱动需要在多个平台中升级使用
		3, 平台总线只是一个功能代码:将操作方法和操作资源进行了分离
		
	
	
	
	
	
	
	
	
	
	
	
		
		
		
		