

//  产生一个头文件D:\LedControl\src>javah com.hq.lowlevel.LedNative

#include <jni.h>
#include <android/log.h>


#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define LOG_TAG "led_jni_ndk_log"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

static jint fd;

JNIEXPORT jint JNICALL Java_com_hq_lowlevel_LedNative_openDev
  (JNIEnv *env, jobject thiz)
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

JNIEXPORT jint JNICALL Java_com_hq_lowlevel_LedNative_devOn
(JNIEnv *env, jobject thiz)
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
JNIEXPORT jint JNICALL Java_com_hq_lowlevel_LedNative_devOff
(JNIEnv *env, jobject thiz)
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
JNIEXPORT jint JNICALL Java_com_hq_lowlevel_LedNative_closeDev
(JNIEnv *env, jobject thiz)
{
	LOGD("-----------%s--------------\n", __FUNCTION__);


		close(fd);

		return 0;


}

JNIEXPORT jint JNICALL Java_com_hq_lowlevel_LedNative_addNum
(JNIEnv *env, jobject thiz, jint a, jint b)
{
	LOGD("-----------%s--------------\n", __FUNCTION__);
		return a+b;


}
JNIEXPORT jstring JNICALL Java_com_hq_lowlevel_LedNative_getJniString
(JNIEnv *env, jobject thiz)
{

	LOGD("-----------%s--------------\n", __FUNCTION__);
		// 将char *字符串转换成 jstring--unicode
		// 参数1---被转化的char *字符串
		return env->NewStringUTF("&_& jni jni jni jni ....");

}


JNIEXPORT void JNICALL Java_com_hq_lowlevel_LedNative_setJniString
(JNIEnv *env, jobject thiz, jstring str)
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
JNIEXPORT jobject JNICALL Java_com_hq_lowlevel_LedNative_getJniGirl
(JNIEnv *env, jobject thiz)
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
JNIEXPORT void JNICALL Java_com_hq_lowlevel_LedNative_setJniGirl
(JNIEnv *env, jobject thiz, jobject girlObj)
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

#ifdef __cplusplus
}
#endif

