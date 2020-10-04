主要内容---输入子系统编程--主要针对输入设备
1,输入系统的作用和框架
2,输入子系统的编程
3,输入子系统和平台总线的结合
4,输入子系统的执行流程
---------------------------------

1,输入系统的作用和框架
	针对输入设备: button, keyboard, mouse, ts, gsensor, joystick
	按照产生数据的类型进行分类:
		1,按键数据---键值
			button, keyboard
		2, 绝对坐标数据---有最大值和最小值
			ts, gsensor,陀螺仪
		3, 相对坐标数据---下一个坐标是相对于之前的坐标
			mouse
	
	引入输入子系统对输入设备进行驱动管理:
		1, 为驱动定义一个标准的编程方式
		2, 用户空间到数据格式是统一的
		
		
	框架:
		
		应用层
		------------------------------------
		input handler 数据处理层: drivers/input/evdev.c
			1, 和用户进行交互,实现fops
			2, 不知道数据是如何得到,但是知道如何将数据交给用户
		
		
		-------------------------------------
		input core层--drivers/input/input.c
			1,维护了两个链表
			2, 为上下两层提供接口
		
		
		---------------------------------------
		input device层: 输入设备硬件层--自己去写
			自带的input device代码:
				drivers/input/touchscreen/
				
			1, 初始化硬件,获取硬件中数据
			2, 知道得到数据,但是不知道如何将数据给用
			
		-------------------------------------------
		
		
		确保zImage已经包含了input core和input handler层代码
		make menuconfig
			Device Drivers  --->
				Input device support  --->
					-*- Generic input layer (needed for keyboard, mouse, ...)// input.c
					<*>   Event interface //  input handler层代码 evdev.c 
		
		
2,输入子系统的编程:Documentation/input/input-programming.txt
	a, 分配一个input device对象
	b, 初始化input device对象
	c, 注册input device对象
	d, 硬件初始化
		
		
		struct input_dev *input_allocate_device(void)
		void input_free_device(struct input_dev *dev)
		
		int input_register_device(struct input_dev *dev)
		void input_unregister_device(struct input_dev *dev)
		
		//上报数据
			void input_report_key(struct input_dev *dev, unsigned int code, int value)
					|
					void input_event(struct input_dev *dev, unsigned int type, unsigned int code, int value)
		
		
		struct input_dev {
			const char *name;
			const char *phys;
			const char *uniq;
			struct input_id id;


			//位表
			// evbit表示能够产生哪些数据类型
			unsigned long evbit[BITS_TO_LONGS(EV_CNT)];
			// keybit表示能够产生哪些按键数据
			unsigned long keybit[BITS_TO_LONGS(KEY_CNT)];//keybit[BITS_TO_LONGS(768)]=keybit[24]
			// relbit表示能够产生哪些相对坐标数据
			unsigned long relbit[BITS_TO_LONGS(REL_CNT)];
			// absbit 表示能够产生哪些绝对坐标数据
			unsigned long absbit[BITS_TO_LONGS(ABS_CNT)];//absbit[2];
			unsigned long mscbit[BITS_TO_LONGS(MSC_CNT)];
			unsigned long ledbit[BITS_TO_LONGS(LED_CNT)];
			unsigned long sndbit[BITS_TO_LONGS(SND_CNT)];
			unsigned long ffbit[BITS_TO_LONGS(FF_CNT)];
			unsigned long swbit[BITS_TO_LONGS(SW_CNT)];
		
			struct device dev; //父类

			struct list_head	h_list;
			struct list_head	node;//链表的结点
		};
	
	
		struct input_event { //用户读取到输入设备的数据包
			struct timeval time;//时间戳
			__u16 type;//读取到数据类型: EV_KEY, EV_ABS, EV_REL
			__u16 code;//码值
			__s32 value;//状态
		};

	
================================================================================
看源码:

	1,看整个分层
	2,应用和驱动是如何交互
		open, read()
	
