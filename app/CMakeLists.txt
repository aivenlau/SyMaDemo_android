# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

#cmake_minimum_required(VERSION 3.4.1)
#ENABLE_LANGUAGE(ASM)

cmake_minimum_required(VERSION 3.6.0)



enable_language(ASM)

add_definitions(-DANDROID -DOC_ARM_ASM)
#add_compile_options(-std=gnu++11)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

#设定ingluce 路径
#src/main/cpp/ffmpeginclude;
include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR};
    src/main/cpp/mp4v2-2.0.0;
    src/main/cpp/mp4v2-2.0.0/include;
    src/main/cpp/mp4v2-2.0.0/include/mp4v2;
    src/main/cpp/GL;
    src/main/cpp/GL/opengles;
    src/main/cpp/h264prase;
    /System/Volumes/Data/WORK/ffmpeg/ffmpeglib/include;
    /System/Volumes/Data/WORK/ffmpeg/libyuv/jni/include;
    /System/Volumes/Data/WORK/ffmpeg/speexdsp-1.2rc3/jni/include;
    /System/Volumes/Data/WORK/ffmpeg/OpenCV/include;
    /System/Volumes/Data//WORK/ffmpeg/faac-1.28.0/jni/include;
    #/System/Volumes/Data/WORK/ffmpeg/myx264/include;

#src/main/cpp/ffmpeginclude;
)




#添加 .c 源文件，这样配合 下面的 ${HJ_WIFI_source_files} 再配合Android Studio 中的 “Refresh Linked C++ Projects“ 就可以把设定目录中的源文件都加入的系统中，
#无需每次都需要手动添加源文件到本配置文件中。

file (GLOB_RECURSE jh_wifi_files "${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/*.c*" )


add_library( # Sets the name of the library.
             jh_wifi
             # Sets the library as a shared library.
             SHARED
             # Provides a relative path to your source file(s).
             ${jh_wifi_files}
             )

# 设置opencv的动态库
#add_library(libopencv_java3 SHARED IMPORTED)
#set_target_properties(libopencv_java3 PROPERTIES IMPORTED_LOCATION
#            /WORK/ffmpeg/OpenCV/libs/${ANDROID_ABI}/libopencv_java3.so)


# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.

              log )

find_library( # Sets the name of the path variable.
              jnigraphics-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              jnigraphics )


#add_library( ffmpeg-lib
#             SHARED
#             IMPORTED )

#set_target_properties( ffmpeg-lib
#                       PROPERTIES IMPORTED_LOCATION
#                       ${CMAKE_CURRENT_SOURCE_DIR}/src/main/JniLibs/${ANDROID_ABI}/libffmpeg.so )





#用IF控制，可以实现Release版本不输出debug信息
#如果是Debug编译，就define  DEBUG，这样再源代码中就可以通这个宏来判断不同的编译类型

IF (CMAKE_BUILD_TYPE STREQUAL Debug)
    ADD_DEFINITIONS(-DDEBUG)
ENDIF()

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.



#加入OpenCV

#set(OpenCV_STATIC ON) #1
#set(OpenCV_DIR /WORK/ffmpeg/OpenCV-android-sdk/sdk/native/jni) #2
#find_package(OpenCV REQUIRED) #3

target_link_libraries( # Specifies the target library.
                       jh_wifi
                       mediandk
                       log
                       z
                       jnigraphics
                       GLESv2
                       OpenSLES
                       #GLESv3

                       /System/Volumes/Data/WORK/ffmpeg/faac-1.28.0/obj/local/${ANDROID_ABI}/libfaac.a
                       /System/Volumes/Data/WORK/ffmpeg/mp4v2-2.0.0/obj/local/${ANDROID_ABI}/libmp4v2.a
                       /System/Volumes/Data/WORK/ffmpeg/libyuv/obj/local/${ANDROID_ABI}/libyuv.a
                       /System/Volumes/Data/WORK/ffmpeg/ffmpeglib/${ANDROID_ABI}/libavformat.a
                       /System/Volumes/Data/WORK/ffmpeg/ffmpeglib/${ANDROID_ABI}/libavdevice.a
                       /System/Volumes/Data/WORK/ffmpeg/ffmpeglib/${ANDROID_ABI}/libavcodec.a
                       /System/Volumes/Data/WORK/ffmpeg/ffmpeglib/${ANDROID_ABI}/libavutil.a
                       /System/Volumes/Data/WORK/ffmpeg/ffmpeglib/${ANDROID_ABI}/libavfilter.a
                       /System/Volumes/Data/WORK/ffmpeg/ffmpeglib/${ANDROID_ABI}/libswresample.a
                       /System/Volumes/Data/WORK/ffmpeg/ffmpeglib/${ANDROID_ABI}/libswscale.a
                        /System/Volumes/Data/WORK/ffmpeg/ffmpeglib/${ANDROID_ABI}/libpostproc.a
                        # /System/Volumes/Data/WORK/ffmpeg/myx264/${ANDROID_ABI}/lib/libx264.a
#                        ${OpenCV_LIBS}    #4
                        )



#以下下这些，需要根据具体情况来调整路径
#/WORK/ffmpeg/mp4v2-2.0.0/obj/local/${ANDROID_ABI}/libmp4v2.a
#/WORK/ffmpeg/libyuv/obj/local/${ANDROID_ABI}/libyuv.a
#/WORK/ffmpeg/ffmpeglib/libavformat.a
#/WORK/ffmpeg/ffmpeglib/libavdevice.a
#/WORK/ffmpeg/ffmpeglib/libavcodec.a
#/WORK/ffmpeg/ffmpeglib/libavutil.a
#/WORK/ffmpeg/ffmpeglib/libavfilter.a
#/WORK/ffmpeg/ffmpeglib/libswresample.a
#/WORK/ffmpeg/ffmpeglib/libswscale.a




