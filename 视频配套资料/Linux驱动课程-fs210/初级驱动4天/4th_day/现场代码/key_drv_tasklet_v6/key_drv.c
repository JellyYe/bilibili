#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/input.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/mm.h>

#include <asm/io.h>
#include <asm/uaccess.h>


//#define USE_STATIC_MAJOR 1
#define KEY_MAJOR  260


//设计一个用于传递内存中的数据对象
struct mem_data{
	char buf[128];
};

#define KEY_IOC_GET_DATA  0x8808

//设计一个按键数据包
struct key_event{
	int code; //按键的名字---下键,回车键,ESC--KEY_ESC
	int value; //按键的状态---按下和抬起--1/0
};


//设计一个描述按键的对象--irqno, gpio, code, name,flags
struct key_desc{
	char *name;
	int irqno;
	int gpio;
	int code;
	unsigned long flags;
};


//设计一个全局的设备对象类
struct s5pv210_key{
	dev_t devno;
	int irqno;
	struct cdev *cdev;
	struct class *cls;
	struct device *dev;
	struct key_event event;

	wait_queue_head_t wq_head;
	int have_data; //表示一个标志,是否有数据
	void *virt_mem;
	struct tasklet_struct  tasklet;
};


//声明一个对象

struct s5pv210_key *key_dev;


//定义一个描述所有按键的集合
struct key_desc key_set[] = {
	[0] = {
		.name = "key1_up",
		.irqno = IRQ_EINT(0),
		.gpio = S5PV210_GPH0(0),
		.code = KEY_UP,
		.flags = IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING,
	},
	[1] = {
		.name = "key2_down",
		.irqno = IRQ_EINT(1),
		.gpio = S5PV210_GPH0(1),
		.code = KEY_DOWN,
		.flags = IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING,
	},

	[2] = {
		.name = "key3_right",
		.irqno = IRQ_EINT(2),
		.gpio = S5PV210_GPH0(2),
		.code = KEY_RIGHT,
		.flags = IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING,
	},

		
	[3] = {
				.name = "key4_left",
				.irqno = IRQ_EINT(3),
				.gpio = S5PV210_GPH0(3),
				.code = KEY_LEFT,
				.flags = IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING,
	},


};


int key_drv_open(struct inode *inode, struct file *filp)
{

	printk("-------^_^ %s-------\n", __FUNCTION__);
	
	memset(&key_dev->event, 0, sizeof(struct key_event));
	key_dev->have_data = 0;
	
	return 0;

}

//  write(fd, buf, size);
ssize_t key_drv_write(struct file *filp, const char __user *buf, size_t count, loff_t *fpos)
{

	
	printk("-------^_^ %s-------\n", __FUNCTION__);
	// 区分应用的需求

	
	return 0;

}

int key_drv_close(struct inode *inode, struct file *filp)
{

	
	printk("-------^_^ %s-------\n", __FUNCTION__);

	return 0;
}




ssize_t key_drv_read(struct file *filp, char __user *buf, size_t count, loff_t *fpos)
{
	
	int ret;

	//区分阻塞还是非阻塞
	if((filp->f_flags & O_NONBLOCK) && !key_dev->have_data)
	{
		return -EAGAIN;
	}

	//判断是否有资源
	wait_event_interruptible(key_dev->wq_head,  key_dev->have_data);
	
	//将中断处理函数获取到数据给用户
	ret = copy_to_user(buf, &key_dev->event, count);
	if(ret > 0)
	{
		printk("copy_to_user error\n");
		return -EFAULT;
	}
	

	//清空event
	memset(&key_dev->event, 0, sizeof(struct key_event));
	// 拷贝数据之后表示没有数据
	key_dev->have_data = 0;

	return count;
	
}

unsigned int key_drv_poll(struct file *filp, struct poll_table_struct *pts)
{
	printk("-------^_^ %s-------\n", __FUNCTION__);
	unsigned int mask = 0;
	
	//  1, 将当前的等待队列头注册到vfs中
	//参数1---struct file对象
	//参数2---等待队列头
	//参数3---与等待队列关联的表格
	poll_wait(filp, &key_dev->wq_head, pts);

	// 2, 如果有数据返回一个pollin,没有数据返回一个0
	if(key_dev->have_data)
	{
		mask |= POLLIN;

	}

	return mask;
}


