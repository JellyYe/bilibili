
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>

#include <asm/io.h>


volatile unsigned long *gpc0_conf;
volatile unsigned long *gpc0_data;

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


	// 肯定拿到pdev中的资源对硬件进行初始化
	//获取到内存资源
	//参数3--相同资源中第几个
	struct resource *addr_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	gpc0_conf = ioremap(addr_res->start,  resource_size(addr_res));
	gpc0_data = gpc0_conf + 1;


	//如果要对硬件进行初始化
	*gpc0_conf &= ~(0xff<<12);
	*gpc0_conf |= (0x11<<12);

	*gpc0_data |= (0x3<<3); 

	//如果要获取到中断资源
	//struct resource *irq_res =  platform_get_resource(pdev, IORESOURCE_IRQ, 0);

	//printk("irqno = %d\n", irq_res->start);

	int irqno = platform_get_irq(pdev, 0);
	printk("irqno = %d\n", irqno);


	return 0;

}


int led_drv_remove(struct platform_device *pdev)
{
	printk("-------%s-----------------\n", __FUNCTION__);
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



