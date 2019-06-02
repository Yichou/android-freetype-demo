LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#编译精简虚拟机
LOCAL_MODULE := ft

LOCAL_STATIC_LIBRARIES := libfreetype

LOCAL_LDLIBS := -lm -llog -lc -lz -ljnigraphics -landroid

LOCAL_C_INCLUDES += $(LOCAL_PATH)/freetype/include

LOCAL_SRC_FILES := libft.c \
    libgh.c \
    native-lib.c

include $(BUILD_SHARED_LIBRARY)

include $(LOCAL_PATH)/freetype/Android.mk