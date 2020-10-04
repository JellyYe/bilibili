��Ҫ����: �жϿ���--��������
1,linux��file, cdev, inode֮��Ĺ�ϵ
2,�µ�ע���ַ��豸�ķ�ʽ
3, �ж�����
4, �ļ�ioģ��ʵ��֮�����ͷ�����
-----------------------------------------




struct file����:���������д�openһ���ļ�����Ϣ:�ļ���,��־(�ɶ�д),�ļ�ƫ��
open("/dev/led", O_RDWR|O_CREAT, 0666);
struct file {
	struct path		f_path;
	const struct file_operations	*f_op;
	unsigned int 		f_flags;
	fmode_t			f_mode;
	loff_t			f_pos; //�ļ�ƫ��
	void			*private_data;//����ָ��
	
}


struct cdev����:����һ���ַ��豸������Ϣ(�豸��+�ļ���������),�κ�һ���ַ��豸�������иö���,
struct cdev {
	struct kobject kobj;// ����
	struct module *owner;
	const struct file_operations *ops;//�ļ���������
	struct list_head list;// ����
	dev_t dev; //�豸��
	unsigned int count;
};

struct inode����: �����ļ�ϵͳ�е�ĳ���ļ�������(�ļ�Ȩ��,����,uid,gid,�޸�ʱ���)
struct inode {
	umode_t			i_mode;
	uid_t			i_uid;
	gid_t			i_gid;
	dev_t			i_rdev; //�豸��
	const struct file_operations	*i_fop;
	struct timespec		i_atime;
	struct timespec		i_mtime;
	struct timespec		i_ctime;
}



int led_drv_open(struct inode *inode, struct file *filp)
{

	printk("-------^_^ %s-------\n", __FUNCTION__);

	static int a = 38;
	filp->private_data = &a;
	
	xxxxxxxx
}
long led_drv_ioctl(struct file *filp, unsigned int cmd, unsigned long args)
{

	// ͨ��filp�ҵ�inode
	struct inode *node = filp->f_path.dentry->d_inode;
	//ͨ��inode��ȡ��ע���豸��
	int minor = iminor(node);
	int major = imajor(node);
	//��ȡ������
	int *p = (int *)filp->private_data;
	
	xxxxxxxx
}


2,�µ�ע���ַ��豸�ķ�ʽ
	cdev_alloc(void)
	cdev_init(struct cdev * cdev,const struct file_operations * fops)
	cdev_add(struct cdev * p,dev_t dev,unsigned count)

	
	//��̬�����豸��---�����ǵõ�һ���豸�Ŷ���
	// ����1---�豸��
	// ����2---�豸�ĸ���
	// ����3--�����ַ���--/proc/devices
	//���ظ�������
	dev_t devno = MKDEV(260, 0);
	ret = register_chrdev_region(devno, 1, "key_new_drv");
	if(ret < 0)
	{
		printk(KERN_ERR"register_chrdev_region error\n");
		goto err_free;
	}

	// ��̬����һ��struct cdev ����
	key_dev->cdev = cdev_alloc();
	//��ʼ��cdev��fops
	cdev_init(key_dev->cdev, &key_fops);
	//����ǰcdevע�ᵽϵͳ��ȥ
	//����2---�豸��
	//����3--�豸�ĸ���,һ�㶼��1
	cdev_add(key_dev->cdev, devno, 1);


	
3,�����ж�:
	//  ����1---�жϺ�
	// �жϺŻ�ȡ: IRQ_EINT(1)����ȥ��irqs.h
	//����2--�жϵĴ�����irqreturn_t (*irq_handler_t)(int, void *);
	//����3--�жϵĴ�����ʽ
	/*
		#define IRQF_TRIGGER_NONE	0x00000000 //�ڲ��жϴ���
		#define IRQF_TRIGGER_RISING	0x00000001
		#define IRQF_TRIGGER_FALLING	0x00000002
		#define IRQF_TRIGGER_HIGH	0x00000004
		#define IRQF_TRIGGER_LOW	0x00000008
	*/
	//����3--��ʾһ���ַ���--�Զ���--/proc/interrupts
	// ����4--�������ڶ�������������
	// ��ȷ����0
	request_irq(unsigned int irq,irq_handler_t handler,unsigned long flags,const char * name,void * dev)
	
	
	�жϴ�����:
	irqreturn_t  key_irq_svc(int irqno, void *dev_id)
	{



		return IRQ_HANDLED;

	}

	
	
	
	//����1--�жϺ���
	//����2--��request_irq�����һ����������һ��
	free_irq(int irqno,void * dev_id)


	
	
	
	
ʵ������: Ĭ�����,�󲿷ֵĺ���Ĭ�϶�������
	  scanf()-- fgets()
	  accept(); read/recv/recvfrom
	
	0---��Ҫһ���ȴ�����ͷ
		struct __wait_queue_head wait_queue_head_t;
		init_waitqueue_head(struct wait_queue_head_t *q)
		
		
	// ����1---��ʾ�ȴ�����ͷ
	// ����2---��ʾһ������--���Ϊ��,���ڴ�����,���Ϊ��,�Ͳ�����
	1,�������������ý��̽��뵽����״̬
		wait_event_interruptible(struct wait_queue_head_t wq, int condition)
		
	2, ��Դ�ɴ��ʱ����Ҫ����
		wake_up_interruptible(wait_queue_head_t *q)
	
	
������:
		��Ӧ�����趨������ģʽ:
		int fd = open("/dev/key0", O_RDWR|O_NONBLOCK)
		
		read() �����ݾ͵õ�����,û�����ݾ͵õ�һ��������--EAGAIN;
		-----------------------------------------------------------
		����:
			xxx_read
			{
				�����������Ƿ�����
				if((filp->f_flags & O_NONBLOCK) && !key_dev->have_data)
				{
					return -EAGAIN;
				}

			}
		
		
		
		
		
		
		
		
		
		
		
		
		
	
	
	
	