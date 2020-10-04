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



//���һ��ȫ�ֵ��豸������
struct s5pv210_led{
	int dev_major ;
	struct class *cls;
	struct device *dev;
	int value; // ���ڴ���û�������
};

//����һ������

struct s5pv210_led *led_dev;



volatile unsigned long *gpc0_conf;
volatile unsigned long *gpc0_data;



int led_drv_open(struct inode *inode, struct file *filp)
{

	printk("-------^_^ %s-------\n", __FUNCTION__);
	//����ʼ������
 	*gpc0_conf &= ~(0xff<<12);
		*gpc0_conf |= (0x11<<12);
	
		 
	return 0;

}

// ssize_t write(int fd, const void *buf, size_t count);
ssize_t led_drv_write(struct file *filp, const char __user *buf, size_t count, loff_t *fpos)
{
	int ret;
	
	printk("-------^_^ %s-------\n", __FUNCTION__);
	// ����Ӧ�õ�����

	//���Ҫ���û��ռ��ȡ����,��Ҫ���ض��ĺ���
	//  ���û��ռ��ȡ����, һ�㶼����������д������-- xxx_write
	// ����1---Ŀ���ַ---�ں��еĿռ�ĵ�ַ
	//����2---ԭ��ַ---�û��ռ�ĵ�ַ
	//����3---�������ݸ���
	//����ֵ--û�п����ɹ������ݸ���, 0��ʾ�ɹ�
	ret  = copy_from_user(&led_dev->value, buf, count);
	if(ret > 0)
	{
		printk(KERN_ERR "copy_from_user error\n");
		return -EFAULT;
	}

	if(led_dev->value)
	{
		//���
		*gpc0_data |= (0x3<<3); 
	
	}else{
		//���
		*gpc0_data &= ~(0x3<<3); 
	
	}


	//���ش��ݵ����ݸ���
	return count;

}

int led_drv_close(struct inode *inode, struct file *filp)
{

	
	printk("-------^_^ %s-------\n", __FUNCTION__);

	//���Ҫ���
	*gpc0_data &= ~(0x3<<3); 
	
	return 0;

}

long led_drv_ioctl(struct file *filp, unsigned int cmd, unsigned long args)
{
	//���ֲ�ͬ������
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
		��д��������·
		0, ʵ����ȫ�ֵ��豸����-- kzalloc
		1,  �������豸��---register_chrdev
		2, �Զ������豸�ڵ�---class_create, device_create
		3, ��ʼ��Ӳ��--ioremap
		4,ʵ�� file_operation

	*/ 
	
	// ģ����غ�������Ҫ���ϵͳ��Դ������
	printk("-------^_^ %s-------\n", __FUNCTION__);
	int ret;

	// 0, ʵ����ȫ�ֵ��豸����
	//����1---�����С
	//����2--����ı�־, GFP_KERNEL--�����ǰ��ʱû���ڴ�,�᳢�Եȴ�
	led_dev = kzalloc(sizeof(struct s5pv210_led), GFP_KERNEL);
	if(led_dev == NULL)
	{
		printk(KERN_ERR"kzalloc error\n");
		return -ENOMEM;
	}
	
	
	// 1,  �������豸��
	led_dev->dev_major = 250;
	ret = register_chrdev(led_dev->dev_major, "led_drv",  &led_fops);
	if(ret < 0)
	{
		printk("register_chrdev error\n");
		ret = -EINVAL;
		goto err_free;
	}

	// 2 ---�Զ������豸�ڵ�

	//����һ����
	// ����1---��ǰģ��--THIS_MODULE
	// ����2---�ַ���,��ʾ�������
	//����ֵ--struct classָ������
	led_dev->cls = class_create(THIS_MODULE,"led_cls");
	if(IS_ERR(led_dev->cls))
	{
		printk("class_create error\n");
		ret = PTR_ERR(led_dev->cls);
		goto err_unregister;
		
	}

	//����һ���豸�ڵ�
	// ����1---class_create���ص�ָ��
	// ����2---���豸�Ǹ���--һ�㶼����NULL
	//����3--�豸��--���������豸��major�ʹ��豸��minor 
	//����4---˽������ָ��---һ�㶼����NULL
	//����5---�豸�ڵ������
	//���  /dev/led
	// ����ֵ--struct deviceָ��
	led_dev->dev = device_create(led_dev->cls, NULL,MKDEV(led_dev->dev_major, 0), NULL, "led");
	if(IS_ERR(led_dev->dev))
	{
		printk("device_create error\n");
		ret = PTR_ERR(led_dev->dev);
		goto err_class_destroy;
		
	}

	// 3, ��ʼ��Ӳ��
	//����1---�����ַ
	//����2--ӳ��ĳ���
	//����ֵ--ӳ��֮��������ַ
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
	// ģ��ж�غ�������Ҫ���ϵͳ��Դ���ͷ�
	device_destroy(led_dev->cls, MKDEV(led_dev->dev_major, 0));
	
	class_destroy(led_dev->cls);
	
	//����1---�Ѿ����뵽���豸��
	//����2--�ַ���--�����豸������Ϣ--�Զ���
	unregister_chrdev(led_dev->dev_major, "led_drv");

	kfree(led_dev);

}


module_init(led_drv_init);
module_exit(led_drv_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("wukong@qq.com");






