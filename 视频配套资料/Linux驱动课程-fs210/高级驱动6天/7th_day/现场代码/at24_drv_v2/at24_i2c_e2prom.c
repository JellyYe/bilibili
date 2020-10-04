

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/i2c.h>

#include <asm/io.h>
#include <asm/uaccess.h>




//  ȫ�ֵ��豸����
struct i2c_e2prom{
	int dev_major;
	struct class *cls;
	struct device *dev;
	struct i2c_client *client;//��¼��ǰƥ���client
};

struct i2c_e2prom *at24_dev;

//��дһ������i2c_master_recv/i2c_master_send
int at24_i2c_read(struct i2c_client *client, char *buf, int size)
{
	int ret;
	struct i2c_adapter  *adapter = client->adapter;

	struct i2c_msg msg;
	msg.addr = client->addr;
	msg.flags = I2C_M_RD;
	msg.len = size;
	msg.buf = buf;
	// ����1---������
	//����2--��Ϣ��
	// ����3--��Ϣ�ĸ���
	ret = i2c_transfer(adapter, &msg, 1);


	return ret==1?size:ret;
}


int at24_i2c_write(struct i2c_client *client, char *buf, int size)
{

	int ret;
	struct i2c_adapter  *adapter = client->adapter;

	struct i2c_msg msg;
	msg.addr = client->addr;
	msg.flags = 0;
	msg.len = size;
	msg.buf = buf;
	
	ret = i2c_transfer(adapter, &msg, 1);

	return ret==1?size:ret;

}

int at24_e2prom_drv_open (struct inode *inode, struct file *filp)
{

	return 0;
}

ssize_t at24_e2prom_drv_read(struct file *filp, char __user *buf, size_t count, loff_t *fpos)
{
	printk("-----------%s-----------\n", __FUNCTION__);
	int ret;
	
	if(count < 0 || count > 256)
		return -EINVAL;

	char *tmp = kzalloc(count, GFP_KERNEL);
	

	// 1, ��Ӳ���л�ȡ����
	ret = at24_i2c_read(at24_dev->client, tmp, count);
	if(ret < 0)
	{
		printk("at24_i2c_read error\n");
		goto err_free;
	}

	// 2 ,�����ݸ��û�
	ret = copy_to_user(buf, tmp, count);
	if(ret > 0)
	{
		printk("copy_to_user error\n");
		goto err_free;
	}


	kfree(tmp);
	
	return count;


err_free:
	kfree(tmp);
	return ret;


}

ssize_t at24_e2prom_drv_write(struct file *filp, const char __user *buf, size_t count, loff_t *fpos)
{
	printk("-----------%s-----------\n", __FUNCTION__);
	int ret;
	
	if(count < 0 || count > 256)
		return -EINVAL;

	char *tmp = kzalloc(count, GFP_KERNEL);
	
	// 1, ���û��ռ佫���ݻ�ȡ��
	ret = copy_from_user(tmp, buf, count);
	if(ret > 0)
	{
		printk("copy_from_user error\n");
		goto err_free;
	}
	// 2,  ������д��Ӳ����ȥ
	ret = at24_i2c_write(at24_dev->client, tmp, count);
	if(ret < 0)
	{
		printk("at24_i2c_write error\n");
		goto err_free;
	}


	kfree(tmp);
	
	return count;


err_free:
	kfree(tmp);
	return ret;


}

int at24_e2prom_drv_close(struct inode *inode, struct file *filp)
{

	return 0;
}

const struct file_operations at24_fops = {
	.open = at24_e2prom_drv_open,
	.read = at24_e2prom_drv_read,
	.write = at24_e2prom_drv_write,
	.release = at24_e2prom_drv_close,

};


int at24_drv_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	printk("-----id->name = %s, id->driver_data = 0x%x\n",id->name, id->driver_data);
	/*
			// �����豸��

			// �����豸�ļ�

			// Ӳ����ʼ��

			// ʵ��fops
			

	*/
	at24_dev = kzalloc(sizeof(struct i2c_e2prom), GFP_KERNEL);

	at24_dev->dev_major = register_chrdev(0,"at24_drv", &at24_fops);

	at24_dev->cls = class_create(THIS_MODULE, "at24_cls");

	at24_dev->dev = device_create(at24_dev->cls, NULL,
				MKDEV(at24_dev->dev_major, 0),NULL, "at24_e2prom");

	//��¼��ǰclient
	at24_dev->client = client;
	
	// Ӳ����ʼ�� ---e2promֻҪ�ϵ�Ϳ��Թ���

	// i2cϵͳ��Ϊ���豸�������ݵķ���

	
	return 0;

}


int at24_drv_remove(struct i2c_client *client)
{

	device_destroy(at24_dev->cls, MKDEV(at24_dev->dev_major, 0));
	class_destroy(at24_dev->cls );
	unregister_chrdev(at24_dev->dev_major, "at24_drv");
	kfree(at24_dev);
	return 0;
}
			

const struct i2c_device_id at24_id_table[] = {
		{"at24c02a", 0x2222},
		{"at24c04a", 0x4444},
		{"at24c08a", 0x8888},
};

struct i2c_driver at24_drv = {
	.probe = at24_drv_probe,
	.remove = at24_drv_remove,
	.driver = {
		.name = "at24_e2prom_drv", //�������ڱȶ�
					// /sys/bus/i2c/drivers/at24_e2prom_drv

	},

	.id_table = at24_id_table,
};


static int __init at24_drv_init(void)
{

	//ע��һ��i2c driver
	return i2c_add_driver(&at24_drv);

}

static void __exit at24_drv_exit(void)
{
	 i2c_del_driver(&at24_drv);
}

module_init(at24_drv_init);
module_exit(at24_drv_exit);
MODULE_LICENSE("GPL");

