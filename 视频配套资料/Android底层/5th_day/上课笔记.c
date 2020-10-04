主要内容---Framework的经典实现---AIDL
1,andorid的属性系统和hw_get_module的原理
2,NDK开发----另外一种jni开发方式
3,Framework经典实现之Service+AIDL跨进程间通信
4,Android中GPS系统框架分析
-------------------------------------------------
1,andorid的属性系统和hw_get_module的原理
	属性系统: 比较简单和高效的进程间通信方式
	
属性系统:
android的属性系统:
	1,每个属性都有一个名称和值，他们都是字符串格式
	2,用来记录系统设置或进程之间的信息交换。
	3,android分配一个共享内存区来存储的属性,属性是在整个系统中全局可见的
	4, 每个进程可以get/set属性, set是通过向init进程发出请求,又init去完成,get可以直接读取共享内存


属性的格式: 以点分格式,可以自己定义,但以下为特殊属性
	属性名称以"ro."开头，那么这个属性被视为只读属性。一旦设置，属性值不能改变
	属性名称以"persist."开头，当设置这个属性时，其值也将写入/data/property/
	属性名称以"net."开头，当设置这个属性时，"net.change"属性将会自动修改,用于记录最新被修改的以net.开头的属性
	属性"ctrl.start"和"ctrl.stop"是用来启动和停止服务
	

系统中默认的属性文件/代码:
	/default.prop
	/system/build.prop
	/system/default.prop
	/data/local.prop
	以上文件一般只有前两个,后两个没有
	属性将会以上述顺序加载。后加载的属性将覆盖原先的值。这些属性加载之后，
	最后加载的属性会被保持在/data/property/目录中中。

	/default.prop : 系统默认的属性,一般都在根文件系统目录的顶层
		ro.secure=0
		ro.allow.mock.location=1
		ro.debuggable=1
		persist.sys.usb.config=mass_storage,adb
	
	/system/build.prop: 系统编译的属性,主要记录编译时间,编译主机,编译平台,以及dvm的信息,
				由Makefile自动生成
	
	
	/data/property/:
		persist.sys.country   persist.sys.localevar
		persist.sys.language  persist.sys.profiler_ms

修改和查看属性的方法:
	命令和shell脚本中:
		getprop <属性名>
		setprop <属性名><<属性值>
	代码:
		java: 
			System.getProperty()
			System.setProperty()
		c/c++ :
			int property_get(const char *key, char *value, const char *default_value);
			int property_set(const char *key, const char *value);

			启动服务:
				property_set("ctrl.start", "dhcpcd_p2p");
		

在init.rc中的属性:
	on property:persist.service.adb.enable=1
		start adbd
	on property:persist.service.adb.enable=0
		stop adbd
	如果你设置persist.service.adb.enable为1 ，"init"守护程序就知道需要采取行动：开启adbd服务。



