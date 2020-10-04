#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>


#define LED_GPC0_CONF 0xE0200060
#define LED_GPC0_SIZE  8



struct resource  led_resource[] = {
	[0] = {
		.start = LED_GPC0_CONF,
		.end = LED_GPC0_CONF + LED_GPC0_SIZE -1,
		.flags = IORESOURCE_MEM, 
	},
	// 以下部分--为演示部分
	[1] = {
		.start = 8888,
		.end = 8888,
		.flags = IORESOURCE_IRQ, 
	},

	[2] = {
		.start = 0xE0200160,
		.end = 0xE0200160 + 8 -1,
		.flags = IORESOURCE_MEM, 
	},	

};

struct platform_device led_pdev = {
	.name = "s5pv210_led",
	.id = -1,
	.num_resources = ARRAY_SIZE(led_resource),
	.resource = led_resource,
};

static int __init plat_led_dev_init(void)
{
	printk("-------%s-----------------\n", __FUNCTION__);

	//注册一个pdev
	return platform_device_register(&led_pdev);

}


static void __exit plat_led_dev_exit(void)
{	
	printk("-------%s-----------------\n", __FUNCTION__);
	platform_device_unregister(&led_pdev);

}


module_init(plat_led_dev_init);
module_exit(plat_led_dev_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("wukong@qq.com");


