��Ҫ����---HAL���
1,JNI����֮�߼��÷�--���ݴ���/��������
2, HAL���	
	a, hal������
	b, hal��ʵ��ԭ��ͷ���
	c, jni�е���hal�Ľӿ�
	d, jni��hal֮��Ĺ�ϵ
----------------------------------------------
1,����uboot
	tar -xvf uboot-fs210_V5.tar.bz2
		a,�޸Ľ��湤����
			 150 CROSS_COMPILE = /opt/toolchain/toolchain-4.3.2-farsight/bin/arm-none-linux-gnueabi-
		b,ѡ��һ��board������
			make fs210_nand_config
				Configuring for fs210_nand board...
				
				
				Makefile:
					fs210_nand_config :     unconfig
						@$(MKCONFIG) $(@:_config=) arm s5pc11x fs210 samsung s5pc110
						@echo "TEXT_BASE = 0x23e00000" > $(obj)board/samsung/fs210/config.mk
				
				
				@$(MKCONFIG) $(@:_config=) arm s5pc11x fs210 samsung s5pc110
				==>
				./mkconfig	fs210_nand arm s5pc11x fs210 samsung s5pc110
				
				
					$(@:_config=) ==>fs210_nand_config:_config=   ==> fs210_nand	
					$(@:_config=_abc) ==>fs210_nand_config:_config=_abc   ==> fs210_nand_abc
					
					
				./mkconfig	fs210_nand arm s5pc11x fs210 samsung s5pc110
-----------------------------------------------------------------------------
1,JNI����֮�߼��÷�--���ݴ���/��������

	app:
		//�������ݵĽӿ�
		// ������������
		public native int addNum(int a, int b);
		
		//�ַ���
		public native String getJniString();
		public native void setJniString(String str);
		
		//��/����
		public native Girl getJniGirl();
		public native void setJniGirl(Girl g);

		
		����:
			//��ȡ����
			int result = myled.addNum(30, 8);
			String str = myled.getJniString();
			Girl g = myled.getJniGirl();
			
			//��ʾ
			textView1.setText("a + b =" + result + "\n"
					+"str = " + str + "\n"
					+"name = " + g.getName() + "\n"
					+"age = " + g.getAge() + "\n"
					+"heigth = " + g.getHeight() + "\n"
					);
			
			//��������
			myled.setJniString("^_^ hello from java");
			
			Girl g2 = new Girl();
			g2.setName("marong");
			g2.setAge(30);
			g2.setHeight(1.2);
			myled.setJniGirl(g2);



	��jni��,��ȡ��д:
		D:\LedControl\bin\classes>javap -s com.hq.lowlevel.LedNative
		Compiled from "LedNative.java"
		public class com.hq.lowlevel.LedNative {
		  static {};
			descriptor: ()V

		  public com.hq.lowlevel.LedNative();
			descriptor: ()V

		  public native int openDev();
			descriptor: ()I

		  public native int devOn();
			descriptor: ()I

		  public native int devOff();
			descriptor: ()I

		  public native int closeDev();
			descriptor: ()I

		  public native int addNum(int, int);
			descriptor: (II)I

		  public native java.lang.String getJniString();
			descriptor: ()Ljava/lang/String;

		  public native void setJniString(java.lang.String);
			descriptor: (Ljava/lang/String;)V

		  public native com.hq.bean.Girl getJniGirl();
			descriptor: ()Lcom/hq/bean/Girl;

		  public native void setJniGirl(com.hq.bean.Girl);
			descriptor: (Lcom/hq/bean/Girl;)V
		}


	1,���ӳ���
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
	2, ʵ�ָ����ӿ�
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


����:
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



����:
	apk---����
		
	jni.so -->����
		cp -raf out/target/product/fs210/system/lib/libled_jni.so  /opt/rootfs_dir/system/lib
	ko--> ����




