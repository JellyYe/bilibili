
#define LOG_TAG "led_jni_log"
#include <utils/Log.h>

#include "jni.h"

#include "led_hal.h"

struct led_hw_module_t *pModule;
struct led_hw_device_t *pDevice;

jint open_led(JNIEnv *env, jobject thiz)
{
	LOGD("-----------%s--------------\n", __FUNCTION__);

	
	//  1, 加载hal动态库
	//参数1---动态库的名字
	//参数2---执行hal中module的指针
	//返回值--正确为0
	jint ret;
	
	ret = hw_get_module(LED_MODULE_ID,(const struct hw_module_t * * )&pModule);
	if(ret == 0)
	{
		
		LOGD(" hw_get_module ok\n");

		// 2, 调用module中open方法
		if(pModule != NULL)
		{
			
			//p->common.methods->open
			//参数1---hal中module指针
			//参数2---区分不同的device---一般填NULL
			//参数3--指向hal中device对象的指针
			pModule->common.methods->open(&pModule->common, NULL,
            								(struct hw_device_t** )&pDevice);

			// 3, 调用device中的各个函数
			if(pDevice != NULL)
			{
				pDevice->open_dev();
			}
		}
		
		
	}else{
		LOGD(" hw_get_module error\n");
		return -1;
	
	}

	

	
	

	return 0;

}


jint led_on(JNIEnv *env, jobject thiz)
{
	LOGD("-----------%s--------------\n", __FUNCTION__);
	
	if(pDevice != NULL)
	{
		pDevice->control_dev(1);
	}
	
	return 0;

}


jint led_off(JNIEnv *env, jobject thiz)
{
	LOGD("-----------%s--------------\n", __FUNCTION__);

	if(pDevice != NULL)
	{
		pDevice->control_dev(0);
	}
	
	return 0;

}

jint close_led(JNIEnv *env, jobject thiz)
{
	LOGD("-----------%s--------------\n", __FUNCTION__);
	
	if(pDevice != NULL)
	{
		pDevice->common.close((struct hw_device_t *)pDevice);
	}

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
	// 将char *字符串转换成 jstring--unicode
	// 参数1---被转化的char *字符串
	return env->NewStringUTF("&_& jni jni jni jni ....");
}

void set_jni_string(JNIEnv *env, jobject thiz, jstring str)
{
	LOGD("-----------%s--------------\n", __FUNCTION__);
	// 将jstring--unicode转化成char *
	// 参数1---被转化的jstring字符串
	// 参数2--获取内部是否拷贝--一般都填NULL
	//返回值就是char *字符串
	const char *tmp;
	tmp = env->GetStringUTFChars(str, NULL);
	LOGD("-----------%s--------------\n", tmp);

	//GetStringUTFChars需要配套使用ReleaseStringUTFChars
	// 参数1---被转化的jstring字符串
	//参数2--返回的char *字符串
	env->ReleaseStringUTFChars(str, tmp);
}


jobject get_jni_girl(JNIEnv *env, jobject thiz)
{

	//对对象进行操作之前一定要获取到id--feild/methodsid

	jclass girlCls = env->FindClass("com/hq/bean/Girl");
	if(girlCls == NULL)
	{
		LOGE("env->FindClass girlCls error\n");
		return NULL;
	}

	//参数1---对象的类信息
	//参数2---属性/方法的名字
	//参数3--属性/方法的描述

	jfieldID nameID = env->GetFieldID(girlCls, "name", "Ljava/lang/String;");
	jfieldID ageID = env->GetFieldID(girlCls, "age", "I");
	jfieldID heightID = env->GetFieldID(girlCls, "height", "D");
	
	// 1, 获取构造方法
	jmethodID constructID = env->GetMethodID(girlCls,"<init>", "()V");
	jmethodID setInfoID = env->GetMethodID(girlCls, "setGirlInformation", "(Ljava/lang/String;I)V");

	// new一个对象
	jobject girlObj = env->NewObject(girlCls, constructID);

	// 初始化对象
	env->SetObjectField(girlObj,nameID, env->NewStringUTF("GeDaye"));
	env->SetIntField(girlObj, ageID, 45);
	env->SetDoubleField(girlObj,heightID, 1.8);

	
	// 演示--如何在jni中回调java的方法
	env->CallVoidMethod(girlObj, setInfoID, env->NewStringUTF("NiDaYe"), 35);

	//返回一个对象
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








