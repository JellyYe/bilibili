��Ҫ����----JNI���
1, Android�ĵ���ϵͳ---logϵͳ
2, Androidϵͳ�������������ű�init.rc
3, Android��JNI���
	a, java�������jni�Ľӿ�
	b, jni�����ʵ��
-----------------------------------------------------
1, Android�ĵ���ϵͳ---logϵͳ
root@android:/ # ls /dev/log/
	/dev/log/events
	/dev/log/main
	/dev/log/radio
	/dev/log/system

	
	�鿴������Ϣ: logcat
			D/dalvikvm( 2215): GREF has increased to 401
			�ȼ�/��ǩtag (pid) : ��������
			
		�ȼ�:
			V : verbose
			D : debug
			I : info
			W : warning
			E : error
			F : fatal
		��ǩ:�����ֵ�����Ϣ�Ǵ��ĸ����̻������ļ��д�ӡ������
		
		
		logcat -h : ����
		logcat -b : ָ��������
				logcat -b main
				logcat -b system
				logcat -b radio
				logcat -b events
		
		logcat -s  : ����
			logcat -s "dalvikvm"
			logcat -s "dalvikvm:W"
		
		logcat -c : ���
			
		E:\Android�ײ�ϵͳ����\��������������\���ctrl+c---�滻һ��init		
	

	�ڴ��������д������Ϣ:
		java����
			String TAG = "HelloActitvity";
			Log.d(TAG, "xxxxxxxxxxxxxx");
				-->
					D/HelloActitvity (pid) : xxxxxxxxxxxxxx
		
		c/c++: LOGD()/LOGE/LOGI/LOGW
		
			a, ͷ�ļ�
				#define LOG_TAG "Test_log"
				#include <utils/Log.h>

			b,ʹ��LOGD
				     int a = 34;
					int b = 44;
					char *c = "hello heleoel";

					LOGD("This is a simple log test\n");
					LOGI("a + b = %d\n", a+b);
					LOGW("c = %s\n", c);
					LOGE("error error!!!!\n");

			
			c, �����ʱ�����Ӷ�̬��
				LOCAL_PATH:= $(call my-dir)
				include $(CLEAR_VARS)
				
				#LOGD()������ʵ������liblibcutils.so��ʵ�ֵ�
				#LOGD()��������������utils/Log.h
				#������gcc xxx.c  -o xxx  -lpthread
				LOCAL_SHARED_LIBRARIES := \
						libcutils

				LOCAL_SRC_FILES:= \
						hello.c


				LOCAL_MODULE_TAGS := optional

				LOCAL_MODULE:= hello_arm_elf

				include $(BUILD_EXECUTABLE)
~
		george@George-JI:~/src_210/android4.0-fs210_v2$ mmm mytest/c_test/
		============================================
		PLATFORM_VERSION_CODENAME=REL
		PLATFORM_VERSION=4.0.4
		TARGET_PRODUCT=full_fs210
		TARGET_BUILD_VARIANT=eng
		TARGET_BUILD_TYPE=release
		TARGET_BUILD_APPS=
		TARGET_ARCH=arm
		TARGET_ARCH_VARIANT=armv7-a-neon
		HOST_ARCH=x86
		HOST_OS=linux
		HOST_BUILD_TYPE=release
		BUILD_ID=IMM76I
		============================================
		make:����Ŀ¼'/home/george/src_210/android4.0-fs210_v2'
		target Executable: hello_arm_elf (out/target/product/fs210/obj/EXECUTABLES/hello_arm_elf_intermediates/LINKED/hello_arm_elf)
		target Symbolic: hello_arm_elf (out/target/product/fs210/symbols/system/bin/hello_arm_elf)
		target Strip: hello_arm_elf (out/target/product/fs210/obj/EXECUTABLES/hello_arm_elf_intermediates/hello_arm_elf)
		Install: out/target/product/fs210/system/bin/hello_arm_elf

			
		
		����:
			cp -raf out/target/product/fs210/system/bin/hello_arm_elf  /opt/rootfs_dir
		
			root@android:/ # ./hello_arm_elf
			hello android world
			open: No such file or directory
		
		
		root@android:/ # logcat -s "Test_log"
			--------- beginning of /dev/log/main
			--------- beginning of /dev/log/system
			D/Test_log( 3072): This is a simple log test
			I/Test_log( 3072): a + b = 78
			W/Test_log( 3072): c = hello heleoel
			E/Test_log( 3072): error error!!!!
		
		root@android:/ # logcat -s "Test_log:W"
		--------- beginning of /dev/log/main
		--------- beginning of /dev/log/system
		W/Test_log( 3072): c = hello heleoel
		E/Test_log( 3072): error error!!!!

		
