package com.hq.srv;

import android.os.RemoteException;

import com.hq.lowlevel.LedNative;
import com.hq.rpc.ILedService;

public class ILedServiceImpl extends ILedService.Stub{
	private LedNative myNative;
	
	public ILedServiceImpl(){
		myNative = new LedNative();
	}	
	
	@Override
	public int rpcOpen() throws RemoteException {
		// TODO Auto-generated method stub
		System.out.println("--->rpcOpen()");
		
		myNative.openDev();
		
		return 0;
	}

	@Override
	public int rpcControl(int on) throws RemoteException {
		// TODO Auto-generated method stub
		System.out.println("--->rpcControl()");
		if(on != 0){
			myNative.devOn();
			
		}else{
			myNative.devOff();
		}
		return 0;
	}

	@Override
	public int rpcClose() throws RemoteException {
		// TODO Auto-generated method stub
		System.out.println("--->rpcClose()");
		
		myNative.closeDev();
		
		return 0;
	}



}