----------------------------------------------
1,分层


	input handler 
		evdev.c
			module_init(evdev_init);
			module_exit(evdev_exit);
	
			static struct input_handler evdev_handler = {
					.event          = evdev_event,
					.connect        = evdev_connect,
					.disconnect     = evdev_disconnect,
					.fops           = &evdev_fops,
					.minor          = EVDEV_MINOR_BASE, //64
					.name           = "evdev",
					.id_table       = evdev_ids,//由于比较的
			};

	
			
			 input_register_handler(&evdev_handler);
					|
					INIT_LIST_HEAD(&handler->h_list);//记录儿子的
					//将当前的evdev_handler放入到input_table[2]
					input_table[handler->minor >> 5] = handler;
					
					list_add_tail(&handler->node, &input_handler_list);
					list_for_each_entry(dev, &input_dev_list, node)
							input_attach_handler(dev, handler);
								|
								id = input_match_device(handler, dev);//匹配
								handler->connect(handler, dev, id); //匹配成功,调用了handler中connect方法
							
		调用的connect方法:
			.connect        = evdev_connect,
				|
				//在数组中找出一个没有被占用的位置
				for (minor = 0; minor < EVDEV_MINORS; minor++)
					if (!evdev_table[minor])
						break;
						
				//分配了一个evdev对象
				evdev = kzalloc(sizeof(struct evdev), GFP_KERNEL);
				
				//初始化evdev对象
				INIT_LIST_HEAD(&evdev->client_list);
				spin_lock_init(&evdev->client_lock);
				mutex_init(&evdev->mutex);
				//初始化等待队列头
				init_waitqueue_head(&evdev->wait);

				
				evdev->exist = true;
				evdev->minor = minor;

				//利用handle记录了inputdev 和input handler
				evdev->handle.dev = input_get_device(dev);
				evdev->handle.name = dev_name(&evdev->dev);
				evdev->handle.handler = handler;
				//相互留个联系方式
				evdev->handle.private = evdev;
				
				//创建设备节点--device_create()
				//设置名字---event0, event1,
				dev_set_name(&evdev->dev, "event%d", minor);
				evdev->dev.devt = MKDEV(INPUT_MAJOR, EVDEV_MINOR_BASE + minor);
				evdev->dev.class = &input_class;
				evdev->dev.parent = &dev->dev;
				evdev->dev.release = evdev_free;
				device_initialize(&evdev->dev);
				error = device_add(&evdev->dev);
				
				//inputdev, input handler, input handle建立双向关系
				error = input_register_handle(&evdev->handle);
						|
						struct input_handler *handler = handle->handler;
						struct input_dev *dev = handle->dev;
						
						list_add_tail_rcu(&handle->d_node, &dev->h_list);
						list_add_tail_rcu(&handle->h_node, &handler->h_list);
				
				//将当前的evdev放入到evdev_table[minor]
				error = evdev_install_chrdev(evdev);
							|
							evdev_table[evdev->minor] = evdev;
				
				
			
	


	创建设备节点:
		//创建设备节点--device_create()
		dev_set_name(&evdev->dev, "event%d", minor);
		evdev->dev.devt = MKDEV(INPUT_MAJOR, EVDEV_MINOR_BASE + minor);
		evdev->dev.class = &input_class;
		evdev->dev.parent = &dev->dev;
		evdev->dev.release = evdev_free;
		device_initialize(&evdev->dev);
		device_add(&evdev->dev);

		
	device_create(struct class *class, struct device *parent,
			     dev_t devt, void *drvdata, const char *fmt, ...)
		|
		dev = device_create_vargs(class, parent, devt, drvdata, fmt, vargs);
			|
			struct device *dev = NULL;
			dev = kzalloc(sizeof(*dev), GFP_KERNEL);
			dev->devt = devt;
			dev->class = class;
			dev->parent = parent;
			dev->release = device_create_release;
			device_register(dev);
				|
				device_initialize(dev);
				return device_add(dev);
			
	---------------------------------------
	input core
		input.c
		
	subsys_initcall(input_init);
	module_exit(input_exit);

		static LIST_HEAD(input_dev_list);
		static LIST_HEAD(input_handler_list);
	
			
			 err = class_register(&input_class);
			 err = input_proc_init();
			 register_chrdev(INPUT_MAJOR, "input", &input_fops);//申请设备号
			 
			 
	-----------------------------------
	input device

			input_register_device(inputdev);
				|
				list_add_tail(&dev->node, &input_dev_list);

				list_for_each_entry(handler, &input_handler_list, node)
					input_attach_handler(dev, handler);
						|
						id = input_match_device(handler, dev);//匹配
						handler->connect(handler, dev, id); //调用了handler中connect方法













	2,应用和驱动是如何交互
		fd = open("/dev/event0")
		
		struct input_event event;
		read(fd, &event, sizeof(struct input_event)))
		
		
	----------------------------------------------------------------------
	VFS:
		sys_open
			根据设备号找cdev
	--------------------------------
	驱动层的open:
	input.c
		register_chrdev(INPUT_MAJOR, "input", &input_fops);
			static const struct file_operations input_fops = {
				.owner = THIS_MODULE,
				.open = input_open_file,
				.llseek = noop_llseek,
			};
		
		input_open_file()
			|
			handler = input_table[iminor(inode) >> 5];
			if (handler)
				new_fops = fops_get(handler->fops);
				
			//将evdev_handler中fop给了file的f_op
			file->f_op = new_fops;
			//调用了evdev_handler中fop中open()
			err = new_fops->open(inode, file);//调到evedev.c层,调用了evdev_open
				
		-------------------------------------------------------------------
			static const struct file_operations evdev_fops = {
				.owner		= THIS_MODULE,
				.read		= evdev_read,
				.write		= evdev_write,
				.poll		= evdev_poll,
				.open		= evdev_open,
				.release	= evdev_release,
				.unlocked_ioctl	= evdev_ioctl,
			#ifdef CONFIG_COMPAT
				.compat_ioctl	= evdev_ioctl_compat,
			#endif
				.fasync		= evdev_fasync,
				.flush		= evdev_flush,
				.llseek		= no_llseek,
			};
			
			evdev_open
				|
				struct evdev *evdev;
				struct evdev_client *client;
				//获取到次设备号,并且减去64
				int i = iminor(inode) - EVDEV_MINOR_BASE;
				//从全局数组中获取到evdev
				evdev = evdev_table[i];
				
				//分配evdev_client对象,并且为缓冲队列分配空间
				bufsize = evdev_compute_buffer_size(evdev->handle.dev);

				client = kzalloc(sizeof(struct evdev_client) +
							bufsize * sizeof(struct input_event),
						 GFP_KERNEL);
				
				client->bufsize = bufsize;
				client->evdev = evdev;
				//将evdev_client关联到evdev中去
				evdev_attach_client(evdev, client);
					|
					list_add_tail_rcu(&client->node, &evdev->client_list);
		
				file->private_data = client;
		
		
		
		
		
		
		struct input_event event;
		read(fd, &event, sizeof(struct input_event)))
		---------------------------------------------------------
		sys_read
			|
			filp->f_fop->read()
		----------------------------------------------------------
		evdev.c
			.read			= evdev_read,
				|
				//阻塞
				retval = wait_event_interruptible(evdev->wait,
					client->packet_head != client->tail || !evdev->exist);
				//如果阻塞被唤醒,会从evdev_client中获取数据event
				while (retval + input_event_size() <= count &&
				   evdev_fetch_next_event(client, &event)) {
							|
							*event = client->buffer[client->tail++];
				//拷贝给用户
				if (input_event_to_user(buffer + retval, &event))
						
		
		
		谁唤醒?
		input devce层:
		btn_input->evbit[0] |= BIT_MASK(EV_KEY);
		btn_input->keybit[BIT_WORD(KEY_DOWN)] |= BIT_MASK(KEY_DOWN);
			input_sync()
				|
				input_event(dev, EV_SYN, SYN_REPORT, 0);
			input_event(btn_input, EV_KEY, KEY_DOWN,  0);
				|
				void input_event(struct input_dev *dev,
						unsigned int type, unsigned int code, int value)
						|
						if (is_event_supported(type, dev->evbit, EV_MAX)) {
							input_handle_event(dev, type, code, value);
								|
								switch (type) {
									case EV_KEY:
										if (is_event_supported(code, dev->keybit, KEY_MAX){
											disposition = INPUT_PASS_TO_HANDLERS;
										}
								}
						}
						if (disposition & INPUT_PASS_TO_HANDLERS)
								input_pass_event(dev, type, code, value);
									|
									struct input_handler *handler;
									struct input_handle *handle;
									//通过input dev找到input handle
									list_for_each_entry_rcu(handle, &dev->h_list, d_node)
									//通过input  handle找到input handler
									handler = handle->handler;
									//调用了 input handler中的event(), 调转到evdevc.
									handler->event(handle, type, code, value);
						-----------------------------------
						evdev.c
							static struct input_handler evdev_handler = {
								.event		= evdev_event,
								.connect	= evdev_connect,
								.disconnect	= evdev_disconnect,
								.fops		= &evdev_fops,
								.minor		= EVDEV_MINOR_BASE,
								.name		= "evdev",
								.id_table	= evdev_ids,
							};
							evdev_event(handle, type, code, value)
								|
								struct evdev *evdev = handle->private;
								struct evdev_client *client;
								struct input_event event;
								
								//通过传递过来的参数初始化input_event对象
								do_gettimeofday(&event.time);
								event.type = type;
								event.code = code;
								event.value = value;
								//通过evdev获取到evdev_client
								list_for_each_entry_rcu(client, &evdev->client_list, node)
									evdev_pass_event(client, &event);
										|
										client->buffer[client->head++] = *event;
								if (type == EV_SYN && code == SYN_REPORT)
										wake_up_interruptible(&evdev->wait);//如果调用input_sync那么表示唤醒等待队列
			input_sync(btn_input);
		
		
		
		