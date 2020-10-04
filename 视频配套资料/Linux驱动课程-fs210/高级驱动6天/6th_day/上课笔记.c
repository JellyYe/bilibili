��Ҫ����---������ϵͳ���--��Ҫ��������豸
1,����ϵͳ�����úͿ��
2,������ϵͳ�ı��
3,������ϵͳ��ƽ̨���ߵĽ��
4,������ϵͳ��ִ������
---------------------------------

1,����ϵͳ�����úͿ��
	��������豸: button, keyboard, mouse, ts, gsensor, joystick
	���ղ������ݵ����ͽ��з���:
		1,��������---��ֵ
			button, keyboard
		2, ������������---�����ֵ����Сֵ
			ts, gsensor,������
		3, �����������---��һ�������������֮ǰ������
			mouse
	
	����������ϵͳ�������豸������������:
		1, Ϊ��������һ����׼�ı�̷�ʽ
		2, �û��ռ䵽���ݸ�ʽ��ͳһ��
		
		
	���:
		
		Ӧ�ò�
		------------------------------------
		input handler ���ݴ����: drivers/input/evdev.c
			1, ���û����н���,ʵ��fops
			2, ��֪����������εõ�,����֪����ν����ݽ����û�
		
		
		-------------------------------------
		input core��--drivers/input/input.c
			1,ά������������
			2, Ϊ���������ṩ�ӿ�
		
		
		---------------------------------------
		input device��: �����豸Ӳ����--�Լ�ȥд
			�Դ���input device����:
				drivers/input/touchscreen/
				
			1, ��ʼ��Ӳ��,��ȡӲ��������
			2, ֪���õ�����,���ǲ�֪����ν����ݸ���
			
		-------------------------------------------
		
		
		ȷ��zImage�Ѿ�������input core��input handler�����
		make menuconfig
			Device Drivers  --->
				Input device support  --->
					-*- Generic input layer (needed for keyboard, mouse, ...)// input.c
					<*>   Event interface //  input handler����� evdev.c 
		
		
2,������ϵͳ�ı��:Documentation/input/input-programming.txt
	a, ����һ��input device����
	b, ��ʼ��input device����
	c, ע��input device����
	d, Ӳ����ʼ��
		
		
		struct input_dev *input_allocate_device(void)
		void input_free_device(struct input_dev *dev)
		
		int input_register_device(struct input_dev *dev)
		void input_unregister_device(struct input_dev *dev)
		
		//�ϱ�����
			void input_report_key(struct input_dev *dev, unsigned int code, int value)
					|
					void input_event(struct input_dev *dev, unsigned int type, unsigned int code, int value)
		
		
		struct input_dev {
			const char *name;
			const char *phys;
			const char *uniq;
			struct input_id id;


			//λ��
			// evbit��ʾ�ܹ�������Щ��������
			unsigned long evbit[BITS_TO_LONGS(EV_CNT)];
			// keybit��ʾ�ܹ�������Щ��������
			unsigned long keybit[BITS_TO_LONGS(KEY_CNT)];//keybit[BITS_TO_LONGS(768)]=keybit[24]
			// relbit��ʾ�ܹ�������Щ�����������
			unsigned long relbit[BITS_TO_LONGS(REL_CNT)];
			// absbit ��ʾ�ܹ�������Щ������������
			unsigned long absbit[BITS_TO_LONGS(ABS_CNT)];//absbit[2];
			unsigned long mscbit[BITS_TO_LONGS(MSC_CNT)];
			unsigned long ledbit[BITS_TO_LONGS(LED_CNT)];
			unsigned long sndbit[BITS_TO_LONGS(SND_CNT)];
			unsigned long ffbit[BITS_TO_LONGS(FF_CNT)];
			unsigned long swbit[BITS_TO_LONGS(SW_CNT)];
		
			struct device dev; //����

			struct list_head	h_list;
			struct list_head	node;//����Ľ��
		};
	
	
		struct input_event { //�û���ȡ�������豸�����ݰ�
			struct timeval time;//ʱ���
			__u16 type;//��ȡ����������: EV_KEY, EV_ABS, EV_REL
			__u16 code;//��ֵ
			__s32 value;//״̬
		};

	
================================================================================
��Դ��:

	1,�������ֲ�
	2,Ӧ�ú���������ν���
		open, read()
	
