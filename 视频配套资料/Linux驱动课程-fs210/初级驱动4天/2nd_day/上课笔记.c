主要内容: Linux中led驱动

1, 应用空间调用到驱动的框架
2, 自动创建设备节点的方法
3, 内核对硬件初始化的方式
4, 应用空间和内核之间的数据交互
5, linux中ioctl的实现和gpio库函数的使用
---------------------------------------------------
2, 自动创建设备节点的方法
	#define MINORBITS	20
	#define MINORMASK	((1U << MINORBITS) - 1)

	#define MAJOR(dev)	((unsigned int) ((dev) >> MINORBITS))
	#define MINOR(dev)	((unsigned int) ((dev) & MINORMASK))
	#define MKDEV(ma,mi)	(((ma) << MINORBITS) | (mi))


	// 参数1---当前模块--THIS_MODULE
	// 参数2---字符串,表示类的名字
	//返回值--struct class指针类型
	class_create(owner, name);


	//创建一个设备节点
	// 参数1---class_create返回的指针
	// 参数2---该设备非父类--一般都是填NULL
	//参数3--设备号--包含了主设备号major和次设备号minor 
	//参数4---私有数据指针---一般都是填NULL
	//参数5---设备节点的名字
	//结果  /dev/led
	// 返回值--struct device指针
	struct device *device_create(struct class *cls, struct device *parent,
				    dev_t devt, void *drvdata,
				    const char *fmt, ...)
	
	


	// 3, 初始化硬件
	//参数1---物理地址
	//参数2--映射的长度
	//返回值--映射之后的虚拟地址
	gpc0_conf = ioremap(0xE0200060, 8);


4, 应用空间和内核之间的数据交互
		//  从用户空间获取数据, 一般都用在驱动中写操作中-- xxx_write
		// 参数1---目标地址---内核中的空间的地址
		//参数2---原地址---用户空间的地址
		//参数3---拷贝数据个数
		//返回值--没有拷贝成功的数据个数, 0表示成功
		unsigned long copy_from_user(void * to,const void __user * from,unsigned long n)
		
		
		
		//给用户空间数据, 一般都用在驱动中读操作中-- xxx_read
		unsigned long copy_to_user(void __user * to,const void * from,unsigned long n)



5, linux中ioctl的实现和gpio库函数的使用

	如果需要给用户提供更多的使用api,可以添加一个ioctl接口:
	ioctl()用于给驱动发送指令: 某个灯亮, 某个等灭, 全亮,全灭


			应用空间:
				int ioctl(int fd, int cmd,  .../unsigned long args);
		--------------------------------------------------------
		驱动:
			    xxx_ioctl(struct file *filp, unsigned int cmd, unsigned long args)
				{
					//区分不同的命令
					switch(cmd)
					{
						case 命令1:
						case 命令2:
						case 命令3:
						case 命令4:
					}
				
				}

		命令是如何定义: 由程序猿决定, 一定是要一个整数
			1,直接用一个整数--有可能与系统中的已经存在的命令发生冲突
				#define LED_ALL_ON	0x2222
				#define LED_ALL_OFF 0x3333
		

			2, 用内核提供接口来定义一个整数
				_IO(type,nr) //参数1--魔幻数--字符
							// 参数2--唯一的数字
				_IOW(type,nr,size)
				_IOR(type,nr,size)
				_IOWR(type,nr,size)

				例子:
					#define LED_NUM_ON  _IOW('L',0x3456, int)
					#define LED_NUM_OFF  _IOW('L',0x3457, int)
					
					#define LED_ALL_ON  _IO('L',0x3458)
					#define LED_ALL_OFF  _IO('L',0x3459)
	


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
	