2, Androidϵͳ�������������ű�init.rc	
	
	�������:
		1, ʹ��toolbox������������---������busybox���иĽ�
		2, ÿ������vncserver���鷳
		
	���Ƚ��̵�Դ��:
		system/core/init/init.c
	init.rc�﷨:system/core/init/readme.txt
		Actions, Commands, Services, and Options.
		
	Actions: һ������ļ���:
		Actions take the form of:
			on <trigger>
			   <command>
			   <command>
			   <command>

		����:
			on early-init
			# Set init and its forked children's oom_adj.
			write /proc/1/oom_score_adj -16

			start ueventd

			# create mountpoints
			mkdir /mnt 0775 root system


	����:��ʾһ����̨/�ػ�����
		service <name> <pathname> [ <argument> ]*
		   <option>
		   <option>
		   ...

		    class_start core
			class_start main

		service zygote /system/bin/app_process -Xzygote /system/bin --zygote --start-system-server
			class main
			socket zygote stream 666
			onrestart write /sys/android_power/request_state wake
			onrestart write /sys/power/state on
			onrestart restart media
			onrestart restart netd

		
	1, ʹ��toolbox������������---������busybox���иĽ�
		E:\Android�ײ�ϵͳ����\��������������\busybox���Android������
		
		tar -xvf busybox.tgz -C /opt/rootfs_dir/system/
		
		vim init.rc
			17	export PATH /system/busybox/bin:/system/busybox/sbin:/sbin:/vendor/bin:/system/sbin:/system/bin:/system/xbin
	
	
			438 #service console /system/bin/sh
			439 service console /system/busybox/bin/ash
			440     console
			441     disabled
			442     group log

	
	2, ÿ������vncserver���鷳--�����Զ�����
		mv androidvncserver system/bin/
		
		vim init.rc
			624 service vncserver /system/bin/androidvncserver
			625         class main
	
		ע��:
			ÿ��������ʱ��,��겻Ҫ���Ͽ�����,�ں�����֮��,�ٲ��뿪����	
	
--------------------------------------------------
3, Android��JNI���----androidӦ����������������н���
	a, java�������jni�Ľӿ�
	b, jni�����ʵ��		
		
	
	jni: java native interface,java���ؽӿ�
		������: java����c/c++����Ľӿ�
		
		
		
	
