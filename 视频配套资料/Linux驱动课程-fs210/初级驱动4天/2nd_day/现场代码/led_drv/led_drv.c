#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/slab.h>

//����ָ�����豸��


//���һ��ȫ�ֵ��豸������
struct s5pv210_led{
	int dev_major ;
	struct class *cls;
	struct device *dev;
};

//����һ������
struct s5pv210_led *led_dev;



int led_drv_open(struct inode *inode, struct file *filp)
{

	printk("-------^_^ %s-------\n", __FUNCTION__);
	return 0;

}


const struct file_operations led_fops = {
	.open = led_drv_open,

};


static int __init led_drv_init(void)
{

	/*
		��д��������·
		0, ʵ����ȫ�ֵ��豸����
		1,  �������豸��
		2, �Զ������豸�ڵ�
		3, ��ʼ��Ӳ��
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






