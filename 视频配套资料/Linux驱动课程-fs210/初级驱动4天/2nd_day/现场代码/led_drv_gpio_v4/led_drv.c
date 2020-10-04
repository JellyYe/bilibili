#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/gpio.h>

#include <asm/io.h>
#include <asm/uaccess.h>

#include <mach/gpio.h>


#define LED_NUM_ON  _IOW('L',0x3456, int)
#define LED_NUM_OFF  _IOW('L',0x3457, int)
#define LED_ALL_ON  _IO('L',0x3458)
#define LED_ALL_OFF  _IO('L',0x3459)



//设计一个全局的设备对象类
struct s5pv210_led{
	int dev_major ;
	struct class *cls;
	struct device *dev;
	int value; // 用于存放用户的数据
};

//声明一个对象

struct s5pv210_led *led_dev;



volatile unsigned long *gpc0_conf;
volatile unsigned long *gpc0_data;



int led_drv_open(struct inode *inode, struct file *filp)
{

	printk("-------^_^ %s-------\n", __FUNCTION__);
	//做初始化动作
 	*gpc0_conf &= ~(0xff<<12);
		*gpc0_conf |= (0x11<<12);
	
		 
	return 0;

}

// ssize_t write(int fd, const void *buf, size_t count);
ssize_t led_drv_write(struct file *filp, const char __user *buf, size_t count, loff_t *fpos)
{
	int ret;
	
	printk("-------^_^ %s-------\n", __FUNCTION__);
	// 区分应用的需求

	//如果要从用户空间获取数据,需要用特定的函数
	//  从用户空间获取数据, 一般都用在驱动中写操作中-- xxx_write
	// 参数1---目标地址---内核中的空间的地址
	//参数2---原地址---用户空间的地址
	//参数3---拷贝数据个数
	//返回值--没有拷贝成功的数据个数, 0表示成功
	ret  = copy_from_user(&led_dev->value, buf, count);
	if(ret > 0)
	{
		printk(KERN_ERR "copy_from_user error\n");
		return -EFAULT;
	}

	if(led_dev->value)
	{
		//点灯
		*gpc0_data |= (0x3<<3); 
	
	}else{
		//灭灯
		*gpc0_data &= ~(0x3<<3); 
	
	}


	//返回传递的数据个数
	return count;

}

int led_drv_close(struct inode *inode, struct file *filp)
{

	
	printk("-------^_^ %s-------\n", __FUNCTION__);

	//如果要灭掉
	*gpc0_data &= ~(0x3<<3); 
	
	return 0;

}

long led_drv_ioctl(struct file *filp, unsigned int cmd, unsigned long args)
{
	//区分不同的命令
	int num = args;

		switch(cmd)
		{
		
			case LED_NUM_ON :
				gpio_request(S5PV210_GPC0(3), "gpc0_3_led");
				gpio_direction_output(S5PV210_GPC0(3), 1);
				gpio_free(S5PV210_GPC0(3));
				break;
		
			case LED_NUM_OFF:
			 	gpio_request(S5PV210_GPC0(3), "gpc0_3_led");
				gpio_direction_output(S5PV210_GPC0(3), 0);
				gpio_free(S5PV210_GPC0(3));
				break;
					
			case LED_ALL_ON:

				gpio_request(S5PV210_GPC0(3), "gpc0_3_led");
				gpio_direction_output(S5PV210_GPC0(3), 1);
				gpio_free(S5PV210_GPC0(3));

				
				gpio_request(S5PV210_GPC0(4), "gpc0_4_led");
				gpio_direction_output(S5PV210_GPC0(4), 1);
				gpio_free(S5PV210_GPC0(4));
				break;
		
					
			case LED_ALL_OFF:
				 gpio_request(S5PV210_GPC0(3), "gpc0_3_led");
				gpio_direction_output(S5PV210_GPC0(3), 0);
				gpio_free(S5PV210_GPC0(3));
				
				gpio_request(S5PV210_GPC0(4), "gpc0_4_led");
				gpio_direction_output(S5PV210_GPC0(4), 0);
				gpio_free(S5PV210_GPC0(4));
				break;
		
			default : 
				printk("unkown cmd\n");
				return -EINVAL;
			
		}
	
	return 0;
}


const struct file_operations led_fops = {
	.open = led_drv_open,
	.write = led_drv_write,
	.release = led_drv_close,
	.unlocked_ioctl = led_drv_ioctl,
};


static int __init led_drv_init(void)
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






