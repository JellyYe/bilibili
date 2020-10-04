










#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/delay.h>


#include <asm/io.h>
#include <asm/uaccess.h>

#include <plat/gpio-cfg.h>




#define TS_INT_GPIO  S5PV210_GPH1(6)
#define TS_RESET_GPIO S5PV210_GPD0(3)

#define TS_MAX_WIDTH 800
#define TS_MAX_HEIGHT  480



//���һ��ȫ�ֵ��豸����
struct ts_dev{
	int irqno;
	struct i2c_client *client;
	struct input_dev *ts_input;
};

struct  ts_dev *gt811_dev;



int gt811_i2c_write(struct i2c_client *client, char *buf, int count)
{
	int ret;
	
	struct i2c_msg msg ;

	 msg.addr = client->addr;
	 msg.flags = 0;
	 msg.buf = buf;
	 msg.len = count;

	ret = i2c_transfer(client->adapter, &msg, 1);

	return ret==1?count:ret;

}

int gt811_i2c_read(struct i2c_client *client, char *buf, int count)
{
	int ret;
	
	struct i2c_msg msg ;

	 msg.addr = client->addr;
	 msg.flags = I2C_M_RD;
	 msg.buf = buf;
	 msg.len = count;

	ret = i2c_transfer(client->adapter, &msg, 1);

	return ret==1?count:ret;

} 

// char buf[128]={0x6, 0xa2, };
//  ����: gt811_i2c_read_combine(client, buf, 128)
int gt811_i2c_read_combine(struct i2c_client *client, char *buf, int count)
{
	int ret;
	
	struct i2c_msg msg[2] ;
	//��д�Ĵ����ĵ�ַ---2���ֽ�
	 msg[0].addr = client->addr;
	 msg[0].flags = 0;
	 msg[0].buf = &buf[0];
	 msg[0].len = 2;


	 msg[1].addr = client->addr;
	 msg[1].flags = I2C_M_RD;
	 msg[1].buf = &buf[2];
	 msg[1].len = count-2;

	ret = i2c_transfer(client->adapter, msg, 2);

	return ret==2?count-2:ret;

} 


static int gt811_tp_init(void)
{
	int ret =-1;

	uint8_t config_info[] = {
		0x06,0xA2,   //�Ĵ�����ַ
		
		0x12,0x10,0x0E,0x0C,0x0A,0x08,0x06,0x04,0x02,0x00,0x05,0x55,0x15,0x55,0x25,0x55,\
		0x35,0x55,0x45,0x55,0x55,0x55,0x65,0x55,0x75,0x55,0x85,0x55,0x95,0x55,0xA5,0x55,\
		0xB5,0x55,0xC5,0x55,0xD5,0x55,0xE5,0x55,0xF5,0x55,0x1B,0x03,0x00,0x00,0x00,0x13,\
		0x13,0x13,0x0F,0x0F,0x0A,0x50,0x30,0x0D,0x03,0x00,0x05,0x58,0x02,0x00,0x04,0x00,\
		0x00,0x32,0x2C,0x34,0x2E,0x00,0x00,0x04,0x14,0x22,0x04,0x00,0x00,0x00,0x00,0x00,\
		0x20,0x14,0xEC,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x30,\
		0x25,0x28,0x14,0x00,0x00,0x00,0x00,0x00,0x00,0x01\
	};

	/*�������Ĵ�С*/
	printk("62:%x\n",config_info[62]);
	printk("61:%x\n",config_info[61]);
	printk("64:%x\n",config_info[64]);
	printk("63:%x\n",config_info[63]);
	
	config_info[62] = TS_MAX_WIDTH >> 8;
    config_info[61] = TS_MAX_WIDTH & 0xff;
    config_info[64] = TS_MAX_HEIGHT>> 8;
    config_info[63] = TS_MAX_HEIGHT & 0xff;

	ret = gt811_i2c_write(gt811_dev->client, config_info, ARRAY_SIZE(config_info));
	if(ret < 0)
	{
		printk(KERN_ERR "GT811 Send config failed!\n");
		return ret;
	}


	return 0;
	
}


irqreturn_t  gt811_irq_svc(int irqno, void *dev_id)
{
	printk("-----------------%s---------------\n", __FUNCTION__);

	return IRQ_HANDLED;

}



