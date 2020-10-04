/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Original file: D:\\AidlLedClient\\src\\com\\hq\\rpc\\ILedService.aidl
 */
package com.hq.rpc;
public interface ILedService extends android.os.IInterface
{
/** Local-side IPC implementation stub class. */
public static abstract class Stub extends android.os.Binder implements com.hq.rpc.ILedService
{
private static final java.lang.String DESCRIPTOR = "com.hq.rpc.ILedService";
/** Construct the stub at attach it to the interface. */
public Stub()
{
this.attachInterface(this, DESCRIPTOR);
}
/**
 * Cast an IBinder object into an com.hq.rpc.ILedService interface,
 * generating a proxy if needed.
 */
public static com.hq.rpc.ILedService asInterface(android.os.IBinder obj)
{
if ((obj==null)) {
return null;
}
android.os.IInterface iin = obj.queryLocalInterface(DESCRIPTOR);
if (((iin!=null)&&(iin instanceof com.hq.rpc.ILedService))) {
return ((com.hq.rpc.ILedService)iin);
}
return new com.hq.rpc.ILedService.Stub.Proxy(obj);
}
@Override public android.os.IBinder asBinder()
{
return this;
}
@Override public boolean onTransact(int code, android.os.Parcel data, android.os.Parcel reply, int flags) throws android.os.RemoteException
{
switch (code)
{
case INTERFACE_TRANSACTION:
{
reply.writeString(DESCRIPTOR);
return true;
}
case TRANSACTION_rpcOpen:
{
data.enforceInterface(DESCRIPTOR);
int _result = this.rpcOpen();
reply.writeNoException();
reply.writeInt(_result);
return true;
}
case TRANSACTION_rpcControl:
{
data.enforceInterface(DESCRIPTOR);
int _arg0;
_arg0 = data.readInt();
int _result = this.rpcControl(_arg0);
reply.writeNoException();
reply.writeInt(_result);
return true;
}
case TRANSACTION_rpcClose:
{
data.enforceInterface(DESCRIPTOR);
int _result = this.rpcClose();
reply.writeNoException();
reply.writeInt(_result);
return true;
}
}
return super.onTransact(code, data, reply, flags);
}
private static class Proxy implements com.hq.rpc.ILedService
{
private android.os.IBinder mRemote;
Proxy(android.os.IBinder remote)
{
mRemote = remote;
}
@Override public android.os.IBinder asBinder()
{
return mRemote;
}
public java.lang.String getInterfaceDescriptor()
{
return DESCRIPTOR;
}
@Override public int rpcOpen() throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
int _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
mRemote.transact(Stub.TRANSACTION_rpcOpen, _data, _reply, 0);
_reply.readException();
_result = _reply.readInt();
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
@Override public int rpcControl(int on) throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
int _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
_data.writeInt(on);
mRemote.transact(Stub.TRANSACTION_rpcControl, _data, _reply, 0);
_reply.readException();
_result = _reply.readInt();
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
@Override public int rpcClose() throws android.os.RemoteException
{
android.os.Parcel _data = android.os.Parcel.obtain();
android.os.Parcel _reply = android.os.Parcel.obtain();
int _result;
try {
_data.writeInterfaceToken(DESCRIPTOR);
mRemote.transact(Stub.TRANSACTION_rpcClose, _data, _reply, 0);
_reply.readException();
_result = _reply.readInt();
}
finally {
_reply.recycle();
_data.recycle();
}
return _result;
}
}
static final int TRANSACTION_rpcOpen = (android.os.IBinder.FIRST_CALL_TRANSACTION + 0);
static final int TRANSACTION_rpcControl = (android.os.IBinder.FIRST_CALL_TRANSACTION + 1);
static final int TRANSACTION_rpcClose = (android.os.IBinder.FIRST_CALL_TRANSACTION + 2);
}
public int rpcOpen() throws android.os.RemoteException;
public int rpcControl(int on) throws android.os.RemoteException;
public int rpcClose() throws android.os.RemoteException;
}
