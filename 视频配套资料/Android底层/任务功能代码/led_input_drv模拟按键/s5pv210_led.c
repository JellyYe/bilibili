#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/input.h>

#include <asm/io.h>
#include <asm/uaccess.h>

#include <mach/gpio.h>



#define LED_NUM_ON  _IOW('L',0x3456, int)
#define LED_NUM_OFF  _IOW('L',0x3457, int)
#define LED_ALL_ON  _IO('L',0x3458)
#define LED_ALL_OFF  _IO('L',0x3459)

#define VIRT_BTN_VOL_UP _IO('L', 0x3460)
#define VIRT_BTN_VOL_DOWN _IO('L', 0x3461)


//设计一个全局的设备对象类
struct s5pv210_led{
	int dev_major ;
	struct class *cls;
	struct device *dev;
	int value; // 用于存放用户的数据
	struct input_dev *virt_input;
	struct timer_list timer;
};

//声明一个对象

struct s5pv210_led *led_dev;



volatile unsigned long *gpc0_conf;
volatile unsigned long *gpc0_data;



int led_drv_open(struct inode *inode, struct file *filp)
{

	printk("-------^_^ %s-------\n", __FUNCTION__);

	static int a = 38;
	filp->private_data = &a;
	
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

void virt_btn_timer(unsigned long data)
{
	printk("-------^_^ %s-------\n", __FUNCTION__);
	if(data != 3){
		input_event(led_dev->virt_input, EV_KEY, data,  0);
		input_sync(led_dev->virt_input);
	}
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


			case VIRT_BTN_VOL_UP:
				printk("-------^_^ case VIRT_BTN_VOL_UP-------\n");
				input_event(led_dev->virt_input, EV_KEY, KEY_VOLUMEUP,  1);
				input_sync(led_dev->virt_input);

				led_dev->timer.data = KEY_VOLUMEUP;
				mod_timer(&led_dev->timer,  jiffies + 3);
				
				break;
			case VIRT_BTN_VOL_DOWN:
				printk("-------^_^ case VIRT_BTN_VOL_DOWN-------\n");
				input_event(led_dev->virt_input, EV_KEY, KEY_VOLUMEDOWN,  1);
				input_sync(led_dev->virt_input);

				led_dev->timer.data = KEY_VOLUMEDOWN;
				mod_timer(&led_dev->timer,  jiffies + 3);
				
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

	led_dev->dev_major  = register_chrdev(0, "led_virt_btn_drv",  &led_fops);
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
	led_dev->dev = device_create(led_dev->cls, NULL,MKDEV(led_dev->dev_major, 0), NULL, "led1");
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



	//设置模拟输入子系统
	led_dev->virt_input= input_allocate_device();
	if(led_dev->virt_input == NULL)
	{
		printk("input_allocate_device error\n");
		ret =  -ENOMEM;
		goto err_iounmap;
	}
	//初始化input device对象
	led_dev->virt_input->name = "led_virt_input";
	led_dev->virt_input->phys = "virt/input6";
	led_dev->virt_input->uniq = "input/virt";
	led_dev->virt_input->id.bustype = BUS_HOST;
	led_dev->virt_input->id.vendor = 0x3B2B;
	led_dev->virt_input->id.product = 0x0008;
	led_dev->virt_input->id.version = 0x0002;

	
	__set_bit(EV_KEY, led_dev->virt_input->evbit);

	__set_bit(KEY_DOWN, led_dev->virt_input->keybit);
	__set_bit(KEY_UP, led_dev->virt_input->keybit);
	__set_bit(KEY_LEFT, led_dev->virt_input->keybit);
	__set_bit(KEY_RIGHT, led_dev->virt_input->keybit);
	
	__set_bit(KEY_VOLUMEDOWN, led_dev->virt_input->keybit);
	__set_bit(KEY_VOLUMEUP, led_dev->virt_input->keybit);

	
	ret = input_register_device(led_dev->virt_input);
	if(ret != 0)
	{
		printk("input_register_device error\n");
				
		goto err_free_input;
	}

#if 0
	led_dev->timer.data = 3;
	led_dev->timer.function = virt_btn_timer;
	led_dev->timer.expires = jiffies + 3;
	add_timer(&led_dev->timer);
#else
	setup_timer(&led_dev->timer,virt_btn_timer, 3);
	
#endif
	
	return 0;

err_free_input:
	input_free_device(led_dev->virt_input);

err_iounmap:
	iounmap(gpc0_conf);
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


	del_timer(&led_dev->timer);
	
	input_unregister_device(led_dev->virt_input);
	input_free_device(led_dev->virt_input);
	iounmap(gpc0_conf);
	// 模块卸载函数中主要完成系统资源的释放
	device_destroy(led_dev->cls, MKDEV(led_dev->dev_major, 0));
	
	class_destroy(led_dev->cls);
	
	//参数1---已经申请到的设备号
	//参数2--字符串--描述设备驱动信息--自定义
	unregister_chrdev(led_dev->dev_major, "led_virt_btn_drv");

	kfree(led_dev);

}


module_init(led_drv_init);
module_exit(led_drv_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("wukong@qq.com");






