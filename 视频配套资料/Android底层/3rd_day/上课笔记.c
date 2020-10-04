主要内容----JNI编程
1, Android的调试系统---log系统
2, Android系统的启动和启动脚本init.rc
3, Android中JNI编程
	a, java代码调用jni的接口
	b, jni代码的实现
-----------------------------------------------------
1, Android的调试系统---log系统
root@android:/ # ls /dev/log/
	/dev/log/events
	/dev/log/main
	/dev/log/radio
	/dev/log/system

	
	查看调试信息: logcat
			D/dalvikvm( 2215): GREF has increased to 401
			等级/标签tag (pid) : 调试内容
			
		等级:
			V : verbose
			D : debug
			I : info
			W : warning
			E : error
			F : fatal
		标签:　区分调试信息是从哪个进程或者是文件中打印出来的
		
		
		logcat -h : 帮助
		logcat -b : 指定缓冲区
				logcat -b main
				logcat -b system
				logcat -b radio
				logcat -b events
		
		logcat -s  : 过滤
			logcat -s "dalvikvm"
			logcat -s "dalvikvm:W"
		
		logcat -c : 清空
			
		E:\Android底层系统开发\各种问题解决方法\解决ctrl+c---替换一下init		
	

	在代码中如何写调试信息:
		java：　
			String TAG = "HelloActitvity";
			Log.d(TAG, "xxxxxxxxxxxxxx");
				-->
					D/HelloActitvity (pid) : xxxxxxxxxxxxxx
		
		c/c++: LOGD()/LOGE/LOGI/LOGW
		
			a, 头文件
				#define LOG_TAG "Test_log"
				#include <utils/Log.h>

			b,使用LOGD
				     int a = 34;
					int b = 44;
					char *c = "hello heleoel";

					LOGD("This is a simple log test\n");
					LOGI("a + b = %d\n", a+b);
					LOGW("c = %s\n", c);
					LOGE("error error!!!!\n");

			
			c, 编译的时候连接动态库
				LOCAL_PATH:= $(call my-dir)
				include $(CLEAR_VARS)
				
				#LOGD()函数的实现是在liblibcutils.so中实现的
				#LOGD()函数的声明是在utils/Log.h
				#类似与gcc xxx.c  -o xxx  -lpthread
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
		make:进入目录'/home/george/src_210/android4.0-fs210_v2'
		target Executable: hello_arm_elf (out/target/product/fs210/obj/EXECUTABLES/hello_arm_elf_intermediates/LINKED/hello_arm_elf)
		target Symbolic: hello_arm_elf (out/target/product/fs210/symbols/system/bin/hello_arm_elf)
		target Strip: hello_arm_elf (out/target/product/fs210/obj/EXECUTABLES/hello_arm_elf_intermediates/hello_arm_elf)
		Install: out/target/product/fs210/system/bin/hello_arm_elf

			
		
		运行:
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

		
2, Android系统的启动和启动脚本init.rc	
	
	解决问题:
		1, 使用toolbox用起来不方便---可以用busybox进行改进
		2, 每次启动vncserver很麻烦
		
	祖先进程的源码:
		system/core/init/init.c
	init.rc语法:system/core/init/readme.txt
		Actions, Commands, Services, and Options.
		
	Actions: 一个命令的集合:
		Actions take the form of:
			on <trigger>
			   <command>
			   <command>
			   <command>

		例子:
			on early-init
			# Set init and its forked children's oom_adj.
			write /proc/1/oom_score_adj -16

			start ueventd

			# create mountpoints
			mkdir /mnt 0775 root system


	服务:表示一个后台/守护进程
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

		
	1, 使用toolbox用起来不方便---可以用busybox进行改进
		E:\Android底层系统开发\各种问题解决方法\busybox解决Android的命令
		
		tar -xvf busybox.tgz -C /opt/rootfs_dir/system/
		
		vim init.rc
			17	export PATH /system/busybox/bin:/system/busybox/sbin:/sbin:/vendor/bin:/system/sbin:/system/bin:/system/xbin
	
	
			438 #service console /system/bin/sh
			439 service console /system/busybox/bin/ash
			440     console
			441     disabled
			442     group log

	
	2, 每次启动vncserver很麻烦--开机自动启动
		mv androidvncserver system/bin/
		
		vim init.rc
			624 service vncserver /system/bin/androidvncserver
			625         class main
	
		注意:
			每次启动的时候,鼠标不要插上开发板,内核启动之后,再插入开发板	
	
--------------------------------------------------
3, Android中JNI编程----android应用是如何与驱动进行交互
	a, java代码调用jni的接口
	b, jni代码的实现		
		
	
	jni: java native interface,java本地接口
		或者是: java调用c/c++代码的接口
		
		
		
	
