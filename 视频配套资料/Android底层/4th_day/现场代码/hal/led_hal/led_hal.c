









#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define LOG_TAG "led_hal_log"
#include <utils/Log.h>

#include "led_hal.h"

static int fd;

int led_hal_dev_close(struct hw_device_t* device)
{

	if(device != NULL)
	{
		struct led_hw_device_t *tmp = (struct led_hw_device_t *)device;
		free(tmp);
	}
	
	close(fd);

	return 0;

}


int led_hal_dev_open()
{
	LOGD("-----------%s--------------\n", __FUNCTION__);
	

	fd = open("/dev/led1", O_RDWR);
	if(fd < 0)
	{
		LOGE("open error : %s\n", strerror(errno));
		return -1;
	}

	return 0;

}


int led_hal_control_dev(int on)
{

	LOGD("-----------%s--------------\n", __FUNCTION__);
	int ret;

	ret = write(fd, &on, 4);
	if(ret < 0)
	{
		LOGE("write on error : %s\n", strerror(errno));
		return -1;
	}

	return 0;
}

// 如果给这个函数传递了第三个参数为一个一级指针,该指针就等同于mydev
int led_hal_module_open(const struct hw_module_t* module, const char* id,
		   struct hw_device_t** device)
{

	//实例化device对象
	struct led_hw_device_t *mydev = NULL;
	
	mydev = (struct led_hw_device_t *)malloc(sizeof(struct led_hw_device_t));
	if(mydev == NULL)
	{
		LOGE("malloc for led device error\n");
		return -ENOMEM;
	}

	//初始化
	mydev->common.tag = HARDWARE_DEVICE_TAG;
	mydev->common.module = module;
	mydev->common.version = 1;
	mydev->common.close = led_hal_dev_close;

	mydev->open_dev = led_hal_dev_open;
	mydev->control_dev = led_hal_control_dev;
	

	//将当前的mydev传递给jni
	*device = (struct hw_device_t*)mydev;
	

	return 0;

}

struct hw_module_methods_t led_module_methods = {
	open : led_hal_module_open,

};



//实例化两个对象

//module对象
//p->common.methods->open
struct led_hw_module_t HMI = {//必须叫做HMI
	common : {
		tag : HARDWARE_MODULE_TAG,
		version_major : 1,
		version_minor : 0,
		id : LED_MODULE_ID,
		name : "This is a simple led hal for s5pv210",
		author : "bajie@qq.com",
		methods : &led_module_methods,
	},
	
	serial_id : 0x888,
}; 



