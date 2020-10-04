#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define LOG_TAG "led_jni_log"
#include <utils/Log.h>

#include "jni.h"

static jint fd;
jint open_led(JNIEnv *env, jobject thiz)
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


jint led_on(JNIEnv *env, jobject thiz)
{
	LOGD("-----------%s--------------\n", __FUNCTION__);
	jint on = 1;
	jint ret;

	ret = write(fd, &on, 4);
	if(ret < 0)
	{
		LOGE("write on error : %s\n", strerror(errno));
		return -1;
	}
	
	
	return 0;

}


jint led_off(JNIEnv *env, jobject thiz)
{
	LOGD("-----------%s--------------\n", __FUNCTION__);

	jint on = 0;
	jint ret;

	ret = write(fd, &on, 4);
	if(ret < 0)
	{
		LOGE("write off error : %s\n", strerror(errno));
		return -1;
	}
	
	return 0;


}

jint close_led(JNIEnv *env, jobject thiz)
{
	LOGD("-----------%s--------------\n", __FUNCTION__);


	close(fd);

	return 0;

	
}




const JNINativeMethod led_jni_methods[] = {
	{"openDev", "()I", (void*)open_led},
	{"devOn", "()I", (void*)led_on},	
	{"devOff", "()I", (void*)led_off},
	{"closeDev", "()I", (void*)close_led},
};



//实现JNI_onLoad()
//JNI_OnLoad返回值－－正确为JNI_VERSION_1_4，错误为负数
jint JNI_OnLoad(JavaVM * vm,void * reserved)
{
	LOGD("-----------%s--------------\n", __FUNCTION__);
	JNIEnv *env = NULL;
	jint ret;
	//获取到环境变量对象---提供各种操作方法---注册方法
	// 参数1--被初始化的env
	//参数2--jni的版本
	//返回值--正确为0,错误为负数
	ret = vm->GetEnv((void * * )&env, JNI_VERSION_1_4);
	if(ret != JNI_OK)
	{
		LOGE("vm->GetEnv error\n");
		return -1;
	}


	//构建映射表,注册给dvm
	
	jclass clz = env->FindClass("com/hq/lowlevel/LedNative");
	if(clz == NULL)
	{
		LOGE("env->FindClass error\n");
		return -1;
	}
	// 参数1-- java方法所在的包路径信息--通过env->FindClass得到
	//参数2---映射表
	//参数3--映射表中的项目个数
	//返回值--错误为负数
	ret = env->RegisterNatives(clz, 
							led_jni_methods, 
							sizeof(led_jni_methods)/sizeof(led_jni_methods[0]));
	if(ret < 0)
	{
		LOGE("env->RegisterNatives error\n");
		return -1;
	}


	return JNI_VERSION_1_4;


}