root@android:/data # getprop
[dalvik.vm.dexopt-flags]: [m=y]
[dalvik.vm.heapgrowthlimit]: [64m]
[dalvik.vm.heapsize]: [256m]
[dalvik.vm.heapstartsize]: [8m]
[dalvik.vm.stack-trace-file]: [/data/anr/traces.txt]
[dev.bootcomplete]: [1]
[dhcp.eth0.gateway]: [192.168.7.1]
[dhcp.eth0.ipaddress]: [192.168.7.108]
[dhcp.eth0.leasetime]: [7200]
[dhcp.eth0.mask]: [255.255.255.0]
[dhcp.eth0.pid]: [3842]
[dhcp.eth0.reason]: [BOUND]
[dhcp.eth0.result]: [ok]
[dhcp.eth0.server]: [192.168.7.1]
[gsm.current.phone-type]: [1]
[gsm.operator.alpha]: []
[gsm.operator.iso-country]: []
[gsm.operator.isroaming]: [false]
[gsm.operator.numeric]: []
[gsm.sim.operator.alpha]: []
[gsm.sim.operator.iso-country]: []
[gsm.sim.operator.numeric]: []
[gsm.sim.state]: [UNKNOWN]
[gsm.version.ril-impl]: [ztemt @ version 4.0.2.0 beta3 GSM]
[hwui.render_dirty_regions]: [false]
[init.svc.adbd]: [running]
[init.svc.bootanim]: [stopped]
[init.svc.console]: [running]
[init.svc.dbus]: [running]
[init.svc.debuggerd]: [running]
[init.svc.dhcpcd_eth0]: [running]
[init.svc.drm]: [running]
[init.svc.fs210-setup]: [stopped]
[init.svc.ifcfg_ralink]: [stopped]
[init.svc.installd]: [running]
[init.svc.keystore]: [running]
[init.svc.media]: [running]
[init.svc.netd]: [running]
[init.svc.pvrsrvinit]: [stopped]
[init.svc.ril-daemon]: [running]
[init.svc.servicemanager]: [running]
[init.svc.surfaceflinger]: [running]
[init.svc.vold]: [running]
[init.svc.zygote]: [running]
[net.bt.name]: [Android]
[net.change]: [net.dnschange]
[net.dns1]: [8.8.8.8]
[net.dnschange]: [1]
[net.eth0.dns1]: []
[net.eth0.dns2]: []
[net.hostname]: [android-df7a73bbc7e738da]
[net.qtaguid_enabled]: [0]
[net.tcp.buffersize.default]: [4096,87380,110208,4096,16384,110208]
[net.tcp.buffersize.edge]: [4093,26280,35040,4096,16384,35040]
[net.tcp.buffersize.gprs]: [4092,8760,11680,4096,8760,11680]
[net.tcp.buffersize.hspa]: [4094,87380,262144,4096,16384,262144]
[net.tcp.buffersize.lte]: [524288,1048576,2097152,262144,524288,1048576]
[net.tcp.buffersize.umts]: [4094,87380,110208,4096,16384,110208]
[net.tcp.buffersize.wifi]: [524288,1048576,2097152,262144,524288,1048576]
[persist.sys.profiler_ms]: [0]
[persist.sys.usb.config]: [mass_storage,adb]
[ril.ZTEMT_DEBUG_LEVEL]: [3]
[ril.ZTEMT_DEBUG_MASK]: [255]
[ro.allow.mock.location]: [1]
[ro.baseband]: [unknown]
[ro.board.platform]: [fs210]
[ro.bootloader]: [unknown]
[ro.bootmode]: [unknown]
[ro.build.characteristics]: [default]
[ro.build.date.utc]: [1381833737]
[ro.build.date]: [二 10月 15 18:42:17 HKT 2013]
[ro.build.description]: [full_fs210-eng 4.0.4 IMM76I eng.george.20131015.155801 test-keys]
[ro.build.display.id]: [full_fs210-eng 4.0.4 IMM76I eng.george.20131015.155801 test-keys]
[ro.build.fingerprint]: [Android/full_fs210/fs210:4.0.4/IMM76I/eng.george.20131015.155801:eng/test-keys]
[ro.build.host]: [George-JI]
[ro.build.id]: [IMM76I]
[ro.build.product]: [fs210]
[ro.build.tags]: [test-keys]
[ro.build.type]: [eng]
[ro.build.user]: [george]
[ro.build.version.codename]: [REL]
[ro.build.version.incremental]: [eng.george.20131015.155801]
[ro.build.version.release]: [4.0.4]
[ro.build.version.sdk]: [15]
[ro.carrier]: [unknown]
[ro.com.android.dateformat]: [MM-dd-yyyy]
[ro.config.alarm_alert]: [Alarm_Classic.ogg]
[ro.config.notification_sound]: [pixiedust.ogg]
[ro.config.ringtone]: [Ring_Synth_04.ogg]
[ro.debuggable]: [1]
[ro.factorytest]: [0]
[ro.hardware]: [fs210]
[ro.kernel.android.checkjni]: [1]
[ro.opengles.version]: [131072]
[ro.product.board]: [fs210]
[ro.product.brand]: [Android]
[ro.product.cpu.abi2]: [armeabi]
[ro.product.cpu.abi]: [armeabi-v7a]
[ro.product.device]: [fs210]
[ro.product.locale.language]: [en]
[ro.product.locale.region]: [US]
[ro.product.manufacturer]: [unknown]
[ro.product.model]: [Full AOSP on fs210]
[ro.product.name]: [full_fs210]
[ro.revision]: [0]
[ro.runtime.firstboot]: [946684902493]
[ro.secure]: [0]
[ro.serialno]: []
[ro.setupwizard.mode]: [OPTIONAL]
[ro.sf.lcd_density]: [170]
[ro.wifi.channels]: []
[sys.boot_completed]: [1]
[sys.usb.config]: [mass_storage,adb]
[sys.usb.state]: [mass_storage,adb]
[system_init.startsurfaceflinger]: [0]
[vold.post_fs_data_done]: [1]
[wifi.interface]: [wlan0]
[wifi.supp_delay_para]: [800]
[wifi.supplicant_scan_interval]: [10]
[wifi.wapi.supported]: [true]
[wlan.driver.status]: [unloaded]
[wlan.interface]: [wlan0]


