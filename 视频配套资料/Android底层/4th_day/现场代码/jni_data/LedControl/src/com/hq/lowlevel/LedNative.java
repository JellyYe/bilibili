package com.hq.lowlevel;

import com.hq.bean.Girl;

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
	
	//�������ݵĽӿ�
	// ������������
	public native int addNum(int a, int b);
	
	//�ַ���
	public native String getJniString();
	public native void setJniString(String str);
	
	//��/����
	public native Girl getJniGirl();
	public native void setJniGirl(Girl g);

	
	
}
