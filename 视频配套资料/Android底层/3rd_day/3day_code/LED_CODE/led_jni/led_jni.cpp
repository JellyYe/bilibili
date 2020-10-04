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



//ʵ��JNI_onLoad()
//JNI_OnLoad����ֵ������ȷΪJNI_VERSION_1_4������Ϊ����
jint JNI_OnLoad(JavaVM * vm,void * reserved)
{
	LOGD("-----------%s--------------\n", __FUNCTION__);
	JNIEnv *env = NULL;
	jint ret;
	//��ȡ��������������---�ṩ���ֲ�������---ע�᷽��
	// ����1--����ʼ����env
	//����2--jni�İ汾
	//����ֵ--��ȷΪ0,����Ϊ����
	ret = vm->GetEnv((void * * )&env, JNI_VERSION_1_4);
	if(ret != JNI_OK)
	{
		LOGE("vm->GetEnv error\n");
		return -1;
	}


	//����ӳ���,ע���dvm
	
	jclass clz = env->FindClass("com/hq/lowlevel/LedNative");
	if(clz == NULL)
	{
		LOGE("env->FindClass error\n");
		return -1;
	}
	// ����1-- java�������ڵİ�·����Ϣ--ͨ��env->FindClass�õ�
	//����2---ӳ���
	//����3--ӳ����е���Ŀ����
	//����ֵ--����Ϊ����
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








