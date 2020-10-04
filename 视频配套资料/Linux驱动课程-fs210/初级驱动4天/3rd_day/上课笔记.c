主要内容: 中断开发--按键驱动
1,linux中file, cdev, inode之间的关系
2,新的注册字符设备的方式
3, 中断申请
4, 文件io模型实现之阻塞和非阻塞
-----------------------------------------




struct file对象:描述进程中打开open一个文件的信息:文件名,标志(可读写),文件偏移
open("/dev/led", O_RDWR|O_CREAT, 0666);
struct file {
	struct path		f_path;
	const struct file_operations	*f_op;
	unsigned int 		f_flags;
	fmode_t			f_mode;
	loff_t			f_pos; //文件偏移
	void			*private_data;//万能指针
	
}


struct cdev对象:描述一个字符设备对象信息(设备号+文件操作对象),任何一个字符设备驱动都有该对象,
struct cdev {
	struct kobject kobj;// 基类
	struct module *owner;
	const struct file_operations *ops;//文件操作对象
	struct list_head list;// 链表
	dev_t dev; //设备号
	unsigned int count;
};

struct inode对象: 描述文件系统中的某个文件的属性(文件权限,类型,uid,gid,修改时间等)
struct inode {
	umode_t			i_mode;
	uid_t			i_uid;
	gid_t			i_gid;
	dev_t			i_rdev; //设备号
	const struct file_operations	*i_fop;
	struct timespec		i_atime;
	struct timespec		i_mtime;
	struct timespec		i_ctime;
}



int led_drv_open(struct inode *inode, struct file *filp)
{

	printk("-------^_^ %s-------\n", __FUNCTION__);

	static int a = 38;
	filp->private_data = &a;
	
	xxxxxxxx
}
long led_drv_ioctl(struct file *filp, unsigned int cmd, unsigned long args)
{

	// 通过filp找到inode
	struct inode *node = filp->f_path.dentry->d_inode;
	//通过inode获取到注册设备号
	int minor = iminor(node);
	int major = imajor(node);
	//获取到数据
	int *p = (int *)filp->private_data;
	
	xxxxxxxx
}


2,新的注册字符设备的方式
	cdev_alloc(void)
	cdev_init(struct cdev * cdev,const struct file_operations * fops)
	cdev_add(struct cdev * p,dev_t dev,unsigned count)

	
	//静态申请设备号---仅仅是得到一个设备号而已
	// 参数1---设备号
	// 参数2---设备的个数
	// 参数3--描述字符串--/proc/devices
	//返回负数出错
	dev_t devno = MKDEV(260, 0);
	ret = register_chrdev_region(devno, 1, "key_new_drv");
	if(ret < 0)
	{
		printk(KERN_ERR"register_chrdev_region error\n");
		goto err_free;
	}

	// 动态分配一个struct cdev 对象
	key_dev->cdev = cdev_alloc();
	//初始化cdev中fops
	cdev_init(key_dev->cdev, &key_fops);
	//将当前cdev注册到系统中去
	//参数2---设备号
	//参数3--设备的个数,一般都填1
	cdev_add(key_dev->cdev, devno, 1);


	
3,申请中断:
	//  参数1---中断号
	// 中断号获取: IRQ_EINT(1)或者去找irqs.h
	//参数2--中断的处理方法irqreturn_t (*irq_handler_t)(int, void *);
	//参数3--中断的触发方式
	/*
		#define IRQF_TRIGGER_NONE	0x00000000 //内部中断触发
		#define IRQF_TRIGGER_RISING	0x00000001
		#define IRQF_TRIGGER_FALLING	0x00000002
		#define IRQF_TRIGGER_HIGH	0x00000004
		#define IRQF_TRIGGER_LOW	0x00000008
	*/
	//参数3--表示一个字符串--自定义--/proc/interrupts
	// 参数4--参数给第二个参数的数据
	// 正确返回0
	request_irq(unsigned int irq,irq_handler_t handler,unsigned long flags,const char * name,void * dev)
	
	
	中断处理函数:
	irqreturn_t  key_irq_svc(int irqno, void *dev_id)
	{



		return IRQ_HANDLED;

	}

	
	
	
	//参数1--中断号码
	//参数2--和request_irq中最后一个参数保持一致
	free_irq(int irqno,void * dev_id)


	
	
	
	
实现阻塞: 默认情况,大部分的函数默认都是阻塞
	  scanf()-- fgets()
	  accept(); read/recv/recvfrom
	
	0---需要一个等待队列头
		struct __wait_queue_head wait_queue_head_t;
		init_waitqueue_head(struct wait_queue_head_t *q)
		
		
	// 参数1---表示等待队列头
	// 参数2---表示一个条件--如果为假,就在此休眠,如果为真,就不休眠
	1,根据条件可以让进程进入到休眠状态
		wait_event_interruptible(struct wait_queue_head_t wq, int condition)
		
	2, 资源可达的时候需要唤醒
		wake_up_interruptible(wait_queue_head_t *q)
	
	
非阻塞:
		在应用中设定非阻塞模式:
		int fd = open("/dev/key0", O_RDWR|O_NONBLOCK)
		
		read() 有数据就得到数据,没有数据就得到一个出错码--EAGAIN;
		-----------------------------------------------------------
		驱动:
			xxx_read
			{
				区分阻塞还是非阻塞
				if((filp->f_flags & O_NONBLOCK) && !key_dev->have_data)
				{
					return -EAGAIN;
				}

			}
		
		
		
		
		
		
		
		
		
		
		
		
		
	
	
	
	