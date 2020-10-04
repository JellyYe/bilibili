package com.hq.net.srv;

import com.hq.aidlledsrv.R;

import android.app.Activity;
import android.os.Bundle;

public class MainLedNet extends Activity{
	
	ListenThread lt;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main_led_net);
		
		lt = new ListenThread();
		
		lt.start();
		
	}
	
	@Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
		lt.setStop();
	}
}