----------------------------------------------
1,�ֲ�


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
					.id_table       = evdev_ids,//���ڱȽϵ�
			};

	
			
			 input_register_handler(&evdev_handler);
					|
					INIT_LIST_HEAD(&handler->h_list);//��¼���ӵ�
					//����ǰ��evdev_handler���뵽input_table[2]
					input_table[handler->minor >> 5] = handler;
					
					list_add_tail(&handler->node, &input_handler_list);
					list_for_each_entry(dev, &input_dev_list, node)
							input_attach_handler(dev, handler);
								|
								id = input_match_device(handler, dev);//ƥ��
								handler->connect(handler, dev, id); //ƥ��ɹ�,������handler��connect����
							
		���õ�connect����:
			.connect        = evdev_connect,
				|
				//���������ҳ�һ��û�б�ռ�õ�λ��
				for (minor = 0; minor < EVDEV_MINORS; minor++)
					if (!evdev_table[minor])
						break;
						
				//������һ��evdev����
				evdev = kzalloc(sizeof(struct evdev), GFP_KERNEL);
				
				//��ʼ��evdev����
				INIT_LIST_HEAD(&evdev->client_list);
				spin_lock_init(&evdev->client_lock);
				mutex_init(&evdev->mutex);
				//��ʼ���ȴ�����ͷ
				init_waitqueue_head(&evdev->wait);

				
				evdev->exist = true;
				evdev->minor = minor;

				//����handle��¼��inputdev ��input handler
				evdev->handle.dev = input_get_device(dev);
				evdev->handle.name = dev_name(&evdev->dev);
				evdev->handle.handler = handler;
				//�໥������ϵ��ʽ
				evdev->handle.private = evdev;
				
				//�����豸�ڵ�--device_create()
				//��������---event0, event1,
				dev_set_name(&evdev->dev, "event%d", minor);
				evdev->dev.devt = MKDEV(INPUT_MAJOR, EVDEV_MINOR_BASE + minor);
				evdev->dev.class = &input_class;
				evdev->dev.parent = &dev->dev;
				evdev->dev.release = evdev_free;
				device_initialize(&evdev->dev);
				error = device_add(&evdev->dev);
				
				//inputdev, input handler, input handle����˫���ϵ
				error = input_register_handle(&evdev->handle);
						|
						struct input_handler *handler = handle->handler;
						struct input_dev *dev = handle->dev;
						
						list_add_tail_rcu(&handle->d_node, &dev->h_list);
						list_add_tail_rcu(&handle->h_node, &handler->h_list);
				
				//����ǰ��evdev���뵽evdev_table[minor]
				error = evdev_install_chrdev(evdev);
							|
							evdev_table[evdev->minor] = evdev;
				
				
			
	


	�����豸�ڵ�:
		//�����豸�ڵ�--device_create()
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
			 register_chrdev(INPUT_MAJOR, "input", &input_fops);//�����豸��
			 
			 
	-----------------------------------
	input device

			input_register_device(inputdev);
				|
				list_add_tail(&dev->node, &input_dev_list);

				list_for_each_entry(handler, &input_handler_list, node)
					input_attach_handler(dev, handler);
						|
						id = input_match_device(handler, dev);//ƥ��
						handler->connect(handler, dev, id); //������handler��connect����













	2,Ӧ�ú���������ν���
		fd = open("/dev/event0")
		
		struct input_event event;
		read(fd, &event, sizeof(struct input_event)))
		
		
	----------------------------------------------------------------------
	VFS:
		sys_open
			�����豸����cdev
	--------------------------------
	�������open:
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
				
			//��evdev_handler��fop����file��f_op
			file->f_op = new_fops;
			//������evdev_handler��fop��open()
			err = new_fops->open(inode, file);//����evedev.c��,������evdev_open
				
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
				//��ȡ�����豸��,���Ҽ�ȥ64
				int i = iminor(inode) - EVDEV_MINOR_BASE;
				//��ȫ�������л�ȡ��evdev
				evdev = evdev_table[i];
				
				//����evdev_client����,����Ϊ������з���ռ�
				bufsize = evdev_compute_buffer_size(evdev->handle.dev);

				client = kzalloc(sizeof(struct evdev_client) +
							bufsize * sizeof(struct input_event),
						 GFP_KERNEL);
				
				client->bufsize = bufsize;
				client->evdev = evdev;
				//��evdev_client������evdev��ȥ
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
				//����
				retval = wait_event_interruptible(evdev->wait,
					client->packet_head != client->tail || !evdev->exist);
				//�������������,���evdev_client�л�ȡ����event
				while (retval + input_event_size() <= count &&
				   evdev_fetch_next_event(client, &event)) {
							|
							*event = client->buffer[client->tail++];
				//�������û�
				if (input_event_to_user(buffer + retval, &event))
						
		
		
		˭����?
		input devce��:
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
									//ͨ��input dev�ҵ�input handle
									list_for_each_entry_rcu(handle, &dev->h_list, d_node)
									//ͨ��input  handle�ҵ�input handler
									handler = handle->handler;
									//������ input handler�е�event(), ��ת��evdevc.
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
								
								//ͨ�����ݹ����Ĳ�����ʼ��input_event����
								do_gettimeofday(&event.time);
								event.type = type;
								event.code = code;
								event.value = value;
								//ͨ��evdev��ȡ��evdev_client
								list_for_each_entry_rcu(client, &evdev->client_list, node)
									evdev_pass_event(client, &event);
										|
										client->buffer[client->head++] = *event;
								if (type == EV_SYN && code == SYN_REPORT)
										wake_up_interruptible(&evdev->wait);//�������input_sync��ô��ʾ���ѵȴ�����
			input_sync(btn_input);
		
		
		
		