/ # logcat -c
/ # logcat
--------- beginning of /dev/log/system
I/ActivityManager( 2225): START {act=android.intent.action.MAIN cat=[android.intent.category.LAUNCHER] flg=0x10000000 pkg=com.hq.ledcontrol cmp=com.hq.ledcontrol/.LedActivity} from pid 2834
--------- beginning of /dev/log/main
D/dalvikvm( 2955): Not late-enabling CheckJNI (already on)
I/ActivityManager( 2225): Start proc com.hq.ledcontrol for activity com.hq.ledcontrol/.LedActivity: pid=2955 uid=10053 gids={}
D/OpenGLRenderer( 2834): Flushing caches (mode 0)
D/led_jni_log( 2955): -----------JNI_OnLoad--------------
D/led_jni_log( 2955): -----------open_led--------------
D/led_jni_log( 2955): -----------add_num--------------
D/led_jni_log( 2955): -----------get_jni_string--------------
D/led_jni_log( 2955): -----------set_jni_string--------------
D/led_jni_log( 2955): -----------^_^ hello from java--------------
D/led_jni_log( 2955): ----name = marong, age = 30, height = 1.200000
D/libEGL  ( 2955): loaded /system/lib/egl/libGLES_android.so
D/libEGL  ( 2955): loaded /vendor/lib/egl/libEGL_POWERVR_SGX540_120.so
D/libEGL  ( 2955): loaded /vendor/lib/egl/libGLESv1_CM_POWERVR_SGX540_120.so
D/libEGL  ( 2955): loaded /vendor/lib/egl/libGLESv2_POWERVR_SGX540_120.so
I/Process ( 2225): Sending signal. PID: 2955 SIG: 3
I/dalvikvm( 2955): threadid=3: reacting to signal 3
I/dalvikvm( 2955): Wrote stack traces to '/data/anr/traces.txt'
D/OpenGLRenderer( 2955): Enabling debug mode 0
I/ActivityManager( 2225): Displayed com.hq.ledcontrol/.LedActivity: +897ms
I/ActivityManager( 2225): No longer want android.process.media (pid 2440): hidden #16
D/OpenGLRenderer( 2834): Flushing caches (mode 1)
W/ThrottleService( 2225): unable to find stats for iface rmnet0
E/dhcpcd  ( 2649): timed out
D/LedActivity( 2955): ��Ϲ�˹���
D/led_jni_log( 2955): -----------led_on--------------

=================================================================
Hal : hardware abstract layer Ӳ�������
	1,������Ӳ�����컯�����װ�ڴ�
	2, ���GPL������,����Ӳ�����̵ĺ��Ĵ���


	ģ��:
		hardware/libhardware/modules/gralloc
	
	
	module����:����hal.so����Ϣ
	typedef struct hw_module_t {
		/** tag must be initialized to HARDWARE_MODULE_TAG */
		uint32_t tag; // ��ǩ--�����ʼ��Ϊ HARDWARE_MODULE_TAG

		/** major version number for the module */
		uint16_t version_major; // ���汾---һ�㶼��1

		/** minor version number of the module */
		uint16_t version_minor;// �ΰ汾---һ�㶼��0

		/** Identifier of module */
		const char *id; //��Ҫ---��ʾso�ļ�������

		/** Name of this module */
		const char *name; // ������̬��Ľ������--�Զ���

		/** Author/owner/implementor of the module */
		const char *author;//����--�Զ���

		/** Modules methods */
		struct hw_module_methods_t* methods;//����

		/** module's dso */
		void* dso; //˽��ָ��

		/** padding to 128 bytes, reserved for future use */
		uint32_t reserved[32-7];//Ԥ��

	} hw_module_t;
	

	typedef struct hw_module_methods_t {
		/** Open a specific device */
		int (*open)(const struct hw_module_t* module, const char* id,
				struct hw_device_t** device);

	} hw_module_methods_t;


	device����: ��ʾһ�������Ӳ���豸
		typedef struct hw_device_t {
			/** tag must be initialized to HARDWARE_DEVICE_TAG */
			uint32_t tag;// ��ǩ--�����ʼ��Ϊ HARDWARE_DEVICE_TAG

			/** version number for hw_device_t */
			uint32_t version;// �汾---һ�㶼��1

			/** reference to the module this device belongs to */
			struct hw_module_t* module;//ָ��������module����

			/** padding reserved for future use */
			uint32_t reserved[12];

			/** Close this device */
			int (*close)(struct hw_device_t* device);//����

		} hw_device_t;


	Android.mk
	LOCAL_PATH := $(call my-dir)
	include $(CLEAR_VARS)


	LOCAL_SHARED_LIBRARIES :=  libcutils

	LOCAL_SRC_FILES := led_hal.c
	
	#Ŀ����һ��Ҫ�ʹ��뱣��һ��,����Ҫ�к�׺.default
	LOCAL_MODULE := s5pv210_led_hal.default
	#ָ��Ŀ���ļ���װ·��--out/target/product/fs210/system/lib/hw
	LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw

	LOCAL_MODULE_TAGS := optional

	include $(BUILD_SHARED_LIBRARY)




	george@George-JI:~/src_210/android4.0-fs210_v2$ mmm mytest/led_hal/
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
	target thumb C: s5pv210_led_hal.default <= mytest/led_hal/led_hal.c
	mytest/led_hal/led_hal.c: In function 'led_hal_module_open':
	mytest/led_hal/led_hal.c:91: warning: assignment discards qualifiers from pointer target type
	target SharedLib: s5pv210_led_hal.default (out/target/product/fs210/obj/SHARED_LIBRARIES/s5pv210_led_hal.default_intermediates/LINKED/s5pv210_led_hal.default.so)
	target Symbolic: s5pv210_led_hal.default (out/target/product/fs210/symbols/system/lib/hw/s5pv210_led_hal.default.so)
	target Strip: s5pv210_led_hal.default (out/target/product/fs210/obj/lib/s5pv210_led_hal.default.so)
	Install: out/target/product/fs210/system/lib/hw/s5pv210_led_hal.default.so

	
	
