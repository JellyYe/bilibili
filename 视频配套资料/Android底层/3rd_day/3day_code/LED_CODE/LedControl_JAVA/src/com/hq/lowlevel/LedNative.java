package com.hq.lowlevel;

public class LedNative {
	// 1, ���ض�̬��
	
	static{//����ִ��
		System.loadLibrary("led_jni"); // /system/lib/libled_jni.so
	}
	
	
	// 2, �������ط���--���ֺͲ��������Զ���
	public native int openDev();
	public native int devOn();
	public native int devOff();
	public native int closeDev();
}
