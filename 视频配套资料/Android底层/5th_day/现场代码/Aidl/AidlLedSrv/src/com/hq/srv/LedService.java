package com.hq.srv;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;

public class LedService extends Service{
	
	@Override
	public void onCreate() {
		// TODO Auto-generated method stub
		super.onCreate();
	}

	// 客户端连接的时候
	@Override
	public IBinder onBind(Intent intent) {
		// TODO Auto-generated method stub
		return new ILedServiceImpl();
	}
	
	@Override
	public boolean onUnbind(Intent intent) {
		// TODO Auto-generated method stub
		return super.onUnbind(intent);
	}
	
	@Override
	public void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
	}

}
