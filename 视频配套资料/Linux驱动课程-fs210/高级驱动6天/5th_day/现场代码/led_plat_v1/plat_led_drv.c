
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
		��д��������·
		0, ʵ����ȫ�ֵ��豸����-- kzalloc
		1,  �������豸��---register_chrdev
		2, �Զ������豸�ڵ�---class_create, device_create
		3, ��ʼ��Ӳ��--ioremap
		4,ʵ�� file_operation

	*/ 


	// �϶��õ�pdev�е���Դ��Ӳ�����г�ʼ��
	//��ȡ���ڴ���Դ
	//����3--��ͬ��Դ�еڼ���
	struct resource *addr_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	gpc0_conf = ioremap(addr_res->start,  resource_size(addr_res));
	gpc0_data = gpc0_conf + 1;


	//���Ҫ��Ӳ�����г�ʼ��
	*gpc0_conf &= ~(0xff<<12);
	*gpc0_conf |= (0x11<<12);

	*gpc0_data |= (0x3<<3); 

	//���Ҫ��ȡ���ж���Դ
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
		.name = "samsung_led_drv", //���д,����һ��Ҫ��
					// �������ں�pdev����ƥ���

	},
	.id_table =led_id_table ,//  һ�������ں�pdev����ƥ��, ���Ƚ���ƥ��
};


static int __init plat_led_drv_init(void)
{
	printk("-------%s-----------------\n", __FUNCTION__);

	//ע��һ��pdrv
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



