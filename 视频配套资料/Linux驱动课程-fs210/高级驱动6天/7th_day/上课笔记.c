��Ҫ����---i2c��ϵͳ--e2prom���
1, i2cЭ�齲��
2, i2c��ϵͳ���
3, i2c��ϵͳ�������--���豸����at24c02
4, i2c��ϵͳ��ܴ�������
----------------------------------------------
i2c����:
	i2c�豸�Ƚ϶�:
		ts, camera, e2prom, gsensor, hdmi..
	

	
2, i2c��ϵͳ���


		Ӧ�ò�
	----------------------------------------
	 i2c driver��: ���豸������
		1,���û����н���
		2, ֪��Ҫ��������豸��������ʲô
			��֪�����ͨ��Ӳ�����������豸
	
	------------------------------------------
	
	i2c core�㣺ά����һ��i2c ����
		drivers/i2c/i2c-core.c
	
	
	----------------------------------------
	i2c adapter: i2c��������
		drivers/i2c/busses/i2c-s3c2410.c
		1,��Ӳ������,���Ҹ���Ӳ��i2c�������ĳ�ʼ��
		2,֪����ν����ݸ����豸,���ǲ�֪��������ʲô
		
	ȷ���������ں��Դ�--zImage:
		make menuconfig
			Device Drivers  --->
				<*> I2C support  ---> //  i2c-core.c
					I2C Hardware Bus support  --->
						 <*> S3C2410 I2C Driver //i2c-s3c2410.c
		make zImage -j2
		�����ں�:
			cp -raf arch/arm/boot/zImage  /tftpboot/zImage
					
		
