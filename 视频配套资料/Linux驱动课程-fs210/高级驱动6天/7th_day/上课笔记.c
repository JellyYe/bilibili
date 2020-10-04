主要内容---i2c子系统--e2prom编程
1, i2c协议讲解
2, i2c子系统框架
3, i2c子系统驱动编程--从设备驱动at24c02
4, i2c子系统框架代码流程
----------------------------------------------
i2c驱动:
	i2c设备比较多:
		ts, camera, e2prom, gsensor, hdmi..
	

	
2, i2c子系统框架


		应用层
	----------------------------------------
	 i2c driver层: 从设备驱动层
		1,和用户进行交互
		2, 知道要传输给从设备的数据是什么
			不知道如何通过硬件操作给从设备
	
	------------------------------------------
	
	i2c core层：维护了一个i2c 总线
		drivers/i2c/i2c-core.c
	
	
	----------------------------------------
	i2c adapter: i2c控制器层
		drivers/i2c/busses/i2c-s3c2410.c
		1,与硬件交互,并且负责硬件i2c控制器的初始化
		2,知道如何将数据给从设备,但是不知道数据是什么
		
	确保低两层内核自带--zImage:
		make menuconfig
			Device Drivers  --->
				<*> I2C support  ---> //  i2c-core.c
					I2C Hardware Bus support  --->
						 <*> S3C2410 I2C Driver //i2c-s3c2410.c
		make zImage -j2
		更新内核:
			cp -raf arch/arm/boot/zImage  /tftpboot/zImage
					
		
3, i2c子系统驱动编程--从设备驱动at24c02


		1, 间接为i2c_client提供信息,有了信息后,系统会自动的创建i2c client
		
			static struct i2c_board_info smdkv210_i2c_devs0[] __initdata = {
				{ I2C_BOARD_INFO("at24c02a", 0x50), // 名字用于进行匹配, 0x50表示从设备地址
					//.type = "24c08",
					//.addr = 0x50
				},     /* Samsung S524AD0XD1 */
				{ I2C_BOARD_INFO("wm8580", 0x1b), },
			};

			static struct i2c_board_info smdkv210_i2c_devs1[] __initdata = {
				/* To Be Updated */
			};

			static struct i2c_board_info smdkv210_i2c_devs2[] __initdata = {
				/* To Be Updated */
			};
			arch/arm/mach-s5pv210/mach-smdkv210.c
				|
				smdkv210_machine_init()
					|
					i2c_register_board_info(0, smdkv210_i2c_devs0,ARRAY_SIZE(smdkv210_i2c_devs0));
					i2c_register_board_info(1, smdkv210_i2c_devs1,ARRAY_SIZE(smdkv210_i2c_devs1));
					i2c_register_board_info(2, smdkv210_i2c_devs2,	ARRAY_SIZE(smdkv210_i2c_devs2));
		
		
		
		make zImage -j2
		更新内核:
			cp -raf arch/arm/boot/zImage  /tftpboot/zImage
			
			
			
		如何查看:
			/sys/bus/i2c/devices/0-0050
			[root@farsight 0-0050]# cat name
				at24c02a

		
		2, 构建i2c_driver,注册到总线
			
	
			extern void i2c_del_driver(struct i2c_driver *);

			static inline int i2c_add_driver(struct i2c_driver *driver)
	
	
			struct i2c_driver {//描述一个i2c从设备的驱动
				int (*probe)(struct i2c_client *, const struct i2c_device_id *);
				int (*remove)(struct i2c_client *);
				struct device_driver driver;//父类
				const struct i2c_device_id *id_table;//比对的列表
			
			}
	
		
		
			int i2c_master_send(const struct i2c_client *client, const char *buf, int count)
			int i2c_master_recv(const struct i2c_client *client, char *buf, int count)
					|
					int i2c_transfer(struct i2c_adapter *adap, struct i2c_msg *msgs, int num)
				
				
				
			struct i2c_client {//描述一个从设备信息的对象---里面所有的成员都是自动初始化
				unsigned short addr;		// 7bit的从设备地址
				char name[I2C_NAME_SIZE];//名字,用于和driver匹配
				struct i2c_adapter *adapter;	//指向创建自己的adapter
				struct i2c_driver *driver;	// 和自己匹配的driver
				struct device dev;		//   父类
			}	
				
				
			struct i2c_adapter {//描述的是一个控制器
				const struct i2c_algorithm *algo;
								|
								int (*master_xfer)(struct i2c_adapter *adap, struct i2c_msg *msgs,int num);
				struct device dev;		//父类

				int nr;//控制器的编号
			
			}

			struct i2c_msg {//传送给从设备的数据包
				__u16 addr;	//从设备地址
				__u16 flags;//读还是写
				__u16 len;	// 数据的字节数
				__u8 *buf;	//数据缓冲指针
			};			
				
				


