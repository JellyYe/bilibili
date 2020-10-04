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


jint add_num(JNIEnv *env, jobject thiz, jint a, jint b)
{
	LOGD("-----------%s--------------\n", __FUNCTION__);
	return a+b;
}


jstring get_jni_string(JNIEnv *env, jobject thiz)
{
	LOGD("-----------%s--------------\n", __FUNCTION__);
	// ��char *�ַ���ת���� jstring--unicode
	// ����1---��ת����char *�ַ���
	return env->NewStringUTF("&_& jni jni jni jni ....");
}

void set_jni_string(JNIEnv *env, jobject thiz, jstring str)
{
	LOGD("-----------%s--------------\n", __FUNCTION__);
	// ��jstring--unicodeת����char *
	// ����1---��ת����jstring�ַ���
	// ����2--��ȡ�ڲ��Ƿ񿽱�--һ�㶼��NULL
	//����ֵ����char *�ַ���
	const char *tmp;
	tmp = env->GetStringUTFChars(str, NULL);
	LOGD("-----------%s--------------\n", tmp);

	//GetStringUTFChars��Ҫ����ʹ��ReleaseStringUTFChars
	// ����1---��ת����jstring�ַ���
	//����2--���ص�char *�ַ���
	env->ReleaseStringUTFChars(str, tmp);
}


jobject get_jni_girl(JNIEnv *env, jobject thiz)
{

	//�Զ�����в���֮ǰһ��Ҫ��ȡ��id--feild/methodsid

	jclass girlCls = env->FindClass("com/hq/bean/Girl");
	if(girlCls == NULL)
	{
		LOGE("env->FindClass girlCls error\n");
		return NULL;
	}

	//����1---���������Ϣ
	//����2---����/����������
	//����3--����/����������

	jfieldID nameID = env->GetFieldID(girlCls, "name", "Ljava/lang/String;");
	jfieldID ageID = env->GetFieldID(girlCls, "age", "I");
	jfieldID heightID = env->GetFieldID(girlCls, "height", "D");
	
	// 1, ��ȡ���췽��
	jmethodID constructID = env->GetMethodID(girlCls,"<init>", "()V");
	jmethodID setInfoID = env->GetMethodID(girlCls, "setGirlInformation", "(Ljava/lang/String;I)V");

	// newһ������
	jobject girlObj = env->NewObject(girlCls, constructID);

	// ��ʼ������
	env->SetObjectField(girlObj,nameID, env->NewStringUTF("GeDaye"));
	env->SetIntField(girlObj, ageID, 45);
	env->SetDoubleField(girlObj,heightID, 1.8);
	

	// ��ʾ--�����jni�лص�java�ķ���
	env->CallVoidMethod(girlObj, setInfoID, env->NewStringUTF("NiDaYe"), 35);

	//����һ������
	return girlObj;

}

void set_jni_girl(JNIEnv *env, jobject thiz, jobject girlObj)
{
	
	//jclass girlCls = env->FindClass("com/hq/bean/Girl");
	jclass girlCls = env->GetObjectClass(girlObj);
	if(girlCls == NULL)
	{
		LOGE("env->FindClass girlCls error\n");
		return;
	}
	
	jfieldID nameID = env->GetFieldID(girlCls, "name", "Ljava/lang/String;");
	jfieldID ageID = env->GetFieldID(girlCls, "age", "I");
	jfieldID heightID = env->GetFieldID(girlCls, "height", "D");
		
	jint age = env->GetIntField(girlObj,ageID);
	jdouble height = env->GetDoubleField(girlObj,heightID);
	
	jstring nameStr = (jstring)env->GetObjectField(girlObj,nameID);

	const char *name;
	name = env->GetStringUTFChars(nameStr, NULL);
	LOGD("----name = %s, age = %d, height = %f\n",name ,age, height);
	env->ReleaseStringUTFChars(nameStr, name);

}

const JNINativeMethod led_jni_methods[] = {
	{"openDev", "()I", (void*)open_led},
	{"devOn", "()I", (void*)led_on},	
	{"devOff", "()I", (void*)led_off},
	{"closeDev", "()I", (void*)close_led},

	{"addNum", "(II)I", (void*)add_num},
	{"getJniString", "()Ljava/lang/String;", (void*)get_jni_string},
	{"setJniString", "(Ljava/lang/String;)V", (void*)set_jni_string},
	{"getJniGirl", "()Lcom/hq/bean/Girl;", (void*)get_jni_girl},
	{"setJniGirl", "(Lcom/hq/bean/Girl;)V", (void*)set_jni_girl},
	
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








