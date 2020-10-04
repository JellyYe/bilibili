#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>

#include <asm/io.h>


//#define USE_STATIC_MAJOR 1
#define KEY_MAJOR  260


//设计一个全局的设备对象类
struct s5pv210_key{
	dev_t devno;
	int irqno;
	struct cdev *cdev;
	struct class *cls;
	struct device *dev;
};

//声明一个对象

struct s5pv210_key *key_dev;



int key_drv_open(struct inode *inode, struct file *filp)
{

	printk("-------^_^ %s-------\n", __FUNCTION__);
	
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


const struct file_operations key_fops = {
	.open = key_drv_open,
	.write = key_drv_write,
	.release = key_drv_close,
};


//表示当前的中断号码
irqreturn_t  key_irq_svc(int irqno, void *dev_id)
{
	printk("-------^_^ %s-------\n", __FUNCTION__);

	// 区分是按下还是抬起
		
	int value = gpio_get_value(S5PV210_GPH0(1));

	if(value){
		//抬起

		printk("key1 up\n");
	}else{
		printk("key1 pressed\n");

	}
	

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
		
	ret = request_irq(key_dev->irqno, key_irq_svc, IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING,
						 "eint1-keydown", NULL);
	if(ret != 0)
	{
		printk("request_irq error\n");
		ret = -EBUSY;
		goto err_device_destrory;
	}

	
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
	free_irq(key_dev->irqno, NULL);
	
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






