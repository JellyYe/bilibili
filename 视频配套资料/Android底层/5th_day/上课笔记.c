��Ҫ����---Framework�ľ���ʵ��---AIDL
1,andorid������ϵͳ��hw_get_module��ԭ��
2,NDK����----����һ��jni������ʽ
3,Framework����ʵ��֮Service+AIDL����̼�ͨ��
4,Android��GPSϵͳ��ܷ���
-------------------------------------------------
1,andorid������ϵͳ��hw_get_module��ԭ��
	����ϵͳ: �Ƚϼ򵥺͸�Ч�Ľ��̼�ͨ�ŷ�ʽ
	
����ϵͳ:
android������ϵͳ:
	1,ÿ�����Զ���һ�����ƺ�ֵ�����Ƕ����ַ�����ʽ
	2,������¼ϵͳ���û����֮�����Ϣ������
	3,android����һ�������ڴ������洢������,������������ϵͳ��ȫ�ֿɼ���
	4, ÿ�����̿���get/set����, set��ͨ����init���̷�������,��initȥ���,get����ֱ�Ӷ�ȡ�����ڴ�


���Եĸ�ʽ: �Ե�ָ�ʽ,�����Լ�����,������Ϊ��������
	����������"ro."��ͷ����ô������Ա���Ϊֻ�����ԡ�һ�����ã�����ֵ���ܸı�
	����������"persist."��ͷ���������������ʱ����ֵҲ��д��/data/property/
	����������"net."��ͷ���������������ʱ��"net.change"���Խ����Զ��޸�,���ڼ�¼���±��޸ĵ���net.��ͷ������
	����"ctrl.start"��"ctrl.stop"������������ֹͣ����
	

ϵͳ��Ĭ�ϵ������ļ�/����:
	/default.prop
	/system/build.prop
	/system/default.prop
	/data/local.prop
	�����ļ�һ��ֻ��ǰ����,������û��
	���Խ���������˳����ء�����ص����Խ�����ԭ�ȵ�ֵ����Щ���Լ���֮��
	�����ص����Իᱻ������/data/property/Ŀ¼���С�

	/default.prop : ϵͳĬ�ϵ�����,һ�㶼�ڸ��ļ�ϵͳĿ¼�Ķ���
		ro.secure=0
		ro.allow.mock.location=1
		ro.debuggable=1
		persist.sys.usb.config=mass_storage,adb
	
	/system/build.prop: ϵͳ���������,��Ҫ��¼����ʱ��,��������,����ƽ̨,�Լ�dvm����Ϣ,
				��Makefile�Զ�����
	
	
	/data/property/:
		persist.sys.country   persist.sys.localevar
		persist.sys.language  persist.sys.profiler_ms

�޸ĺͲ鿴���Եķ���:
	�����shell�ű���:
		getprop <������>
		setprop <������><<����ֵ>
	����:
		java: 
			System.getProperty()
			System.setProperty()
		c/c++ :
			int property_get(const char *key, char *value, const char *default_value);
			int property_set(const char *key, const char *value);

			��������:
				property_set("ctrl.start", "dhcpcd_p2p");
		

��init.rc�е�����:
	on property:persist.service.adb.enable=1
		start adbd
	on property:persist.service.adb.enable=0
		stop adbd
	���������persist.service.adb.enableΪ1 ��"init"�ػ������֪����Ҫ��ȡ�ж�������adbd����



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
[ro.build.date]: [�� 10�� 15 18:42:17 HKT 2013]
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
			hmi = (struct hw_module_t *)dlsym(handle, sym);// ֻ��HMI�ı���
			*pHmi = hmi;


����hal��̬��ʱ��:
	1, ��̬��ĺ�׺������.fs210.so ����default.so
	2, ��̬���·�����Է���:
		/system/lib/hw   <---�����default.so, ֻ�ܷ������·��
		/system/vendor/lib/hw	


	Android.mk
		#Ŀ����һ��Ҫ�ʹ��뱣��һ��,����Ҫ�к�׺.default
		LOCAL_MODULE := s5pv210_led_hal.fs210
		#LOCAL_MODULE := s5pv210_led_hal.default
		#ָ��Ŀ���ļ���װ·��--out/target/product/fs210/system/lib/hw
		#LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
		LOCAL_MODULE_PATH := $(TARGET_OUT)/vendor/lib/hw


