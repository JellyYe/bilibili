package com.hq.aidlledclient;

import com.hq.rpc.ILedService;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.os.RemoteException;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.Toast;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.ToggleButton;


public class LedClientActivity extends Activity {
	Button btn_bind_srv;
	ToggleButton tg_btn_switch;
	
	ILedService mService;
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_led_client);
        
        initUI();
    }
    
  //---链接状态处理对象
    ServiceConnection connection = new ServiceConnection() {
		
		@Override
		public void onServiceDisconnected(ComponentName name) {
			// TODO Auto-generated method stub
			try {
				mService.rpcClose();
			} catch (RemoteException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			mService = null;
		}
		
		@Override
		public void onServiceConnected(ComponentName name, IBinder service) {
			// TODO Auto-generated method stub
			if(service != null){
				Toast.makeText(LedClientActivity.this, "连接rpc服务器成功", 1000).show();
				//将服务端传递过来的对象进行转换
				mService = ILedService.Stub.asInterface(service);
				
				//远程调用
				try {
					mService.rpcOpen();
				} catch (RemoteException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
			
		}
	};
    OnClickListener clickListener = new OnClickListener() {
		
		@Override
		public void onClick(View v) {
			// TODO Auto-generated method stub
			//连接服务器
			//参数1---绑定哪个服务
			//参数2---链接状态处理对象
			//参数3--随着客户端的使用,服务才会启动
			Intent intent = new Intent();
			intent.setAction("com.hq.rpc.ledservice");
			bindService(intent, connection, BIND_AUTO_CREATE);
		}
	};
	
	OnCheckedChangeListener onCheckedChangeListener = new OnCheckedChangeListener() {
		
		@Override
		public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
			// TODO Auto-generated method stub
			if(isChecked){//开关打开
				try {
					mService.rpcControl(1);
				} catch (RemoteException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				
			}else{
				try {
					mService.rpcControl(0);
				} catch (RemoteException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
	};

	private void initUI() {
		// TODO Auto-generated method stub
		
		btn_bind_srv = (Button) findViewById(R.id.btn_bind_srv);
		btn_bind_srv.setOnClickListener(clickListener);
		tg_btn_switch = (ToggleButton) findViewById(R.id.tg_btn_switch);
		tg_btn_switch.setOnCheckedChangeListener(onCheckedChangeListener);
		
	}
	
	@Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
		
	}


   
}
