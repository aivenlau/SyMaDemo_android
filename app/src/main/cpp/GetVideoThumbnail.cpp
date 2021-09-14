//
// Created by AivenLau on 2019-10-17.
//

#include "jh_wifi.h"
#include "Defines.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "libavcodec/jni.h"
#include "speex/speex_preprocess.h"
#include "speex/speex_echo.h"
#ifdef __cplusplus
}
#endif


#include <jni.h>

#include "RTL_DownLoad.h"

#include "JPEG_BUFFER.h"
#include "myRTP.h"
#include <pthread.h>
#include "FFMpegPlayerCore.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <cstdlib>
#include <fcntl.h>

#include <ctime>


#include <string>

#include<unistd.h>
#include<arpa/inet.h>


#include <sys/ioctl.h>


#include "MySocket_GKA.h"
#include <algorithm>
#include <cctype>
#include<net/if.h>

#include <vector>
#include <sys/socket.h>
#include <sstream>
#include <sys/system_properties.h>

#include <cstdlib>
#include <cstdio>
#include <cstdarg>


#include "MySonix.h"
#include "MyDownLoad_GKA.h"
#include "jh_wifi.h"
#include "phone_rl_protocol.h"

#include "MyMediaCoder.h"
#include "JH_TestInfo.h"


#include "GP4225_FileStruct.h"
#include "OnLinePlayer.h"
#include "MyRevBuffer.h"

#import <netinet/tcp.h>

#include  <android/bitmap.h>


#ifdef __cplusplus
extern "C" {
#endif

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/pixfmt.h"
#include "libavutil/imgutils.h"
#include "libavutil/time.h"
#include "libavutil/error.h"
#include "libavutil/frame.h"
#include "libavutil/opt.h"

#ifdef __cplusplus
}
#endif


#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include "MySocketData.h"

#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "media/NdkMediaCodec.h"
//#include "mp4v2.h"
#include "MyFrame.h"
#include "libyuv.h"


int F_Get(const char *path, unsigned char *addrPtr, int w, int h) {
    int err_code = 0;
    int videoindex = -1;

    AVFormatContext *m_formatCtx;
    AVCodec *pCodec;
    AVCodecContext *m_codecCtx;
    AVFrame *m_decodedFrame;

    struct SwsContext *img_convert_ctx;

    m_formatCtx = avformat_alloc_context();


    if (avformat_open_input(&m_formatCtx, path, nullptr, nullptr) != 0) {
        avformat_free_context(m_formatCtx);
        return -1;
    }
    if (avformat_find_stream_info(m_formatCtx, nullptr) < 0) {
        avformat_close_input(&m_formatCtx);
        avformat_free_context(m_formatCtx);
        m_formatCtx = nullptr;
        return -2;
    }

    int i = 0;
    for (i = 0; i < m_formatCtx->nb_streams; i++)
        if (m_formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoindex = i;
            break;
        }
    if (videoindex == -1) {
        avformat_close_input(&m_formatCtx);
        avformat_free_context(m_formatCtx);
        m_formatCtx = nullptr;
        return -3;
    }

    AVStream *stream = m_formatCtx->streams[videoindex];
    pCodec = avcodec_find_decoder(stream->codecpar->codec_id);
    if (pCodec == nullptr) {

        avformat_close_input(&m_formatCtx);
        avformat_free_context(m_formatCtx);
        m_formatCtx = nullptr;
        return -4;
    }

    m_codecCtx = avcodec_alloc_context3(pCodec);
    avcodec_parameters_to_context(m_codecCtx, stream->codecpar);
    if (avcodec_open2(m_codecCtx, pCodec, nullptr) < 0) {
        avformat_close_input(&m_formatCtx);
        avformat_free_context(m_formatCtx);
        m_formatCtx = nullptr;
        avcodec_free_context(&m_codecCtx);
        return -5;
    }


    if (m_codecCtx->width <= 0 || m_codecCtx->height <= 0) {
        avformat_close_input(&m_formatCtx);
        avformat_free_context(m_formatCtx);
        m_formatCtx = nullptr;
        return -6;
    }

    if (m_codecCtx->pix_fmt == AV_PIX_FMT_NONE) {
        avformat_close_input(&m_formatCtx);
        avformat_free_context(m_formatCtx);
        m_formatCtx = nullptr;
        return -7;
    }


    m_decodedFrame = av_frame_alloc();


    AVPacket packet;
    int ret = 0;


    AVFrame *pFrameRGBA_Real;

    pFrameRGBA_Real = av_frame_alloc();

    pFrameRGBA_Real->format = AV_PIX_FMT_RGBA;
    pFrameRGBA_Real->width = w;
    pFrameRGBA_Real->height = h;
    av_image_alloc(
            pFrameRGBA_Real->data, pFrameRGBA_Real->linesize, w,
            h,
            AV_PIX_FMT_RGBA, 4);


    img_convert_ctx = sws_getContext(m_codecCtx->width, m_codecCtx->height, m_codecCtx->pix_fmt,
                                     w, h, AV_PIX_FMT_RGBA,
                                     SWS_FAST_BILINEAR, nullptr, nullptr, nullptr); //


    int re = -10;

    while (1) {
        if (av_read_frame(m_formatCtx, &packet) < 0) {
            re = -10;
            break;
        }

        if (packet.stream_index == videoindex) {
            if (avcodec_send_packet(m_codecCtx, &packet) == 0) {
                if (avcodec_receive_frame(m_codecCtx, m_decodedFrame) != 0) {
                    ret = -1;
                } else {
                    ret = 0;
                }
            } else {
                ret = -1;
            }
            if (ret == 0) {
                sws_scale(img_convert_ctx,
                          (const uint8_t *const *) m_decodedFrame->data,
                          m_decodedFrame->linesize, 0,
                          m_codecCtx->height,
                          pFrameRGBA_Real->data, pFrameRGBA_Real->linesize);

                memcpy(addrPtr, pFrameRGBA_Real->data[0], w * h * 4);
                re = 0;

                break;
            }

        }
    }
    av_freep(&pFrameRGBA_Real->data[0]);
    av_frame_free(&pFrameRGBA_Real);
    sws_freeContext(img_convert_ctx);
    avformat_close_input(&m_formatCtx);
    avformat_free_context(m_formatCtx);
    return re;

}


int naGetVideoThumbnail(JNIEnv *env, const char *sfilename, jobject bitmap) {

    if (nullptr == bitmap) {
        return -1;
    }

    AndroidBitmapInfo info;
    int result;


    //获取图片信息
    result = AndroidBitmap_getInfo(env, bitmap, &info);
    if (result != ANDROID_BITMAP_RESUT_SUCCESS) {
        return -2;
    }

    unsigned char *addrPtr;

    // 获取像素信息
    result = AndroidBitmap_lockPixels(env, bitmap, reinterpret_cast<void **>(&addrPtr));
    if (result != ANDROID_BITMAP_RESULT_SUCCESS) {
        return -3;
    }


    // 执行图片操作的逻辑
    int length = info.stride * info.height;

    int w = info.width;
    int h = info.height;

    int re = F_Get(sfilename, addrPtr, w, h);

//    for (int i = 0; i < length; ++i) {
//        //LOGD("value: %x", addrPtr[i]);
//    }
    // 像素信息不再使用后需要解除锁定

    result = AndroidBitmap_unlockPixels(env, bitmap);
    if (result != ANDROID_BITMAP_RESULT_SUCCESS) {
        return -4;
    }

    return re;

}