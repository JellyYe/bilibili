

#ifndef __LED_HAL_H__
#define __LED_HAL_H__

#include <hardware/hardware.h>


#define LED_MODULE_ID "s5pv210_led_hal"


//需要扩展device对象/module对象
//自定义两个对象

struct led_hw_module_t{
	//继承父类
	struct hw_module_t common; //第一个成员必须是父类,名字一定要做common
	//以下部分--自行扩展
	int serial_id;
};


struct led_hw_device_t{
	struct hw_device_t common;//第一个成员必须是父类,名字一定要做common
	//以下部分--自行扩展
	int (*open_dev)();
	int (*control_dev)(int on);
};


#endif 






