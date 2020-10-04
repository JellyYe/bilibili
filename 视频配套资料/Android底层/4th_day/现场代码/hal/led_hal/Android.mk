LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)


LOCAL_SHARED_LIBRARIES :=  libcutils

LOCAL_SRC_FILES := led_hal.c

LOCAL_MODULE := s5pv210_led_hal.default
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw

LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
