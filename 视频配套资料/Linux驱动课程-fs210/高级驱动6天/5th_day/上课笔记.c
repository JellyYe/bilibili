��Ҫ����---ƽ̨����
1, ƽ̨���ߵ�����
2, ƽ̨���ߵı��
3, ƽ̨�����е��Զ�������
4, �ں��е�ƽ̨�豸

-----------------------------------
linux�ַ��豸�������

	1��ʵ����ں��� xxx_init()��ж�غ��� xxx_exit()
	2���������豸��  register_chrdev_region (���ں����)
	3��ע���ַ��豸���� cdev_alloc  cdev_init  cdev_add (���ں����)
	4������udev/mdev���ƴ����豸�ļ�(�ڵ�) class_create, device_create (���ں����)
	5��Ӳ�����ֳ�ʼ��
		io��Դӳ�� ioremap,�ں��ṩgpio�⺯�� (��Ӳ�����)
		ע���ж�(��Ӳ�����)
		��ʼ���ȴ����� (���ں����)
	6������ file_operation�ṹ (���ں����)
	7��ʵ�ֲ���Ӳ������  xxx_open,xxx_read,xxxx_write...(��Ӳ�����)

	
	
	���Ҫʵ��һ������������ݶ���豸
		����: ��ͨ��/���ƵĴ���Ͳ��컯�������
		�ϲ�: ��ʵ������ʱ��,��Ҫ�����컯�����ó���,���в���
		
	�ܶ�����Ƶ�Ӳ��: 
		�������ƵĲ�������
		�в�ͬ�Ĳ�����ַ
		
	
ƽ̨���ߵ�����:�����ڣ���������� �������Ƶ��豸


	soc:
		s3c2440                   s3c6410              s5pv210
		  arm9                      arm11               A8
		
		gpio                        gpio                gpio
		uart                         uart                uart
		i2c                         i2c                   i2c


	gpio������:
		1, ����gpio�Ĺ���
		2, �����ݼĴ�����ֵ
		
	uart: 
		1,���ò�����115200,8n1
		2,����no fifo, no AFC
		3, ��������---д�������ݼĴ���
			��ȡ����---��ȡ�������ݼĴ���
			
	��ͬsoc��:
		�����߼�/������������
		������Ӳ����ַ��һ��
		
	���Ҫ��дһ��gpio����,�ܹ����ڲ�ͬ��soc��,����ô��
		�ڴ����̵�ʱ��:
			���������Ĵ���(ͨ��)---------����---------Ӳ������Դ(���컯����)
									Ϊ�˸��õĴ���ά��
			
		�����е�ʱ��:
			���������Ĵ���(ͨ��)---------�ϲ�---------Ӳ������Դ(���컯����)
					
			
			
			


	�����Ҫ�ϲ�����Ҫ����:
		 bus
		 device
		 driver

	
2, ƽ̨���ߵı��
	ƽ̨����:struct bus_type���߶���
		struct bus_type platform_bus_type = {
			.name		= "platform",
			.dev_attrs	= platform_dev_attrs,
			.match		= platform_match, //ƥ�䷽��
			.uevent		= platform_uevent,
			.pm		= &platform_dev_pm_ops,
		};
	pdev:
		struct platform_device {// ����һ���豸����Ϣ
			const char	* name; // ����,����ƥ��
			int		id; // ��ʾ��ͬ�Ĵ�����ı��, һ�������-1
			struct device	dev;//   ����
			u32		num_resources;//  ��Դ�ĸ���
			struct resource	* resource;// ��Դ����ϸ��Ϣ--�����жϺ��ڴ���Դ
		};
		
		struct resource {
			resource_size_t start;
			resource_size_t end;
			const char *name;
			unsigned long flags;
			struct resource *parent, *sibling, *child;
		};
		
		ע��:
			int platform_device_register(struct platform_device *);
			void platform_device_unregister(struct platform_device *);


	pdrv:
		struct platform_driver {
			int (*probe)(struct platform_device *);//��ʾƥ��֮��ĺ���
			int (*remove)(struct platform_device *);
			struct device_driver driver;//   ����
			const struct platform_device_id *id_table;// ����ƥ���б�
		};
		ע���ע��:
			extern int platform_driver_register(struct platform_driver *);
			extern void platform_driver_unregister(struct platform_driver *);

		
		probe��Ҫ������:
			//Ϊ�û��ṩ�ӿ�
			0, ʵ����ȫ�ֵ��豸����-- kzalloc
			1,  �������豸��---register_chrdev
			2, �Զ������豸�ڵ�---class_create, device_create
			3, ��ʼ��Ӳ��--ioremap
			4,ʵ�� file_operation

			
			
			5,�õ�pdev�е���Դ��Ӳ�����г�ʼ��




gpio��������:
		1, ֱ�Ӳ���gpio�ڶ�Ӧ�ļĴ�����ַ(��ԭ��ͼ---�����ֲ�---��ַ--ioremap)
			*gpc0_conf &= ~(0xff<<12);
			*gpc0_conf |= (0x11<<12);
	
		2, gpio�⺯���Ľӿ�---ֻ��Ҫ֪��gpio�ڵĺ��뼴��
			gpio_request(unsigned gpio,const char * label)
			// ��ĳ��gpio���ó��������,����ֱ������ߵ͵�ƽ
			gpio_direction_output(unsigned gpio,int value)
			
			// ��ĳ��gpio���ó����빦��
			gpio_direction_input(unsigned gpio)
			
			// ��ĳ��gpio���ó��ض�����
			s3c_gpio_cfgpin(unsigned int pin,unsigned int config)
			
			//��ĳ��gpio���ڲ�������������
			s3c_gpio_setpull(unsigned int pin,s3c_gpio_pull_t pull)
			
			//��ȡ��gpio��ֵ
			gpio_get_value
			//����gpio��ֵ
			gpio_set_value
			//ͨ��gpio�ڻ�ȡ���жϺ���
			gpio_to_irq	
			gpio_free(unsigned gpio)


		3,�ں��ṩ�ӿ�:readl/writel---__raw_readl/__raw_writel
			��Ҫ���ڲ�����ַ
			
			__raw_readl(a)
			__raw_writel(d,ad)
		
		

3, ƽ̨�����е��Զ�������
	pdev:
		resource: ��ַ ���жϺ� (��Դ)
		�����������͵���Դ/����
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

	pdrv: ��ȡ���Զ�������
		led_dev->pdata = pdev->dev.platform_data;

4, �ں��е�ƽ̨�豸
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
		smdkv210_machine_init(void)//���ں�������ʱ����Զ�ִ��
			|
			platform_add_devices(smdkv210_devices, ARRAY_SIZE(smdkv210_devices));	
				|
					for (i = 0; i < num; i++) {
						ret = platform_device_register(devs[i]);//������ʱ��������ע��pdev	
		
		
		
	
	
	
	
	ʲôʱ����ƽ̨����
		1, ֻҪ���豸�ĵ�ַ���ж϶�������ƽ̨����
		2, ���д��������Ҫ�ڶ��ƽ̨������ʹ��
		3, ƽ̨����ֻ��һ�����ܴ���:�����������Ͳ�����Դ�����˷���
		
	
	
	
	
	
	
	
	
	
	
	
		
		
		
		