==============================================================
ʵ��jni����hal:
	//  1, ����hal��̬��
	//����1---��̬�������
	//����2---ִ��hal��module��ָ��
	//����ֵ--��ȷΪ0
	jint ret;
	
	ret = hw_get_module(LED_MODULE_ID,(const struct hw_module_t * * )&pModule);

	
	
	// 2, ����module��open����
			//p->common.methods->open
			//����1---hal��moduleָ��
			//����2---���ֲ�ͬ��device---һ����NULL
			//����3--ָ��hal��device�����ָ��
			pModule->common.methods->open(&pModule->common, NULL,
            								(struct hw_device_t** )&pDevice);


	// 3, ����device�еĸ�������
	pDevice->open_dev();

		
Android.mk
	LOCAL_PATH:= $(call my-dir)
	include $(CLEAR_VARS)

	LOCAL_MODULE_TAGS := optional


	LOCAL_MODULE:= libled_jni


	LOCAL_SRC_FILES:= led_jni.cpp


	LOCAL_SHARED_LIBRARIES := \
			libutils \
			libhardware  #��Ϊ������hw_get_module

	
	LOCAL_C_INCLUDES += \
			$(JNI_H_INCLUDE) \
			$(LOCAL_PATH)/../led_hal/ #��Ϊʹ��led_hal.h


	include $(BUILD_SHARED_LIBRARY)
		
		
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
target SharedLib: libled_jni (out/target/product/fs210/obj/SHARED_LIBRARIES/libled_jni_intermediates/LINKED/libled_jni.so)
target Symbolic: libled_jni (out/target/product/fs210/symbols/system/lib/libled_jni.so)
target Strip: libled_jni (out/target/product/fs210/obj/lib/libled_jni.so)
Install: out/target/product/fs210/system/lib/libled_jni.so





����

apk--����
jni.so --����
	cp -raf out/target/product/fs210/system/lib/libled_jni.so  /opt/rootfs_dir/system/lib
hal.so -->����
	cp -raf  out/target/product/fs210/system/lib/hw/s5pv210_led_hal.default.so  /opt/rootfs_dir/system/lib/hw



--------- beginning of /dev/log/system
I/ActivityManager( 2216): START {act=android.intent.action.MAIN cat=[android.intent.category.LAUNCHER] flg=0x10200000 cmp=com.hq.ledcontrol/.LedActivity} from pid 2490
D/PermissionCache( 2139): checking android.permission.READ_FRAME_BUFFER for uid=1000 => granted (266 us)
D/dalvikvm( 2890): Not late-enabling CheckJNI (already on)
I/ActivityManager( 2216): Start proc com.hq.ledcontrol for activity com.hq.ledcontrol/.LedActivity: pid=2890 uid=10053 gids={}
D/dalvikvm( 2216): GC_CONCURRENT freed 354K, 6% free 8864K/9415K, paused 3ms+4ms
D/OpenGLRenderer( 2490): Flushing caches (mode 1)
I/Process ( 2216): Sending signal. PID: 2890 SIG: 3
I/dalvikvm( 2890): threadid=3: reacting to signal 3
I/dalvikvm( 2890): Wrote stack traces to '/data/anr/traces.txt'
D/led_jni_log( 2890): -----------JNI_OnLoad--------------
D/led_jni_log( 2890): -----------open_led--------------
D/led_jni_log( 2890):  hw_get_module ok
D/led_hal_log( 2890): -----------led_hal_dev_open--------------
D/led_jni_log( 2890): -----------add_num--------------
D/led_jni_log( 2890): -----------get_jni_string--------------
D/led_jni_log( 2890): -----------set_jni_string--------------
D/led_jni_log( 2890): -----------^_^ hello from java--------------
D/led_jni_log( 2890): ----name = marong, age = 30, height = 1.200000
D/OpenGLRenderer( 2490): Flushing caches (mode 0)
D/libEGL  ( 2890): loaded /system/lib/egl/libGLES_android.so
D/libEGL  ( 2890): loaded /vendor/lib/egl/libEGL_POWERVR_SGX540_120.so
D/libEGL  ( 2890): loaded /vendor/lib/egl/libGLESv1_CM_POWERVR_SGX540_120.so
D/libEGL  ( 2890): loaded /vendor/lib/egl/libGLESv2_POWERVR_SGX540_120.so
D/OpenGLRenderer( 2890): Enabling debug mode 0
W/InputManagerService( 2216): Starting input on non-focused client com.android.internal.view.IInputMethodClient$Stub$Proxy@4110f108 (uid=10028 pid=2490)
I/Process ( 2216): Sending signal. PID: 2890 SIG: 3
I/dalvikvm( 2890): threadid=3: reacting to signal 3
I/dalvikvm( 2890): Wrote stack traces to '/data/anr/traces.txt'
I/ActivityManager( 2216): Displayed com.hq.ledcontrol/.LedActivity: +1s299ms
D/LedActivity( 2890): ��Ϲ�˹���
D/led_jni_log( 2890): -----------led_on--------------
D/led_hal_log( 2890): -----------led_hal_control_dev--------------
E/dhcpcd  ( 2636): timed out


		