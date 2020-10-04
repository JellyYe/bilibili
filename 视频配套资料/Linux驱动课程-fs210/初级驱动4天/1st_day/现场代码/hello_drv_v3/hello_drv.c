#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>

static int dev_major;


int hello_drv_open(struct inode *inode, struct file *filp)
{

	printk("-------^_^ %s-------\n", __FUNCTION__);
	return 0;

}


const struct file_operations hello_fops = {
	.open = hello_drv_open,

};


static int __init hello_init(void)
{

	// ģ����غ�������Ҫ���ϵͳ��Դ������
	printk("-------^_^ %s-------\n", __FUNCTION__);

	
	// ��Ҫ��һ���豸��----����/ע��
	//����1---ָ��һ������Ҳ������ϵͳ����,��0��ʾ��ϵͳ����
	//����2--�ַ���--�����豸������Ϣ--�Զ���--/proc/devices
	//����3---�ļ���������
	// ����ֵ---�����ϵͳ����,���ط���֮��ĺ���,���򷵻ظ���Ϊ����
	dev_major = register_chrdev(0, "hello_drv",  &hello_fops);
	if(dev_major < 0)
	{
		printk("register_chrdev error\n");
		return -EINVAL;
	}


	
	return 0;
}


static void __exit hello_exit(void)
{

	printk("-------^_^ %s-------\n", __FUNCTION__);

	// ģ��ж�غ�������Ҫ���ϵͳ��Դ���ͷ�
	//����1---�Ѿ����뵽���豸��
	//����2--�ַ���--�����豸������Ϣ--�Զ���
	unregister_chrdev(dev_major, "hello_drv");

}


module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("wukong@qq.com");
MODULE_DESCRIPTION("This is a simplest hello driver");





