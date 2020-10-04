package com.hq.rpc;

interface ILedService{
	int rpcOpen();
	int rpcControl(int on);
	int rpcClose();
	
}