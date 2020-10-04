LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := libled_ndk_srv
LOCAL_CFLAGS    := -Werror
LOCAL_SRC_FILES := myled_ndk_jni.cpp
LOCAL_LDLIBS    := -llog 

include $(BUILD_SHARED_LIBRARY)