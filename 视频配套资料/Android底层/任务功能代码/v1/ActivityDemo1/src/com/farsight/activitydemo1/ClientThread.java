package com.farsight.activitydemo1;

import java.io.IOException;
import java.io.OutputStream;
import java.net.Socket;
import java.net.UnknownHostException;

import android.content.res.Resources.Theme;

public class ClientThread extends Thread{
	private Socket client;
	private OutputStream out;
	
	@Override
	public void run() {
		// TODO Auto-generated method stub
		super.run();
		
		try {
			client = new Socket("192.168.7.7", 18889);
			
			if(client != null){
				//连接服务器ok
				System.out.println("连接服务器ok");
				out = client.getOutputStream();
				
				while(true){
					
					
					Thread.sleep(3000);
					
					out.write(new String("led_on").getBytes());
					out.flush();
					
				}
				
			}
			
			
			
		} catch (UnknownHostException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();

		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
	}
	
}
