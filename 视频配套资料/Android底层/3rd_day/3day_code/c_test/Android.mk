LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := \
        libcutils 

LOCAL_SRC_FILES:= \
	hello.c 


LOCAL_MODULE_TAGS := optional

LOCAL_MODULE:= hello_arm_elf

include $(BUILD_EXECUTABLE)
