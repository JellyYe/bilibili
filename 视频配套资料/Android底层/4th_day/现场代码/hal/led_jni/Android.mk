LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)



LOCAL_MODULE_TAGS := optional


LOCAL_MODULE:= libled_jni


LOCAL_SRC_FILES:= led_jni.cpp


LOCAL_SHARED_LIBRARIES := \
	libutils \
	libhardware


LOCAL_C_INCLUDES += \
        $(JNI_H_INCLUDE) \
	$(LOCAL_PATH)/../led_hal/


include $(BUILD_SHARED_LIBRARY)
