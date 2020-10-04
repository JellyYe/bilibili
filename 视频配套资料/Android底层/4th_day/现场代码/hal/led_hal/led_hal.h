

#ifndef __LED_HAL_H__
#define __LED_HAL_H__

#include <hardware/hardware.h>


#define LED_MODULE_ID "s5pv210_led_hal"


//��Ҫ��չdevice����/module����
//�Զ�����������

struct led_hw_module_t{
	//�̳и���
	struct hw_module_t common; //��һ����Ա�����Ǹ���,����һ��Ҫ��common
	//���²���--������չ
	int serial_id;
};


struct led_hw_device_t{
	struct hw_device_t common;//��һ����Ա�����Ǹ���,����һ��Ҫ��common
	//���²���--������չ
	int (*open_dev)();
	int (*control_dev)(int on);
};


#endif 






