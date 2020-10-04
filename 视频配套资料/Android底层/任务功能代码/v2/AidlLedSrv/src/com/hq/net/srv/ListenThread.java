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
		
		//��listen���߳̾ͼ���jni��so
		LedNative mynative = null;
		
		HandleClientThread hct;
		
		public ListenThread(){
			//����jni.so
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
				Log.d("My_TAG", "-->������ip: " + srvSocket.getInetAddress().getHostAddress());
				//Log.d("My_TAG", "-->������ip: " + InetAddress.getLocalHost().getHostAddress());
			} catch (UnknownHostException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			
			
			
			while(true){
				
				try {
							
					//������
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
