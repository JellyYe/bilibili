#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/input.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/mm.h>

#include <asm/io.h>
#include <asm/uaccess.h>


//#define USE_STATIC_MAJOR 1
#define KEY_MAJOR  260


//���һ�����ڴ����ڴ��е����ݶ���
struct mem_data{
	char buf[128];
};

#define KEY_IOC_GET_DATA  0x8808

//���һ���������ݰ�
struct key_event{
	int code; //����������---�¼�,�س���,ESC--KEY_ESC
	int value; //������״̬---���º�̧��--1/0
};


//���һ�����������Ķ���--irqno, gpio, code, name,flags
struct key_desc{
	char *name;
	int irqno;
	int gpio;
	int code;
	unsigned long flags;
};


//���һ��ȫ�ֵ��豸������
struct s5pv210_key{
	dev_t devno;
	int irqno;
	struct cdev *cdev;
	struct class *cls;
	struct device *dev;
	struct key_event event;

	wait_queue_head_t wq_head;
	int have_data; //��ʾһ����־,�Ƿ�������
	void *virt_mem;
	struct tasklet_struct  tasklet;
};


//����һ������

struct s5pv210_key *key_dev;


//����һ���������а����ļ���
struct key_desc key_set[] = {
	[0] = {
		.name = "key1_up",
		.irqno = IRQ_EINT(0),
		.gpio = S5PV210_GPH0(0),
		.code = KEY_UP,
		.flags = IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING,
	},
	[1] = {
		.name = "key2_down",
		.irqno = IRQ_EINT(1),
		.gpio = S5PV210_GPH0(1),
		.code = KEY_DOWN,
		.flags = IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING,
	},

	[2] = {
		.name = "key3_right",
		.irqno = IRQ_EINT(2),
		.gpio = S5PV210_GPH0(2),
		.code = KEY_RIGHT,
		.flags = IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING,
	},

		
	[3] = {
				.name = "key4_left",
				.irqno = IRQ_EINT(3),
				.gpio = S5PV210_GPH0(3),
				.code = KEY_LEFT,
				.flags = IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING,
	},


};