int key_drv_mmap(struct file *filp, struct vm_area_struct *vma)
{

	unsigned long addr;

	// 只要调用io_remap_pfn_range就能将驱动中内存映射到用户空间
	//vma用于描述用户空间映射的需求, vma是vfs层传递过来

	addr = virt_to_phys(key_dev->virt_mem);

	vma->vm_flags |= VM_IO;
	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	
	//参数1---表示需求
	//参数2---映射到用户空间的起始位置
	//参数3--被映射的物理地址的页地址
	//参数4---映射的大小
	//参数5--映射的权限
	if (io_remap_pfn_range(vma, vma->vm_start, addr >> PAGE_SHIFT,
					PAGE_SIZE, vma->vm_page_prot)) {
		printk(KERN_ERR "%s: io_remap_pfn_range failed\n",
			__func__);
		return -EAGAIN;
	}

	return 0;

}


long key_drv_ioctl(struct file *filp, unsigned int cmd, unsigned long args)
{
	//将第三个参数转换成一个指针
	void __user *argp = (void __user *)args;
	int ret;
	struct mem_data data;
	
	
	switch(cmd){
		case KEY_IOC_GET_DATA:
			memcpy(data.buf, key_dev->virt_mem, 128);
			ret = copy_to_user(argp, &data, sizeof(struct mem_data));
			if(ret > 0)
				return -EFAULT;
			
			break;


		default:
			printk("unkown cmd\n");

	}


	return 0;

}



const struct file_operations key_fops = {
	.open = key_drv_open,
	.write = key_drv_write,
	.read = key_drv_read,
	.poll = key_drv_poll,
	.release = key_drv_close,
	.unlocked_ioctl = key_drv_ioctl,
	.mmap = key_drv_mmap,
};

//中断的下半部
void key_irq_tasklet(unsigned long data)
{
	printk("-------^_^ %s-------\n", __FUNCTION__);
	//此时就有数据
	key_dev->have_data = 1;
	//唤醒等待队列
	wake_up_interruptible(&key_dev->wq_head);

}


//表示当前的中断号码
irqreturn_t  key_irq_svc(int irqno, void *dev_id)
{
	printk("-------^_^ %s-------\n", __FUNCTION__);

	
	//区分不同的中断
	struct key_desc *p = (struct key_desc *)dev_id;

	// 区分是按下还是抬起
		
	int value = gpio_get_value(p->gpio);

	if(value){
		//抬起

		printk("<KERNEL>-------%s up\n", p->name);
		key_dev->event.code = p->code;
		key_dev->event.value = 0;
		
	}else{
		printk("<KERNEL>-------%s pressed\n", p->name);
		key_dev->event.code =  p->code;
		key_dev->event.value = 1;
	}

	
	// 启动下半部
	tasklet_schedule(&key_dev->tasklet);


	return IRQ_HANDLED;

}


