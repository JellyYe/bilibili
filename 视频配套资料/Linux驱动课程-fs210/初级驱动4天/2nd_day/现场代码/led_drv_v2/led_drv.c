#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/slab.h>

#include <asm/io.h>

//可以指定主设备号


//设计一个全局的设备对象类
struct s5pv210_led{
	int dev_major ;
	struct class *cls;
	struct device *dev;
};

//声明一个对象

struct s5pv210_led *led_dev;



volatile unsigned long *gpc0_conf;
volatile unsigned long *gpc0_data;



int led_drv_open(struct inode *inode, struct file *filp)
{

	printk("-------^_^ %s-------\n", __FUNCTION__);
	//做初始化动作
	
	//如果要配置输出
		*gpc0_conf &= ~(0xff<<12);
		*gpc0_conf |= (0x11<<12);
	
	return 0;

}

//  write(fd, buf, size);
ssize_t led_drv_write(struct file *filp, const char __user *buf, size_t count, loff_t *fpos)
{

	
	printk("-------^_^ %s-------\n", __FUNCTION__);
	// 区分应用的需求

	
	return 0;

}

int led_drv_close(struct inode *inode, struct file *filp)
{

	
	printk("-------^_^ %s-------\n", __FUNCTION__);

	//如果要灭掉
	*gpc0_data &= ~(0x3<<3); 
	
	return 0;
}


const struct file_operations led_fops = {
	.open = led_drv_open,
	.write = led_drv_write,
	.release = led_drv_close,
};


static int __init led_drv_init(void)
{

	/*
		编写驱动的套路
		0, 实例化全局的设备对象
		1,  申请主设备号
		2, 自动创建设备节点
		3, 初始化硬件
		4,实现 file_operation

	*/ 
	
	// 模块加载函数中主要完成系统资源的申请
	printk("-------^_^ %s-------\n", __FUNCTION__);
	int ret;

	// 0, 实例化全局的设备对象
	//参数1---分配大小
	//参数2--分配的标志, GFP_KERNEL--如果当前暂时没有内存,会尝试等待
	led_dev = kzalloc(sizeof(struct s5pv210_led), GFP_KERNEL);
	if(led_dev == NULL)
	{
		printk(KERN_ERR"kzalloc error\n");
		return -ENOMEM;
	}
	
	
	// 1,  申请主设备号
	led_dev->dev_major = 250;
	ret = register_chrdev(led_dev->dev_major, "led_drv",  &led_fops);
	if(ret < 0)
	{
		printk("register_chrdev error\n");
		ret = -EINVAL;
		goto err_free;
	}

	// 2 ---自动创建设备节点

	//创建一个类
	// 参数1---当前模块--THIS_MODULE
	// 参数2---字符串,表示类的名字
	//返回值--struct class指针类型
	led_dev->cls = class_create(THIS_MODULE,"led_cls");
	if(IS_ERR(led_dev->cls))
	{
		printk("class_create error\n");
		ret = PTR_ERR(led_dev->cls);
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
	led_dev->dev = device_create(led_dev->cls, NULL,MKDEV(led_dev->dev_major, 0), NULL, "led");
	if(IS_ERR(led_dev->dev))
	{
		printk("device_create error\n");
		ret = PTR_ERR(led_dev->dev);
		goto err_class_destroy;
		
	}

	// 3, 初始化硬件
	//参数1---物理地址
	//参数2--映射的长度
	//返回值--映射之后的虚拟地址
	gpc0_conf = ioremap(0xE0200060, 8);
	gpc0_data = gpc0_conf + 1;


	
	return 0;


err_class_destroy:
	class_destroy(led_dev->cls);

err_unregister:
	unregister_chrdev(led_dev->dev_major, "led_drv");

err_free:
	kfree(led_dev);
	return ret;


}


static void __exit led_drv_exit(void)
{

	printk("-------^_^ %s-------\n", __FUNCTION__);
	
	iounmap(gpc0_conf);
	// 模块卸载函数中主要完成系统资源的释放
	device_destroy(led_dev->cls, MKDEV(led_dev->dev_major, 0));
	
	class_destroy(led_dev->cls);
	
	//参数1---已经申请到的设备号
	//参数2--字符串--描述设备驱动信息--自定义
	unregister_chrdev(led_dev->dev_major, "led_drv");

	kfree(led_dev);

}


module_init(led_drv_init);
module_exit(led_drv_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("wukong@qq.com");






