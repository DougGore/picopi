LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE:= libttscompat

LOCAL_SRC_FILES:= \
	com_android_tts_compat_SynthProxy.cpp

LOCAL_C_INCLUDES += \
	frameworks/base/native/include \
	$(JNI_H_INCLUDE)

LOCAL_SHARED_LIBRARIES := \
	libandroid_runtime \
	libnativehelper \
	libmedia \
	libutils \
	libcutils \
	liblog \
	libdl

include $(BUILD_SHARED_LIBRARY)
