package com.hq.net.srv;


import java.io.IOException;
import java.io.InputStream;

import java.net.Socket;

import org.json.JSONException;

import com.hq.json.LedJson;
import com.hq.lowlevel.LedNative;

import android.util.Log;

public class HandleClientThread extends Thread{
	private Socket client;
	private InputStream in;
	private boolean stop = false;
	LedNative mynative = null;
	
	
	public HandleClientThread(Socket c){
		this.client = c;
	}
	
	//传递过来
	public void setLedNative(LedNative mynative){
		this.mynative = mynative;
	}
	
	public void setClientSocket(Socket c){
		this.client = c;
	}
	
	public void setStop(){
		stop = true;

	}
	
	
	@Override
	public void run() {
		// TODO Auto-generated method stub
		super.run();
		
		try {
			in = client.getInputStream();
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		
		//BufferedReader bReader = new BufferedReader(new InputStreamReader(in));
		int ret;
		LedJson ledjson = new LedJson();
		while(true){
			
			//不同的读取数据
			try {
				
				
				byte[] buffer = new byte[1024];
				
				ret = in.read(buffer);
				Log.d("My_TAG", "ret = " + ret + " 读取到客户端的数据 : " + new String(buffer, 0, ret));
				
//				String result = bReader.readLine();
//				
//				Log.d("My_TAG", "读取到客户端的数据 : " + result);
				
				try {
					
					boolean ledStatus = ledjson.getLedStatus(LedJson.ALLLED_KEY, new String(buffer, 0, ret));
					
					if(ledStatus){
						//往jni调用
						mynative.devOn();
					}else{
						
						mynative.devOff();
					}
					
				} catch (JSONException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				
				if(stop){
					in.close();
					break;
				}
				

				
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
		}
		
		
		
		
	}
}
