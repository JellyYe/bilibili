/*1. ͷ�ļ�*/
#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/device.h>
#include<linux/slab.h>
#include<linux/cdev.h>
#include<asm/uaccess.h>
#include<asm/io.h>

static unsigned int led_major=0;
volatile unsigned long *gpc0con = NULL;
volatile unsigned long *gpc0dat = NULL;

struct led_device{
	struct class *led_class ;	//��ʾһ���豸, �洢ĳЩ��Ϣ
	struct device *led_device ;	//��ʾһ���豸
	struct cdev  *led_cdev;
	unsigned int val;

};

struct led_device *s5pv_led_dev;

static ssize_t led_read(struct file *file, char __user *buf, size_t count, loff_t *opps)
{
	int ret;

	ret = copy_to_user(buf, &s5pv_led_dev->val, count);
	if(ret>0)
	{
		printk(KERN_ERR "copy to user failed!\n");
		return ret;
	}

	printk(KERN_INFO "val=%d\n", s5pv_led_dev->val);
	
	return ret?0:count;
}
static ssize_t led_write(struct file *file, const char __user *buf, size_t count, loff_t *opps)
{
	int ret;

	/*�����ɹ�,����0; ����ʧ��, ����û�б��������ֽ���*/
	ret = copy_from_user(&s5pv_led_dev->val, buf, count);
	if(ret>0)
	{
		printk(KERN_ERR "copy from user failed!\n");
		return ret;
	}

	if(s5pv_led_dev->val)
	{
		/*����LED*/
		*gpc0dat |= ((0x1<<3)|(0x1<<4));
	}
	else
	{
		/*Ϩ��LED*/
		*gpc0dat &= ~((0x1<<3)|(0x1<<4));
	}
	
	return ret?0:count;
}


static int led_open(struct inode *inode, struct file *file)
{

	/*1. �������ַӳ��Ϊ�����ַ*/
	gpc0con = ioremap(0xE0200060, 8);
	gpc0dat = gpc0con +1;
	
	/*2. ��ʼ��GPC0_3,4���Ź���Ϊ���*/
	*gpc0con &= ~((0xf<<12)|(0xf<<16));
	*gpc0con |=  ((0x1<<12)|(0x1<<16));	

	
	return 0;
}

static int led_close(struct inode *inode, struct file *file)
{
	*gpc0con &= ~((0xf<<12)|(0xf<<16));
	iounmap(gpc0con);
}

/*Ӳ����������*/
struct file_operations led_fops={
	.owner = THIS_MODULE,	//��ǰģ������
	.open  = led_open,
	.write = led_write,
	.read  = led_read,
	.release = led_close,

};

static void setup_led_cdev(void)
{
	/*1. Ϊcdev�ṹ�����ռ�*/
	s5pv_led_dev->led_cdev = cdev_alloc();

	/*2. ��ʼ��cdev�ṹ��*/
	cdev_init(s5pv_led_dev->led_cdev, &led_fops);

	/*3. ע��cdev,���ص��ں˹�ϣ����*/
	cdev_add(s5pv_led_dev->led_cdev, MKDEV(led_major, 0), 1);

}

/*2. ʵ��ģ����غ���*/
static int __init led_init(void)
{
	dev_t devno;
	int ret;
	/*1. �µ��������豸�ŵķ���*/
	if(led_major)
	{
		/*��̬����*/
		devno = MKDEV(led_major, 0);
		register_chrdev_region(devno, 1, "led");
	}
	else
	{
		/*��̬����*/
		alloc_chrdev_region(&devno, 0, 1, "led");
		led_major = MAJOR(devno);
	}

	/*2. Ϊ���ؽṹ�����ռ�*/
		/*
		**	param1: ��С
		**	param2:	���: GFP_KERNEL--->��ʾ������䲻�ɹ�,������
		*/
	s5pv_led_dev = kmalloc(sizeof(struct led_device), GFP_KERNEL);
	if (!s5pv_led_dev)
	{
		printk(KERN_ERR "NO memory for malloc!\n");
		ret = -ENOMEM;
		goto out_err_1;
	}

	/*3. ����struct cdev�ṹ��*/
	setup_led_cdev();
			

	/*4. �����豸�ļ�*/
	/*
	**	param1:	struct class
	**	param2:	����, һ��ΪNULL
	**	param3:	dev_t ---> ��ʾһ���豸��, ��һ���޷���32λ����
	**						���и�12λΪ���豸��, ��20Ϊ���豸��
	**			��β����豸��: MKDEV(major, minor)�������豸�źʹ��豸�����һ���豸��
	*/	

	s5pv_led_dev->led_class = class_create(THIS_MODULE, "led_class");
	if (IS_ERR(s5pv_led_dev->led_class)) {
		printk(KERN_ERR "class_create() failed for led_class\n");
		ret = -EINVAL;
		goto out_err_2;
	}

	
	s5pv_led_dev->led_device = device_create(s5pv_led_dev->led_class, NULL, MKDEV(led_major, 0), NULL, "led1");	// �����豸�ļ�/dev/led1
	if (IS_ERR(s5pv_led_dev->led_device)) {
		printk(KERN_ERR "device_create failed for led_device\n");
		ret = -ENODEV;
		goto out_err_3;
	}

	return 0;

out_err_3:
	class_destroy(s5pv_led_dev->led_class);

out_err_2:
	cdev_del(s5pv_led_dev->led_cdev);
	kfree(s5pv_led_dev);

out_err_1:
	unregister_chrdev_region(MKDEV(led_major, 0), 1);
	return ret;
	
}

/*3. ʵ��ģ��ж�غ���*/
static void __exit led_exit(void)
{
	unregister_chrdev_region(MKDEV(led_major, 0), 1);
	device_destroy(s5pv_led_dev->led_class, MKDEV(led_major, 0));
	class_destroy(s5pv_led_dev->led_class);
	cdev_del(s5pv_led_dev->led_cdev);
	kfree(s5pv_led_dev);
}

/*4. ģ���������*/
module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");








