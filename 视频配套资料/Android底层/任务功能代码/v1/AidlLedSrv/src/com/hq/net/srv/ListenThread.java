package com.hq.net.srv;

import java.io.IOException;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;

import android.util.Log;

public class ListenThread extends Thread{
		ServerSocket srvSocket;
		Socket client;
		boolean stop = false;
		
		HandleClientThread hct;
		
		public void setStop(){
			stop = true;
			
			hct.setStop();
		}
		
		@Override
		public void run() {
			// TODO Auto-generated method stub
			super.run();
			
			InetAddress  serverAddr = null;
			try {
				serverAddr = InetAddress.getByName("192.168.7.7");
				//srvSocket = new ServerSocket(18889, 5, serverAddr);
				srvSocket = new ServerSocket(18889);
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