---------------------------------------------------------------		
	app��: ��jni�����Ľӿ�;
	
	1, ���ض�̬��
		static{//����ִ��
		System.loadLibrary("led_jni"); // /system/lib/libled_jni.so
	}
	
	
	2, �������ط���,���ֺͲ��������Զ���
		public native int openDev();
		public native int devOn();
		public native int devOff();
		public native int closeDev();
	
	
	3, ���ñ��ط���
		LedNative myled;
		myled = new LedNative();
        
        myled.openDev();
	
	-----------------------------------------------------------------
	��Ubuntu��ȥ��дjni: ��sourceinsight
		E:\Android�ײ�ϵͳ����\ʵ��2 jni�ı�д\si_android404-android.tgz

	jni.cpp	
		
		��д����:�ο�ģ��---development/samples/SimpleJNI/jni
		ʵ��JNI_onLoad()
		
		jint JNI_OnLoad(JavaVM* vm, void* reserved)
		{
			
			//��ȡ��������������---�ṩ���ֲ�������---ע�᷽��
			JNIEnv *env = NULL;
			jint vm->GetEnv(void * * env,jint version)
			
			//����ӳ���,ע���dvm
			
			typedef struct {
				const char* name; //java�ķ�����
				const char* signature; //����������
				void*       fnPtr;//c/c++�ĺ�����
			} JNINativeMethod;
			
			// ����1-- java�������ڵİ�·����Ϣ--ͨ��env->FindClass�õ�
			//����2---ӳ���
			//����3--ӳ����е���Ŀ����
			//����ֵ--����Ϊ����
			jint env->RegisterNatives(jclass clazz,const JNINativeMethod * methods,jint nMethods)
		}
	
		
	Android.mk
	LOCAL_PATH:= $(call my-dir)
	include $(CLEAR_VARS)

	LOCAL_MODULE_TAGS := optional

	#һ��Ҫ��System.loadLibrary()���ֱ���һ��
	LOCAL_MODULE:= libled_jni


	LOCAL_SRC_FILES:= led_jni.cpp

	#��Ϊʹ����LOGD
	LOCAL_SHARED_LIBRARIES := \
			libutils

	#��Ϊʹ����jni.h, LOCAL_C_INCLUDES����jni.h������·��
	LOCAL_C_INCLUDES += \
			$(JNI_H_INCLUDE)


	include $(BUILD_SHARED_LIBRARY
		
		
		
	ע��:
	george@George-JI:~/src_210/android4.0-fs210_v2$ get_build_var JNI_H_INCLUDE
	dalvik/libnativehelper/include/nativehelper
	george@George-JI:~/src_210/android4.0-fs210_v2$ ls dalvik/libnativehelper/include/nativehelper
	jni.h  JNIHelp.h

	george@George-JI:~/src_210/android4.0-fs210_v2$ mmm mytest/led_jni/
	============================================
	PLATFORM_VERSION_CODENAME=REL
	PLATFORM_VERSION=4.0.4
	TARGET_PRODUCT=full_fs210
	TARGET_BUILD_VARIANT=eng
	TARGET_BUILD_TYPE=release
	TARGET_BUILD_APPS=
	TARGET_ARCH=arm
	TARGET_ARCH_VARIANT=armv7-a-neon
	HOST_ARCH=x86
	HOST_OS=linux
	HOST_BUILD_TYPE=release
	BUILD_ID=IMM76I
	============================================
	make:����Ŀ¼'/home/george/src_210/android4.0-fs210_v2'
	target thumb C++: libled_jni <= mytest/led_jni/led_jni.cpp
	target SharedLib: libled_jni (out/target/product/fs210/obj/SHARED_LIBRARIES/libled_jni_intermediates/LINKED/libled_jni.so)
	target Symbolic: libled_jni (out/target/product/fs210/symbols/system/lib/libled_jni.so)
	target Strip: libled_jni (out/target/product/fs210/obj/lib/libled_jni.so)
	Install: out/target/product/fs210/system/lib/libled_jni.so
	
	
	
	---------------------------------------------------
	����: ko
		george@George-JI:~/src_210/mydrv/led_drv$ make
		make -C /home/george/src_210/linux-3.0.8-FS210 M=/home/george/src_210/mydrv/led_drv modules
		make[1]: ���ڽ���Ŀ¼ `/home/george/src_210/linux-3.0.8-FS210'
		  CC [M]  /home/george/src_210/mydrv/led_drv/s5pv210_led.o
		/home/george/src_210/mydrv/led_drv/s5pv210_led.c: In function 'led_close':
		/home/george/src_210/mydrv/led_drv/s5pv210_led.c:86:1: warning: no return statement in function returning non-void
		  Building modules, stage 2.
		  MODPOST 1 modules
		  CC      /home/george/src_210/mydrv/led_drv/s5pv210_led.mod.o
		  LD [M]  /home/george/src_210/mydrv/led_drv/s5pv210_led.ko

	
===================================================================

����:
	apk--->��װ��������
		1,�Զ���װ---/system/lib
		2,�ֶ���װ---/mnt/sdcard/
				vim  init.fs210.rc
			
				 7     mkdir /mnt/sdcard 0777 system system
				 8     mkdir /mnt/ext_sd 0777 system system
				 9     mkdir /mnt/usb 0777 system system

	
	jni.so --->  /system/lib
		cp -raf out/target/product/fs210/system/lib/libled_jni.so  /opt/rootfs_dir/system/lib
		
	drv.ko --> /system/lib/modules/
		cp -raf s5pv210_led.ko  /opt/rootfs_dir/system/lib/modules/



	logcat -c
	
	logcat
	I/ActivityManager( 2213): START {act=android.intent.action.MAIN cat=[android.intent.category.LAUNCHER] flg=0x10000000 pkg=com.hq.ledcontrol cmp=com.hq.ledcontrol/.LedActivity} from pid 2884
	D/dalvikvm( 3010): Not late-enabling CheckJNI (already on)
	E/AudioHardware( 2139): AudioStreamOutALSA::write END WITH ERROR !!!!!!!!!(0x4667d0, 4096)
	D/dalvikvm( 2213): GC_CONCURRENT freed 424K, 8% free 9263K/9991K, paused 14ms+45ms
	I/ActivityManager( 2213): Start proc com.hq.ledcontrol for activity com.hq.ledcontrol/.LedActivity: pid=3010 uid=10053 gids={}
	D/OpenGLRenderer( 2884): Flushing caches (mode 0)
	D/led_jni_log( 3010): -----------JNI_OnLoad--------------
	D/led_jni_log( 3010): -----------open_led--------------
	E/led_jni_log( 3010): open error : No such file or directory  // ����û�а�װ
	
	
	���:
		/ # insmod /system/lib/modules/s5pv210_led.ko





	/ # logcat
	--------- beginning of /dev/log/system
	I/ActivityManager( 2213): START {act=android.intent.action.MAIN cat=[android.intent.category.LAUNCHER] flg=0x10200000 cmp=com.hq.ledcontrol/.LedActivity} from pid 2487
	--------- beginning of /dev/log/main
	D/dalvikvm( 2213): GC_FOR_ALLOC freed 317K, 10% free 9181K/10119K, paused 31ms
	D/led_jni_log( 3010): -----------open_led--------------
	E/led_jni_log( 3010): open error : Permission denied  //Ȩ��������
	I/ActivityManager( 2213): Displayed com.hq.ledcontrol/.LedActivity: +240ms
	W/IInputConnectionWrapper( 3010): showStatusIcon on inactive InputConnection
	D/OpenGLRenderer( 2487): Flushing caches (mode 1)
	D/OpenGLRenderer( 2487): Flushing caches (mode 0)
	W/InputManagerService( 2213): Starting input on non-focused client com.android.internal.view.IInputMethodClient$Stub$Proxy@4139d230 (uid=10028 pid=2487)
	D/LedActivity( 3010): ��Ϲ�˹���
	D/led_jni_log( 3010): -----------led_on--------------
	E/led_jni_log( 3010): write on error : Bad file number

	���:
		/ # chmod 777 /dev/led1
	
	
	/ # logcat
	--------- beginning of /dev/log/main
	E/dhcpcd  ( 2669): timed out
	--------- beginning of /dev/log/system
	I/ActivityManager( 2213): START {act=android.intent.action.MAIN cat=[android.intent.category.LAUNCHER] flg=0x10200000 cmp=com.hq.ledcontrol/.LedActivity} from pid 2487
	D/dalvikvm( 2213): GC_FOR_ALLOC freed 288K, 10% free 9197K/10119K, paused 42ms
	D/led_jni_log( 3010): -----------open_led--------------
	I/ActivityManager( 2213): Displayed com.hq.ledcontrol/.LedActivity: +220ms
	D/OpenGLRenderer( 2487): Flushing caches (mode 1)
	D/OpenGLRenderer( 2487): Flushing caches (mode 0)
	W/InputManagerService( 2213): Starting input on non-focused client com.android.internal.view.IInputMethodClient$Stub$Proxy@4139d230 (uid=10028 pid=2487)
	D/LedActivity( 3010): ��Ϲ�˹���
	D/led_jni_log( 3010): -----------led_on--------------
	D/LedActivity( 3010): �ú�,����,�ü�į
	D/led_jni_log( 3010): -----------led_off--------------
	D/LedActivity( 3010): ��Ϲ�˹���
	D/led_jni_log( 3010): -----------led_on--------------





	�Զ�װ��ko���޸�Ȩ��: init.rc
		283         insmod /system/lib/modules/s5pv210_led.ko
		284         chmod 777 /dev/led1
		285
		286
		287 on boot
		288 # basic network init







	
	
	
		