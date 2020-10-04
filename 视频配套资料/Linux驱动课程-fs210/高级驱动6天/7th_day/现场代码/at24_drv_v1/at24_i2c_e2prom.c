

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


const struct file_operations at24_fops = {


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

	at24_dev->dev = device_create(at24_dev->cls, NULL, MKDEV(at24_dev->dev_major, 0),NULL, "at24_e2prom");


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

