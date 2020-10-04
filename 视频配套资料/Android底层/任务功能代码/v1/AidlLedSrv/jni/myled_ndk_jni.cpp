

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

#define LOG_TAG "led_jni_srv_log"
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



#ifdef __cplusplus
}
#endif

