#ifndef __LED_INFO_H__


#define __LED_INFO_H__

#define LED_GPC0_CONF 0xE0200060
#define LED_GPC0_SIZE  8

//设计一个自定义平台数据
struct led_platdata{
	char *name;
	int shift;//移位数
	int conf_reg_data;// 配置寄存器的值
	int conf_reg_clear;//配置寄存器的清空值
	int data_reg;// 数据寄存器的值
};


struct s5pv210_led{
        int dev_major ;
        struct class *cls;
        struct device *dev;
		int value;
		void *reg_base;// gpc0_conf起始位置
		struct led_platdata *pdata;
};


#endif