int gt811_ts_drv_probe(struct i2c_client *client, const struct i2c_device_id *id)
{		

	printk("-----------------%s---------------\n", __FUNCTION__);
	int ret;

	//  1,  ʵ����ȫ���豸����
	gt811_dev = kzalloc(sizeof(struct ts_dev), GFP_KERNEL);

	//  2, ��¼��ǰ��client
	gt811_dev->client = client;

	// 3, Ӳ����ʼ��-- i2c_tranfer + �жϳ�ʼ��
	/*
			1,  ����INT����Ϊ����̬, RESET���óɸߵ�ƽ(�ڲ�����)
			2,  RESET���Ϊ��, ��ʱ1ms,ת������̬
			3,��ʱ����20ms,ͨ��i2cѰַ�ж��Ƿ�����Ӧ
			4, �������Ӧ�ͷ�һ�λ��γ�ʼ������106���Ĵ���

	*/
	gpio_request(TS_INT_GPIO, "TS_INT");
	gpio_direction_input(TS_INT_GPIO);
	gpio_free(TS_INT_GPIO);

	gpio_request(TS_RESET_GPIO, "reset_gpio");
	s3c_gpio_setpull(TS_RESET_GPIO, S3C_GPIO_PULL_UP);
	gpio_free(TS_RESET_GPIO);
	
	//��΢��ʱһ��
	msleep(5);
	gpio_request(TS_RESET_GPIO, "reset_gpio");
	gpio_direction_output(TS_RESET_GPIO, 0);
	msleep(1);
	gpio_direction_input(TS_RESET_GPIO);
	gpio_free(TS_RESET_GPIO);

	msleep(50);	
	//�����Ƿ�������
	char value = 1;
	ret = i2c_master_send(gt811_dev->client, &value, 1);
	if(ret > 0)
	{
		printk("gt811 device found ok\n");
	}else{
		printk("gt811 device not found \n");
		kfree(gt811_dev);
		return -ENODEV;
	}
	// ��ʼ������106���Ĵ���--��ֵд�뵽�Ĵ�����
	gt811_tp_init();
	
	//�����ж�
	gt811_dev->irqno = IRQ_EINT(14);
	ret = request_irq(gt811_dev->irqno, gt811_irq_svc, IRQF_TRIGGER_FALLING, "ts_irq", NULL);
	if(ret != 0)
	{
		printk("request_irq error \n");
		kfree(gt811_dev);
		return ret;
	}

	//4, ����input  device
	gt811_dev->ts_input = input_allocate_device();

	//  5,��ʼ��input  device
	//�ṩ�������Ϣ���û��鿴--/sys/class/input/eventX/device
	gt811_dev->ts_input->name = "gt811_ts";
	gt811_dev->ts_input->phys = "ts/input0";
	gt811_dev->ts_input->uniq = "gt811/ts";
	gt811_dev->ts_input->id.bustype = BUS_I2C;
	gt811_dev->ts_input->id.vendor = 0x2B2B;
	gt811_dev->ts_input->id.product = 0x0008;
	gt811_dev->ts_input->id.version = 0x0002;
	//�ܹ�������Щ��������--��������
	//gt811_dev->ts_input->evbit[0] |= BIT_MASK(EV_ABS);
	__set_bit(EV_ABS, gt811_dev->ts_input->evbit);


	//������Щ��������
	__set_bit(ABS_X, gt811_dev->ts_input->absbit);
	__set_bit(ABS_Y, gt811_dev->ts_input->absbit);
	__set_bit(ABS_PRESSURE, gt811_dev->ts_input->absbit);

	//����ֵ����������С��
	/*
		
		����1�� input�豸	
		����2�� ���귽��	
		����3�� ��Сֵ	
		����4�����ֵ	
		����5����ʾ����Ƕ��٣�һ�����ֱ�����ó�0	
		����6�� ��ʾ����λ�ã�һ�㶼������Ϊ0	
	*/
	input_set_abs_params(gt811_dev->ts_input, ABS_X, 0, 800, 0, 0);
	input_set_abs_params(gt811_dev->ts_input, ABS_Y, 0, 480, 0, 0);
	input_set_abs_params(gt811_dev->ts_input, ABS_PRESSURE, 0, 256, 0, 0);
	

	// 6,  ע��input device
	ret = input_register_device(gt811_dev->ts_input);
	
	
	return 0;
}


int gt811_ts_drv_remove(struct i2c_client *client)
{
	printk("-----------------%s---------------\n", __FUNCTION__);
	free_irq(gt811_dev->irqno, NULL);
	input_unregister_device(gt811_dev->ts_input);
	input_free_device(gt811_dev->ts_input);
	kfree(gt811_dev);	
	return 0;
}

const struct i2c_device_id gt_id_table[] = {
		{"gt811_ts", 0x811},
		{"gt911_ts", 0x911},

};

struct i2c_driver gt811_drv = {
	.probe = gt811_ts_drv_probe,
	.remove = gt811_ts_drv_remove,
	.driver = {
		.name = "gt811_ts_drv", //�������ڱȶ�
				
	},

	.id_table = gt_id_table,
};


static int __init gt811_drv_init(void)
{

	//ע��һ��i2c driver
	return i2c_add_driver(&gt811_drv);
	

}

static void __exit gt811_drv_exit(void)
{
	 i2c_del_driver(&gt811_drv);
}

module_init(gt811_drv_init);
module_exit(gt811_drv_exit);
MODULE_LICENSE("GPL");










