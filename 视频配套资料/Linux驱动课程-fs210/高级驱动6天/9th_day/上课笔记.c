��Ҫ����---���ݴ���������
1,�������Ļ�������ԭ��
2, ���ݴ��������������
3,���ݴ�������ȡ�����ԭ���Ӳ����ʼ��
4,linux�¶�㴥����Э��
5,���ݴ�������������д--gt811




�������:
	1, �ṩi2c client��Ϣ
		Mach-smdkv210.c (arch\arm\mach-s5pv210)

		
		static struct i2c_board_info smdkv210_i2c_devs2[] __initdata = {
			/* To Be Updated */
			{ I2C_BOARD_INFO("gt811_ts", 0x5d), },
		};
		
		
		
		make zImage -j2
		�����ں�:
			cp -raf arch/arm/boot/zImage /tftpboot
			
			
		[root@farsight devices]# ls
		0-001b  0-0050  2-005d  i2c-0   i2c-1   i2c-2
		[root@farsight devices]# pwd
		/sys/bus/i2c/devices
		[root@farsight 2-005d]# ls
		modalias   name       power      subsystem  uevent
		[root@farsight 2-005d]# cat name
		gt811_ts
		
		
	2, ��д���豸����:
		EINT14---GPH1_6
		RESET ---GPD0_3
		
		Ӳ����ʼ��:
			1,  ����INT����Ϊ����̬, RESET���óɸߵ�ƽ(�ڲ�����)
			2,  RESET���Ϊ��, ��ʱ1ms,ת������̬
			3,��ʱ����20ms,ͨ��i2cѰַ�ж��Ƿ�����Ӧ
			4, �������Ӧ�ͷ�һ�λ��γ�ʼ������106���Ĵ���
			
			
			
			
	linux�¶�㴥����Э��:
		����:	
		   ABS_MT_POSITION_X x[0]
		   ABS_MT_POSITION_Y y[0]
		   SYN_MT_REPORT //��ʾ��һ�����ϱ����
		   ABS_MT_POSITION_X x[1]
		   ABS_MT_POSITION_Y y[1]
		   SYN_MT_REPORT //��ʾ�ڶ������ϱ����
		   SYN_REPORT // ���е㶼�ϱ����
		
			
		����ô���ȥʵ��:
			input_event(dev, EV_ABS, ABS_MT_POSITION_X,  333);
			input_event(dev, EV_ABS, ABS_MT_POSITION_Y,  133);
			input_mt_sync(dev);
				|
				input_event(dev, EV_SYN, SYN_MT_REPORT, 0);
			input_event(dev, EV_ABS, ABS_MT_POSITION_X,  433);
			input_event(dev, EV_ABS, ABS_MT_POSITION_Y,  533);
			input_mt_sync(dev);
			input_sync(dev)
				|
				input_event(dev, EV_SYN, SYN_REPORT, 0);
			
			
			
			
			

