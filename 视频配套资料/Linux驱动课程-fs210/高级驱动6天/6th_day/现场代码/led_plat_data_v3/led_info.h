#ifndef __LED_INFO_H__


#define __LED_INFO_H__

#define LED_GPC0_CONF 0xE0200060
#define LED_GPC0_SIZE  8

//���һ���Զ���ƽ̨����
struct led_platdata{
	char *name;
	int shift;//��λ��
	int conf_reg_data;// ���üĴ�����ֵ
	int conf_reg_clear;//���üĴ��������ֵ
	int data_reg;// ���ݼĴ�����ֵ
};


struct s5pv210_led{
        int dev_major ;
        struct class *cls;
        struct device *dev;
		int value;
		void *reg_base;// gpc0_conf��ʼλ��
		struct led_platdata *pdata;
};


#endif