-----------------------------------------------------------
4,Android��GPSϵͳ��ܷ���
	
	
	gpsоƬ---------����/dev/ttySAC1(ttyUSB0)------------soc
	
	
	�õ���������---ʵ�ʾ�����Ӧ�ó����ж�ȡ��������
	
	��������:��NEMAЭ��
	GPGGA����λ��Ϣ��

		����$GPGGA,092204.999,4250.5589,S,14718.5084,E,1,04,24.4,19.7,M,,,,0000*1F

		�ֶ�0��$GPGGA�����ID�����������ΪGlobal Positioning System Fix Data��GGA��GPS��λ��Ϣ

		�ֶ�1��UTC ʱ�䣬hhmmss.sss��ʱ�����ʽ

		�ֶ�2��γ��ddmm.mmmm���ȷָ�ʽ��ǰ��λ��������0��

		�ֶ�3��γ��N����γ����S����γ��

		�ֶ�4������dddmm.mmmm���ȷָ�ʽ��ǰ��λ��������0��

		�ֶ�5������E����������W��������

		�ֶ�6��GPS״̬��0=δ��λ��1=�ǲ�ֶ�λ��2=��ֶ�λ��3=��ЧPPS��6=���ڹ���

		�ֶ�7������ʹ�õ�����������00 - 12����ǰ��λ��������0��

		�ֶ�8��HDOPˮƽ�������ӣ�0.5 - 99.9��

		�ֶ�9�����θ߶ȣ�-9999.9 - 99999.9��

		�ֶ�10��������������Դ��ˮ׼��ĸ߶�

		�ֶ�11�����ʱ�䣨�����һ�ν��յ�����źſ�ʼ��������������ǲ�ֶ�λ��Ϊ�գ�

		�ֶ�12�����վID��0000 - 1023��ǰ��λ��������0��������ǲ�ֶ�λ��Ϊ�գ�

		�ֶ�13��У��ֵ




	Androidϵͳ��gps�Ŀ��:
		
		app��: �ٶȵ�ͼ, �ߵ�, �򵥵�gps app
			��̽ӿ�
			
			 LocationManager lm; 
			// ����ϵͳ�ķ���, ���ֽ�location
			lm = (LocationManager) getSystemService("location");
			 
			//�����Ҫ�õ�����
			Location location = lm.getLastKnownLocation(LocationManager.GPS_PROVIDER);
			//����
			location.getLongitude();
			//ά��
			location.getLatitude();
			
		------------------------------------
		framework	
			Java����ʵ�ֵ�API(��Ҫ������APP����,����android.location��):
			frameworks/base/location/java/android/location
						LocationManager.java
								
						public class LocationManager {
							private final ILocationManager mService;
							
							//���еķ������Ǹ�appʹ�õ�
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
								//�����ǵ�����aidl�еĽӿ�
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

			˭ȥʵ����aidl�нӿ�: xxxx  extends ILocationManager.Stub
				
			ϵͳ����ʵ��:
				Ŀ¼1��frameworks/base/location/java/com/android/internal/location
					���Ŀ¼��Framework��Location������ڲ�ʵ��--GpsNetInitiatedHandler.java
				Ŀ¼2��frameworks/base/services/java/com/android/server
					���Ŀ¼�е�LocationManagerService.java��Location������ڲ�ʵ�ֵ�һ�ַ�װ
				Ŀ¼3: frameworks/base/services/java/com/android/server/location/
								GpsLocationProvider.java

				
				public class LocationManagerService extends ILocationManager.Stub implements Runnable {
				
					public boolean isProviderEnabled(String provider) {
						return _isProviderEnabledLocked(provider);
					}
					
					 public Location getLastKnownLocation(String provider) {
						return _getLastKnownLocationLocked(provider);
					 }
					
					//������ִ��
					 public void run(){
						initialize();
					 }
					 
					 private void initialize() {
						loadProviders();
					 
					 }
					 private void _loadProvidersLocked(){
						if (GpsLocationProvider.isSupported()){//�ж��Ƿ�gps֧��
							// GpsLocationProvider.java��
						
						
						}
					 
					 }
				
				}
				
				frameworks/base/services/java/com/android/server/location/GpsLocationProvider.java
					
					public class GpsLocationProvider implements LocationProviderInterface {
						 static { class_init_native(); }//����ִ��
						private static native void class_init_native();
						private static native boolean native_is_supported();
	
						 public static boolean isSupported() {
							return native_is_supported();
						}
					
					}

		------------------------------------
		jni��frameworks/base/services/jni/
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
				//����hal��so
				 hw_get_module(GPS_HARDWARE_MODULE_ID, (hw_module_t const**)&module);
				 
				 hw_device_t* device;
				err = module->methods->open(module, GPS_HARDWARE_MODULE_ID, &device);
				gps_device_t* gps_device = (gps_device_t *)device;
				//����device�ķ���
				sGpsInterface = gps_device->get_gps_interface(gps_device);

			}
		-----------------------------------
		hal��:<---------��Ҫ�����Ķ���hal����
			<---1,���豸�ڵ�,  2,��ʼ������ 3, ��ȡ��gps��nmea����,����
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
		����
		---------------------------------
		Ӳ��













