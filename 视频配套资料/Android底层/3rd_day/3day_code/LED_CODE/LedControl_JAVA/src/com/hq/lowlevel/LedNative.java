package com.hq.lowlevel;

public class LedNative {
	// 1, 加载动态库
	
	static{//优先执行
		System.loadLibrary("led_jni"); // /system/lib/libled_jni.so
	}
	
	
	// 2, 声明本地方法--名字和参数都是自定义
	public native int openDev();
	public native int devOn();
	public native int devOff();
	public native int closeDev();
}
