#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>

#include <asm/io.h>


//#define USE_STATIC_MAJOR 1
#define KEY_MAJOR  260


//���һ��ȫ�ֵ��豸������
struct s5pv210_key{
	dev_t devno;
	int irqno;
	struct cdev *cdev;
	struct class *cls;
	struct device *dev;
};

//����һ������

struct s5pv210_key *key_dev;



int key_drv_open(struct inode *inode, struct file *filp)
{

	printk("-------^_^ %s-------\n", __FUNCTION__);
	
	return 0;

}

//  write(fd, buf, size);
ssize_t key_drv_write(struct file *filp, const char __user *buf, size_t count, loff_t *fpos)
{

	
	printk("-------^_^ %s-------\n", __FUNCTION__);
	// ����Ӧ�õ�����

	
	return 0;

}

int key_drv_close(struct inode *inode, struct file *filp)
{

	
	printk("-------^_^ %s-------\n", __FUNCTION__);

	return 0;
}


const struct file_operations key_fops = {
	.open = key_drv_open,
	.write = key_drv_write,
	.release = key_drv_close,
};


//��ʾ��ǰ���жϺ���
irqreturn_t  key_irq_svc(int irqno, void *dev_id)
{
	printk("-------^_^ %s-------\n", __FUNCTION__);

	// �����ǰ��»���̧��
		
	int value = gpio_get_value(S5PV210_GPH0(1));

	if(value){
		//̧��

		printk("key1 up\n");
	}else{
		printk("key1 pressed\n");

	}
	

	return IRQ_HANDLED;

}


static int __init key_drv_init(void)
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
	key_dev = kzalloc(sizeof(struct s5pv210_key), GFP_KERNEL);
	if(key_dev == NULL)
	{
		printk(KERN_ERR"kzalloc error\n");
		return -ENOMEM;
	}
	
	
	// 1,  �������豸��
	#ifdef USE_STATIC_MAJOR
		//��̬�����豸��---�����ǵõ�һ���豸�Ŷ���
		// ����1---�豸��
		// ����2---�豸�ĸ���
		// ����3--�����ַ���--/proc/devices
		//���ظ�������
		key_dev->devno = MKDEV(KEY_MAJOR, 32);
		ret = register_chrdev_region(devno, 1, "key_new_drv");
		if(ret < 0)
		{
			printk(KERN_ERR"register_chrdev_region error\n");
			goto err_free;
		}

	#else
		// ����1---ϵͳ��̬����֮����豸��
		// ����2--���豸�ŵ���ʼֵ
		// ����3---�豸�ĸ���
		// ����4--�����ַ���--/proc/devices
		//��ȷ����0
		ret = alloc_chrdev_region(&key_dev->devno,32, 1, "key_new_drv");
		if(ret != 0)
		{
			printk(KERN_ERR"register_chrdev_region error\n");
			goto err_free;
		}
	#endif

	// ��̬����һ��struct cdev ����
	key_dev->cdev = cdev_alloc();
	//��ʼ��cdev��fops
	cdev_init(key_dev->cdev, &key_fops);
	//����ǰcdevע�ᵽϵͳ��ȥ
	//����2---�豸��
	//����3--�豸�ĸ���,һ�㶼��1
	cdev_add(key_dev->cdev, key_dev->devno, 1);



	// 2 ---�Զ������豸�ڵ�

	//����һ����
	// ����1---��ǰģ��--THIS_MODULE
	// ����2---�ַ���,��ʾ�������
	//����ֵ--struct classָ������
	key_dev->cls = class_create(THIS_MODULE,"key_cls");
	if(IS_ERR(key_dev->cls))
	{
		printk("class_create error\n");
		ret = PTR_ERR(key_dev->cls);
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
	key_dev->dev = device_create(key_dev->cls, NULL,key_dev->devno, NULL, "key%d", 0);
	if(IS_ERR(key_dev->dev))
	{
		printk("device_create error\n");
		ret = PTR_ERR(key_dev->dev);
		goto err_class_destroy;
		
	}

	// 3, ��ʼ��Ӳ��---Ҫôӳ���ַ/�����ж�
	//  ����1---�жϺ�
	// �жϺŻ�ȡ: IRQ_EINT(1)����ȥ��irqs.h

	key_dev->irqno = IRQ_EINT(1);
		
	ret = request_irq(key_dev->irqno, key_irq_svc, IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING,
						 "eint1-keydown", NULL);
	if(ret != 0)
	{
		printk("request_irq error\n");
		ret = -EBUSY;
		goto err_device_destrory;
	}

	
	return 0;


err_device_destrory:
	device_destroy(key_dev->cls, key_dev->devno);

err_class_destroy:
	class_destroy(key_dev->cls);

err_unregister:
	cdev_del(key_dev->cdev);
	unregister_chrdev_region(key_dev->devno, 1);

err_free:
	kfree(key_dev);
	return ret;

}


static void __exit key_drv_exit(void)
{

	printk("-------^_^ %s-------\n", __FUNCTION__);

	//�ͷ��ж�
	//����1--�жϺ���
	//����2--��request_irq�����һ����������һ��
	free_irq(key_dev->irqno, NULL);
	
	// ģ��ж�غ�������Ҫ���ϵͳ��Դ���ͷ�
	device_destroy(key_dev->cls, key_dev->devno);
	
	class_destroy(key_dev->cls);

	cdev_del(key_dev->cdev);
	unregister_chrdev_region(key_dev->devno, 1);
	
	kfree(key_dev);

}


module_init(key_drv_init);
module_exit(key_drv_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("wukong@qq.com");






