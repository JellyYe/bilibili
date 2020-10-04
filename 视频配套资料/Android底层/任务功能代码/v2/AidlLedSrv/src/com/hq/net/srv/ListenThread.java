package com.hq.net.srv;

import java.io.IOException;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;

import com.hq.lowlevel.LedNative;

import android.util.Log;

public class ListenThread extends Thread{
		ServerSocket srvSocket;
		Socket client;
		boolean stop = false;
		
		//在listen的线程就加载jni的so
		LedNative mynative = null;
		
		HandleClientThread hct;
		
		public ListenThread(){
			//加载jni.so
			mynative = new LedNative();
			mynative.openDev();
		}
		
		public void setStop(){
			stop = true;
			
			hct.setStop();
			
			mynative.closeDev();
		}
		
		@Override
		public void run() {
			// TODO Auto-generated method stub
			super.run();
			
			InetAddress  serverAddr = null;
			try {
				//serverAddr = InetAddress.getByName("192.168.7.7");
				//srvSocket = new ServerSocket(18889, 5, serverAddr);
				//srvSocket = new ServerSocket(18889, 5, null);
				srvSocket = new ServerSocket(18889);
				srvSocket.setReuseAddress(true);
				Log.d("My_TAG", "-->服务器ip: " + srvSocket.getInetAddress().getHostAddress());
				//Log.d("My_TAG", "-->服务器ip: " + InetAddress.getLocalHost().getHostAddress());
			} catch (UnknownHostException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			
			
			
			while(true){
				
				try {
							
					//阻塞等
					client = srvSocket.accept();
					if(client != null){
						
						Log.d("My_TAG", "--->new client connect ..");
						
						hct = new HandleClientThread(client);
						hct.setLedNative(mynative);
						
						hct.start();
						
					}
					if(stop){
						srvSocket.close();
						Log.d("My_TAG", "--->srvSocket stop ..");
						break;
					}

				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}	
				
			}
					
			
		}
	
}