static int __init key_drv_init(void)
{

	/*
		编写驱动的套路
		0, 实例化全局的设备对象-- kzalloc
		1,  申请主设备号---register_chrdev
		2, 自动创建设备节点---class_create, device_create
		3, 初始化硬件--ioremap
		4,实现 file_operation

	*/ 
	
	// 模块加载函数中主要完成系统资源的申请
	printk("-------^_^ %s-------\n", __FUNCTION__);
	int ret;

	// 0, 实例化全局的设备对象
	//参数1---分配大小
	//参数2--分配的标志, GFP_KERNEL--如果当前暂时没有内存,会尝试等待
	key_dev = kzalloc(sizeof(struct s5pv210_key), GFP_KERNEL);
	if(key_dev == NULL)
	{
		printk(KERN_ERR"kzalloc error\n");
		return -ENOMEM;
	}
	
	
	// 1,  申请主设备号
	#ifdef USE_STATIC_MAJOR
		//静态申请设备号---仅仅是得到一个设备号而已
		// 参数1---设备号
		// 参数2---设备的个数
		// 参数3--描述字符串--/proc/devices
		//返回负数出错
		key_dev->devno = MKDEV(KEY_MAJOR, 32);
		ret = register_chrdev_region(devno, 1, "key_new_drv");
		if(ret < 0)
		{
			printk(KERN_ERR"register_chrdev_region error\n");
			goto err_free;
		}

	#else
		// 参数1---系统动态分配之后的设备号
		// 参数2--次设备号的起始值
		// 参数3---设备的个数
		// 参数4--描述字符串--/proc/devices
		//正确返回0
		ret = alloc_chrdev_region(&key_dev->devno,32, 1, "key_new_drv");
		if(ret != 0)
		{
			printk(KERN_ERR"register_chrdev_region error\n");
			goto err_free;
		}
	#endif

	// 动态分配一个struct cdev 对象
	key_dev->cdev = cdev_alloc();
	//初始化cdev中fops
	cdev_init(key_dev->cdev, &key_fops);
	//将当前cdev注册到系统中去
	//参数2---设备号
	//参数3--设备的个数,一般都填1
	cdev_add(key_dev->cdev, key_dev->devno, 1);



	// 2 ---自动创建设备节点

	//创建一个类
	// 参数1---当前模块--THIS_MODULE
	// 参数2---字符串,表示类的名字
	//返回值--struct class指针类型
	key_dev->cls = class_create(THIS_MODULE,"key_cls");
	if(IS_ERR(key_dev->cls))
	{
		printk("class_create error\n");
		ret = PTR_ERR(key_dev->cls);
		goto err_unregister;
		
	}

	//创建一个设备节点
	// 参数1---class_create返回的指针
	// 参数2---该设备非父类--一般都是填NULL
	//参数3--设备号--包含了主设备号major和次设备号minor 
	//参数4---私有数据指针---一般都是填NULL
	//参数5---设备节点的名字
	//结果  /dev/led
	// 返回值--struct device指针
	key_dev->dev = device_create(key_dev->cls, NULL,key_dev->devno, NULL, "key%d", 0);
	if(IS_ERR(key_dev->dev))
	{
		printk("device_create error\n");
		ret = PTR_ERR(key_dev->dev);
		goto err_class_destroy;
		
	}

	// 3, 初始化硬件---要么映射地址/申请中断
	//  参数1---中断号
	// 中断号获取: IRQ_EINT(1)或者去找irqs.h

	key_dev->irqno = IRQ_EINT(1);

	int i;
	int irqno;
	unsigned long flags;
	char *name;
	for(i=0; i<ARRAY_SIZE(key_set); i++)
	{


		irqno = key_set[i].irqno;
		flags = key_set[i].flags;
		name = key_set[i].name;
		
		ret = request_irq(irqno, key_irq_svc, flags, name, &key_set[i]);
		if(ret != 0)
		{
			printk("request_irq error\n");
			ret = -EBUSY;
			goto err_device_destrory;
		}

	}
		
	


	//初始化等待队列头
	init_waitqueue_head(&key_dev->wq_head);


	//分配一块内存
	key_dev->virt_mem = kzalloc(PAGE_SIZE, GFP_KERNEL);


	// 初始化tasklet
	tasklet_init(&key_dev->tasklet, key_irq_tasklet, 45);
	
	return 0;


err_device_destrory:
	device_destroy(key_dev->cls, key_dev->devno);

err_class_destroy:
	class_destroy(key_dev->cls);

err_unregister:
	cdev_del(key_dev->cdev);
	unregister_chrdev_region(key_dev->devno, 1);

err_free:
	kfree(key_dev);
	return ret;

}


static void __exit key_drv_exit(void)
{

	printk("-------^_^ %s-------\n", __FUNCTION__);

	//释放中断
	//参数1--中断号码
	//参数2--和request_irq中最后一个参数保持一致
	int i;
	
	tasklet_kill(&key_dev->tasklet);
	
	kfree(key_dev->virt_mem);
	
	for(i=0; i<ARRAY_SIZE(key_set); i++)
	{
		free_irq(key_set[i].irqno, &key_set[i]);
	}	
	
	// 模块卸载函数中主要完成系统资源的释放
	device_destroy(key_dev->cls, key_dev->devno);
	
	class_destroy(key_dev->cls);

	cdev_del(key_dev->cdev);
	unregister_chrdev_region(key_dev->devno, 1);
	
	kfree(key_dev);

}


module_init(key_drv_init);
module_exit(key_drv_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("wukong@qq.com");






