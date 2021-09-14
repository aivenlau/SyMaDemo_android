//
// Created by generalplus_sa1 on 4/29/16.
//

#ifndef FFMPEGTEST_DEFINES_H
#define FFMPEGTEST_DEFINES_H

#define   aligD       4

#define   IC_NO         -1
#define   IC_GK         0
#define   IC_GP         1
#define   IC_SN         2
#define   IC_GKA        3
#define   IC_GPRTSP     4
#define   IC_GPH264     5
#define   IC_GPRTP      6
#define   IC_GPH264A    7
#define   IC_GPRTPB      8
#define   IC_GK_UDP      9

#define   IC_GPRTPC      10
#define   IC_RTLH264     11


//支持SD卡   支持H264

#define   IC_GPH264_34     12


#define   IC_FILES      100


//#ifdef DEBUG
#include <android/log.h>

#ifdef DEBUG

#if defined(ANDROID) || defined(__ANDROID__)

#define DEBUG_PRINT(...) __android_log_print(ANDROID_LOG_ERROR  , "ffmpegJNI",__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR ,"ffmpegJNI—",__VA_ARGS__)
#define LOGA(...) __android_log_print(ANDROID_LOG_ERROR ,"RRR",__VA_ARGS__)
#define M_LOGE(Var, ...) __android_log_print(ANDROID_LOG_ERROR  , ""+Var,__VA_ARGS__)


#else
#define DEBUG_PRINT(format, ...)  printf( \
        "%*.*s(%.5d): " \
        format, 40, 40 , __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__ \
        )
#endif
#else
#define DEBUG_PRINT(format, ...)  ((void)0)
#define LOGE(...) ((void)0)
#define LOGA(...) ((void)0)
#define M_LOGE(Var, ...) ((void)0)

#endif

#define LOGE_B(...) __android_log_print(ANDROID_LOG_ERROR ,"ffmpegJNI—",__VA_ARGS__)

#endif //FFMPEGTEST_DEFINES_H