int key_drv_open(struct inode *inode, struct file *filp)
{

	printk("-------^_^ %s-------\n", __FUNCTION__);
	
	memset(&key_dev->event, 0, sizeof(struct key_event));
	key_dev->have_data = 0;
	
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




ssize_t key_drv_read(struct file *filp, char __user *buf, size_t count, loff_t *fpos)
{
	
	int ret;

	//�����������Ƿ�����
	if((filp->f_flags & O_NONBLOCK) && !key_dev->have_data)
	{
		return -EAGAIN;
	}

	//�ж��Ƿ�����Դ
	wait_event_interruptible(key_dev->wq_head,  key_dev->have_data);
	
	//���жϴ�������ȡ�����ݸ��û�
	ret = copy_to_user(buf, &key_dev->event, count);
	if(ret > 0)
	{
		printk("copy_to_user error\n");
		return -EFAULT;
	}
	

	//���event
	memset(&key_dev->event, 0, sizeof(struct key_event));
	// ��������֮���ʾû������
	key_dev->have_data = 0;

	return count;
	
}

unsigned int key_drv_poll(struct file *filp, struct poll_table_struct *pts)
{
	printk("-------^_^ %s-------\n", __FUNCTION__);
	unsigned int mask = 0;
	
	//  1, ����ǰ�ĵȴ�����ͷע�ᵽvfs��
	//����1---struct file����
	//����2---�ȴ�����ͷ
	//����3---��ȴ����й����ı��
	poll_wait(filp, &key_dev->wq_head, pts);

	// 2, ��������ݷ���һ��pollin,û�����ݷ���һ��0
	if(key_dev->have_data)
	{
		mask |= POLLIN;

	}

	return mask;
}


int key_drv_mmap(struct file *filp, struct vm_area_struct *vma)
{

	unsigned long addr;

	// ֻҪ����io_remap_pfn_range���ܽ��������ڴ�ӳ�䵽�û��ռ�
	//vma���������û��ռ�ӳ�������, vma��vfs�㴫�ݹ���

	addr = virt_to_phys(key_dev->virt_mem);

	vma->vm_flags |= VM_IO;
	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	
	//����1---��ʾ����
	//����2---ӳ�䵽�û��ռ����ʼλ��
	//����3--��ӳ��������ַ��ҳ��ַ
	//����4---ӳ��Ĵ�С
	//����5--ӳ���Ȩ��
	if (io_remap_pfn_range(vma, vma->vm_start, addr >> PAGE_SHIFT,
					PAGE_SIZE, vma->vm_page_prot)) {
		printk(KERN_ERR "%s: io_remap_pfn_range failed\n",
			__func__);
		return -EAGAIN;
	}

	return 0;

}


long key_drv_ioctl(struct file *filp, unsigned int cmd, unsigned long args)
{
	//������������ת����һ��ָ��
	void __user *argp = (void __user *)args;
	int ret;
	struct mem_data data;
	
	
	switch(cmd){
		case KEY_IOC_GET_DATA:
			memcpy(data.buf, key_dev->virt_mem, 128);
			ret = copy_to_user(argp, &data, sizeof(struct mem_data));
			if(ret > 0)
				return -EFAULT;
			
			break;


		default:
			printk("unkown cmd\n");

	}


	return 0;

}



const struct file_operations key_fops = {
	.open = key_drv_open,
	.write = key_drv_write,
	.read = key_drv_read,
	.poll = key_drv_poll,
	.release = key_drv_close,
	.unlocked_ioctl = key_drv_ioctl,
	.mmap = key_drv_mmap,
};

//�жϵ��°벿
void key_irq_tasklet(unsigned long data)
{
	printk("-------^_^ %s-------\n", __FUNCTION__);
	//��ʱ��������
	key_dev->have_data = 1;
	//���ѵȴ�����
	wake_up_interruptible(&key_dev->wq_head);

}


//��ʾ��ǰ���жϺ���
irqreturn_t  key_irq_svc(int irqno, void *dev_id)
{
	printk("-------^_^ %s-------\n", __FUNCTION__);

	
	//���ֲ�ͬ���ж�
	struct key_desc *p = (struct key_desc *)dev_id;

	// �����ǰ��»���̧��
		
	int value = gpio_get_value(p->gpio);

	if(value){
		//̧��

		printk("<KERNEL>-------%s up\n", p->name);
		key_dev->event.code = p->code;
		key_dev->event.value = 0;
		
	}else{
		printk("<KERNEL>-------%s pressed\n", p->name);
		key_dev->event.code =  p->code;
		key_dev->event.value = 1;
	}

	
	// �����°벿
	tasklet_schedule(&key_dev->tasklet);


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

	int i;
	int irqno;
	unsigned long flags;
	char *name;
	for(i=0; i<ARRAY_SIZE(key_set); i++)
	{


		irqno = key_set[i].irqno;
		flags = key_set[i].flags;
		name = key_set[i].name;
		
		ret = request_irq(irqno, key_irq_svc, flags, name, &key_set[i]);
		if(ret != 0)
		{
			printk("request_irq error\n");
			ret = -EBUSY;
			goto err_device_destrory;
		}

	}
		
	


	//��ʼ���ȴ�����ͷ
	init_waitqueue_head(&key_dev->wq_head);


	//����һ���ڴ�
	key_dev->virt_mem = kzalloc(PAGE_SIZE, GFP_KERNEL);


	// ��ʼ��tasklet
	tasklet_init(&key_dev->tasklet, key_irq_tasklet, 45);
	
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
	int i;
	
	tasklet_kill(&key_dev->tasklet);
	
	kfree(key_dev->virt_mem);
	
	for(i=0; i<ARRAY_SIZE(key_set); i++)
	{
		free_irq(key_set[i].irqno, &key_set[i]);
	}	
	
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