---------------------------------------------------------------		
	app中: 与jni交互的接口;
	
	1, 加载动态库
		static{//优先执行
		System.loadLibrary("led_jni"); // /system/lib/libled_jni.so
	}
	
	
	2, 声明本地方法,名字和参数都是自定义
		public native int openDev();
		public native int devOn();
		public native int devOff();
		public native int closeDev();
	
	
	3, 调用本地方法
		LedNative myled;
		myled = new LedNative();
        
        myled.openDev();
	
	-----------------------------------------------------------------
	在Ubuntu中去编写jni: 用sourceinsight
		E:\Android底层系统开发\实验2 jni的编写\si_android404-android.tgz

	jni.cpp	
		
		编写方法:参考模板---development/samples/SimpleJNI/jni
		实现JNI_onLoad()
		
		jint JNI_OnLoad(JavaVM* vm, void* reserved)
		{
			
			//获取到环境变量对象---提供各种操作方法---注册方法
			JNIEnv *env = NULL;
			jint vm->GetEnv(void * * env,jint version)
			
			//构建映射表,注册给dvm
			
			typedef struct {
				const char* name; //java的方法名
				const char* signature; //方法的描述
				void*       fnPtr;//c/c++的函数名
			} JNINativeMethod;
			
			// 参数1-- java方法所在的包路径信息--通过env->FindClass得到
			//参数2---映射表
			//参数3--映射表中的项目个数
			//返回值--错误为负数
			jint env->RegisterNatives(jclass clazz,const JNINativeMethod * methods,jint nMethods)
		}
	
		
	Android.mk
	LOCAL_PATH:= $(call my-dir)
	include $(CLEAR_VARS)

	LOCAL_MODULE_TAGS := optional

	#一定要和System.loadLibrary()名字保持一致
	LOCAL_MODULE:= libled_jni


	LOCAL_SRC_FILES:= led_jni.cpp

	#因为使用了LOGD
	LOCAL_SHARED_LIBRARIES := \
			libutils

	#因为使用了jni.h, LOCAL_C_INCLUDES用于jni.h的所在路径
	LOCAL_C_INCLUDES += \
			$(JNI_H_INCLUDE)


	include $(BUILD_SHARED_LIBRARY
		
		
		
	注意:
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
	make:进入目录'/home/george/src_210/android4.0-fs210_v2'
	target thumb C++: libled_jni <= mytest/led_jni/led_jni.cpp
	target SharedLib: libled_jni (out/target/product/fs210/obj/SHARED_LIBRARIES/libled_jni_intermediates/LINKED/libled_jni.so)
	target Symbolic: libled_jni (out/target/product/fs210/symbols/system/lib/libled_jni.so)
	target Strip: libled_jni (out/target/product/fs210/obj/lib/libled_jni.so)
	Install: out/target/product/fs210/system/lib/libled_jni.so
	
	
	
	---------------------------------------------------
	驱动: ko
		george@George-JI:~/src_210/mydrv/led_drv$ make
		make -C /home/george/src_210/linux-3.0.8-FS210 M=/home/george/src_210/mydrv/led_drv modules
		make[1]: 正在进入目录 `/home/george/src_210/linux-3.0.8-FS210'
		  CC [M]  /home/george/src_210/mydrv/led_drv/s5pv210_led.o
		/home/george/src_210/mydrv/led_drv/s5pv210_led.c: In function 'led_close':
		/home/george/src_210/mydrv/led_drv/s5pv210_led.c:86:1: warning: no return statement in function returning non-void
		  Building modules, stage 2.
		  MODPOST 1 modules
		  CC      /home/george/src_210/mydrv/led_drv/s5pv210_led.mod.o
		  LD [M]  /home/george/src_210/mydrv/led_drv/s5pv210_led.ko

	
===================================================================

运行:
	apk--->安装到开发板
		1,自动安装---/system/lib
		2,手动安装---/mnt/sdcard/
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
	E/led_jni_log( 3010): open error : No such file or directory  // 驱动没有安装
	
	
	解决:
		/ # insmod /system/lib/modules/s5pv210_led.ko





	/ # logcat
	--------- beginning of /dev/log/system
	I/ActivityManager( 2213): START {act=android.intent.action.MAIN cat=[android.intent.category.LAUNCHER] flg=0x10200000 cmp=com.hq.ledcontrol/.LedActivity} from pid 2487
	--------- beginning of /dev/log/main
	D/dalvikvm( 2213): GC_FOR_ALLOC freed 317K, 10% free 9181K/10119K, paused 31ms
	D/led_jni_log( 3010): -----------open_led--------------
	E/led_jni_log( 3010): open error : Permission denied  //权限有问题
	I/ActivityManager( 2213): Displayed com.hq.ledcontrol/.LedActivity: +240ms
	W/IInputConnectionWrapper( 3010): showStatusIcon on inactive InputConnection
	D/OpenGLRenderer( 2487): Flushing caches (mode 1)
	D/OpenGLRenderer( 2487): Flushing caches (mode 0)
	W/InputManagerService( 2213): Starting input on non-focused client com.android.internal.view.IInputMethodClient$Stub$Proxy@4139d230 (uid=10028 pid=2487)
	D/LedActivity( 3010): 亮瞎了狗眼
	D/led_jni_log( 3010): -----------led_on--------------
	E/led_jni_log( 3010): write on error : Bad file number

	解决:
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
	D/LedActivity( 3010): 亮瞎了狗眼
	D/led_jni_log( 3010): -----------led_on--------------
	D/LedActivity( 3010): 好黑,好冷,好寂寞
	D/led_jni_log( 3010): -----------led_off--------------
	D/LedActivity( 3010): 亮瞎了狗眼
	D/led_jni_log( 3010): -----------led_on--------------





	自动装载ko和修改权限: init.rc
		283         insmod /system/lib/modules/s5pv210_led.ko
		284         chmod 777 /dev/led1
		285
		286
		287 on boot
		288 # basic network init







	
	
	
		