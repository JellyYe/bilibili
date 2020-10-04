主要内容---电容触摸屏驱动
1,触摸屏的基本工作原理
2, 电容触摸屏的驱动框架
3,电容触摸屏读取坐标的原理和硬件初始化
4,linux下多点触摸的协议
5,电容触摸屏的驱动编写--gt811




驱动编程:
	1, 提供i2c client信息
		Mach-smdkv210.c (arch\arm\mach-s5pv210)

		
		static struct i2c_board_info smdkv210_i2c_devs2[] __initdata = {
			/* To Be Updated */
			{ I2C_BOARD_INFO("gt811_ts", 0x5d), },
		};
		
		
		
		make zImage -j2
		更新内核:
			cp -raf arch/arm/boot/zImage /tftpboot
			
			
		[root@farsight devices]# ls
		0-001b  0-0050  2-005d  i2c-0   i2c-1   i2c-2
		[root@farsight devices]# pwd
		/sys/bus/i2c/devices
		[root@farsight 2-005d]# ls
		modalias   name       power      subsystem  uevent
		[root@farsight 2-005d]# cat name
		gt811_ts
		
		
	2, 编写从设备驱动:
		EINT14---GPH1_6
		RESET ---GPD0_3
		
		硬件初始化:
			1,  设置INT引脚为输入态, RESET设置成高电平(内部上拉)
			2,  RESET输出为低, 延时1ms,转成输入态
			3,延时至少20ms,通过i2c寻址判断是否有响应
			4, 如果有响应就分一次或多次初始化配置106个寄存器
			
			
			
			
	linux下多点触摸的协议:
		两点:	
		   ABS_MT_POSITION_X x[0]
		   ABS_MT_POSITION_Y y[0]
		   SYN_MT_REPORT //表示第一个点上报完毕
		   ABS_MT_POSITION_X x[1]
		   ABS_MT_POSITION_Y y[1]
		   SYN_MT_REPORT //表示第二个点上报完毕
		   SYN_REPORT // 所有点都上报完毕
		
			
		如果用代码去实现:
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
			
			
			
			
			

