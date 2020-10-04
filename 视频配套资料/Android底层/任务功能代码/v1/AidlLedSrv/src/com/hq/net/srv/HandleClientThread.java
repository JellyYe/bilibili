package com.hq.net.srv;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.Socket;

import android.util.Log;

public class HandleClientThread extends Thread{
	private Socket client;
	private InputStream in;
	private boolean stop = false;
	public HandleClientThread(Socket c){
		this.client = c;
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
		while(true){
			
			//不同的读取数据
			try {
				
				
				byte[] buffer = new byte[1024];
				
				ret = in.read(buffer);
				Log.d("My_TAG", "ret = " + ret + " 读取到客户端的数据 : " + new String(buffer, 0, ret));
				
//				String result = bReader.readLine();
//				
//				Log.d("My_TAG", "读取到客户端的数据 : " + result);
				
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
