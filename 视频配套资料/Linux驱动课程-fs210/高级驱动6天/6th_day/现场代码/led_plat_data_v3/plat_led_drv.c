
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/slab.h>

#include <asm/io.h>
#include <asm/uaccess.h>

#include "led_info.h"

#define REG_DATA   4

struct s5pv210_led *led_dev;

int led_drv_open(struct inode *inode, struct file *filp)
{

	printk("-------^_^ %s-------\n", __FUNCTION__);
	//做初始化动作
	int shift = led_dev->pdata->shift * 4;
	int conf_reg_clear = led_dev->pdata->conf_reg_clear;
	int conf_reg_data = led_dev->pdata->conf_reg_data;

	#if 0
	
		unsigned long value = __raw_readl(led_dev->reg_base);
		value &= ~(0xff<<shift);
		value |= (0x11<<shift;
		__raw_writel(value,led_dev->reg_base);
	#else
		unsigned long value = __raw_readl(led_dev->reg_base);
		value &= ~(conf_reg_clear<<shift);
		value |= (conf_reg_data<<shift);
		__raw_writel(value,led_dev->reg_base);
	#endif
		 
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


	int shift = led_dev->pdata->shift ;
	int data_reg = led_dev->pdata->data_reg;
	
	if(led_dev->value)
	{
		//点灯
		writel(readl(led_dev->reg_base + REG_DATA)|(data_reg<<shift),  led_dev->reg_base + REG_DATA);
		
	}else{
		//灭灯
		writel(readl(led_dev->reg_base + REG_DATA)&~(data_reg<<shift),  led_dev->reg_base + REG_DATA);
	
	}


	//返回传递的数据个数
	return count;

}

int led_drv_close(struct inode *inode, struct file *filp)
{

	
	printk("-------^_^ %s-------\n", __FUNCTION__);
	int shift = led_dev->pdata->shift ;
	int data_reg = led_dev->pdata->data_reg;
	
	writel(readl(led_dev->reg_base + REG_DATA)&~(data_reg<<shift),  led_dev->reg_base + REG_DATA);
	
	return 0;

}



const struct file_operations led_fops = {
	.open = led_drv_open,
	.write = led_drv_write,
	.release = led_drv_close,
};

int led_drv_probe(struct platform_device *pdev)
{
	printk("-------%s-----------------\n", __FUNCTION__);
	/*
		编写驱动的套路
		0, 实例化全局的设备对象-- kzalloc
		1,  申请主设备号---register_chrdev
		2, 自动创建设备节点---class_create, device_create
		3, 初始化硬件--ioremap
		4,实现 file_operation

	*/ 
	int ret;
	
	led_dev = kzalloc(sizeof(struct s5pv210_led), GFP_KERNEL);
	if(led_dev == NULL)
	{
		printk(KERN_ERR"kzalloc error\n");
		return -ENOMEM;
	}

	//获取到自定义平台数据
	led_dev->pdata = pdev->dev.platform_data;

	led_dev->dev_major = register_chrdev(0, "led_drv",  &led_fops);	
	if(led_dev->dev_major < 0)
	{
		printk("register_chrdev error\n");
		ret = -EINVAL;
		goto err_free;
	}

	
	led_dev->cls = class_create(THIS_MODULE,"led_cls");
	if(IS_ERR(led_dev->cls))
	{
		printk("class_create error\n");
		ret = PTR_ERR(led_dev->cls);
		goto err_unregister;
	
	}

	
	led_dev->dev = device_create(led_dev->cls, NULL,MKDEV(led_dev->dev_major, 0), NULL, "plat_led");
	if(IS_ERR(led_dev->dev))
	{
		printk("device_create error\n");
		ret = PTR_ERR(led_dev->dev);
		goto err_class_destroy;
	
	}
	// 肯定拿到pdev中的资源对硬件进行初始化
	//获取到内存资源
	//参数3--相同资源中第几个
	struct resource *addr_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if(addr_res == NULL)
	{
		printk("platform_get_resource IORESOURCE_MEM error\n");
		ret = PTR_ERR(addr_res);
		goto err_device_create;
	}
	

	led_dev->reg_base = ioremap(addr_res->start,  resource_size(addr_res));
	if(led_dev->reg_base == NULL)
	{
		printk("ioremap  error\n");
		ret = PTR_ERR(led_dev->reg_base);
		goto err_device_create;
	}

	
	//如果要获取到中断资源--只是用于测试
	//struct resource *irq_res =  platform_get_resource(pdev, IORESOURCE_IRQ, 0);

	//printk("irqno = %d\n", irq_res->start);

	int irqno = platform_get_irq(pdev, 0);
	printk("irqno = %d\n", irqno);


	static int a = 7890;
	platform_set_drvdata(pdev, &a);

	return 0;

err_device_create:
	 device_destroy(led_dev->cls, MKDEV(led_dev->dev_major, 0));

err_class_destroy:
        class_destroy(led_dev->cls);

err_unregister:
        unregister_chrdev(led_dev->dev_major, "led_drv");

err_free:
        kfree(led_dev);
        return ret;


}


int led_drv_remove(struct platform_device *pdev)
{
	printk("-------%s-----------------\n", __FUNCTION__);

	int *p = (int *)platform_get_drvdata(pdev);
	printk(" a = %d\n", *p);
	
	iounmap(led_dev->reg_base);
		
	device_destroy(led_dev->cls, MKDEV(led_dev->dev_major, 0));
	
	class_destroy(led_dev->cls);

 	unregister_chrdev(led_dev->dev_major, "led_drv");

	kfree(led_dev);

	
	return 0;
}
	

const struct platform_device_id led_id_table[] = {
		{"s5pv210_led", 0x5210},
		{"s3c2410_led", 0x4444},
		{"s3c6410_led", 0x4444},
};

struct platform_driver led_pdrv = {
	.probe = led_drv_probe,
	.remove = led_drv_remove,
	.driver = {
		.name = "samsung_led_drv", //随便写,但是一定要有
					// 可以用于和pdev进行匹配的
					//   /sys/bus/platform/drivers/samsung_led_drv/

	},
	.id_table =led_id_table ,//  一定是用于和pdev进行匹配, 优先进行匹配
};


static int __init plat_led_drv_init(void)
{
	printk("-------%s-----------------\n", __FUNCTION__);

	//注册一个pdrv
	return platform_driver_register(&led_pdrv);

}


static void __exit plat_led_drv_exit(void)
{
	printk("-------%s-----------------\n", __FUNCTION__);
	platform_driver_unregister(&led_pdrv);

}


module_init(plat_led_drv_init);
module_exit(plat_led_drv_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("wukong@qq.com");



