/*1. 头文件*/
#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/device.h>
#include<linux/slab.h>
#include<linux/cdev.h>
#include<asm/uaccess.h>
#include<asm/io.h>

static unsigned int led_major=0;
volatile unsigned long *gpc0con = NULL;
volatile unsigned long *gpc0dat = NULL;

struct led_device{
	struct class *led_class ;	//表示一类设备, 存储某些信息
	struct device *led_device ;	//表示一个设备
	struct cdev  *led_cdev;
	unsigned int val;

};

struct led_device *s5pv_led_dev;

static ssize_t led_read(struct file *file, char __user *buf, size_t count, loff_t *opps)
{
	int ret;

	ret = copy_to_user(buf, &s5pv_led_dev->val, count);
	if(ret>0)
	{
		printk(KERN_ERR "copy to user failed!\n");
		return ret;
	}

	printk(KERN_INFO "val=%d\n", s5pv_led_dev->val);
	
	return ret?0:count;
}
static ssize_t led_write(struct file *file, const char __user *buf, size_t count, loff_t *opps)
{
	int ret;

	/*拷贝成功,返回0; 拷贝失败, 返回没有被拷贝的字节数*/
	ret = copy_from_user(&s5pv_led_dev->val, buf, count);
	if(ret>0)
	{
		printk(KERN_ERR "copy from user failed!\n");
		return ret;
	}

	if(s5pv_led_dev->val)
	{
		/*点亮LED*/
		*gpc0dat |= ((0x1<<3)|(0x1<<4));
	}
	else
	{
		/*熄灭LED*/
		*gpc0dat &= ~((0x1<<3)|(0x1<<4));
	}
	
	return ret?0:count;
}


static int led_open(struct inode *inode, struct file *file)
{

	/*1. 将物理地址映射为虚拟地址*/
	gpc0con = ioremap(0xE0200060, 8);
	gpc0dat = gpc0con +1;
	
	/*2. 初始化GPC0_3,4引脚功能为输出*/
	*gpc0con &= ~((0xf<<12)|(0xf<<16));
	*gpc0con |=  ((0x1<<12)|(0x1<<16));	

	
	return 0;
}

static int led_close(struct inode *inode, struct file *file)
{
	*gpc0con &= ~((0xf<<12)|(0xf<<16));
	iounmap(gpc0con);
}

/*硬件操作方法*/
struct file_operations led_fops={
	.owner = THIS_MODULE,	//当前模块所用
	.open  = led_open,
	.write = led_write,
	.read  = led_read,
	.release = led_close,

};

static void setup_led_cdev(void)
{
	/*1. 为cdev结构体分配空间*/
	s5pv_led_dev->led_cdev = cdev_alloc();

	/*2. 初始化cdev结构体*/
	cdev_init(s5pv_led_dev->led_cdev, &led_fops);

	/*3. 注册cdev,加载到内核哈希表中*/
	cdev_add(s5pv_led_dev->led_cdev, MKDEV(led_major, 0), 1);

}

/*2. 实现模块加载函数*/
static int __init led_init(void)
{
	dev_t devno;
	int ret;
	/*1. 新的申请主设备号的方法*/
	if(led_major)
	{
		/*静态申请*/
		devno = MKDEV(led_major, 0);
		register_chrdev_region(devno, 1, "led");
	}
	else
	{
		/*动态申请*/
		alloc_chrdev_region(&devno, 0, 1, "led");
		led_major = MAJOR(devno);
	}

	/*2. 为本地结构体分配空间*/
		/*
		**	param1: 大小
		**	param2:	标号: GFP_KERNEL--->表示如果分配不成功,则休眠
		*/
	s5pv_led_dev = kmalloc(sizeof(struct led_device), GFP_KERNEL);
	if (!s5pv_led_dev)
	{
		printk(KERN_ERR "NO memory for malloc!\n");
		ret = -ENOMEM;
		goto out_err_1;
	}

	/*3. 构建struct cdev结构体*/
	setup_led_cdev();
			

	/*4. 创建设备文件*/
	/*
	**	param1:	struct class
	**	param2:	父类, 一般为NULL
	**	param3:	dev_t ---> 表示一个设备号, 是一个无符号32位整形
	**						其中高12位为主设备号, 低20为次设备号
	**			如何操作设备号: MKDEV(major, minor)根据主设备号和次设备号组成一个设备号
	*/	

	s5pv_led_dev->led_class = class_create(THIS_MODULE, "led_class");
	if (IS_ERR(s5pv_led_dev->led_class)) {
		printk(KERN_ERR "class_create() failed for led_class\n");
		ret = -EINVAL;
		goto out_err_2;
	}

	
	s5pv_led_dev->led_device = device_create(s5pv_led_dev->led_class, NULL, MKDEV(led_major, 0), NULL, "led1");	// 创建设备文件/dev/led1
	if (IS_ERR(s5pv_led_dev->led_device)) {
		printk(KERN_ERR "device_create failed for led_device\n");
		ret = -ENODEV;
		goto out_err_3;
	}

	return 0;

out_err_3:
	class_destroy(s5pv_led_dev->led_class);

out_err_2:
	cdev_del(s5pv_led_dev->led_cdev);
	kfree(s5pv_led_dev);

out_err_1:
	unregister_chrdev_region(MKDEV(led_major, 0), 1);
	return ret;
	
}

/*3. 实现模块卸载函数*/
static void __exit led_exit(void)
{
	unregister_chrdev_region(MKDEV(led_major, 0), 1);
	device_destroy(s5pv_led_dev->led_class, MKDEV(led_major, 0));
	class_destroy(s5pv_led_dev->led_class);
	cdev_del(s5pv_led_dev->led_cdev);
	kfree(s5pv_led_dev);
}

/*4. 模块许可声明*/
module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");