3, i2c��ϵͳ�������--���豸����at24c02


		1, ���Ϊi2c_client�ṩ��Ϣ,������Ϣ��,ϵͳ���Զ��Ĵ���i2c client
		
			static struct i2c_board_info smdkv210_i2c_devs0[] __initdata = {
				{ I2C_BOARD_INFO("at24c02a", 0x50), // �������ڽ���ƥ��, 0x50��ʾ���豸��ַ
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
		�����ں�:
			cp -raf arch/arm/boot/zImage  /tftpboot/zImage
			
			
			
		��β鿴:
			/sys/bus/i2c/devices/0-0050
			[root@farsight 0-0050]# cat name
				at24c02a

		
		2, ����i2c_driver,ע�ᵽ����
			
	
			extern void i2c_del_driver(struct i2c_driver *);

			static inline int i2c_add_driver(struct i2c_driver *driver)
	
	
			struct i2c_driver {//����һ��i2c���豸������
				int (*probe)(struct i2c_client *, const struct i2c_device_id *);
				int (*remove)(struct i2c_client *);
				struct device_driver driver;//����
				const struct i2c_device_id *id_table;//�ȶԵ��б�
			
			}
	
		
		
			int i2c_master_send(const struct i2c_client *client, const char *buf, int count)
			int i2c_master_recv(const struct i2c_client *client, char *buf, int count)
					|
					int i2c_transfer(struct i2c_adapter *adap, struct i2c_msg *msgs, int num)
				
				
				
			struct i2c_client {//����һ�����豸��Ϣ�Ķ���---�������еĳ�Ա�����Զ���ʼ��
				unsigned short addr;		// 7bit�Ĵ��豸��ַ
				char name[I2C_NAME_SIZE];//����,���ں�driverƥ��
				struct i2c_adapter *adapter;	//ָ�򴴽��Լ���adapter
				struct i2c_driver *driver;	// ���Լ�ƥ���driver
				struct device dev;		//   ����
			}	
				
				
			struct i2c_adapter {//��������һ��������
				const struct i2c_algorithm *algo;
								|
								int (*master_xfer)(struct i2c_adapter *adap, struct i2c_msg *msgs,int num);
				struct device dev;		//����

				int nr;//�������ı��
			
			}

			struct i2c_msg {//���͸����豸�����ݰ�
				__u16 addr;	//���豸��ַ
				__u16 flags;//������д
				__u16 len;	// ���ݵ��ֽ���
				__u8 *buf;	//���ݻ���ָ��
			};			
				
				


����: ����ƥ��֮��,������drv�е� probe()
	i2c_add_driver(&at24_drv);����platform_driver_register(struct platform_driver * drv)

��θ����ں˵Ĵ��뼼��:
	1, ������--��Ҫ�ļ�����
	2, �����жϺͱ�������
	3, ����������
	4, ��ȥ�º���������
	5, �Ҷ�������ҹȸ�
	6, ���ñʼǺ�ע��,�Ѿ��ܽ�
	
	
i2c ��ϵͳ�Ŀ�ܴ���


----------------------------------------------

at24_drv.c:---4


-------------------------------------------
i2c-core.c--2
postcore_initcall(i2c_init);
module_exit(i2c_exit);

	i2c_init(void)
		|
		bus_register(&i2c_bus_type);//������i2c ����--���Ҷ�����ƥ�䷽��
		i2c_add_driver(&dummy_driver); //���ӵ���һ��i2c driver-ʲôҲû��


--------------------------------------------	
i2c-s3c2410.c ---3
subsys_initcall(i2c_adap_s3c_init);
	|
	platform_driver_register(&s3c24xx_i2c_driver);
	s3c24xx_i2c_probe,
		|
		//��ȡ��ƽ̨�Զ�������
		pdata = pdev->dev.platform_data;
		// 1,����һ��adapter
		//����һ��ȫ�ֵ��豸����--����i2c_adapter
		i2c = kzalloc(sizeof(struct s3c24xx_i2c), GFP_KERNEL);	
		// 2, ��ʼ�� adatper
		strlcpy(i2c->adap.name, "s3c2410-i2c", sizeof(i2c->adap.name));
		i2c->adap.owner   = THIS_MODULE;
		i2c->adap.algo    = &s3c24xx_i2c_algorithm; //�㷨
		i2c->adap.retries = 2;
		i2c->adap.class   = I2C_CLASS_HWMON | I2C_CLASS_SPD;
		i2c->tx_setup     = 50;
		
		//��ȡ�ڴ���Դ
		platform_get_resource(pdev, IORESOURCE_MEM, 0);
		i2c->regs = ioremap(res->start, resource_size(res));
		//��ȡ�ж���Դ
		i2c->irq = ret = platform_get_irq(pdev, 0);
		
		//�����ж�
		 request_irq(i2c->irq, s3c24xx_i2c_irq, IRQF_DISABLED,
			  dev_name(&pdev->dev), i2c);
		// 3,ע��  adapter
		i2c_add_numbered_adapter(&i2c->adap);
			|
			 i2c_register_adapter(adap);	
				|
				// i2c_add_adapter��˭���õ�
				//i2c adapter��ע��
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
						//����ǰ��adapterע�ᵽi2c������ȥ
						res = device_register(&adap->dev);
						
						i2c_scan_static_board_info(adap);
							|
							list_for_each_entry(devinfo, &__i2c_board_list, list) {
								if (devinfo->busnum == adapter->nr&& !i2c_new_device(adapter,&devinfo->board_info)
							
							

						bus_for_each_drv(&i2c_bus_type, NULL, adap, __process_new_adapter);
						�ܽ�:
							adapter��ע���ʱ��,�����__i2c_board_list����
							���adapter�ı�ź������нڵ�ĺ���һ��,�ͻṹ��i2c client��ע��client
							i2c client�г�Ա��ֵ������board_info

				//i2c_new_device��˭������

				// i2c client��Ҫ����������,����ע�ᵽ����
				struct i2c_client *i2c_new_device(struct i2c_adapter *adap, struct i2c_board_info const *info)
						|
						struct i2c_client	*client;
						//����i2c client����
						client = kzalloc(sizeof *client, GFP_KERNEL);
					
						//��i2c_adapter������i2c_client
						client->adapter = adap;

						client->dev.platform_data = info->platform_data;
						client->flags = info->flags;
						//���豸��ַ��������i2c_board_info
						client->addr = info->addr;
						
						client->irq = info->irq;

						//client������Ҳ������i2c_board_info
						strlcpy(client->name, info->type, sizeof(client->name));

						//�����豸��ַ�ĺϷ���
						i2c_check_client_addr_validity(client);
						
						i2c_check_addr_busy(adap, client->addr);
						
						//��ʼ��client�еĸ���
						client->dev.parent = &client->adapter->dev;
						//ָ�������Ӧ������,ָ��i2c����
						client->dev.bus = &i2c_bus_type;
						client->dev.type = &i2c_client_type;
						client->dev.of_node = info->of_node;
						//  /sys/bus/i2c/devices/0-0050
						dev_set_name(&client->dev, "%d-%04x", i2c_adapter_id(adap),
								 client->addr);
						//ע�ᵽi2c����
						status = device_register(&client->dev);
						return client;
				
	
	�ܽ�:
		i2c_new_device������
			1, ������i2c client
			2, ��ʼ��i2c client,ͨ��i2c_board_info����ʼ��
			3, ע�ᵽi2c ����
			
--------------------------------------------------------------
			static struct i2c_board_info smdkv210_i2c_devs0[] __initdata = {
				{ I2C_BOARD_INFO("at24c02a", 0x50), },// �������ڽ���ƥ��, 0x50��ʾ���豸��ַ
				{ I2C_BOARD_INFO("wm8580", 0x1b), },
			};
mach-smdkv210.c--1
	|
	smdkv210_machine_init(void)
		|
		i2c_register_board_info(0, smdkv210_i2c_devs0,ARRAY_SIZE(smdkv210_i2c_devs0));
		|i2c_register_board_info(int busnum,struct i2c_board_info const *info, unsigned len)
				for (status = 0; len; len--, info++)//��������
					struct i2c_devinfo	*devinfo;
					//��Ҫ����
					devinfo = kzalloc(sizeof(*devinfo), GFP_KERNEL);
					devinfo->busnum = busnum;//��ʾ���߱��--0, 1,2
					devinfo->board_info = *info;//�������г�Ա��ֵ���ڵ�
					list_add_tail(&devinfo->list, &__i2c_board_list);
					
				
				
		i2c_register_board_info(1, smdkv210_i2c_devs1,
				ARRAY_SIZE(smdkv210_i2c_devs1));
		i2c_register_board_info(2, smdkv210_i2c_devs2,
				ARRAY_SIZE(smdkv210_i2c_devs2));
				
				
		platform_add_devices(smdkv210_devices,
			ARRAY_SIZE(smdkv210_devices));


		
		
		�ܽ�:i2c_register_board_info����ʲô��
			���������еĳ�Ա����һ���ڵ����(��¼i2c client����Ҫ����Ϣ�Լ����)
			���ڵ����ע�ᵽ__i2c_board_list������ȥ
		
		platform_add_devices()ע��������i2c��pdev	
			
	
	
	
	
	
	
	
	
	
	
	














				
				
				
				
	
	
	
	