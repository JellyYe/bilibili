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


//���һ��ȫ�ֵ��豸������
struct s5pv210_led{
	int dev_major ;
	struct class *cls;
	struct device *dev;
	int value; // ���ڴ���û�������
	struct input_dev *virt_input;
	struct timer_list timer;
};

//����һ������

struct s5pv210_led *led_dev;



volatile unsigned long *gpc0_conf;
volatile unsigned long *gpc0_data;



int led_drv_open(struct inode *inode, struct file *filp)
{

	printk("-------^_^ %s-------\n", __FUNCTION__);

	static int a = 38;
	filp->private_data = &a;
	
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

	led_dev->dev_major  = register_chrdev(0, "led_virt_btn_drv",  &led_fops);
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
	led_dev->dev = device_create(led_dev->cls, NULL,MKDEV(led_dev->dev_major, 0), NULL, "led1");
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



	//����ģ��������ϵͳ
	led_dev->virt_input= input_allocate_device();
	if(led_dev->virt_input == NULL)
	{
		printk("input_allocate_device error\n");
		ret =  -ENOMEM;
		goto err_iounmap;
	}
	//��ʼ��input device����
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
	// ģ��ж�غ�������Ҫ���ϵͳ��Դ���ͷ�
	device_destroy(led_dev->cls, MKDEV(led_dev->dev_major, 0));
	
	class_destroy(led_dev->cls);
	
	//����1---�Ѿ����뵽���豸��
	//����2--�ַ���--�����豸������Ϣ--�Զ���
	unregister_chrdev(led_dev->dev_major, "led_virt_btn_drv");

	kfree(led_dev);

}


module_init(led_drv_init);
module_exit(led_drv_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("wukong@qq.com");