/data/property # getprop ro.hardware
fs210
/data/property # getprop ro.product.board
fs210
/data/property # getprop ro.board.platform
fs210
/data/property # getprop ro.arch

static const char *variant_keys[] = {
    "ro.hardware",  
    "ro.product.board",
    "ro.board.platform",
    "ro.arch"
};

// /system/lib/hw/s5pv210_led_hal.default.so
hw_get_module("s5pv210_led_hal",(const struct hw_module_t * * )&pModule);
	|
	hw_get_module_by_class(id, NULL, module);//id="s5pv210_led_hal"
		|
		strlcpy(name, class_id, PATH_MAX);//name = s5pv210_led_hal
		for (i=0 ; i<HAL_VARIANT_KEYS_COUNT+1 ; i++) {
        if (i < HAL_VARIANT_KEYS_COUNT) {
            if (property_get(variant_keys[i], prop, NULL) == 0) {// prop="fs210"
                continue;
            }
			//path=/system/vendor/lib/hw/s5pv210_led_hal.fs210.so
            snprintf(path, sizeof(path), "%s/%s.%s.so",
                     HAL_LIBRARY_PATH2, name, prop);
            if (access(path, R_OK) == 0) break;
			//path=/system/lib/hw/s5pv210_led_hal.fs210.so
            snprintf(path, sizeof(path), "%s/%s.%s.so",
                     HAL_LIBRARY_PATH1, name, prop);
            if (access(path, R_OK) == 0) break;
		 } else {
        	//path=/system/lib/hw/s5pv210_led_hal.default.so
            snprintf(path, sizeof(path), "%s/%s.default.so",
                     HAL_LIBRARY_PATH1, name);
            if (access(path, R_OK) == 0) break;
        }
		
		 load(const char *id, const char *path, const struct hw_module_t **pHmi)
			|
			 struct hw_module_t *hmi;
			  const char *sym = HAL_MODULE_INFO_SYM_AS_STR;
			hmi = (struct hw_module_t *)dlsym(handle, sym);// 只找HMI的变量
			*pHmi = hmi;


编译hal动态的时候:
	1, 动态库的后缀可以是.fs210.so 或者default.so
	2, 动态库的路径可以放在:
		/system/lib/hw   <---如果是default.so, 只能放在这个路径
		/system/vendor/lib/hw	


	Android.mk
		#目标名一定要和代码保持一致,并且要有后缀.default
		LOCAL_MODULE := s5pv210_led_hal.fs210
		#LOCAL_MODULE := s5pv210_led_hal.default
		#指定目标文件安装路径--out/target/product/fs210/system/lib/hw
		#LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
		LOCAL_MODULE_PATH := $(TARGET_OUT)/vendor/lib/hw


-----------------------------------------------------------
4,Android中GPS系统框架分析
	
	
	gps芯片---------串口/dev/ttySAC1(ttyUSB0)------------soc
	
	
	得到坐标数据---实际就是在应用程序中读取串口数据
	
	串口数据:　NEMA协议
	GPGGA（定位信息）

		例：$GPGGA,092204.999,4250.5589,S,14718.5084,E,1,04,24.4,19.7,M,,,,0000*1F

		字段0：$GPGGA，语句ID，表明该语句为Global Positioning System Fix Data（GGA）GPS定位信息

		字段1：UTC 时间，hhmmss.sss，时分秒格式

		字段2：纬度ddmm.mmmm，度分格式（前导位数不足则补0）

		字段3：纬度N（北纬）或S（南纬）

		字段4：经度dddmm.mmmm，度分格式（前导位数不足则补0）

		字段5：经度E（东经）或W（西经）

		字段6：GPS状态，0=未定位，1=非差分定位，2=差分定位，3=无效PPS，6=正在估算

		字段7：正在使用的卫星数量（00 - 12）（前导位数不足则补0）

		字段8：HDOP水平精度因子（0.5 - 99.9）

		字段9：海拔高度（-9999.9 - 99999.9）

		字段10：地球椭球面相对大地水准面的高度

		字段11：差分时间（从最近一次接收到差分信号开始的秒数，如果不是差分定位将为空）

		字段12：差分站ID号0000 - 1023（前导位数不足则补0，如果不是差分定位将为空）

		字段13：校验值




	Android系统中gps的框架:
		
		app层: 百度地图, 高德, 简单的gps app
			编程接口
			
			 LocationManager lm; 
			// 连接系统的服务, 名字叫location
			lm = (LocationManager) getSystemService("location");
			 
			//如果需要得到坐标
			Location location = lm.getLastKnownLocation(LocationManager.GPS_PROVIDER);
			//经度
			location.getLongitude();
			//维度
			location.getLatitude();
			
		------------------------------------
		framework	
			Java语言实现的API(主要用来被APP调用,导入android.location包):
			frameworks/base/location/java/android/location
						LocationManager.java
								
						public class LocationManager {
							private final ILocationManager mService;
							
							//所有的方法都是给app使用的
							 boolean isProviderEnabled(String provider){
								 return mService.isProviderEnabled(provider);
							 }
							 public Location getLastLocation() {
								return mService.getLastLocation(null, packageName);
							 }
							
							 public Location getLastKnownLocation(String provider) {
								
								 return mService.getLastLocation(request, packageName);
							 }
							 
							 private void requestLocationUpdates(LocationRequest request, LocationListener listener,
											Looper looper, PendingIntent intent){
								//最终是调用了aidl中的接口
								mService.requestLocationUpdates(request, transport, intent, packageName);			
											
							}
							
						}
						
			frameworks/base/location/java/android/location/ILocationManager.aidl			
						ILocationManager.aidl
						|
						interface ILocationManager
						{
							List<String> getAllProviders();
							List<String> getProviders(in Criteria criteria, boolean enabledOnly);
							String getBestProvider(in Criteria criteria, boolean enabledOnly);
							boolean providerMeetsCriteria(String provider, in Criteria criteria);

							void requestLocationUpdates(String provider, in Criteria criteria, long minTime, float minDistance,
								boolean singleShot, in ILocationListener listener);
							void requestLocationUpdatesPI(String provider, in Criteria criteria, long minTime, float minDistance,
								boolean singleShot, in PendingIntent intent);
							void removeUpdates(in ILocationListener listener);
							void removeUpdatesPI(in PendingIntent intent);
							
							Location getLastKnownLocation(String provider);
						}

			谁去实现了aidl中接口: xxxx  extends ILocationManager.Stub
				
			系统服务实现:
				目录1：frameworks/base/location/java/com/android/internal/location
					这个目录是Framework对Location服务的内部实现--GpsNetInitiatedHandler.java
				目录2：frameworks/base/services/java/com/android/server
					这个目录中的LocationManagerService.java是Location服务对内部实现的一种封装
				目录3: frameworks/base/services/java/com/android/server/location/
								GpsLocationProvider.java

				
				public class LocationManagerService extends ILocationManager.Stub implements Runnable {
				
					public boolean isProviderEnabled(String provider) {
						return _isProviderEnabledLocked(provider);
					}
					
					 public Location getLastKnownLocation(String provider) {
						return _getLastKnownLocationLocked(provider);
					 }
					
					//开机就执行
					 public void run(){
						initialize();
					 }
					 
					 private void initialize() {
						loadProviders();
					 
					 }
					 private void _loadProvidersLocked(){
						if (GpsLocationProvider.isSupported()){//判断是否gps支持
							// GpsLocationProvider.java中
						
						
						}
					 
					 }
				
				}
				
				frameworks/base/services/java/com/android/server/location/GpsLocationProvider.java
					
					public class GpsLocationProvider implements LocationProviderInterface {
						 static { class_init_native(); }//更早执行
						private static native void class_init_native();
						private static native boolean native_is_supported();
	
						 public static boolean isSupported() {
							return native_is_supported();
						}
					
					}

		------------------------------------
		jni层frameworks/base/services/jni/
			com_android_server_location_GpsLocationProvider.cpp
			
			static JNINativeMethod sMethods[] = {
			 /* name, signature, funcPtr */
			{"class_init_native", "()V", (void *)android_location_GpsLocationProvider_class_init_native},
			{"native_is_supported", "()Z", (void*)android_location_GpsLocationProvider_is_supported},
			{"native_init", "()Z", (void*)android_location_GpsLocationProvider_init},
			{"native_cleanup", "()V", (void*)android_location_GpsLocationProvider_cleanup},
		}
		
		android_location_GpsLocationProvider_class_init_native()
			|
			static void android_location_GpsLocationProvider_class_init_native(JNIEnv* env, jclass clazz) {
				hw_module_t* module;
				//加载hal的so
				 hw_get_module(GPS_HARDWARE_MODULE_ID, (hw_module_t const**)&module);
				 
				 hw_device_t* device;
				err = module->methods->open(module, GPS_HARDWARE_MODULE_ID, &device);
				gps_device_t* gps_device = (gps_device_t *)device;
				//调用device的方法
				sGpsInterface = gps_device->get_gps_interface(gps_device);

			}
		-----------------------------------
		hal层:<---------主要开发的都是hal代码
			<---1,打开设备节点,  2,初始化串口 3, 读取到gps中nmea数据,解析
			hardware/libhardware/gps/gps_qemu.c
			hardware/libhardware/include/hardware/gps.h
			const GpsInterface* gps__get_gps_interface(struct gps_device_t* dev)
{
    return &qemuGpsInterface;
}


			static int open_gps(const struct hw_module_t* module, char const* name,
        struct hw_device_t** device)
		{
			struct gps_device_t *dev = malloc(sizeof(struct gps_device_t));
			memset(dev, 0, sizeof(*dev));
			dev->common.tag = HARDWARE_DEVICE_TAG;
			dev->common.version = 0;
			dev->common.module = (struct hw_module_t*)module;
		//    dev->common.close = (int (*)(struct hw_device_t*))close_lights;
			dev->get_gps_interface = gps__get_gps_interface;

			*device = (struct hw_device_t*)dev;
			return 0;
		}


		static struct hw_module_methods_t gps_module_methods = {
			.open = open_gps
		};

		const struct hw_module_t HAL_MODULE_INFO_SYM = {
			.tag = HARDWARE_MODULE_TAG,
			.version_major = 1,
			.version_minor = 0,
			.id = GPS_HARDWARE_MODULE_ID,
			.name = "Goldfish GPS Module",
			.author = "The Android Open Source Project",
			.methods = &gps_module_methods,
		};
                                       
			
 
		------------------------------------
		驱动
		---------------------------------
		硬件