任务: 总线匹配之后,调用了drv中的 probe()
	i2c_add_driver(&at24_drv);或者platform_driver_register(struct platform_driver * drv)

如何跟读内核的代码技巧:
	1, 看主线--主要的技术点
	2, 出错判断和变量不看
	3, 看不懂不看
	4, 大胆去猜函数的作用
	5, 找度娘或者找谷歌
	6, 做好笔记和注释,已经总结
	
	
i2c 子系统的框架代码


----------------------------------------------

at24_drv.c:---4


-------------------------------------------
i2c-core.c--2
postcore_initcall(i2c_init);
module_exit(i2c_exit);

	i2c_init(void)
		|
		bus_register(&i2c_bus_type);//构建了i2c 总线--并且定义了匹配方法
		i2c_add_driver(&dummy_driver); //增加的了一个i2c driver-什么也没做


--------------------------------------------	
i2c-s3c2410.c ---3
subsys_initcall(i2c_adap_s3c_init);
	|
	platform_driver_register(&s3c24xx_i2c_driver);
	s3c24xx_i2c_probe,
		|
		//获取到平台自定义数据
		pdata = pdev->dev.platform_data;
		// 1,分配一个adapter
		//分配一个全局的设备对象--分配i2c_adapter
		i2c = kzalloc(sizeof(struct s3c24xx_i2c), GFP_KERNEL);	
		// 2, 初始化 adatper
		strlcpy(i2c->adap.name, "s3c2410-i2c", sizeof(i2c->adap.name));
		i2c->adap.owner   = THIS_MODULE;
		i2c->adap.algo    = &s3c24xx_i2c_algorithm; //算法
		i2c->adap.retries = 2;
		i2c->adap.class   = I2C_CLASS_HWMON | I2C_CLASS_SPD;
		i2c->tx_setup     = 50;
		
		//获取内存资源
		platform_get_resource(pdev, IORESOURCE_MEM, 0);
		i2c->regs = ioremap(res->start, resource_size(res));
		//获取中断资源
		i2c->irq = ret = platform_get_irq(pdev, 0);
		
		//申请中断
		 request_irq(i2c->irq, s3c24xx_i2c_irq, IRQF_DISABLED,
			  dev_name(&pdev->dev), i2c);
		// 3,注册  adapter
		i2c_add_numbered_adapter(&i2c->adap);
			|
			 i2c_register_adapter(adap);	
				|
				// i2c_add_adapter是谁调用的
				//i2c adapter的注册
				i2c_add_adapter(struct i2c_adapter * adapter)
					|
					i2c_register_adapter(adapter);
						|
						rt_mutex_init(&adap->bus_lock);
						mutex_init(&adap->userspace_clients_lock);
						INIT_LIST_HEAD(&adap->userspace_clients);
						//  /sys/bus/i2c/devices/i2c-0,1, 2
						dev_set_name(&adap->dev, "i2c-%d", adap->nr);
						adap->dev.bus = &i2c_bus_type;
						adap->dev.type = &i2c_adapter_type;
						//将当前的adapter注册到i2c总线中去
						res = device_register(&adap->dev);
						
						i2c_scan_static_board_info(adap);
							|
							list_for_each_entry(devinfo, &__i2c_board_list, list) {
								if (devinfo->busnum == adapter->nr&& !i2c_new_device(adapter,&devinfo->board_info)
							
							

						bus_for_each_drv(&i2c_bus_type, NULL, adap, __process_new_adapter);
						总结:
							adapter在注册的时候,会遍历__i2c_board_list链表
							如果adapter的编号和链表中节点的号码一致,就会构建i2c client并注册client
							i2c client中成员的值来自于board_info

				//i2c_new_device是谁调用了

				// i2c client需要被构建出来,并且注册到总线
				struct i2c_client *i2c_new_device(struct i2c_adapter *adap, struct i2c_board_info const *info)
						|
						struct i2c_client	*client;
						//分配i2c client对象
						client = kzalloc(sizeof *client, GFP_KERNEL);
					
						//将i2c_adapter关联到i2c_client
						client->adapter = adap;

						client->dev.platform_data = info->platform_data;
						client->flags = info->flags;
						//从设备地址是来自于i2c_board_info
						client->addr = info->addr;
						
						client->irq = info->irq;

						//client的名字也来自于i2c_board_info
						strlcpy(client->name, info->type, sizeof(client->name));

						//检查从设备地址的合法性
						i2c_check_client_addr_validity(client);
						
						i2c_check_addr_busy(adap, client->addr);
						
						//初始化client中的父类
						client->dev.parent = &client->adapter->dev;
						//指定父类对应的总线,指定i2c总线
						client->dev.bus = &i2c_bus_type;
						client->dev.type = &i2c_client_type;
						client->dev.of_node = info->of_node;
						//  /sys/bus/i2c/devices/0-0050
						dev_set_name(&client->dev, "%d-%04x", i2c_adapter_id(adap),
								 client->addr);
						//注册到i2c总线
						status = device_register(&client->dev);
						return client;
				
	
	总结:
		i2c_new_device函数中
			1, 构建了i2c client
			2, 初始化i2c client,通过i2c_board_info来初始化
			3, 注册到i2c 总线
			
--------------------------------------------------------------
			static struct i2c_board_info smdkv210_i2c_devs0[] __initdata = {
				{ I2C_BOARD_INFO("at24c02a", 0x50), },// 名字用于进行匹配, 0x50表示从设备地址
				{ I2C_BOARD_INFO("wm8580", 0x1b), },
			};
mach-smdkv210.c--1
	|
	smdkv210_machine_init(void)
		|
		i2c_register_board_info(0, smdkv210_i2c_devs0,ARRAY_SIZE(smdkv210_i2c_devs0));
		|i2c_register_board_info(int busnum,struct i2c_board_info const *info, unsigned len)
				for (status = 0; len; len--, info++)//遍历数组
					struct i2c_devinfo	*devinfo;
					//主要代码
					devinfo = kzalloc(sizeof(*devinfo), GFP_KERNEL);
					devinfo->busnum = busnum;//表示总线编号--0, 1,2
					devinfo->board_info = *info;//将数组中成员赋值给节点
					list_add_tail(&devinfo->list, &__i2c_board_list);
					
				
				
		i2c_register_board_info(1, smdkv210_i2c_devs1,
				ARRAY_SIZE(smdkv210_i2c_devs1));
		i2c_register_board_info(2, smdkv210_i2c_devs2,
				ARRAY_SIZE(smdkv210_i2c_devs2));
				
				
		platform_add_devices(smdkv210_devices,
			ARRAY_SIZE(smdkv210_devices));


		
		
		总结:i2c_register_board_info做了什么：
			根据数组中的成员构建一个节点对象(记录i2c client所需要的信息以及编号)
			将节点对象注册到__i2c_board_list链表中去
		
		platform_add_devices()注册了三个i2c的pdev	
			
	
	
	
	
	
	
	
	
	
	
	














				
				
				
				
	
	
	
	