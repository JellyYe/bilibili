#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>

static int dev_major;


int hello_drv_open(struct inode *inode, struct file *filp)
{

	printk("-------^_^ %s-------\n", __FUNCTION__);
	return 0;

}


const struct file_operations hello_fops = {
	.open = hello_drv_open,

};


static int __init hello_init(void)
{

	// 模块加载函数中主要完成系统资源的申请
	printk("-------^_^ %s-------\n", __FUNCTION__);

	
	// 需要有一个设备号----申请/注册
	//参数1---指定一个号码也可以由系统分配,填0表示由系统分配
	//参数2--字符串--描述设备驱动信息--自定义--/proc/devices
	//参数3---文件操作对象
	// 返回值---如果是系统分配,返回分配之后的号码,否则返回负数为错误
	dev_major = register_chrdev(0, "hello_drv",  &hello_fops);
	if(dev_major < 0)
	{
		printk("register_chrdev error\n");
		return -EINVAL;
	}


	
	return 0;
}


static void __exit hello_exit(void)
{

	printk("-------^_^ %s-------\n", __FUNCTION__);

	// 模块卸载函数中主要完成系统资源的释放
	//参数1---已经申请到的设备号
	//参数2--字符串--描述设备驱动信息--自定义
	unregister_chrdev(dev_major, "hello_drv");

}


module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("wukong@qq.com");
MODULE_DESCRIPTION("This is a simplest hello driver");





