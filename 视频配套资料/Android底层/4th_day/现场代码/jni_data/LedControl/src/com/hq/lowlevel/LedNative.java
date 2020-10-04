package com.hq.lowlevel;

import com.hq.bean.Girl;

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
	
	//传递数据的接口
	// 基本数据类型
	public native int addNum(int a, int b);
	
	//字符串
	public native String getJniString();
	public native void setJniString(String str);
	
	//类/对象
	public native Girl getJniGirl();
	public native void setJniGirl(Girl g);

	
	
}
