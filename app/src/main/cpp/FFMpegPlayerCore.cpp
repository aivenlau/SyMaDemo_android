//
// Created by generalplus_sa1 on 4/29/16.
//
#include <algorithm>
#include <string>
#include <iostream>
#include <cctype>

#include "FFMpegPlayerCore.h"
#include "Defines.h"


#include <jni.h>
#include <unistd.h>
#include <fcntl.h>
//#include <src/main/cpp/h264prase/h264_rbsp.h>
//#include <src/main/cpp/h264prase/sps_parse.h>
#include <sstream>

#include "MyMediaCoder.h"
#include "phone_rl_protocol.h"

//#include "XJH264.h"



#define TAG "VideoFilter"


extern int g_sdkVersion;
extern AVFrame *m3dFrame;
extern AVFrame *m3dFrame_YUV420;
int nCount_fps = -1;
extern int64_t nPreTimeA;
extern bool b3D_noise_reduction;
//extern bool b3D_noise_reduction_bak;
extern int nleftTopDx;
extern int nleftTopDy;

extern int nrightbottomDx;
extern int nrightbottomDy;

bool bNeedChangedRotate = false;
float nFilterRotate = 0;
float nPreAngle = 0;

extern MyMediaCoder myMediaCoder;

//
extern bool bEnableGsensor;
extern bool bEnableCircul;
extern int nCameraDataRota;
extern int nDataType;

extern float nScal;
extern uint8_t nSDStatus;
extern bool bRealRecording;
extern uint32_t nRecFrameCount;
//extern int nFrameCount;

extern int PlatformDisplay();

extern void F_CloseSocket();


int64_t time_out = 0;
int64_t time_out_dat = 5000;

#define OMX_COLOR_FormatYUV420SemiPlanar  21
#define OMX_COLOR_FormatYUV420Planar    19

volatile bool firsttimeplay = true;


using namespace std;

void Adj23D(AVFrame *src, AVFrame *des);

int64_t getCurrentTime() {
    return av_gettime() / 1000;
}


void F_ResetCheckT(int n) {
    time_out_dat = n;
    time_out = getCurrentTime();

}

static int interrupt_cb(void *ctx) {
    int64_t currentTime = getCurrentTime();
    if (time_out_dat == 0)
        return 0;
    int64_t da = currentTime - time_out;

    if (da >= time_out_dat) {
        LOGE("error-------------%ld", (long) time_out_dat);
        return 1;
    }
    return 0;

}

//----------------------------------------------------------------------


int C_FFMpegPlayer::SaveSnapshot(const char *path) {
    if (m_bSaveSnapshot)
        return -1;
    memset(m_snapShotPath, 0, 256);
    strcpy(m_snapShotPath, path);
    m_bSaveSnapshot = true;
    return FFMPEGPLAYER_NOERROR;
}

int C_FFMpegPlayer::GetStatus() {
    return m_Status;
}

C_FFMpegPlayer::C_FFMpegPlayer() :
        m_Status(E_PlayerStatus_Stoped), nRecFps(20), m_decodeThread(-1), m_dispid(-1),
        bNeedDisp(true),bNeedAdd(true),
        m_formatCtx(nullptr), m_videoStream(0), m_codecCtx(nullptr), m_codecCtx_T(nullptr),
        m_decodedFrame(nullptr),
        m_width(0), m_height(0), filter_graph(nullptr), sps(nullptr), pps(nullptr),
        m_bSaveSnapshot(false), m_bSaveVideo(false), pFrameresolution(nullptr),
        m_writeThread(0), m_bOpenOK(false),  //AV_CODEC_ID_MPEG4
        bPause(true), nIC_Type(IC_GK), bGetSps(false),
        frame_c(nullptr),
        m_parser(nullptr), bFlip(false),
        nDisplayWidth(640), nDisplayHeight(360), nNeedRedraw(false), b3D(false),
        frame_b(nullptr), frame_a(nullptr), b480(false), bFollow(false), Rgbabuffer(nullptr),
        bIsH264(false),
        nSpsSet(0), nSecT(0),
        bContinue(true), b3DA(false), nfps(20), nErrorFrame(0), bStatWrite(false),
        YUVbuffer(nullptr), pFrameRecord(nullptr), nRecordWidth(640), pFrameYUV_D(nullptr),
        bMirror(false), pFrameYUV_Real(nullptr), nRecordHeight(360), bAdjFocus(false), nAdjX(0),
        nAdjY(0), buffersrc_ctx(nullptr),
        buffersink_ctx(nullptr), hqdn3d_ctx(nullptr), pFrameYUV_Square(nullptr),
        mMediaCodec(nullptr),
        mColorFormat(-1), mediacoder_frame(nullptr),
        rotate_ctx(nullptr)//,video(MP4_INVALID_TRACK_ID), fileHandle(MP4_INVALID_FILE_HANDLE),
{
    m_snapShotPath[0] = 0;  //= m_VideoPath[0]
    pthread_mutex_init(&m_Frame_Queuelock, nullptr);
    // pthread_cond_init(&m_Frame_condition, nullptr);

    pthread_mutex_init(&m_Rev_Queuelock, nullptr);
    pthread_mutex_init(&m_SetResulotion_mutex_t, nullptr);



    pthread_cond_init(&m_Rev_condition, nullptr);

    //sem_init(&sem_Rec, 0, 0);





    //pthread_mutex_init(&m_Frame_Queuelock_Display, nullptr);
    //pthread_cond_init(&m_Frame_condition_disp, nullptr);

    //sRecordFileName = "";
//    for(int i=0;i<10;i++)
//    {
//        myPacket[i].data = nullptr;
//        myPacket[i].nLen = 0;
//    }
//    myPacketA.data = nullptr;
//    myPacketA.nLen = 0;


#ifdef  D_H264file
    h64fileHandle = -1;
#endif

}
//pCodecCtx(nullptr),
//pFormatCtx(nullptr),
//m_EncodeID(AV_CODEC_ID_MPEG4),
//fmt(nullptr),
//

//nativeWindow(nullptr),

//out_buffer(nullptr),


void C_FFMpegPlayer::F_ReSetRecordWH(int w, int h) {

    pthread_mutex_lock(&m_SetResulotion_mutex_t);


    nRecordWidth = w;
    nRecordHeight = h;
    if (pFrameRecord != nullptr) {
        if(pFrameRecord->width !=w || pFrameRecord->height !=h) {
            av_freep(&pFrameRecord->data[0]);
            av_frame_free(&pFrameRecord);
            pFrameRecord = nullptr;
        }
    }
    if(pFrameRecord== nullptr) {
        pFrameRecord = av_frame_alloc();
        pFrameRecord->format = AV_PIX_FMT_YUV420P;
        pFrameRecord->width = nRecordWidth;
        pFrameRecord->height = nRecordHeight;
        int ret = av_image_alloc(
                pFrameRecord->data, pFrameRecord->linesize, nRecordWidth,
                nRecordHeight,
                AV_PIX_FMT_YUVJ420P, aligD);
    }
    pthread_mutex_unlock(&m_SetResulotion_mutex_t);

}

//----------------------------------------------------------------------
C_FFMpegPlayer::~C_FFMpegPlayer() {
    LOGE("~C_FFMpegPlayer()");
    Stop();
    CloseVideo();
    if (frame_a != nullptr) {
        av_frame_free(&frame_a);
        frame_a = nullptr;
    }
    if (frame_b != nullptr) {
        av_frame_free(&frame_b);
        frame_b = nullptr;
    }

}
//----------------------------------------------------------------------


void C_FFMpegPlayer::F_InitFFfmpeg(void) {
    //avcodec_register_all();
    //av_register_all();
    avformat_network_init();
}

int C_FFMpegPlayer::InitMediaMjpeg(void) {
    AVCodec *pCodec = nullptr;
    AVDictionary *optionsDict = nullptr;
    int err_code = 0;

    if (m_decodedFrame == nullptr) {
        F_InitFFfmpeg();
        bGetSps = false;
        bGetPps = false;
        nBufferedCount = 0;
        av_log_set_level(AV_LOG_QUIET);

        m_decodedFrame = av_frame_alloc();
        AVCodec *codec = avcodec_find_decoder(AV_CODEC_ID_MJPEG);
        m_codecCtx = avcodec_alloc_context3(codec);
        m_codecCtx->codec_id = AV_CODEC_ID_MJPEG;
        m_codecCtx->pix_fmt = AV_PIX_FMT_YUVJ422P;
        m_codecCtx->time_base.den = 1;
        m_codecCtx->time_base.num = 1;
        int ret = avcodec_open2(m_codecCtx, codec, nullptr);
        if (ret != 0) {
            m_bOpenOK = false;
        }
        return 2;
    }

    if (m_codecCtx->width <= 0 || m_codecCtx->height <= 0) {
        return 2;
    }

    if (pFrameYUV_Real != nullptr) {
        if (pFrameYUV_Real->width != m_codecCtx->width ||
            pFrameYUV_Real->height != m_codecCtx->height) {
            av_freep(&pFrameYUV_Real->data[0]);
            av_frame_free(&pFrameYUV_Real);
            pFrameYUV_Real = nullptr;
        } else {
            return 2;
        }

    }


    pix_format = AV_PIX_FMT_YUV420P;

    if (pFrameYUV_Real != nullptr) {
        av_freep(&pFrameYUV_Real->data[0]);
        av_frame_free(&pFrameYUV_Real);
        pFrameYUV_Real = nullptr;
    }

    if (frame_SnapBuffer != nullptr) {
        av_freep(&frame_SnapBuffer->data[0]);
        av_frame_free(&frame_SnapBuffer);
        frame_SnapBuffer = nullptr;
    }
    if (pFrameRecord != nullptr) {
        av_freep(&pFrameRecord->data[0]);
        av_frame_free(&pFrameRecord);
        pFrameRecord = nullptr;
    }

    if (frame_a != nullptr) {
        av_freep(&frame_a->data[0]);
        av_frame_free(&frame_a);
        frame_b = nullptr;
    }
    if (frame_b != nullptr) {
        av_freep(&frame_b->data[0]);
        av_frame_free(&frame_b);
        frame_b = nullptr;
    }

    if (img_convert_ctx != nullptr) {
        sws_freeContext(img_convert_ctx);
        img_convert_ctx = nullptr;
    }


    pFrameYUV_Real = av_frame_alloc();

    frame_SnapBuffer = av_frame_alloc();

    frame_SnapBuffer->format = pix_format;
    frame_SnapBuffer->width = m_codecCtx->width;
    frame_SnapBuffer->height = m_codecCtx->height;
    int ret = av_image_alloc(
            frame_SnapBuffer->data, frame_SnapBuffer->linesize, m_codecCtx->width,
            m_codecCtx->height,
            pix_format, aligD);

    pFrameYUV_Real->format = pix_format;
    pFrameYUV_Real->width = m_codecCtx->width;
    pFrameYUV_Real->height = m_codecCtx->height;
    ret = av_image_alloc(
            pFrameYUV_Real->data, pFrameYUV_Real->linesize, m_codecCtx->width,
            m_codecCtx->height,
            pix_format, aligD);


    if(pFrameRecord!= nullptr) {
        av_freep(&pFrameRecord->data[0]);
        av_frame_free(&pFrameRecord);
    }

    pFrameRecord = av_frame_alloc();
    pFrameRecord->format = pix_format;
    pFrameRecord->width = nRecordWidth;
    pFrameRecord->height = nRecordHeight;
    ret = av_image_alloc(
            pFrameRecord->data, pFrameRecord->linesize, nRecordWidth,
            nRecordHeight,
            pix_format, aligD);


    img_convert_ctx = sws_getContext(m_codecCtx->width, m_codecCtx->height, m_codecCtx->pix_fmt,
                                     m_codecCtx->width, m_codecCtx->height, pix_format,
                                     SWS_FAST_BILINEAR, nullptr, nullptr, nullptr); //





    if (frame_a == nullptr) {
        frame_a = av_frame_alloc();
        frame_a->format = AV_PIX_FMT_YUV420P;
        frame_a->width = m_codecCtx->width;
        frame_a->height = m_codecCtx->height;
        av_image_alloc(frame_a->data, frame_a->linesize, m_codecCtx->width,
                       m_codecCtx->height,
                       AV_PIX_FMT_YUV420P, aligD);
    }


    if (frame_b == nullptr) {
        frame_b = av_frame_alloc();
        frame_b->format = AV_PIX_FMT_YUV420P;
        frame_b->width = m_codecCtx->width / 2;
        frame_b->height = m_codecCtx->height / 2;
        av_image_alloc(frame_b->data, frame_b->linesize, frame_b->width,
                       frame_b->height,
                       AV_PIX_FMT_YUV420P, aligD);
    }
    m_bOpenOK = true;
    return 0;
}


int C_FFMpegPlayer::InitMediaH264(void) {
    AVCodec *pCodec = nullptr;
    AVDictionary *optionsDict = nullptr;
    int err_code = 0;
    m_bOpenOK = false;

    AVDictionary *avCodecOptions = nullptr;

    if (m_decodedFrame == nullptr) {

        bGetSps = false;
        bGetPps = false;
        nBufferedCount = 0;
        F_InitFFfmpeg();
        av_log_set_level(AV_LOG_QUIET);
        m_decodedFrame = av_frame_alloc();
        AVCodec *codec = avcodec_find_decoder(AV_CODEC_ID_H264);
        m_codecCtx_T = avcodec_alloc_context3(codec);
        int ret = avcodec_open2(m_codecCtx_T, nullptr, nullptr);
        m_codecCtx = avcodec_alloc_context3(codec);
        m_codecCtx->thread_count = 2;
        m_codecCtx->thread_type = FF_THREAD_FRAME;
        ret = avcodec_open2(m_codecCtx, nullptr, nullptr);
        m_parser = av_parser_init(AV_CODEC_ID_H264);  //);
        return 2;
    }

    if (m_codecCtx == nullptr) {
        return 2;
    }


    if (m_codecCtx->width <= 0 || m_codecCtx->height <= 0) {
        return 2;
    }

    //LOGE("w = %d h=%d",m_codecCtx->width,m_codecCtx->height);

    if (pFrameYUV_Real != nullptr) {
        if (pFrameYUV_Real->width != m_codecCtx->width ||
            pFrameYUV_Real->height != m_codecCtx->height) {
            av_freep(&pFrameYUV_Real->data[0]);
            av_frame_free(&pFrameYUV_Real);
            pFrameYUV_Real = nullptr;
        } else {
            return 2;
        }

    }


    pix_format = AV_PIX_FMT_YUV420P;

    if (frame_SnapBuffer != nullptr) {
        av_freep(&frame_SnapBuffer->data[0]);
        av_frame_free(&frame_SnapBuffer);
        frame_SnapBuffer = nullptr;
    }
    if (pFrameRecord != nullptr) {
        av_freep(&pFrameRecord->data[0]);
        av_frame_free(&pFrameRecord);
        pFrameRecord = nullptr;
    }

    if (frame_a != nullptr) {
        av_freep(&frame_a->data[0]);
        av_frame_free(&frame_a);
        frame_b = nullptr;
    }
    if (frame_b != nullptr) {
        av_freep(&frame_b->data[0]);
        av_frame_free(&frame_b);
        frame_b = nullptr;
    }

    if (img_convert_ctx != nullptr) {
        sws_freeContext(img_convert_ctx);
        img_convert_ctx = nullptr;
    }

    //if (pFrameYUV != nullptr)
    //    return 0;
    //if (m_codecCtx == nullptr)
    //    return 0;


    pix_format = AV_PIX_FMT_YUV420P;
    //disp_pix_format = AV_PIX_FMT_RGBA;


    pFrameYUV_Real = av_frame_alloc();
    //pFrameRGB = av_frame_alloc();


    frame_SnapBuffer = av_frame_alloc();
    frame_SnapBuffer->format = pix_format;
    frame_SnapBuffer->width = m_codecCtx->width;
    frame_SnapBuffer->height = m_codecCtx->height;
    int ret = av_image_alloc(
            frame_SnapBuffer->data, frame_SnapBuffer->linesize, m_codecCtx->width,
            m_codecCtx->height,
            pix_format, aligD);


    pFrameYUV_Real->format = pix_format;
    pFrameYUV_Real->width = m_codecCtx->width;
    pFrameYUV_Real->height = m_codecCtx->height;
    ret = av_image_alloc(
            pFrameYUV_Real->data, pFrameYUV_Real->linesize, m_codecCtx->width,
            m_codecCtx->height,
            pix_format, aligD);

    /*
    pFrameRGB->format = disp_pix_format;//AV_PIX_FMT_BGR24;
    pFrameRGB->width = m_codecCtx->width;
    pFrameRGB->height = m_codecCtx->height;
    ret = av_image_alloc(
            pFrameRGB->data, pFrameRGB->linesize, nDisplayWidth,
            nDisplayHeight,
            disp_pix_format, aligD);
    */

    pFrameRecord = av_frame_alloc();
    pFrameRecord->format = pix_format;
    pFrameRecord->width = nRecordWidth;
    pFrameRecord->height = nRecordHeight;
    ret = av_image_alloc(
            pFrameRecord->data, pFrameRecord->linesize, nRecordWidth,
            nRecordHeight,
            pix_format, aligD);

    img_convert_ctx = sws_getContext(m_codecCtx->width, m_codecCtx->height, m_codecCtx->pix_fmt,
                                     m_codecCtx->width, m_codecCtx->height, pix_format,
                                     SWS_FAST_BILINEAR, nullptr, nullptr, nullptr); //

    if (frame_a == nullptr) {
        frame_a = av_frame_alloc();
        frame_a->format = AV_PIX_FMT_YUV420P;
        frame_a->width = m_codecCtx->width;
        frame_a->height = m_codecCtx->height;
        av_image_alloc(frame_a->data, frame_a->linesize, m_codecCtx->width,
                       m_codecCtx->height,
                       AV_PIX_FMT_YUV420P, aligD);
    }


    if (frame_b == nullptr) {
        frame_b = av_frame_alloc();
        frame_b->format = AV_PIX_FMT_YUV420P;
        frame_b->width = m_codecCtx->width / 2;
        frame_b->height = m_codecCtx->height / 2;
        av_image_alloc(frame_b->data, frame_b->linesize, frame_b->width,
                       frame_b->height,
                       AV_PIX_FMT_YUV420P, aligD);
    }

    m_bOpenOK = true;


    return 0;
}

bool C_FFMpegPlayer::F_RecreateEnv(void) {
    if (nNeedRedraw) {
        nNeedRedraw = false;
        /*
        if (pFrameRGB != nullptr) {
            av_freep(pFrameRGB->data);
            pFrameRGB->format = AV_PIX_FMT_BGR24;
            pFrameRGB->width = m_codecCtx->width;
            pFrameRGB->height = m_codecCtx->height;
            av_image_alloc(
                    pFrameRGB->data, pFrameRGB->linesize, nDisplayWidth,
                    nDisplayHeight,
                    AV_PIX_FMT_BGR24, aligD);

        }
         */
        /*
        if (img_convert_ctxBmp != nullptr) {
            sws_freeContext(img_convert_ctxBmp);
            img_convert_ctxBmp = sws_getContext(m_codecCtx->width, m_codecCtx->height,
                                                m_codecCtx->pix_fmt,
                                                nDisplayWidth, nDisplayHeight, AV_PIX_FMT_BGR24,
                                                SWS_BILINEAR, nullptr, nullptr, nullptr); //
        }
         */
    }
    return true;
}


extern bool bFilterChanged;
extern bool b3D_noise_reduction;
extern bool bEnableRotateFilter;

int C_FFMpegPlayer::InitMedia(const char *a_path) {

    string spath = a_path;
    transform(spath.begin(), spath.end(), spath.begin(), ::tolower);
    const char *path = spath.c_str();
    int r1 = spath.find("http://");
    int r2 = spath.find("rtsp:");
    int r3 = spath.find("file:");

    if (r1 != 0 && r2 != 0 && r3 != 0) {
        int re = -1;
        if (nDataType == DATA_Type_H264) {
            re = InitMediaH264();
        } else if (nDataType == DATA_Type_MJPEG) {
            re = InitMediaMjpeg();
        }
        if (re >= 0) {
            if (re == 0) {
                bFilterChanged = true;
                bNeedChangedRotate = true;
            }
            if (bFilterChanged) {
                init_filters("4:3:6:3.5", "0");
                LOGA("init_filters");

            }
            F_RecreateEnv();
            return 0;
        }
        return -1;
    }

    if (m_bOpenOK) {
        return 0;
    }
    AVCodec *pCodec = nullptr;
    AVDictionary *optionsDict = nullptr;
    int err_code = 0;
    char buffer[1024];
    buffer[1023] = 0;
    m_bOpenOK = false;
    if (m_formatCtx != nullptr) {
        Stop();
    }

    ClearQueue();

    F_InitFFfmpeg();

    av_log_set_level(AV_LOG_QUIET);

    time_out = getCurrentTime();
    firsttimeplay = true;

    int nVer = avcodec_version();
    m_formatCtx = avformat_alloc_context();
    m_formatCtx->interrupt_callback.callback = interrupt_cb; //--------注册回调函数
    m_formatCtx->interrupt_callback.opaque = m_formatCtx;
    AVDictionary *opts = nullptr;
    AVDictionary *options = nullptr;
    av_dict_set(&options, "rtsp_transport", "tcp", 0);
    F_ResetCheckT(1000 * 10);
    bGetSps = false;
    bGetPps = false;
    nBufferedCount = 0;
    if ((err_code = avformat_open_input(&m_formatCtx, path, nullptr, nullptr)) != 0) {
        avformat_free_context(m_formatCtx);
        m_formatCtx = nullptr;
        return FFMPEGPLAYER_INITMEDIAFAILED;
    }
    strcpy(m_MediaPath, path);
    std::string str(m_MediaPath);
    std::string per = "";
    int is = str.find(':', 0);
    if (is >= 0) {
        per = str.substr(0, is);
    }
    transform(per.begin(), per.end(), per.begin(), ::tolower);
    F_ResetCheckT(5000);
    {
        if (nIC_Type == IC_GPH264) {
            m_formatCtx->probesize = 8 * 1024;
            m_formatCtx->max_analyze_duration = 50 * AV_TIME_BASE;
        } else {
            m_formatCtx->probesize = 30 * 1024;
            m_formatCtx->max_analyze_duration = 5 * AV_TIME_BASE;//100 * AV_TIME_BASE;
        }

        if (avformat_find_stream_info(m_formatCtx, nullptr) < 0) {
            avformat_close_input(&m_formatCtx);
            avformat_free_context(m_formatCtx);
            m_formatCtx = nullptr;
            return FFMPEGPLAYER_INITMEDIAFAILED;
        }
    }

    F_ResetCheckT(5000);
    int videoindex = -1;
    int i = 0;
    for (i = 0; i < m_formatCtx->nb_streams; i++)
        if (m_formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoindex = i;
            m_videoStream = i;
            break;
        }
    if (videoindex == -1) {
        avformat_close_input(&m_formatCtx);
        avformat_free_context(m_formatCtx);
        m_formatCtx = nullptr;
        return -1;
    }

    AVStream *stream = m_formatCtx->streams[videoindex];
    pCodec = avcodec_find_decoder(stream->codecpar->codec_id);

    if (pCodec == nullptr) {
        avformat_close_input(&m_formatCtx);
        avformat_free_context(m_formatCtx);
        m_formatCtx = nullptr;
        return FFMPEGPLAYER_INITMEDIAFAILED; // Codec not found
    }

    m_codecCtx = avcodec_alloc_context3(pCodec);
    avcodec_parameters_to_context(m_codecCtx, stream->codecpar);
    F_ResetCheckT(5000);
    if (avcodec_open2(m_codecCtx, pCodec, &optionsDict) < 0) {
        avformat_close_input(&m_formatCtx);
        avformat_free_context(m_formatCtx);
        m_formatCtx = nullptr;
        return FFMPEGPLAYER_INITMEDIAFAILED;
    }


    if (m_codecCtx->width <= 0 || m_codecCtx->height <= 0) {
        avformat_close_input(&m_formatCtx);
        avformat_free_context(m_formatCtx);
        m_formatCtx = nullptr;
        return FFMPEGPLAYER_INITMEDIAFAILED;
    }


    buffer[1023] = 0;
    m_bOpenOK = false;


    if (m_decodedFrame == nullptr) {
        m_decodedFrame = av_frame_alloc();
    }

    pix_format = AV_PIX_FMT_YUV420P;

    if (pFrameYUV_Real != nullptr) {
        av_freep(&pFrameYUV_Real->data[0]);
        av_frame_free(&pFrameYUV_Real);
        pFrameYUV_Real = nullptr;
    }

    pFrameYUV_Real = av_frame_alloc();
    pFrameYUV_Real->format = pix_format;
    pFrameYUV_Real->width = m_codecCtx->width;
    pFrameYUV_Real->height = m_codecCtx->height;
    int ret = av_image_alloc(
            pFrameYUV_Real->data, pFrameYUV_Real->linesize, m_codecCtx->width,
            m_codecCtx->height,
            pix_format, aligD);

    frame_SnapBuffer = av_frame_alloc();
    frame_SnapBuffer->format = pix_format;
    frame_SnapBuffer->width = m_codecCtx->width;
    frame_SnapBuffer->height = m_codecCtx->height;
    ret = av_image_alloc(
            frame_SnapBuffer->data, frame_SnapBuffer->linesize, m_codecCtx->width,
            m_codecCtx->height,
            pix_format, aligD);

    pFrameRecord = av_frame_alloc();
    pFrameRecord->format = pix_format;
    pFrameRecord->width = nRecordWidth;
    pFrameRecord->height = nRecordHeight;
    ret = av_image_alloc(
            pFrameRecord->data, pFrameRecord->linesize, nRecordWidth,
            nRecordHeight,
            pix_format, aligD);

    img_convert_ctx = sws_getContext(m_codecCtx->width, m_codecCtx->height, m_codecCtx->pix_fmt,
                                     m_codecCtx->width, m_codecCtx->height, pix_format,
                                     SWS_FAST_BILINEAR, nullptr, nullptr, nullptr); //
    if (frame_a == nullptr) {
        frame_a = av_frame_alloc();
        frame_a->format = AV_PIX_FMT_YUV420P;
        frame_a->width = m_codecCtx->width;
        frame_a->height = m_codecCtx->height;
        av_image_alloc(frame_a->data, frame_a->linesize, m_codecCtx->width,
                       m_codecCtx->height,
                       AV_PIX_FMT_YUV420P, aligD);
    }


    if (frame_b == nullptr) {
        frame_b = av_frame_alloc();
        frame_b->format = AV_PIX_FMT_YUV420P;
        frame_b->width = m_codecCtx->width / 2;
        frame_b->height = m_codecCtx->height / 2;
        av_image_alloc(frame_b->data, frame_b->linesize, frame_b->width,
                       frame_b->height,
                       AV_PIX_FMT_YUV420P, aligD);
    }
    m_bOpenOK = true;
    return FFMPEGPLAYER_NOERROR;
}

int C_FFMpegPlayer::PlayMedia() {
    if (nIC_Type == IC_GKA)
        return 0;
    if (nIC_Type == IC_SN) {
        //m_Status = E_PlayerStatus_Playing;
        return 0;
    }
    if (m_Status == E_PlayerStatus_Playing)
        return FFMPEGPLAYER_MEDIAISPLAYING;
    else if (m_Status == E_PlayerStatus_Stoping) {
        void *ret = nullptr;
        pthread_join(m_decodeThread, &ret);
        m_decodeThread = -1;
    }

    if (m_formatCtx == nullptr) {
        int i32Ret = InitMedia(m_MediaPath);
        if (i32Ret != 0)
            return i32Ret;
    }
    if (!m_bOpenOK)
        return -1;

    m_Status = E_PlayerStatus_Playing;
    F_ResetCheckT(0);
    pthread_create(&m_decodeThread, nullptr, decodeThreadFunction, (void *) this);
    LOGE_B("m_decodeThread = %d", (int) m_decodeThread);
    return FFMPEGPLAYER_NOERROR;
}

//----------------------------------------------------------------------
int C_FFMpegPlayer::Stop(bool bStopVideo) {
#ifdef D_H264file
    if(h64fileHandle!=-1)
        {
            close(h64fileHandle);
            h64fileHandle = -1;
        }
#endif

    if (m_Status != E_PlayerStatus_Playing)
        return 0;
    m_Status = E_PlayerStatus_Stoping;
    if (bStopVideo) {
        m_bSaveVideo = false;
    }
    usleep(1000 * 10);
    if (m_decodeThread != -1) {
        pthread_join(m_decodeThread, nullptr);
        m_decodeThread = -1;
    }
    if (m_dispid != -1) {
        pthread_join(m_dispid, nullptr);
        m_dispid = -1;
    }
    F_ResetCheckT(10);
    //usleep(1000 * 20);
    //if (nIC_Type == IC_GKA || nIC_Type == IC_SN || nIC_Type == IC_GPRTP || nIC_Type == IC_GPH264A || nIC_Type == IC_GPRTPB || nIC_Type == IC_GK_UDP)
    if (nIC_Type == IC_GPH264 ||
        nIC_Type == IC_GPRTSP) { ;
    } else {
        Releaseffmpeg();
        LOGE("111");
    }
    m_bOpenOK = false;
    return FFMPEGPLAYER_NOERROR;
}

//----------------------------------------------------------------------
extern pthread_mutex_t m_mp4Write_lock;

int C_FFMpegPlayer::WriteMp4Frame(uint8_t *data, int nLen, bool b) {

#if 0
    if (nLen == 0)
        return -1;
    int ds;
    ds = 90000 / nRecFps;


    if (fileHandle != MP4_INVALID_FILE_HANDLE && video != MP4_INVALID_TRACK_ID) {
        int nL = nLen - 4;
        data[0] = (uint8_t) ((nL & 0xFF000000) >> 24);
        data[1] = (uint8_t) ((nL & 0x00FF0000) >> 16);
        data[2] = (uint8_t) ((nL & 0x0000FF00) >> 8);
        data[3] = (uint8_t) ((nL & 0x000000FF));
        {
            pthread_mutex_lock(&m_mp4Write_lock);
            MP4WriteSample(fileHandle, video, data, (uint32_t) nLen, (MP4Duration) ds, 0, b);
            pthread_mutex_unlock(&m_mp4Write_lock);
        }
        return 0;
    }
#endif
    return -1;
}

#define NUM_ADTS_SAMPLING_RATES    16
uint32_t AdtsSamplingRates[NUM_ADTS_SAMPLING_RATES] = {
        96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050,
        16000, 12000, 11025, 8000, 7350, 0, 0, 0
};

uint8_t C_FFMpegPlayer::MP4AdtsFindSamplingRateIndex(uint32_t samplingRate) {
    uint8_t i;
    for (i = 0; i < NUM_ADTS_SAMPLING_RATES; i++) {
        if (samplingRate == AdtsSamplingRates[i]) {
            return i;
        }
    }
    return NUM_ADTS_SAMPLING_RATES - 1;
}

bool C_FFMpegPlayer::MP4AacGetConfiguration(uint8_t **ppConfig,
                                            uint32_t *pConfigLength,
                                            uint8_t profile,
                                            uint32_t samplingRate,
                                            uint8_t channels) {
    /* create the appropriate decoder config */

    uint8_t *pConfig = (uint8_t *) malloc(2);

    if (pConfig == nullptr) {
        return false;
    }

    uint8_t samplingRateIndex = MP4AdtsFindSamplingRateIndex(samplingRate);

    pConfig[0] = (uint8_t) (((profile) << 3) | ((samplingRateIndex & 0xe) >> 1));
    pConfig[1] = (uint8_t) (((samplingRateIndex & 0x1) << 7) | (channels << 3));


    *ppConfig = pConfig;
    *pConfigLength = 2;

    return true;
}

//extern bool bG_Audio;

int C_FFMpegPlayer::AddMp4Video(uint8_t *sps, int len1, uint8_t *pps, int len2) {

#if 0
    if (fileHandle != MP4_INVALID_FILE_HANDLE) {
        video = MP4AddH264VideoTrack(fileHandle, 90000, (uint32_t) (90000 / nRecFps),
                                     (uint16_t) nRecordWidth, (uint16_t) nRecordHeight, sps[1],
                                     sps[2], sps[3], 3);

        if (video == MP4_INVALID_TRACK_ID) {
            MP4Close(fileHandle, 0);
            fileHandle = MP4_INVALID_FILE_HANDLE;
            return -1;
        } else {
            MP4AddH264SequenceParameterSet(fileHandle, video, sps, (uint16_t) len1);
            MP4AddH264PictureParameterSet(fileHandle, video, pps, (uint16_t) len2);
            MP4SetVideoProfileLevel(fileHandle, sps[3]);  //0x1F;
            LOGE("SPS PPS");
        }


        if (bG_Audio) {

            uint32_t samplesPerSecond;
            uint8_t profile;
            uint8_t channelConfig;

            /*
            0: Null
            1: AAC Main
            2: AAC LC (Low Complexity)
            3: AAC SSR (Scalable Sample Rate)
            4: AAC LTP (Long Term Prediction)
            5: SBR (Spectral Band Replication)
            6: AAC Scalable
            */

            music = MP4AddAudioTrack(fileHandle, 44100, 1024, MP4_MPEG4_AUDIO_TYPE);
            MP4SetAudioProfileLevel(fileHandle, 0x0F);
            samplesPerSecond = 44100;
            profile = 2; // AAC LC
            channelConfig = 1;
            uint8_t *pConfig = nullptr;
            uint32_t configLength = 0;
            MP4AacGetConfiguration(&pConfig, &configLength, profile, samplesPerSecond,
                                   channelConfig);
            MP4SetTrackESConfiguration(fileHandle, music, pConfig, configLength);
            free(pConfig);
        }

        return 0;
    }
#endif
    return -1;

}

bool F_SetRecordAudio(int n);

int C_FFMpegPlayer::SaveVideo(const char *path, bool bisH264) {

    bisH264 = true;
    bRealRecording = false;
    nRecFrameCount = 0;

//    if (bG_Audio)
//    {
//        if(!F_SetRecordAudio(1)) //因为权限问题，可能无法录音
//            bG_Audio = false;
//    } else {
//        F_SetRecordAudio(0);
//    }

//    music = MP4_INVALID_TRACK_ID;
    bStatWrite = false;
    this->bIsH264 = bisH264;

//    sRecordFileName = path;
//    sRecordFileName_tmp = path;
//    sRecordFileName_tmp = sRecordFileName_tmp + "_.tmp";
    if (m_codecCtx == nullptr) {
        this->bIsH264 = false;
//        fileHandle = MP4_INVALID_FILE_HANDLE;
        return -1;
    }
    if (m_codecCtx->width <= 0 || m_codecCtx->height <= 0) {
        this->bIsH264 = false;
//        fileHandle = MP4_INVALID_FILE_HANDLE;
        return -1;
    }


    {
#if 0
        if (fileHandle != MP4_INVALID_FILE_HANDLE) {
            MP4Close(fileHandle, 0);
            fileHandle = MP4_INVALID_FILE_HANDLE;
        }

        video = MP4_INVALID_TRACK_ID;
        //创建mp4文件
        fileHandle = MP4Create(sRecordFileName_tmp.c_str(), 0);
        if (fileHandle == MP4_INVALID_FILE_HANDLE) {
            return -1;
        }

        MP4SetTimeScale(fileHandle, 90000);
#endif
        pthread_create(&m_writeThread, nullptr, WriteThreadFunction, this);
        LOGE_B("m_writeThread = %d", (int) m_writeThread);


        return 0;
    }
    return FFMPEGPLAYER_NOERROR;
}


//----------------------------------------------------------------------
void C_FFMpegPlayer::ClearQueue() {
    pthread_mutex_lock(&m_Frame_Queuelock);
    //sem_wait(&sem_Rec);
    while (m_FrameQueue_H264.size()) {
        MyFrame *fa = m_FrameQueue_H264.front();
        m_FrameQueue_H264.pop();
        delete fa;
    }
    pthread_mutex_unlock(&m_Frame_Queuelock);
}


//----------------------------------------------------------------------
void *C_FFMpegPlayer::decodeThreadFunction(void *param) {
    C_FFMpegPlayer *pPlayer = (C_FFMpegPlayer *) param;

    pPlayer->decodeAndRender();

    return FFMPEGPLAYER_NOERROR;
}

//----------------------------------------------------------------------

void *C_FFMpegPlayer::WriteThreadFunction(void *param) {
    C_FFMpegPlayer *pPlayer = (C_FFMpegPlayer *) param;
    pPlayer->writeVideo();
    return nullptr;
}


//----------------------------------------------------------------------

void F_ResetRelinker();

char typeOfNalu(char *data) {
    char first = *data;
    return (char) (first & 0x1f);
}

uint16_t ixxxx = 0;


int64_t nPre = 0;


bool C_FFMpegPlayer::F_WriteAudio(jbyte *data, int nLen) {
#if 0
    if (m_bSaveVideo && fileHandle != MP4_INVALID_FILE_HANDLE && music != MP4_INVALID_TRACK_ID) {
        bool bre;
        pthread_mutex_lock(&m_mp4Write_lock);
        bre = MP4WriteSample(fileHandle, music, (const uint8_t *) data, (uint32_t) nLen,
                             MP4_INVALID_DURATION, 0, 1);
        pthread_mutex_unlock(&m_mp4Write_lock);
        return bre;
    } else {
        return false;
    }
#else

    return false;
#endif
}

//void C_FFMpegPlayer::F_DispH264NoBuffer(MySocketData *data) {
//    {
//        bCanDisplay = false;
//        decodeAndRender_GKA_B(data);
//    }
//}


//extern int64_t disp_no;
//extern int64_t start_time;
extern int nDelayDisplayT;

//extern FILE *testFile;
//GKA,GPH264A
string getstring(const int n) {
    std::stringstream newstr;
    newstr << n;
    return newstr.str();
}


int64_t TT1 = 0;
int64_t TT2 = 0;

int F_FindNaul(char *pData, int *pnDataLen, int *ResultBuffer, int *ResultBuffer_Len,
               int *nResult_Len, bool bHeader);


void C_FFMpegPlayer::DispPaket(void *data) {

}


void *DecordPacket(void *data) {
    auto self = (C_FFMpegPlayer *) data;
    //pthread_detach(pthread_self());
    while (self->m_Status == E_PlayerStatus_Playing) {
        pthread_mutex_lock(&(self->m_Rev_Queuelock));

        if (self->m_RevFrameQueue.size() > 0)
        {
            AVPacket *packetA = (AVPacket *) self->m_RevFrameQueue.front();
            self->m_RevFrameQueue.pop();
            if (self->m_RevFrameQueue.size() > 2)
            {
                self->bNeedDisp = false;
            } else {
                self->bNeedDisp = true;
            }
            pthread_mutex_unlock(&(self->m_Rev_Queuelock));

            if (!self->bNeedDisp) {
                if (b3D_noise_reduction) {
                    bFilterChanged = true;
                    b3D_noise_reduction = false;
                    //b3D_noise_reduction_bak = false;
                }
            }
            int ret = -1;
            if (avcodec_send_packet(self->m_codecCtx, packetA) == 0) {
                if ((b3D_noise_reduction || bEnableRotateFilter) &&
                    self->buffersrc_ctx != nullptr) {
                    if (avcodec_receive_frame(self->m_codecCtx, m3dFrame) != 0) {
                        ret = -1;
                    } else {
                        ret = 0;
                    }
                } else {
                    if (avcodec_receive_frame(self->m_codecCtx, self->m_decodedFrame) != 0) {
                        ret = -1;
                    } else {
                        ret = 0;
                    }
                }
            } else {
                ret = -1;
            }

            //usleep(1000*80);

            delete[]packetA->data;
            //delete packetA;
            //av_packet_free_side_data(packetA);
            av_packet_free(&packetA);



            if (ret == 0) {
                self->InitMedia("");
                if (bNeedChangedRotate) {
                    bNeedChangedRotate = false;
                    if (bEnableGsensor) {
                        self->F_SetRotate(nFilterRotate);
                    }
                }
                if ((b3D_noise_reduction || bEnableRotateFilter) &&
                    self->buffersrc_ctx != nullptr) {
                    ret = av_buffersrc_add_frame_flags(self->buffersrc_ctx, m3dFrame,
                                                       AV_BUFFERSRC_FLAG_KEEP_REF);
                    if (ret >= 0) {
                        ret = av_buffersink_get_frame(self->buffersink_ctx, self->m_decodedFrame);
                        if (ret >= 0)
                            ret = 0;
                    }
                }
                if (ret == 0) {
                    self->_DispDecordData();
                    av_frame_unref(self->m_decodedFrame);
                    if ((b3D_noise_reduction || bEnableRotateFilter) && self->buffersrc_ctx != nullptr) {
                        av_frame_unref(m3dFrame);
                    }
                }
                //int64_t T2 = av_gettime() / 1000;
//                LOGE("Decode t = %d",T2-T1);
            }
        } else {
            pthread_mutex_unlock(&(self->m_Rev_Queuelock));
            usleep(10000);
        }


    }

    while (self->m_RevFrameQueue.size()) {
        AVPacket *fa = (AVPacket *) self->m_RevFrameQueue.front();
        delete[]fa->data;
        delete fa;
        self->m_RevFrameQueue.pop();
    }

    LOGE("Exit DispThread");

    return nullptr;
}





int C_FFMpegPlayer::DecodeH264(MySocketData *data) {


    F_ResetRelinker();
    if (m_Status != E_PlayerStatus_Playing)
        return 0;
    if (data->nLen < 4)
        return 0;


    int size = 0;
    int len;
    int in_len = (int) data->nLen;
    uint8_t *outbuff = nullptr;
    uint8_t *in_data;
    in_data = data->data;
    int ret = 0;
    while (in_len) {
        len = av_parser_parse2(m_parser, m_codecCtx_T, &outbuff, &size, in_data, in_len, 0, 0, 0);
        in_data += len;
        in_len -= len;
        if (size > 0) {

#if 0

            AVPacket packetA = {0};
            av_init_packet(&packetA);
            packetA.data = outbuff;
            packetA.size = size;

            int nTy = outbuff[4]&0x1F;
//            LOGE(" profile id = %d",nTy);
//            if(nTy == 0x07)
//            {
////                packetA.data[5]= 66;
//
//            }
            if((!bGetSps || !bGetPps) && (nTy==0x07 || nTy == 0x08))
            {

                int ResultArray[100];
                int ResultArray_Len[100];
                int nResult;
                Byte *data_;
                int dataLen_;
                int nLen1 = size;
                int aa = F_FindNaul((char *)outbuff, &nLen1, ResultArray, ResultArray_Len,&nResult,false);

                int offset;
                char nType;
                char *pData = (char *)outbuff;

                for (int i = 0; i < aa; i++)
                {
                    offset = ResultArray[i] + 4;
                    nType = (char) (pData[offset] & 0x1F);
                    data_ = (Byte *) pData + ResultArray[i];
                    dataLen_ = ResultArray_Len[i];
                    if (nType == 7 && !bGetSps) {
                        bGetSps = true;
                        spsLen = dataLen_;//ResultArray_Len[i];
                        if(sps!= nullptr)
                        {
                            delete []sps;
                            sps = nullptr;
                        }
                        sps = new Byte[spsLen];
                        memcpy(sps, data_ + 4, spsLen);
                        h264_parse_sps((const unsigned char *)sps,(unsigned int)spsLen,&spsinfo);
                        bGetSps = true;
                    }
                    if (nType == 8 && !bGetPps) {
                        bGetPps = true;
                        ppsLen = dataLen_;

                        if(pps!= nullptr)
                        {
                            delete []pps;
                            pps = nullptr;
                        }
                        pps = new Byte[ppsLen];
                        memcpy(pps, data_ + 4, ppsLen);
                    }
                    if(bGetPps && bGetSps)
                    {
                        F_InitMediaCodecDecord(spsinfo.width,spsinfo.height,sps,spsLen,pps,ppsLen);
                        break;
                    }
                }
            }
            if(nTy==7)
            {
                packetA.data = outbuff+spsLen+ppsLen+8;
                packetA.size = size-spsLen-ppsLen-8;
            }

            if(bGetPps && bGetSps) {
                F_DecorderData((char *) (packetA.data), (int) (packetA.size));
            }
            av_packet_unref(&packetA);
#else
#if 1

            AVPacket *p = av_packet_alloc();
            av_init_packet(p);
            p->data = new uint8_t[size];
            p->size = size;
            memcpy(p->data, outbuff, p->size);

            pthread_mutex_lock(&m_Rev_Queuelock);
            {
                bool  bIPacket = ((p->data[4]& 0x1f) != 1);
                if(bIPacket) {
                    bNeedAdd = true;
                }

                if (m_RevFrameQueue.size() > 20)
                {
                    bNeedAdd = false;
                    while (m_RevFrameQueue.size()) {
                        AVPacket *fa = (AVPacket *)m_RevFrameQueue.front();
                        delete[]fa->data;
                        av_packet_free(&fa);
                        //delete fa;
                        m_RevFrameQueue.pop();
                    }
                    LOGE("Packet type miss %d",p->data[4]& 0x1f);
                }
                else {
                    if(bNeedAdd)
                    {
                        LOGE("Packet type =  %d",p->data[4]& 0x1f);
                        m_RevFrameQueue.push(p);
                    }
                }
            }
            pthread_mutex_unlock(&(m_Rev_Queuelock));

#else

            AVPacket packetA = {0};
            av_init_packet(&packetA);
            packetA.data = outbuff;
            packetA.size = size;

            if (avcodec_send_packet(m_codecCtx, &packetA) == 0) {
                if ((b3D_noise_reduction || bEnableRotateFilter) && buffersrc_ctx != nullptr) {
                    if (avcodec_receive_frame(m_codecCtx, m3dFrame) != 0) {
                        ret = -1;
                    } else {
                        ret = 0;
                    }
                } else {
                    if (avcodec_receive_frame(m_codecCtx, m_decodedFrame) != 0) {
                        ret = -1;
                    } else {
                        ret = 0;
                    }
                }
            } else {
                ret = -1;
            }
            if (ret == 0)
            {
                InitMedia("");
                if (bNeedChangedRotate) {
                    bNeedChangedRotate = false;
                    if (bEnableGsensor) {
                        F_SetRotate(nFilterRotate);
                    }
                }

                if ((b3D_noise_reduction || bEnableRotateFilter) && buffersrc_ctx != nullptr) {
                    ret = av_buffersrc_add_frame_flags(buffersrc_ctx, m3dFrame,
                                                       AV_BUFFERSRC_FLAG_KEEP_REF);
                    if (ret >= 0) {
                        ret = av_buffersink_get_frame(buffersink_ctx, m_decodedFrame);
                        if (ret >= 0)
                            ret = 0;
                    }
                }

                if (ret == 0) {
                    _DispDecordData();
                    av_frame_unref(m_decodedFrame);
                    if ((b3D_noise_reduction || bEnableRotateFilter) && buffersrc_ctx != nullptr)
                        av_frame_unref(m3dFrame);
                }
            }
            av_packet_unref(&packetA);
#endif
#endif


        }
    }

    return 0;

}


int F_Decord_JAVA(uint8_t *data, int nLen);


void *DecordPacket(void *data);

void C_FFMpegPlayer::F_StartDispThread() {

    m_Status = E_PlayerStatus_Playing;

    int ret = pthread_create(&m_dispid, nullptr, DecordPacket, (void *) this); // 成功返回0，错误返回错误编号
    LOGE_B("m_dispid = %d", (int) m_dispid);
    if (ret != 0) {
        m_dispid = -1;
    }


}

int F_SendKey2Jave(int nKey);

extern jbyte TestInfo[1024];

int F_SentTestInfo(void);

//extern uint8_t nICType;

//int F_GetH264Frame(byte *data,int nLen);

#if 0
int sps_prase(char *i_sps, int sps_length, int *pwidth, int *pheight) {
    if (!i_sps || sps_length <= 0)
        return -1;

    char rbspbuffer[512] = {0,};
    rbsp spsrsbp(i_sps, sps_length, rbspbuffer, 512);
    int bufl = spsrsbp.torbsp();
    sps tempsps((unsigned char *) rbspbuffer, bufl);
    tempsps.sps_parse();
    *pwidth = (int) tempsps.getWidth();
    *pheight = (int) tempsps.getHeight();
    int fps = tempsps.get_separate_colour_plane_flag();
    return 0;

    /*
    unsigned char frame_mbs_only_flag = tempsps.get_frame_mbs_only_flag();
    unsigned int log2_max_frame_num_minus4 = tempsps.get_log2_max_frame_num_minus4();
    unsigned int pic_order_cnt_type = tempsps.get_pic_order_cnt_type();
    unsigned int num_units_in_tick = tempsps.get_num_units_in_tick();
    unsigned int time_scale = tempsps.get_time_scale();
    unsigned char separate_colour_plane_flag = tempsps.get_separate_colour_plane_flag();
    unsigned int log2_max_pic_order_cnt_lsb_minus4 = tempsps.get_log2_max_pic_order_cnt_lsb_minus4();
    printf("----------------------------------------------------------------\n");
    printf("@@            frame_mbs_only_flag =%d             \n",frame_mbs_only_flag);
    printf("@@            log2_max_frame_num_minus4 =%d             \n",log2_max_frame_num_minus4);
    printf("@@            pic_order_cnt_type =%d             \n",pic_order_cnt_type);
    printf("@@            num_units_in_tick =%d             \n",num_units_in_tick);
    printf("@@            time_scale =%d             \n",time_scale);
    printf("@@            separate_colour_plane_flag =%d             \n",separate_colour_plane_flag);
    printf("@@            log2_max_pic_order_cnt_lsb_minus4 =%d             \n",log2_max_pic_order_cnt_lsb_minus4);
    printf("----------------------------------------------------------------\n");
     */
}
#endif
//extern bool bInitMedia;

int64_t iTime = 0;

int nPIndex = 0;
int nAddCount = 0;

int Head_size = sizeof(T_NET_FRAME_HEADER);


void C_FFMpegPlayer::F_AdjDarkcorner() {
    if (nleftTopDx != 0 || nleftTopDy != 0 || nrightbottomDx != 0 || nrightbottomDy != 0) {

        if (pFrameYUV_Real == nullptr) {
            return;
        }
        if (pFrameYUV_Real->width <= 0 || pFrameYUV_Real->height <= 0) {
            return;
        }


        int lx = nleftTopDx;
        lx = (lx + 1) / 2;
        lx *= 2;

        int ly = nleftTopDy;
        ly = (ly + 1) / 2;
        ly *= 2;

        nleftTopDx = lx;
        nleftTopDy = ly;


        int W1 = pFrameYUV_Real->width;
        int H1 = pFrameYUV_Real->height;
        int W = W1 - nleftTopDx - nrightbottomDx;
        int H = H1 - nleftTopDy - nrightbottomDy;
        if (W < 0 || H < 0)
            return;
        if (W < W1 || H < H1)
            return;

        AVFrame *tmpFrame = av_frame_alloc();
        tmpFrame->format = AV_PIX_FMT_YUV420P;
        tmpFrame->width = W;
        tmpFrame->height = H;
        av_image_alloc(
                tmpFrame->data, tmpFrame->linesize, tmpFrame->width,
                tmpFrame->height,
                AV_PIX_FMT_YUV420P, aligD);

        int length = (pFrameYUV_Real->width * pFrameYUV_Real->height * 3) / 2;
        int nSize = tmpFrame->width;
        int ab = libyuv::ConvertToI420(pFrameYUV_Real->data[0], length,
                                       tmpFrame->data[0], tmpFrame->linesize[0],
                                       tmpFrame->data[1], tmpFrame->linesize[1],
                                       tmpFrame->data[2], tmpFrame->linesize[2],
                                       lx, ly,
                                       W1, H1,
                                       W, H,
                                       libyuv::kRotate0, libyuv::FOURCC_I420);

        libyuv::I420Scale(tmpFrame->data[0], tmpFrame->linesize[0],
                          tmpFrame->data[1], tmpFrame->linesize[1],
                          tmpFrame->data[2], tmpFrame->linesize[2],
                          tmpFrame->width, tmpFrame->height,
                          pFrameYUV_Real->data[0], pFrameYUV_Real->linesize[0],
                          pFrameYUV_Real->data[1], pFrameYUV_Real->linesize[1],
                          pFrameYUV_Real->data[2], pFrameYUV_Real->linesize[2],
                          pFrameYUV_Real->width, pFrameYUV_Real->height,
                          libyuv::kFilterLinear);

        av_freep(&tmpFrame->data[0]);
        av_frame_free(&tmpFrame);

    }

}

void C_FFMpegPlayer::F_MakeCircul(AVFrame *frame) {
#if 0
    int height = frame->height;

    if (bEnableCircul) {

        if(maskImg.nHeight !=frame->height || maskImg.nWidth !=frame->width) {
            maskImg.MakeMaskImg(frame->width, frame->height);
        }

        uint8_t *pY = frame->data[0];
        uint8_t *pU = frame->data[1];
        uint8_t *pV = frame->data[2];
        int d0 = frame->linesize[0];
        int d1 = frame->linesize[1];
        int d2 = frame->linesize[2];
        int nLen = 0;
        int WW;
        for (int y = 0; y < height; y++) {
            nLen = maskImg.pLeftLen[y];
            if (nLen > 0) {
                memset(pY, 0, (size_t) (nLen));
                memset(pU, 128, (size_t) (nLen >> 1));
                memset(pV, 128, (size_t) (nLen >> 1));
                WW = maskImg.pRiggtLen[y];

                memset(pY + WW, 0, (size_t) (nLen));
                memset(pU + WW / 2, 128, (size_t) (nLen >> 1));
                memset(pV + WW / 2, 128, (size_t) (nLen >> 1));
                pY += d0;
                if (y & 1) {
                    pU += d1;
                    pV += d2;
                }
            }
        }
    }
#endif

    int height = frame->height;
    int width = frame->width;

    if (bEnableCircul) {
        int W1, H1;
        W1 = width * 2;
        H1 = height * 2;
        AVFrame *myframe = av_frame_alloc();
        myframe->width = W1;
        myframe->height = H1;
        av_image_alloc(myframe->data, myframe->linesize, W1,
                       H1,
                       AV_PIX_FMT_YUV420P, 4);


        libyuv::I420Scale(frame->data[0], frame->linesize[0],
                          frame->data[1], frame->linesize[1],
                          frame->data[2], frame->linesize[2],
                          frame->width, frame->height,
                          myframe->data[0], myframe->linesize[0],
                          myframe->data[1], myframe->linesize[1],
                          myframe->data[2], myframe->linesize[2],
                          myframe->width, myframe->height, libyuv::kFilterNone);


        if (maskImg.nHeight != myframe->height || maskImg.nWidth != myframe->width) {
            maskImg.MakeMaskImg(myframe->width, myframe->height);
        }


        height = myframe->height;

        uint8_t *pY = myframe->data[0];
        uint8_t *pU = myframe->data[1];
        uint8_t *pV = myframe->data[2];

        int d0 = myframe->linesize[0];
        int d1 = myframe->linesize[1];
        int d2 = myframe->linesize[2];
        int nLen = 0;

        int WW;

        for (int y = 0; y < height; y++) {
            nLen = maskImg.pLeftLen[y];
            if (nLen > 0) {
                memset(pY, 0, (size_t) (nLen));
                memset(pU, 128, (size_t) (nLen >> 1));
                memset(pV, 128, (size_t) (nLen >> 1));
                WW = maskImg.pRiggtLen[y];
                memset(pY + WW, 0, (size_t) (nLen));
                memset(pU + WW / 2, 128, (size_t) (nLen >> 1));
                memset(pV + WW / 2, 128, (size_t) (nLen >> 1));
                pY += d0;
                if (y & 1) {
                    pU += d1;
                    pV += d2;
                }
            }
        }

        libyuv::I420Scale(myframe->data[0], myframe->linesize[0],
                          myframe->data[1], myframe->linesize[1],
                          myframe->data[2], myframe->linesize[2],
                          myframe->width, myframe->height,
                          frame->data[0], frame->linesize[0],
                          frame->data[1], frame->linesize[1],
                          frame->data[2], frame->linesize[2],
                          frame->width, frame->height, libyuv::kFilterBox);

        av_freep(&myframe->data[0]);
        av_frame_free(&myframe);

    }

}

//int C_FFMpegPlayer::decodeAndRender_GKA_B(MySocketData *data) {
//
//    F_ResetRelinker();
//    if (data->nLen < 4)
//        return 0;
//#if 1
//    int size = 0;
//    int len;
//    int in_len = (int) data->nLen;
//    uint8_t *outbuff = nullptr;
//    uint8_t *in_data;
//    in_data = data->data;
//    int ret = 0;
//    while (in_len) {
//        len = av_parser_parse2(m_parser, m_codecCtx, &outbuff, &size, in_data, in_len, 0, 0, 0);
//        in_data += len;
//        in_len -= len;
//        if (size > 0) {
//#if 1
//            AVPacket packetA = {0};
//            av_init_packet(&packetA);
//            packetA.data = outbuff;
//            packetA.size = size;
//            if (avcodec_send_packet(m_codecCtx, &packetA) == 0) {
//                if ((b3D_noise_reduction || bEnableRotateFilter) && buffersrc_ctx != nullptr) {
//                    if (avcodec_receive_frame(m_codecCtx, m3dFrame) != 0) {
//                        ret = -1;
//                    } else {
//                        ret = 0;
//                    }
//                } else {
//                    if (avcodec_receive_frame(m_codecCtx, m_decodedFrame) != 0) {
//                        ret = -1;
//                    } else {
//                        ret = 0;
//                    }
//                }
//            } else {
//                ret = -1;
//            }
//            if (ret == 0) {
//                InitMedia("");
//                if (bNeedChangedRotate) {
//                    bNeedChangedRotate = false;
//                    if (bEnableGsensor) {
//                        F_SetRotate(nFilterRotate);
//                    }
//                }
//
//                if ((b3D_noise_reduction || bEnableRotateFilter) && buffersrc_ctx != nullptr) {
//                    ret = av_buffersrc_add_frame_flags(buffersrc_ctx, m3dFrame,
//                                                       AV_BUFFERSRC_FLAG_KEEP_REF);
//                    if (ret >= 0) {
//                        ret = av_buffersink_get_frame(buffersink_ctx, m_decodedFrame);
//                        if (ret >= 0)
//                            ret = 0;
//                    }
//                }
//
//                if (ret == 0) {
//                    _DispDecordData();
//                    av_frame_unref(m_decodedFrame);
//                    if ((b3D_noise_reduction || bEnableRotateFilter) && buffersrc_ctx != nullptr)
//                        av_frame_unref(m3dFrame);
//                }
//            }
//            av_packet_unref(&packetA);
//#endif
//        }
//    }
//    return 0;
//
//#else
//
//#endif
//}

extern bool bSquare;

void C_FFMpegPlayer::_DispDecordData(void) {
    F_RecreateEnv();

    if (m_decodedFrame == nullptr)
        return;


    if (m_decodedFrame->format == AV_PIX_FMT_YUVJ420P ||
        m_decodedFrame->format == AV_PIX_FMT_YUV420P) {
        libyuv::I420Copy(m_decodedFrame->data[0], m_decodedFrame->linesize[0],
                         m_decodedFrame->data[1], m_decodedFrame->linesize[1],
                         m_decodedFrame->data[2], m_decodedFrame->linesize[2],
                         pFrameYUV_Real->data[0], pFrameYUV_Real->linesize[0],
                         pFrameYUV_Real->data[1], pFrameYUV_Real->linesize[1],
                         pFrameYUV_Real->data[2], pFrameYUV_Real->linesize[2],
                         m_decodedFrame->width, m_decodedFrame->height);
    } else if (m_decodedFrame->format == AV_PIX_FMT_YUV422P ||
               m_decodedFrame->format == AV_PIX_FMT_YUVJ422P) {
        libyuv::I422ToI420(m_decodedFrame->data[0], m_decodedFrame->linesize[0],
                           m_decodedFrame->data[1], m_decodedFrame->linesize[1],
                           m_decodedFrame->data[2], m_decodedFrame->linesize[2],
                           pFrameYUV_Real->data[0], pFrameYUV_Real->linesize[0],
                           pFrameYUV_Real->data[1], pFrameYUV_Real->linesize[1],
                           pFrameYUV_Real->data[2], pFrameYUV_Real->linesize[2],
                           m_decodedFrame->width, m_decodedFrame->height);
    } else {
        if (img_convert_ctx != nullptr) {
            sws_scale(img_convert_ctx,
                      (const uint8_t *const *) m_decodedFrame->data,
                      m_decodedFrame->linesize, 0,
                      m_codecCtx->height,
                      pFrameYUV_Real->data, pFrameYUV_Real->linesize);
        }
    }

    AVFrame *avFrame = nullptr;
    int www1 = pFrameYUV_Real->width;
    int hhh1 = pFrameYUV_Real->height;
    if (nCameraDataRota == 90 || nCameraDataRota == 270)  //并且转了 90 或者 270 度
    {
        www1 = pFrameYUV_Real->height;
        hhh1 = pFrameYUV_Real->width;

    }

    avFrame = av_frame_alloc();
    avFrame->format = AV_PIX_FMT_YUV420P;
    avFrame->width = www1;
    avFrame->height = hhh1;
    av_image_alloc(
            avFrame->data, avFrame->linesize, avFrame->width,
            avFrame->height,
            AV_PIX_FMT_YUV420P, aligD);


    if (nCameraDataRota != 0) {
        int rot = libyuv::kRotate0;
        if (nCameraDataRota == 90) {
            rot = libyuv::kRotate90;
        } else if (nCameraDataRota == 180) {
            rot = libyuv::kRotate180;
        } else if (nCameraDataRota == 270) {
            rot = libyuv::kRotate270;
        }
        libyuv::I420Rotate(pFrameYUV_Real->data[0], pFrameYUV_Real->linesize[0],
                           pFrameYUV_Real->data[1], pFrameYUV_Real->linesize[1],
                           pFrameYUV_Real->data[2], pFrameYUV_Real->linesize[2],
                           avFrame->data[0], avFrame->linesize[0],
                           avFrame->data[1], avFrame->linesize[1],
                           avFrame->data[2], avFrame->linesize[2],
                           pFrameYUV_Real->width, pFrameYUV_Real->height,
                           (libyuv::RotationMode) rot);

    } else {
        libyuv::I420Copy(pFrameYUV_Real->data[0], pFrameYUV_Real->linesize[0],
                         pFrameYUV_Real->data[1], pFrameYUV_Real->linesize[1],
                         pFrameYUV_Real->data[2], pFrameYUV_Real->linesize[2],
                         avFrame->data[0], avFrame->linesize[0],
                         avFrame->data[1], avFrame->linesize[1],
                         avFrame->data[2], avFrame->linesize[2],
                         avFrame->width, avFrame->height);
    }


    if (bSquare) {
        int ww = (avFrame->width > avFrame->height) ? (avFrame->height) : (avFrame->width);
        ww = ((ww) / 4);
        ww *= 4;
        if (pFrameYUV_Square != nullptr) {
            if (pFrameYUV_Square->width != ww || pFrameYUV_Square->height != ww) {
                av_freep(&pFrameYUV_Square->data[0]);
                av_frame_free(&pFrameYUV_Square);
                pFrameYUV_Square = nullptr;
            }
        }
        if (pFrameYUV_Square == nullptr) {
            pFrameYUV_Square = av_frame_alloc();
            pFrameYUV_Square->format = AV_PIX_FMT_YUV420P;
            pFrameYUV_Square->width = ww;
            pFrameYUV_Square->height = ww;
            av_image_alloc(
                    pFrameYUV_Square->data, pFrameYUV_Square->linesize, pFrameYUV_Square->width,
                    pFrameYUV_Square->height,
                    AV_PIX_FMT_YUV420P, aligD);
        }

        int lx, ly;

        lx = (avFrame->width - ww) / 2;
        ly = (avFrame->height - ww) / 2;


        size_t length = ((size_t) (avFrame->width) * (size_t) (avFrame->height * 3)) / 2;
        int nSize = avFrame->width;
        libyuv::ConvertToI420(avFrame->data[0], length,
                              pFrameYUV_Square->data[0], pFrameYUV_Square->linesize[0],
                              pFrameYUV_Square->data[1], pFrameYUV_Square->linesize[1],
                              pFrameYUV_Square->data[2], pFrameYUV_Square->linesize[2],
                              lx, ly,
                              avFrame->width, avFrame->height,
                              pFrameYUV_Square->width, pFrameYUV_Square->height,
                              libyuv::kRotate0, libyuv::FOURCC_I420);
        F_MakeCircul(pFrameYUV_Square);
        SetYUVFrame(pFrameYUV_Square);
    } else {

            pthread_mutex_lock(&m_SetResulotion_mutex_t);
            int  nRecordWidth_rel = nRecordWidth;
            int  nRecordHeight_rel = nRecordHeight;

            if(nRecordWidth<nRecordHeight)
            {  //竖屏录制显示
                if (nCameraDataRota ==0 || nCameraDataRota == 180)
                {
                    nRecordWidth_rel = nRecordHeight;
                    nRecordHeight_rel = nRecordWidth;
                }
            }
            else if(nRecordWidth>nRecordHeight) {
                if (nCameraDataRota ==90 || nCameraDataRota == 270)
                {
                    nRecordWidth_rel = nRecordHeight;
                    nRecordHeight_rel = nRecordWidth;
                }
            }

            int W1 = avFrame->width;
            int H1 = avFrame->height;
            int W2 = W1;
            int H2 = H2;

            int da1 = (int) ((((float) W1) / H1) * 10);
            int da2 = (int) ((((float) nRecordWidth_rel) / nRecordHeight_rel) * 10);
            float d1 = ((float) W1) / H1;
            float d2 = ((float) nRecordWidth_rel) / nRecordHeight_rel;

            bool ba1 = false;
            if (da2 > da1)
            {
                ba1 = true;
                W2 = W1;
                H2 = (int) (W2 / d2);
                H2 = ((H2 + 3) / 4) * 4;
            } else if (da2 < da1) {
                ba1 = true;
                H2 = H1;
                W2 = (int) (H2 * d2);
                W2 = ((W2 + 3) / 4) * 4;
            }
            if (W2 == W1 && H2 == H1) {
                ba1 = false;
            }
            if (ba1) {
                if (pFrameresolution != nullptr) {
                    if (pFrameresolution->width != W2 || pFrameresolution->height != H2) {
                        av_freep(&pFrameresolution->data[0]);
                        av_frame_free(&pFrameresolution);
                        pFrameresolution = nullptr;
                    }
                }
                if (pFrameresolution == nullptr) {
                    pFrameresolution = av_frame_alloc();
                    pFrameresolution->format = AV_PIX_FMT_YUV420P;
                    pFrameresolution->width = W2;
                    pFrameresolution->height = H2;
                    av_image_alloc(pFrameresolution->data, pFrameresolution->linesize,
                                   pFrameresolution->width, pFrameresolution->height,
                                   AV_PIX_FMT_YUV420P, aligD);
                }
                int ww = W2;
                ww = ((ww) / 4);
                ww *= 4;
                int hh = H2;
                hh = ((hh) / 4);
                hh *= 4;
                int lx, ly;
                lx = (avFrame->width - ww) / 2;
                ly = (avFrame->height - hh) / 2;
                size_t length =
                        ((size_t) (avFrame->width) * (size_t) (avFrame->height * 3)) / 2;
                libyuv::ConvertToI420(avFrame->data[0], length,
                                      pFrameresolution->data[0], pFrameresolution->linesize[0],
                                      pFrameresolution->data[1], pFrameresolution->linesize[1],
                                      pFrameresolution->data[2], pFrameresolution->linesize[2],
                                      lx, ly,
                                      avFrame->width, avFrame->height,
                                      pFrameresolution->width, pFrameresolution->height,
                                      libyuv::kRotate0, libyuv::FOURCC_I420);

                F_MakeCircul(pFrameresolution);
                SetYUVFrame(pFrameresolution);
               // LOGE("wq = %d H = %d",pFrameresolution->width,pFrameresolution->height);
            } else
            {
                F_MakeCircul(avFrame);
                SetYUVFrame(avFrame);
                //LOGE("wq = %d HA = %d",avFrame->width,avFrame->height);
            }
        pthread_mutex_unlock(&m_SetResulotion_mutex_t);

    }

    if (avFrame != nullptr) {
        av_freep(&avFrame->data[0]);
        av_frame_free(&avFrame);
    }

}


//int C_FFMpegPlayer::decodeAndRender_RTP(char *data, int nLen) {
//
//
//    return DecodeMjpeg(data, nLen);
//
//}

//GPH264

int C_FFMpegPlayer::DecodeMjpeg(char *data, int nLen) {
    if (nLen <= 0) {
        return 0;
    }

    uint8_t *in_data = (uint8_t *) data;// new uint8_t[in_len];
    int ret;
    AVPacket packet = {0};
    av_init_packet(&packet);
    packet.data = in_data;
    packet.size = nLen;
    bool bChange = false;
    if (m_codecCtx == nullptr)
        return 0;
    m_Status = E_PlayerStatus_Playing;

#if 1
    if (avcodec_send_packet(m_codecCtx, &packet) == 0) {
        //InitMedia("");
        if ((b3D_noise_reduction || bEnableRotateFilter) && buffersrc_ctx != nullptr) {
            if (avcodec_receive_frame(m_codecCtx, m3dFrame) != 0) {
                ret = -1;
            } else {
                if (m_decodedFrame != nullptr) {
                    if (m_decodedFrame->width == 0) {
                        m_decodedFrame->width = m3dFrame->width;
                        m_decodedFrame->height = m3dFrame->height;
                    }
                }
                if (m_codecCtx->pix_fmt == AV_PIX_FMT_YUVJ422P) {
                    if (m3dFrame_YUV420 != nullptr) {
                        if (m3dFrame_YUV420->width != m3dFrame->width ||
                            m3dFrame_YUV420->height != m3dFrame->height) {
                            av_freep(&m3dFrame_YUV420->data[0]);
                            av_frame_free(&m3dFrame_YUV420);
                            m3dFrame_YUV420 = nullptr;
                        }
                    }
                    if (m3dFrame_YUV420 == nullptr) {
                        m3dFrame_YUV420 = av_frame_alloc();
                        m3dFrame_YUV420->format = AV_PIX_FMT_YUV420P;
                        m3dFrame_YUV420->width = m3dFrame->width;
                        m3dFrame_YUV420->height = m3dFrame->height;
                        av_image_alloc(
                                m3dFrame_YUV420->data, m3dFrame_YUV420->linesize,
                                m3dFrame_YUV420->width,
                                m3dFrame_YUV420->height,
                                AV_PIX_FMT_YUV420P, aligD);

                    }
                    libyuv::I422ToI420(m3dFrame->data[0], m3dFrame->linesize[0],
                                       m3dFrame->data[1], m3dFrame->linesize[1],
                                       m3dFrame->data[2], m3dFrame->linesize[2],
                                       m3dFrame_YUV420->data[0], m3dFrame_YUV420->linesize[0],
                                       m3dFrame_YUV420->data[1], m3dFrame_YUV420->linesize[1],
                                       m3dFrame_YUV420->data[2], m3dFrame_YUV420->linesize[2],
                                       m3dFrame_YUV420->width, m3dFrame_YUV420->height);
                    bChange = true;
                }

                ret = 0;
            }
        } else {
            if (avcodec_receive_frame(m_codecCtx, m_decodedFrame) != 0) {
                ret = -1;
            } else {
                ret = 0;
            }
        }
    } else {
        ret = -1;
    }
    if (ret == 0) {
        //InitMedia("");
        InitMedia("");
        if (bNeedChangedRotate) {
            bNeedChangedRotate = false;
            if (bEnableGsensor) {
                F_SetRotate(nFilterRotate);
            }
        }
        if ((b3D_noise_reduction || bEnableRotateFilter) && buffersrc_ctx != nullptr) {
            if (!bChange) {
                ret = av_buffersrc_add_frame_flags(buffersrc_ctx, m3dFrame,
                                                   AV_BUFFERSRC_FLAG_KEEP_REF);
            } else {
                ret = av_buffersrc_add_frame_flags(buffersrc_ctx, m3dFrame_YUV420,
                                                   AV_BUFFERSRC_FLAG_KEEP_REF);
            }
            if (ret >= 0) {
                ret = av_buffersink_get_frame(buffersink_ctx, m_decodedFrame);
                if (ret >= 0)
                    ret = 0;
            }
        }


        if (ret == 0) {
            //m_Status = E_PlayerStatus_Playing;
            _DispDecordData();

            av_frame_unref(m_decodedFrame);
            if ((b3D_noise_reduction || bEnableRotateFilter) && buffersrc_ctx != nullptr)
                av_frame_unref(m3dFrame);
        }
    }
    av_packet_unref(&packet);

#else

    if (avcodec_send_packet(m_codecCtx, &packet) == 0) {

        if(b3D_noise_reduction && buffersrc_ctx!= nullptr){
            if (avcodec_receive_frame(m_codecCtx, m3dFrame) != 0) {
                ret = -1;
            } else {
                ret = 0;
            }
        } else {
            if (avcodec_receive_frame(m_codecCtx, m_decodedFrame) != 0) {
                ret = -1;
            } else {
                ret = 0;
            }
        }


    } else {
        ret = -1;
    }

    if (ret == 0) {
        InitMedia("");


        m_Status = E_PlayerStatus_Playing;

        if (b3D_noise_reduction && buffersrc_ctx!= nullptr) {

            ret = av_buffersrc_add_frame_flags(buffersrc_ctx, m3dFrame,
                                               AV_BUFFERSRC_FLAG_KEEP_REF);
            if (ret >= 0) {
                ret = av_buffersink_get_frame(buffersink_ctx, m_decodedFrame);
                if(ret>=0)
                    ret = 0;
            }
        }
        if (ret == 0) {


         //   _DispDecordData();
            av_frame_unref(m_decodedFrame);
            if(b3D_noise_reduction && buffersrc_ctx!= nullptr)
            {
                av_frame_unref(m3dFrame);
            }

//            if (m_decodedFrame->format == AV_PIX_FMT_YUVJ420P ||
//                m_decodedFrame->format == AV_PIX_FMT_YUV420P) {
//                libyuv::I420Copy(m_decodedFrame->data[0], m_decodedFrame->linesize[0],
//                                 m_decodedFrame->data[1], m_decodedFrame->linesize[1],
//                                 m_decodedFrame->data[2], m_decodedFrame->linesize[2],
//                                 pFrameYUV_Real->data[0], pFrameYUV_Real->linesize[0],
//                                 pFrameYUV_Real->data[1], pFrameYUV_Real->linesize[1],
//                                 pFrameYUV_Real->data[2], pFrameYUV_Real->linesize[2],
//                                 m_decodedFrame->width, m_decodedFrame->height);
//            } else if (m_decodedFrame->format == AV_PIX_FMT_YUV422P ||
//                       m_decodedFrame->format == AV_PIX_FMT_YUVJ422P) {
//                libyuv::I422ToI420(m_decodedFrame->data[0], m_decodedFrame->linesize[0],
//                                   m_decodedFrame->data[1], m_decodedFrame->linesize[1],
//                                   m_decodedFrame->data[2], m_decodedFrame->linesize[2],
//                                   pFrameYUV_Real->data[0], pFrameYUV_Real->linesize[0],
//                                   pFrameYUV_Real->data[1], pFrameYUV_Real->linesize[1],
//                                   pFrameYUV_Real->data[2], pFrameYUV_Real->linesize[2],
//                                   m_decodedFrame->width, m_decodedFrame->height);
//            } else {
//                if (img_convert_ctx != nullptr) {
//                    sws_scale(img_convert_ctx,
//                              (const uint8_t *const *) m_decodedFrame->data,
//                              m_decodedFrame->linesize, 0,
//                              m_codecCtx->height,
//                              pFrameYUV_Real->data, pFrameYUV_Real->linesize);
//                }
//
//
//            }
        }
        //SetYUVFrame(pFrameYUV_Real);
    }

    av_packet_unref(&packet);
#endif
    return 0;

}

#define coordinate_value(YData, x0, y0, w)    (*((YData) + (x0) + (y0) * w))

void F_GetEvaluation(int n);

int brenner_cpu(int w, int h, uint8_t *yData, int ww) {
    int x0, y0, x1, y1;
    double score = 0;
    x0 = 0;
    y0 = 0;
    x1 = w;
    y1 = h;
    int value;
    //for(int xx = x0;xx < x1 - 2;xx++)
    for (int xx = x0; xx < x1; xx++) {
        for (int yy = y0; yy < y1 - 2; yy++) {
            value = coordinate_value(yData, xx, (yy + 2), ww) - coordinate_value(yData, xx, yy, ww);
            score += pow(value / 255.0, 2.0);
        }
    }
    return (int) score;
}


int C_FFMpegPlayer::SetYUVFrame(AVFrame *yunframe) {
    if (yunframe->width <= 0 || yunframe->height <= 0) {
        return -1;
    }
    int www1 = yunframe->width;
    int hhh1 = yunframe->height;

    if (pFrameYUV != nullptr) {
        if (pFrameYUV->width != www1 || pFrameYUV->height != hhh1) {
            av_freep(&pFrameYUV->data[0]);
            av_frame_free(&pFrameYUV);
            pFrameYUV = nullptr;
        }
    }

    if (pFrameYUV == nullptr) {
        pFrameYUV = av_frame_alloc();
        pFrameYUV->format = AV_PIX_FMT_YUV420P;
        pFrameYUV->width = www1;
        pFrameYUV->height = hhh1;
        av_image_alloc(
                pFrameYUV->data, pFrameYUV->linesize, pFrameYUV->width,
                pFrameYUV->height,
                AV_PIX_FMT_YUV420P, aligD);
    }

    libyuv::I420Copy(yunframe->data[0], yunframe->linesize[0],
                     yunframe->data[1], yunframe->linesize[1],
                     yunframe->data[2], yunframe->linesize[2],
                     pFrameYUV->data[0], pFrameYUV->linesize[0],
                     pFrameYUV->data[1], pFrameYUV->linesize[1],
                     pFrameYUV->data[2], pFrameYUV->linesize[2],
                     yunframe->width, yunframe->height);


    nDisplayWidth = pFrameYUV->width;
    nDisplayHeight = pFrameYUV->height;


    if (bAdjFocus) {
        bAdjFocus = false;
        //调焦用
        int hh = pFrameYUV->height < pFrameYUV->width ? pFrameYUV->height : pFrameYUV->width;
        int WWWW = hh / 4;
        WWWW = ((WWWW + 1) / 2) * 2;

        AVFrame *adjFrame = av_frame_alloc();
        adjFrame->format = AV_PIX_FMT_YUV420P;
        adjFrame->width = WWWW;
        adjFrame->height = WWWW;
        int ret = av_image_alloc(
                adjFrame->data, adjFrame->linesize, adjFrame->width,
                adjFrame->height,
                AV_PIX_FMT_YUV420P, aligD);

        if (nAdjX < WWWW / 2) {
            nAdjX = WWWW / 2;
        }
        if (nAdjY < WWWW / 2) {
            nAdjY = WWWW / 2;
        }
        if (pFrameYUV->width - nAdjX < WWWW / 2) {
            nAdjX = pFrameYUV->width - (WWWW / 2);
        }
        if (pFrameYUV->height - nAdjY < (WWWW / 2)) {
            nAdjY = pFrameYUV->height - (WWWW / 2);
        }

        int cx = nAdjX;
        int cy = nAdjY;

        int lx = cx - (adjFrame->width / 2);
        lx = (lx + 1) / 2;
        lx *= 2;

        int ly = cy - (adjFrame->height / 2);
        ly = (ly + 1) / 2;
        ly *= 2;


        int length = (pFrameYUV->width * pFrameYUV->height * 3) / 2;
        int nSize = adjFrame->width;
        libyuv::ConvertToI420(pFrameYUV->data[0], length,
                              adjFrame->data[0], adjFrame->linesize[0],
                              adjFrame->data[1], adjFrame->linesize[1],
                              adjFrame->data[2], adjFrame->linesize[2],
                              lx, ly,
                              pFrameYUV->width, pFrameYUV->height,
                              adjFrame->width, adjFrame->height,
                              libyuv::kRotate0, libyuv::FOURCC_I420);

        int nEvaluation = brenner_cpu(WWWW, WWWW, adjFrame->data[0], adjFrame->linesize[0]);
        F_GetEvaluation(nEvaluation);
        av_freep(&(adjFrame->data[0]));
        av_frame_free(&adjFrame);
    }


    int ret = 0;
    int nSS = (int) (nScal * 100);
    if (nSS > 100) {
        float fScal = nScal;
        int nSW = (int) (pFrameYUV->width * fScal);
        nSW = ((nSW + 7) / 8) * 8;
        int nSH = (int) (pFrameYUV->height * fScal);

        if (pFrameYUV_D != nullptr) {
            if (pFrameYUV_D->width != nSW || pFrameYUV_D->height != nSH) {
                av_freep(&pFrameYUV_D->data[0]);
                av_frame_free(&pFrameYUV_D);
                pFrameYUV_D = nullptr;
            }
        }

        if (pFrameYUV_D == nullptr) {
            pFrameYUV_D = av_frame_alloc();
            pFrameYUV_D->format = AV_PIX_FMT_YUV420P;
            pFrameYUV_D->width = nSW;
            pFrameYUV_D->height = nSH;
            ret = av_image_alloc(
                    pFrameYUV_D->data, pFrameYUV_D->linesize, pFrameYUV_D->width,
                    pFrameYUV_D->height,
                    AV_PIX_FMT_YUV420P, aligD);
        }

        libyuv::I420Scale(pFrameYUV->data[0], pFrameYUV->linesize[0],
                          pFrameYUV->data[1], pFrameYUV->linesize[1],
                          pFrameYUV->data[2], pFrameYUV->linesize[2],
                          pFrameYUV->width, pFrameYUV->height,
                          pFrameYUV_D->data[0], pFrameYUV_D->linesize[0],
                          pFrameYUV_D->data[1], pFrameYUV_D->linesize[1],
                          pFrameYUV_D->data[2], pFrameYUV_D->linesize[2],
                          pFrameYUV_D->width, pFrameYUV_D->height,
                          libyuv::kFilterLinear);


        int cx = pFrameYUV_D->width / 2;
        int cy = pFrameYUV_D->height / 2;

        int lx = cx - (pFrameYUV->width / 2);
        lx = (lx + 1) / 2;
        lx *= 2;

        int ly = cy - (pFrameYUV->height / 2);
        ly = (ly + 1) / 2;
        ly *= 2;


        int length = (pFrameYUV_D->width * pFrameYUV_D->height * 3) / 2;
        int nSize = pFrameYUV->width;
        libyuv::ConvertToI420(pFrameYUV_D->data[0], length,
                              pFrameYUV->data[0], pFrameYUV->linesize[0],
                              pFrameYUV->data[1], pFrameYUV->linesize[1],
                              pFrameYUV->data[2], pFrameYUV->linesize[2],
                              lx, ly,
                              pFrameYUV_D->width, pFrameYUV_D->height,
                              pFrameYUV->width, pFrameYUV->height,
                              libyuv::kRotate0, libyuv::FOURCC_I420);
    }


    if (frame_SnapBuffer != nullptr) {
        if (frame_SnapBuffer->width != pFrameYUV->width ||
            frame_SnapBuffer->height != pFrameYUV->height) {
            av_freep(&frame_SnapBuffer->data[0]);
            av_frame_free(&frame_SnapBuffer);
            frame_SnapBuffer = nullptr;
        }
    }
    if (frame_SnapBuffer == nullptr) {
        frame_SnapBuffer = av_frame_alloc();
        frame_SnapBuffer->format = AV_PIX_FMT_YUV420P;
        frame_SnapBuffer->width = pFrameYUV->width;
        frame_SnapBuffer->height = pFrameYUV->height;
        av_image_alloc(
                frame_SnapBuffer->data, frame_SnapBuffer->linesize, pFrameYUV->width,
                pFrameYUV->height,
                AV_PIX_FMT_YUV420P, aligD);
    }

    if (bMirror) {
        if (frame_a != nullptr) {
            if (frame_a->width != pFrameYUV->width || frame_a->height != pFrameYUV->height) {
                av_freep(&frame_a->data[0]);
                av_frame_free(&frame_a);
                frame_a = nullptr;
            }
        }

        if (frame_a == nullptr) {
            frame_a = av_frame_alloc();
            frame_a->format = AV_PIX_FMT_YUV420P;
            frame_a->width = pFrameYUV->width;
            frame_a->height = pFrameYUV->height;
            av_image_alloc(frame_a->data, frame_a->linesize, pFrameYUV->width,
                           pFrameYUV->height,
                           AV_PIX_FMT_YUV420P, aligD);
        }
        libyuv::I420Mirror(pFrameYUV->data[0], pFrameYUV->linesize[0],
                           pFrameYUV->data[1], pFrameYUV->linesize[1],
                           pFrameYUV->data[2], pFrameYUV->linesize[2],
                           frame_a->data[0], frame_a->linesize[0],
                           frame_a->data[1], frame_a->linesize[1],
                           frame_a->data[2], frame_a->linesize[2],
                           frame_a->width, frame_a->height);

        libyuv::I420Copy(frame_a->data[0], frame_a->linesize[0],
                         frame_a->data[1], frame_a->linesize[1],
                         frame_a->data[2], frame_a->linesize[2],
                         pFrameYUV->data[0], frame_a->linesize[0],
                         pFrameYUV->data[1], frame_a->linesize[1],
                         pFrameYUV->data[2], frame_a->linesize[2],
                         frame_a->width, frame_a->height);


    }

    if (bFlip) {

        if (frame_a != nullptr) {
            if (frame_a->width != pFrameYUV->width || frame_a->height != pFrameYUV->height) {
                av_freep(&frame_a->data[0]);
                av_frame_free(&frame_a);
                frame_a = nullptr;
            }
        }

        if (frame_a == nullptr) {
            frame_a = av_frame_alloc();
            frame_a->format = AV_PIX_FMT_YUV420P;
            frame_a->width = pFrameYUV->width;
            frame_a->height = pFrameYUV->height;
            av_image_alloc(frame_a->data, frame_a->linesize, pFrameYUV->width,
                           pFrameYUV->height,
                           AV_PIX_FMT_YUV420P, aligD);
        }

        libyuv::I420Rotate(pFrameYUV->data[0], pFrameYUV->linesize[0],
                           pFrameYUV->data[1], pFrameYUV->linesize[1],
                           pFrameYUV->data[2], pFrameYUV->linesize[2],
                           frame_a->data[0], frame_a->linesize[0],
                           frame_a->data[1], frame_a->linesize[1],
                           frame_a->data[2], frame_a->linesize[2],
                           frame_a->width, frame_a->height, libyuv::kRotate180);


        libyuv::I420Copy(frame_a->data[0], frame_a->linesize[0],
                         frame_a->data[1], frame_a->linesize[1],
                         frame_a->data[2], frame_a->linesize[2],
                         pFrameYUV->data[0], frame_a->linesize[0],
                         pFrameYUV->data[1], frame_a->linesize[1],
                         pFrameYUV->data[2], frame_a->linesize[2],
                         frame_a->width, frame_a->height);

    }

    if (b3D) {

        if (frame_b != nullptr) {
            if (frame_b->width != pFrameYUV->width || frame_b->height != pFrameYUV->height) {
                av_freep(&frame_b->data[0]);
                av_frame_free(&frame_b);
                frame_b = nullptr;
            }
        }

        if (frame_b == nullptr) {
            frame_b = av_frame_alloc();
            frame_b->format = AV_PIX_FMT_YUV420P;
            frame_b->width = pFrameYUV->width / 2;
            frame_b->height = pFrameYUV->height / 2;
            av_image_alloc(frame_b->data, frame_b->linesize, pFrameYUV->width / 2,
                           pFrameYUV->height / 2,
                           AV_PIX_FMT_YUV420P, aligD);
        }

        libyuv::I420Scale(pFrameYUV->data[0], pFrameYUV->linesize[0],
                          pFrameYUV->data[1], pFrameYUV->linesize[1],
                          pFrameYUV->data[2], pFrameYUV->linesize[2],
                          pFrameYUV->width, pFrameYUV->height,
                          frame_b->data[0], frame_b->linesize[0],
                          frame_b->data[1], frame_b->linesize[1],
                          frame_b->data[2], frame_b->linesize[2],
                          frame_b->width, frame_b->height,
                          libyuv::kFilterLinear);
        Adj23D(frame_b, pFrameYUV);
    }


    PlatformDisplay();
    return 0;

}

//extern int nGetData_;
void onGetVideoData(void);

int C_FFMpegPlayer::decodeAndRender() {
    if (nIC_Type == IC_SN)
        return 0;
    int frameFinished = 0;
    AVPacket packet;
    int ret = 0;
    time_out = getCurrentTime();
    firsttimeplay = true;
    F_ResetCheckT(0);

    while (m_Status == E_PlayerStatus_Playing) {
        if (av_read_frame(m_formatCtx, &packet) < 0)
            break;
        time_out = getCurrentTime();
        firsttimeplay = true;
        onGetVideoData();
        if (packet.stream_index == m_videoStream) {
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

                if (pFrameYUV_Real != nullptr) {
                    if (pFrameYUV_Real->width != m_codecCtx->width ||
                        pFrameYUV_Real->height != m_codecCtx->height) {
                        av_freep(&pFrameYUV_Real->data[0]);
                        av_frame_free(&pFrameYUV_Real);
                        pFrameYUV_Real = nullptr;
                    }
                }
                if (pFrameYUV_Real == nullptr) {
                    pFrameYUV_Real = av_frame_alloc();
                    pFrameYUV_Real->format = AV_PIX_FMT_YUV420P;
                    pFrameYUV_Real->width = m_codecCtx->width;
                    pFrameYUV_Real->height = m_codecCtx->height;
                    av_image_alloc(
                            pFrameYUV_Real->data, pFrameYUV_Real->linesize, pFrameYUV_Real->width,
                            pFrameYUV_Real->height,
                            AV_PIX_FMT_YUV420P, aligD);
                }

                if (m_decodedFrame->format == AV_PIX_FMT_YUVJ420P ||
                    m_decodedFrame->format == AV_PIX_FMT_YUV420P) {
                    libyuv::I420Copy(m_decodedFrame->data[0], m_decodedFrame->linesize[0],
                                     m_decodedFrame->data[1], m_decodedFrame->linesize[1],
                                     m_decodedFrame->data[2], m_decodedFrame->linesize[2],
                                     pFrameYUV_Real->data[0], pFrameYUV_Real->linesize[0],
                                     pFrameYUV_Real->data[1], pFrameYUV_Real->linesize[1],
                                     pFrameYUV_Real->data[2], pFrameYUV_Real->linesize[2],
                                     m_decodedFrame->width, m_decodedFrame->height);
                } else if (m_decodedFrame->format == AV_PIX_FMT_YUV422P ||
                           m_decodedFrame->format == AV_PIX_FMT_YUVJ422P) {
                    libyuv::I422ToI420(m_decodedFrame->data[0], m_decodedFrame->linesize[0],
                                       m_decodedFrame->data[1], m_decodedFrame->linesize[1],
                                       m_decodedFrame->data[2], m_decodedFrame->linesize[2],
                                       pFrameYUV_Real->data[0], pFrameYUV_Real->linesize[0],
                                       pFrameYUV_Real->data[1], pFrameYUV_Real->linesize[1],
                                       pFrameYUV_Real->data[2], pFrameYUV_Real->linesize[2],
                                       m_decodedFrame->width, m_decodedFrame->height);
                } else {
                    if (img_convert_ctx != nullptr) {
                        sws_scale(img_convert_ctx,
                                  (const uint8_t *const *) m_decodedFrame->data,
                                  m_decodedFrame->linesize, 0,
                                  m_codecCtx->height,
                                  pFrameYUV_Real->data, pFrameYUV_Real->linesize);
                    }
                }
                SetYUVFrame(pFrameYUV_Real);
            }
        }
        av_packet_unref(&packet);
    }
    m_bOpenOK = false;
    F_CloseSocket();
    m_bSaveVideo = false;
    Releaseffmpeg();
    m_Status = E_PlayerStatus_Stoped;
    return FFMPEGPLAYER_NOERROR;
}

void F_OnSave2ToGallery_mid(int i);

//----------------------------------------------------------------------
int C_FFMpegPlayer::StopSaveVideo() {
    void *ret = nullptr;
    //sem_post(&sem_Rec);
    m_bSaveVideo = false;
    bRealRecording = false;
    usleep(1000 * 50);
    F_SetRecordAudio(0);
    if (m_writeThread != 0) {
        pthread_join(m_writeThread, &ret);
        m_writeThread = 0;
    }



//    if (pthread_mutex_trylock(&m_Frame_Queuelock) == 0) {
//        pthread_cond_signal(&m_Frame_condition);
//        pthread_mutex_unlock(&m_Frame_Queuelock);
//    }

#if 0
    if (bIsH264) {
        MP4Close(fileHandle, 0);
        fileHandle = MP4_INVALID_FILE_HANDLE;
        video = MP4_INVALID_TRACK_ID;
        music = MP4_INVALID_TRACK_ID;
    }

    if (access(sRecordFileName_tmp.c_str(), F_OK) == 0) {
        rename(sRecordFileName_tmp.c_str(), sRecordFileName.c_str());
        F_OnSave2ToGallery_mid(1);
    }
#else
    //F_OnSave2ToGallery_mid(1);
#endif

#ifdef  D_H264file
    if (h64fileHandle != -1) {
        close(h64fileHandle);
        h64fileHandle = -1;
    }
#endif
    return FFMPEGPLAYER_NOERROR;
}

void F_SentRevYUV(int32_t wh);


#define LOGE_A(...) __android_log_print(ANDROID_LOG_ERROR ,"ffmpegJNI—",__VA_ARGS__)
//extern  XJH264 *xjh264;



int C_FFMpegPlayer::writeVideo() {
    m_bSaveVideo = true;
    int64_t T1 = av_gettime() / 1000;
    int64_t T2 = av_gettime() / 1000;
    int nDelay = 1000 / nRecFps;
    int nCount = 0;
    if (bIsH264) {
        int nLen = 0;
        MyFrame *pMyFrame = nullptr;
        bool bNeedDelete = false;
        while (m_bSaveVideo) //&& m_Status == E_PlayerStatus_Playing)
        {
            pMyFrame = nullptr;
            struct timespec timespec1;
            timespec1.tv_sec = 0;
            timespec1.tv_nsec = 1000 * 1000 * 5;
            pthread_mutex_lock(&m_Frame_Queuelock);

            //pthread_cond_timedwait(&m_Frame_condition, &m_Frame_Queuelock, &timespec1);
            if (!m_FrameQueue_H264.empty()) {
                pMyFrame = m_FrameQueue_H264.front();
                if (m_FrameQueue_H264.size() > 1) {
                    m_FrameQueue_H264.pop();
                    bNeedDelete = true;
                } else {
                    bNeedDelete = false;
                }
            }
            pthread_mutex_unlock(&m_Frame_Queuelock);
            if (pMyFrame != nullptr) {
                myMediaCoder.offerEncoder(pMyFrame->data, pMyFrame->nLen);
                if (bNeedDelete)
                    delete pMyFrame;
#if 0
                T1 = av_gettime() / 1000;
#else
                T2 = av_gettime() / 1000;
                int daa = (int) (T2 - T1);
//                LOGE("Delay encord time  1 = %d  %d",daa,nDelay);
                if (daa < nDelay) {
                    usleep((useconds_t) ((nDelay - daa) * 1000));
                    //LOGE("Delay encord time  2 = %d",(nDelay - daa) );
                } else {
                    //LOGE("Delay encord time  3");
                }
                T1 = av_gettime() / 1000;
#endif
            } else {
                usleep(10 * 1000); //delay 10ms  让出CPU给其他线程
                //LOGE("Delay encord");
            }

        }
        CloseVideo();
        LOGE("Exit Write Frame!");
        return 0;
    }

    return FFMPEGPLAYER_NOERROR;
}


//----------------------------------------------------------------------


int C_FFMpegPlayer::CloseVideo() {
    ClearQueue();
    return FFMPEGPLAYER_NOERROR;
}


int nSteppp = 0;

void F_SentRevBmp(int32_t wh);

void F_SentGestureBmp(int32_t wh);

extern int nBufferLen;
int encord_colorformat;

extern int nHightQuqlity;

void C_FFMpegPlayer::F_Adj_Resolution(void) const {

    float d1 = (float) ((pFrameYUV->width) / (float) (pFrameYUV->height));
    float d2 = (float)(nRecordWidth / (float)(nRecordHeight));
    int da1 = (int) (d1 * 10);
    int da2 = (int) (d2 * 10);

    libyuv::FilterModeEnum dq = libyuv::kFilterLinear;
    if(nHightQuqlity == 0) {
        dq = libyuv::kFilterNone;
    }
    else if(nHightQuqlity == 1) {
        dq = libyuv::kFilterLinear;
    }
    else if(nHightQuqlity == 2) {
        dq = libyuv::kFilterBilinear;
    }
    else {
        dq = libyuv::kFilterBox;
    }


    if (da1 == da2) {



            libyuv::I420Scale(pFrameYUV->data[0], pFrameYUV->linesize[0],
                              pFrameYUV->data[1], pFrameYUV->linesize[1],
                              pFrameYUV->data[2], pFrameYUV->linesize[2],
                              pFrameYUV->width, pFrameYUV->height,
                              pFrameRecord->data[0], pFrameRecord->linesize[0],
                              pFrameRecord->data[1], pFrameRecord->linesize[1],
                              pFrameRecord->data[2], pFrameRecord->linesize[2],
                              pFrameRecord->width, pFrameRecord->height,
                              dq);



    } else {

        memset(pFrameRecord->data[0], 16,
               (size_t) pFrameRecord->linesize[0] *pFrameRecord->height);              //yuv 黑色
        memset(pFrameRecord->data[1], 128, (size_t) (pFrameRecord->height/2) *pFrameRecord->linesize[1]);// pFrameRecord->height / 4);
        memset(pFrameRecord->data[2], 128, (size_t) (pFrameRecord->height/2) *pFrameRecord->linesize[2]);// pFrameRecord->height / 4);

        int H2 = pFrameRecord->height;
        int W2 = pFrameRecord->width;
        int H1 = pFrameYUV->width;
        int W1 = pFrameYUV->height;

        int w, h;
        if (da2 > da1) {
            h = H2;
            w = (int) (h * d1);
            w = (w / 4) * 4;
        } else {
            w = W2;
            h = (int) (w / d1);
            h = ((h ) / 4) * 4;
        }

        AVFrame *frame1 = av_frame_alloc();
        frame1->format = AV_PIX_FMT_YUV420P;
        frame1->width = w;
        frame1->height = h;
        av_image_alloc(
                frame1->data, frame1->linesize, frame1->width,
                frame1->height,
                AV_PIX_FMT_YUV420P, aligD);
        libyuv::I420Scale(pFrameYUV->data[0], pFrameYUV->linesize[0],
                          pFrameYUV->data[1], pFrameYUV->linesize[1],
                          pFrameYUV->data[2], pFrameYUV->linesize[2],
                          pFrameYUV->width, pFrameYUV->height,
                          frame1->data[0], frame1->linesize[0],
                          frame1->data[1], frame1->linesize[1],
                          frame1->data[2], frame1->linesize[2],
                          frame1->width, frame1->height,
                          dq);


        int ww = 0;
        int hh = 0;

        if (nRecordWidth > frame1->width) {
            ww = nRecordWidth - frame1->width;
            ww = ((ww + 3) / 4) * 4;
        }
        ww /= 2;

        if (nRecordHeight > frame1->height) {
            hh = nRecordHeight - frame1->height;
            hh = ((hh + 3) / 4) * 4;
        }
        hh /= 2;

        byte *pdes;
        byte *pdes1;
        byte *psrc;
        byte *psrc1;

        pdes = (byte *) (pFrameRecord->data[0]);
        psrc = (byte *) (frame1->data[0]);

        byte *srcp;
        byte *desp;


        int dat = hh * pFrameRecord->linesize[0] + ww;
        pdes += dat;

        for (h = 0; h < frame1->height; h++) {
            memcpy(pdes, psrc, (size_t) frame1->width);
            pdes += pFrameRecord->linesize[0];
            psrc += frame1->linesize[0];
        }

        //int deshw = pFrameRecord->width >> 1;

        pdes = pFrameRecord->data[1];
        psrc = frame1->data[1];

        hh/=2;

        dat = hh * pFrameRecord->linesize[1] + ww / 2;
        pdes += dat;

        pdes1 = pFrameRecord->data[2];
        dat = hh * pFrameRecord->linesize[2] + ww / 2;
        pdes1 += dat;
        psrc1 = frame1->data[2];

        int wn = frame1->width/2;

        for (h = 0; h < frame1->height >> 1; h++) {
            memcpy(pdes, psrc, (size_t)wn);
            pdes += pFrameRecord->linesize[1];
            psrc += frame1->linesize[2];
            memcpy(pdes1, psrc1, (size_t)wn);
            pdes1 += pFrameRecord->linesize[2];
            psrc1 += frame1->linesize[2];

        }


        av_freep(&frame1->data[0]);
        av_frame_free(&frame1);

    }

}

AVPacket *C_FFMpegPlayer::F_GetPacket(void) {
    int ret, got_output;
    if (pFrameYUV == nullptr)
        return nullptr;
    if (m_codecCtx == nullptr)
        return nullptr;
    AVFrame *frame_rec1 = nullptr;
    if (m_decodedFrame != nullptr) {
        if (m_bSaveSnapshot || m_bSaveVideo) {
            if (pFrameYUV->width != nRecordWidth || pFrameYUV->height != nRecordHeight) {
                F_Adj_Resolution();
                frame_rec1 = pFrameRecord;
            } else {
                frame_rec1 = pFrameYUV;
            }
            if (m_bSaveSnapshot) {
                if (frame_SnapBuffer != nullptr) {
                    if (frame_SnapBuffer->width != frame_rec1->width ||
                        frame_SnapBuffer->height != frame_rec1->height) {
                        av_freep(&frame_SnapBuffer->data[0]);
                        av_frame_free(&frame_SnapBuffer);
                        frame_SnapBuffer = nullptr;
                    }
                }
                if (frame_SnapBuffer == nullptr) {
                    frame_SnapBuffer = av_frame_alloc();
                    frame_SnapBuffer->width = frame_rec1->width;
                    frame_SnapBuffer->height = frame_rec1->height;
                    frame_SnapBuffer->format = AV_PIX_FMT_YUV420P;
                    av_image_alloc(
                            frame_SnapBuffer->data, frame_SnapBuffer->linesize,
                            frame_SnapBuffer->width,
                            frame_SnapBuffer->height,
                            AV_PIX_FMT_YUV420P, aligD);
                }

                libyuv::I420Copy(frame_rec1->data[0], frame_rec1->linesize[0],
                                 frame_rec1->data[1], frame_rec1->linesize[1],
                                 frame_rec1->data[2], frame_rec1->linesize[2],
                                 frame_SnapBuffer->data[0], frame_SnapBuffer->linesize[0],
                                 frame_SnapBuffer->data[1], frame_SnapBuffer->linesize[1],
                                 frame_SnapBuffer->data[2], frame_SnapBuffer->linesize[2],
                                 frame_SnapBuffer->width, frame_SnapBuffer->height);
                EncodeSnapshot();
            }
        }
    }

    if (m_FrameQueue_H264.size() < 8) {
        if (m_bSaveVideo) {
            AVFrame *frame_rec = frame_rec1;
            int HH = nRecordHeight;
            int H2 = HH >> 1;

            int WW = nRecordWidth;
            int W2 = WW >> 1;

            uint8_t *srcY = frame_rec->data[0];
            uint8_t *srcU = frame_rec->data[1];
            uint8_t *srcV = frame_rec->data[2];

            uint32_t nCount = 0;
            MyFrame *myfram = new MyFrame();

            nCount += frame_rec->linesize[0] * HH;
            nCount += frame_rec->linesize[1] * H2;
            nCount += frame_rec->linesize[2] * H2;


            myfram->data = new uint8_t[nCount];
            myfram->nLen = nCount;
            unsigned char *bufY = myfram->data;
            unsigned char *bufU = bufY + WW * HH;
            unsigned char *bufV = bufU + (W2 * H2);

            if (encord_colorformat ==
                OMX_COLOR_FormatYUV420SemiPlanar)   //YUV420 或者 NV2 主要是Android  硬编码，有些机型支持的 数据格式只有OMX_COLOR_FormatYUV420SemiPlanar
            {
                libyuv::I420ToNV12(srcY, frame_rec->linesize[0],
                                   srcU, frame_rec->linesize[1],
                                   srcV, frame_rec->linesize[2],
                                   bufY, WW,
                                   bufU, WW,
                                   WW, HH);
            } else {
                libyuv::I420Copy(srcY, frame_rec->linesize[0],
                                 srcU, frame_rec->linesize[1],
                                 srcV, frame_rec->linesize[2],
                                 bufY, WW,
                                 bufU, W2,
                                 bufV, W2,
                                 WW, HH);
            }
            pthread_mutex_lock(&m_Frame_Queuelock);
            {
                m_FrameQueue_H264.push(myfram);
            }
            pthread_mutex_unlock(&m_Frame_Queuelock);
        }
    }


    F_DispSurface();
    return nullptr;
}

void C_FFMpegPlayer::F_AdjUV(int8_t u, int8_t v) {

}

void F_OnSave2ToGallery_mid(int n);

extern int nDispStyle;


extern int nPhotoW;// = 640;
extern int nPhotoH;// = 360;
extern bool bPhotoSet;// = false;  //  如果 为 true 表示拍照时用  nPhotoW   nPhotoH
extern bool bSaveByName;

int C_FFMpegPlayer::EncodeSnapshot() {


    int WW = nRecordWidth;
    int HH = nRecordHeight;
    if (bPhotoSet) {
        WW = nPhotoW;
        HH = nPhotoH;
    }

    AVCodec *codec1;
    AVCodecContext *cSnap = nullptr;
    int ret, got_output;
    AVPacket pkt;

    m_bSaveSnapshot = false;


    char *ps = (char *) m_snapShotPath;
    int nStringLen = strlen(ps);
    if (nStringLen < 5) {
        return -1;
    }


    AVCodecID nSaveTypeid = AV_CODEC_ID_MJPEG;

    char *ext2 = ps + (nStringLen - 3);
    char ext[3];
    memcpy(ext, ext2, 3);
    std::string sExt = ext;
    transform(sExt.begin(), sExt.end(), sExt.begin(), ::toupper);
    if (sExt == "PNG") {
        nSaveTypeid = AV_CODEC_ID_PNG;
    }
    //bSaveJpeg
    if (!bSaveByName) {
        nSaveTypeid = AV_CODEC_ID_MJPEG;
    }

    codec1 = avcodec_find_encoder(nSaveTypeid);
    if (!codec1) {
        return FFMPEGPLAYER_SAVESNAPSHOTFAILED;
    }
    cSnap = avcodec_alloc_context3(nullptr);
    if (!cSnap) {
        return FFMPEGPLAYER_SAVESNAPSHOTFAILED;
    }


    av_init_packet(&pkt);
    pkt.data = nullptr;
    pkt.size = 0;
    ret = 0;
    AVFrame *frame_Snap = nullptr;

    bool bAlloc = false;
    bool bAlloc_A = false;

    if (WW != frame_SnapBuffer->width ||
        HH != frame_SnapBuffer->height) {

        frame_Snap = av_frame_alloc();
        bAlloc = true;

        frame_Snap->format = pix_format;
        frame_Snap->width = WW;
        frame_Snap->height = HH;
        ret = av_image_alloc(
                frame_Snap->data, frame_Snap->linesize, WW,
                HH,
                pix_format, aligD);


        libyuv::I420Scale(frame_SnapBuffer->data[0], frame_SnapBuffer->linesize[0],
                          frame_SnapBuffer->data[1], frame_SnapBuffer->linesize[1],
                          frame_SnapBuffer->data[2], frame_SnapBuffer->linesize[2],
                          frame_SnapBuffer->width, frame_SnapBuffer->height,
                          frame_Snap->data[0], frame_Snap->linesize[0],
                          frame_Snap->data[1], frame_Snap->linesize[1],
                          frame_Snap->data[2], frame_Snap->linesize[2],
                          frame_Snap->width, frame_Snap->height,
                          libyuv::kFilterLinear);

    } else {
        frame_Snap = frame_SnapBuffer;
    }

    if (nDispStyle != 0) {

        uint8_t *pbufferA = new uint8_t[WW * 4 * HH];
        uint8_t *pbuffer = pbufferA;

        libyuv::I420ToABGR(frame_Snap->data[0], frame_Snap->linesize[0],
                           frame_Snap->data[1], frame_Snap->linesize[1],
                           frame_Snap->data[2], frame_Snap->linesize[2],
                           pbuffer, frame_Snap->width * 4,
                           frame_Snap->width, frame_Snap->height);

        RGBA_STRUCT df = {0, 0, 0, 0};
        RGBA_STRUCT df2 = {255, 0, 0, 20};
        RGBA_STRUCT df3 = {255, 255, 0, 20};
        RGBA_STRUCT df4 = {0, 255, 0, 20};
        RGBA_STRUCT df5 = {128, 69, 9, 50};
        RGBA_STRUCT df6 = {0, 0, 255, 20};


        {
            int r, g, b;
            if (nDispStyle == 2) {
                df = df2;//{255, 0, 0, 20};
            } else if (nDispStyle == 3) {
                df = df3;//{255, 255, 0, 20};
            } else if (nDispStyle == 4) {
                df = df4;//{0, 255, 0, 20};
            } else if (nDispStyle == 5) {
                df = df5;//{128, 69, 9, 50};
            } else if (nDispStyle == 6) {
                df = df6;//{0, 0, 255, 20};
            }


            RGBA_STRUCT *buffer;
            uint8_t dat = 0;
            float ap = df.a / 100.0f;
            for (int y = 0; y < WW; y++) {
                for (int x = 0; x < HH; x++) {
                    buffer = (RGBA_STRUCT *) pbuffer;
                    if (nDispStyle == 1) {
                        dat = (uint8_t) (((*buffer).r * 38 + (*buffer).g * 75 + (*buffer).b * 15)
                                >> 7);
                        (*buffer).r = dat;
                        (*buffer).g = dat;
                        (*buffer).b = dat;
                    } else {

                        r = (int) (ap * df.r + (1 - ap) * (*buffer).r);
                        g = (int) (ap * df.g + (1 - ap) * (*buffer).g);
                        b = (int) (ap * df.b + (1 - ap) * (*buffer).b);


                        if (r > 255)
                            r = 255;
                        if (r < 0)
                            r = 0;
                        if (g > 255)
                            g = 255;
                        if (g < 0)
                            g = 0;
                        if (b > 255)
                            b = 255;
                        if (b < 0)
                            b = 0;
                        (*buffer).r = (uint8_t) r;
                        (*buffer).g = (uint8_t) g;
                        (*buffer).b = (uint8_t) b;
                    }
                    pbuffer += 4;
                }
            }
            pbuffer = pbufferA;
            ret = libyuv::ABGRToI420((uint8_t *) pbuffer, frame_Snap->width * 4,
                                     frame_Snap->data[0], frame_Snap->linesize[0],
                                     frame_Snap->data[1], frame_Snap->linesize[1],
                                     frame_Snap->data[2], frame_Snap->linesize[2],
                                     frame_Snap->width, frame_Snap->height);
            delete[]pbufferA;
        }
    }

    ret = -1;


    AVFrame *frame_Snap_A = nullptr;

    cSnap->codec_type = AVMEDIA_TYPE_VIDEO;
    cSnap->width = WW;   // 1280;//m_codecCtx->width;
    cSnap->height = HH; //720;//m_codecCtx->height;
    cSnap->time_base.num = 1;//m_codecCtx->time_base.num;
    cSnap->time_base.den = 1;//m_codecCtx->time_base.den;  //AV_PIX_FMT_RGB24;//
    cSnap->pix_fmt = AV_PIX_FMT_YUVJ420P;  //  pix_format;//m_codecCtx->pix_fmt;

    int nJpegQuality = 80;
    if (codec1->id == AV_CODEC_ID_PNG) {
        cSnap->compression_level = 9;
        nJpegQuality = 50;
        cSnap->qcompress = (float) nJpegQuality / 100.f; // 0~1.0, default is 0.5
        cSnap->pix_fmt = AV_PIX_FMT_RGBA;  //  pix_format;//m_codecCtx->pix_fmt;
        bAlloc_A = true;
        frame_Snap_A = av_frame_alloc();
        frame_Snap_A->format = AV_PIX_FMT_RGBA;
        frame_Snap_A->width = WW;
        frame_Snap_A->height = HH;
        ret = av_image_alloc(frame_Snap_A->data, frame_Snap_A->linesize, WW,
                             HH, AV_PIX_FMT_RGBA, aligD);


        struct SwsContext *img_convert_ctx_A = sws_getContext(WW, HH, AV_PIX_FMT_YUV420P,
                                                              WW, HH, AV_PIX_FMT_RGBA,
                                                              SWS_FAST_BILINEAR, nullptr, nullptr,
                                                              nullptr); //




        sws_scale(img_convert_ctx_A,
                  (const uint8_t *const *) frame_Snap->data,
                  frame_Snap->linesize, 0,
                  HH,
                  frame_Snap_A->data, frame_Snap_A->linesize);


        sws_freeContext(img_convert_ctx_A);


//        libyuv::I420ToRGB24(frame_Snap->data[0], frame_Snap->linesize[0],
//                            frame_Snap->data[1], frame_Snap->linesize[1],
//                            frame_Snap->data[2], frame_Snap->linesize[2],
//                            frame_Snap_A->data[0],frame_Snap_A->linesize[0],
//                            WW,HH);


    } else {
        cSnap->qcompress = (float) nJpegQuality / 100.f; // 0~1.0, default is 0.5
        cSnap->qmin = 2;
        cSnap->qmax = 31;
        cSnap->max_qdiff = 3;
        cSnap->pix_fmt = AV_PIX_FMT_YUVJ420P;

        frame_Snap_A = frame_Snap;
    }


    if ((ret = avcodec_open2(cSnap, codec1, nullptr)) < 0) {
        DEBUG_PRINT("Could not open codecc\n");
        if (frame_Snap != nullptr) {
            if (bAlloc) {
                av_freep(&frame_Snap->data[0]);
                av_frame_free(&frame_Snap);
                frame_Snap = nullptr;
            }
        }

        if (frame_Snap_A != nullptr) {
            if (bAlloc_A) {
                av_freep(&frame_Snap_A->data[0]);
                av_frame_free(&frame_Snap_A);
                frame_Snap_A = nullptr;
            }
        }
        return FFMPEGPLAYER_SAVESNAPSHOTFAILED;
    }


    if (avcodec_send_frame(cSnap, frame_Snap_A) == 0) {
        if (avcodec_receive_packet(cSnap, &pkt) == 0) {
            ret = 0;
        }
    }


    if (frame_Snap != nullptr) {
        if (bAlloc) {
            av_freep(&frame_Snap->data[0]);
            av_frame_free(&frame_Snap);
            frame_Snap = nullptr;
        }
    }

    if (frame_Snap_A != nullptr) {
        if (bAlloc_A) {
            av_freep(&frame_Snap_A->data[0]);
            av_frame_free(&frame_Snap_A);
            frame_Snap_A = nullptr;
        }
    }


    if (ret < 0) {
        DEBUG_PRINT("Error encoding frame\n");
        memset(m_snapShotPath, 0, 256);

        av_packet_unref(&pkt);
        avcodec_close(cSnap);
        avcodec_free_context(&cSnap);
        return FFMPEGPLAYER_SAVESNAPSHOTFAILED;
    }
    if (ret == 0) {
        DEBUG_PRINT("Write frame (size=%5d)\n", pkt.size);
        FILE *fp = fopen(m_snapShotPath, "wb");
        if (fp) {
            fwrite(pkt.data, 1, (size_t) pkt.size, fp);
            fclose(fp);
        } else {
            DEBUG_PRINT("Failed to open file: %s\n", m_snapShotPath);
            memset(m_snapShotPath, 0, 256);

            av_packet_unref(&pkt);
            avcodec_close(cSnap);
            avcodec_free_context(&cSnap);
            return FFMPEGPLAYER_SAVESNAPSHOTFAILED;
        }
    }

    av_packet_unref(&pkt);
    avcodec_close(cSnap);
    avcodec_free_context(&cSnap);
    F_OnSave2ToGallery_mid(0);
    memset(m_snapShotPath, 0, 256);
    return FFMPEGPLAYER_NOERROR;
}

//----------------------------------------------------------------------
int C_FFMpegPlayer::Releaseffmpeg() {


    m_Status = E_PlayerStatus_Stoped;

    if (m_codecCtx != nullptr) {
        avcodec_close(m_codecCtx);
        m_codecCtx = nullptr;
    }

    if (m_codecCtx_T != nullptr) {
        avcodec_close(m_codecCtx_T);
        m_codecCtx_T = nullptr;
    }

    if (m_formatCtx != nullptr) {
        avformat_close_input(&m_formatCtx);
        m_formatCtx = nullptr;
    }

    if (img_convert_ctx != nullptr) {
        sws_freeContext(img_convert_ctx);
        img_convert_ctx = nullptr;
    }
    if (pFrameYUV != nullptr) {
        av_freep(&pFrameYUV->data[0]);
        av_frame_free(&pFrameYUV);
        pFrameYUV = 0;
    }

    if (pFrameYUV_D != nullptr) {
        av_freep(&pFrameYUV_D->data[0]);
        av_frame_free(&pFrameYUV_D);
        pFrameYUV_D = nullptr;
    }


    img_convert_ctx = nullptr;

    m_formatCtx = nullptr;
    m_codecCtx = nullptr;
    m_decodedFrame = nullptr;


    if (frame_a != nullptr) {
        av_freep(&frame_a->data[0]);
        av_frame_free(&frame_a);
        frame_a = nullptr;
    }
    if (frame_b != nullptr) {
        av_freep(&frame_b->data[0]);
        av_frame_free(&frame_b);
        frame_b = nullptr;
    }

    if (frame_SnapBuffer != nullptr) {
        av_freep(&frame_SnapBuffer->data[0]);
        av_frame_free(&frame_SnapBuffer);
        frame_SnapBuffer = nullptr;

    }
    if (pFrameRecord != nullptr) {
        av_freep(&pFrameRecord->data[0]);
        av_frame_free(&pFrameRecord);
        pFrameRecord = nullptr;
    }
    //if(nCameraDataRota !=0)
    //if(pFrameYUV_Real!=pFrameYUV)
    {
        if (pFrameYUV_Real != nullptr) {
            av_freep(&pFrameYUV_Real->data[0]);
            av_frame_free(&pFrameYUV_Real);
            pFrameYUV_Real = nullptr;
        }

        if (pFrameYUV != nullptr) {
            av_freep(&pFrameYUV->data[0]);
            av_frame_free(&pFrameYUV);
            pFrameYUV = nullptr;
        }
    }

    if (bInitMediaA) {
        bInitMediaA = false;
        if (m_decodedFrame != nullptr) {
            av_freep(&m_decodedFrame->data[0]);
            av_frame_free(&m_decodedFrame);
            m_decodedFrame = nullptr;
        }
    }
    LOGE("Exit Play...");
    return FFMPEGPLAYER_NOERROR;
}


uint8_t *bufferA;
int bufflen;
int pos;

extern bool bSentRevBMP;
extern bool bGesture;
extern AVFrame *gl_Frame;


extern bool bTransferSize;
extern int nTransferWidth;
extern int nTransferHeight;
extern AVFrame *pTranFrame;


AVFrame *pGestureFrame = nullptr;

static AVFrame *pTmpAvFrame = nullptr;

void C_FFMpegPlayer::F_DispSurface(void) {

    bool bSentRevBMP_bak = bSentRevBMP;
#if 0
    if (bGesture) {
           bSentRevBMP_bak = true;
        }
#else
    if (bGesture) {
        //bSentRevBMP_bak = true;

        int ww = 300;
        int hh = 300;

        if (pGestureFrame != nullptr) {
            if (pGestureFrame->width != ww || pGestureFrame->height != hh) {
                av_freep(&pGestureFrame->data[0]);
                av_frame_free(&pGestureFrame);
                pGestureFrame = nullptr;
            }
        }
        if (pGestureFrame == nullptr) {
            pGestureFrame = av_frame_alloc();
            pGestureFrame->width = ww;
            pGestureFrame->height = hh;
            av_image_alloc(pGestureFrame->data, pGestureFrame->linesize, ww,
                           hh,
                           AV_PIX_FMT_YUV420P, aligD);
        }

        if (pFrameYUV->width != ww || pFrameYUV->height != hh)   //转换为 设定的尺寸，然后传给应用层。
        {
            libyuv::I420Scale(pFrameYUV->data[0], pFrameYUV->linesize[0],
                              pFrameYUV->data[1], pFrameYUV->linesize[1],
                              pFrameYUV->data[2], pFrameYUV->linesize[2],
                              pFrameYUV->width, pFrameYUV->height,
                              pGestureFrame->data[0], pGestureFrame->linesize[0],
                              pGestureFrame->data[1], pGestureFrame->linesize[1],
                              pGestureFrame->data[2], pGestureFrame->linesize[2],
                              ww, hh,
                              libyuv::kFilterLinear);

            libyuv::I420ToABGR(pGestureFrame->data[0], pGestureFrame->linesize[0],
                               pGestureFrame->data[1], pGestureFrame->linesize[1],
                               pGestureFrame->data[2], pGestureFrame->linesize[2],
                               Rgbabuffer, pGestureFrame->width * 4,
                               pGestureFrame->width, pGestureFrame->height);
        } else {
            libyuv::I420ToABGR(pFrameYUV->data[0], pFrameYUV->linesize[0],
                               pFrameYUV->data[1], pFrameYUV->linesize[1],
                               pFrameYUV->data[2], pFrameYUV->linesize[2],
                               Rgbabuffer, pFrameYUV->width * 4,
                               pFrameYUV->width, pFrameYUV->height);
        }

        F_SentGestureBmp(ww + hh * 0x10000);

    } else {
        if (pGestureFrame != nullptr) {
            av_freep(&pGestureFrame->data[0]);
            av_frame_free(&pGestureFrame);
            pGestureFrame = nullptr;
        }
    }
#endif
    if (bSentRevBMP_bak) {
        if (Rgbabuffer != nullptr) {
            int W1 = pFrameYUV->width;
            int H1 = pFrameYUV->height;
            bool bCopy = true;
            if (bTransferSize && pTranFrame != nullptr) {
                if (pFrameYUV->width != pTranFrame->width ||
                    pFrameYUV->height != pTranFrame->height)   //转换为 设定的尺寸，然后传给应用层。
                {

                    libyuv::I420Scale(pFrameYUV->data[0], pFrameYUV->linesize[0],
                                      pFrameYUV->data[1], pFrameYUV->linesize[1],
                                      pFrameYUV->data[2], pFrameYUV->linesize[2],
                                      pFrameYUV->width, pFrameYUV->height,
                                      pTranFrame->data[0], pTranFrame->linesize[0],
                                      pTranFrame->data[1], pTranFrame->linesize[1],
                                      pTranFrame->data[2], pTranFrame->linesize[2],
                                      nTransferWidth, nTransferHeight,
                                      libyuv::kFilterNone);

                    libyuv::I420ToABGR(pTranFrame->data[0], pTranFrame->linesize[0],
                                       pTranFrame->data[1], pTranFrame->linesize[1],
                                       pTranFrame->data[2], pTranFrame->linesize[2],
                                       Rgbabuffer, pTranFrame->width * 4,
                                       pTranFrame->width, pTranFrame->height);
                    W1 = pTranFrame->width;
                    H1 = pTranFrame->height;
                    bCopy = false;
                }
            }
            if (bCopy) {
                libyuv::I420ToABGR(pFrameYUV->data[0], pFrameYUV->linesize[0],
                                   pFrameYUV->data[1], pFrameYUV->linesize[1],
                                   pFrameYUV->data[2], pFrameYUV->linesize[2],
                                   Rgbabuffer, pFrameYUV->width * 4,
                                   pFrameYUV->width, pFrameYUV->height);
            }
            if (bNeedDisp) {
                F_SentRevBmp(W1 + H1 * 0x10000);
            } else {
                nDispInx++;
                if ((nDispInx & 03) == 0) {
                    F_SentRevBmp(W1 + H1 * 0x10000);
                }
            }
        }
        if (bSentRevBMP)
            return;
    }

    if (gl_Frame != nullptr) {
        if ((nSDStatus & bit1_LocalRecording) && !bRealRecording) {
            bRealRecording = true;
        }


        gl_Frame->width = pFrameYUV->width;
        gl_Frame->height = pFrameYUV->height;

        gl_Frame->linesize[0] = pFrameYUV->linesize[0];
        gl_Frame->linesize[1] = pFrameYUV->linesize[1];
        gl_Frame->linesize[2] = pFrameYUV->linesize[2];

        libyuv::I420Copy(pFrameYUV->data[0], pFrameYUV->linesize[0],
                         pFrameYUV->data[1], pFrameYUV->linesize[1],
                         pFrameYUV->data[2], pFrameYUV->linesize[2],
                         gl_Frame->data[0], pFrameYUV->linesize[0],
                         gl_Frame->data[1], pFrameYUV->linesize[1],
                         gl_Frame->data[2], pFrameYUV->linesize[2],
                         pFrameYUV->width, pFrameYUV->height);

    }

}

int C_FFMpegPlayer::F_SetRotate(float n) {
    int ret = -1;
    if (filter_graph != nullptr) {
        if (rotate_ctx != nullptr) {
            char args[50];
            memset(args, 0, 50);
            snprintf(args, sizeof(args),
                     "PI/180*%.4f", n);
            ret = avfilter_graph_send_command(filter_graph, "rotate", "angle",
                                              args, nullptr, 0, 0);
        }
    }
    return ret;
}


int C_FFMpegPlayer::init_filters(const char *filters_descr, const char *filters2_descr) {
#if 1

    char args[512];
    int ret = -1;
    const AVFilter *buffersrc = avfilter_get_by_name("buffer");
    const AVFilter *buffersink = avfilter_get_by_name("buffersink");
    const AVFilter *hqdn3d = avfilter_get_by_name("hqdn3d");
    const AVFilter *rotate = avfilter_get_by_name("rotate");
    AVRational time_base;// = m_decodedFrame->time_base;
    enum AVPixelFormat pix_fmts[] = {AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE};

//    if(m_decodedFrame->width == 0) {
//        m_decodedFrame->width = m_codecCtx->coded_width;
//        m_decodedFrame->height = m_codecCtx->coded_height;
//    }
    if (m_decodedFrame != nullptr && (m_decodedFrame->width == 0 || m_decodedFrame->height == 0)) {
        goto errorA;
    }
    bFilterChanged = false;
    if (buffersink_ctx != nullptr) {
        avfilter_free(buffersink_ctx);
        buffersink_ctx = nullptr;
    }

    if (buffersrc_ctx != nullptr) {
        avfilter_free(buffersrc_ctx);
        buffersrc_ctx = nullptr;
    }

    if (rotate_ctx != nullptr) {
        avfilter_free(rotate_ctx);
        rotate_ctx = nullptr;
    }

    if (hqdn3d_ctx != nullptr) {
        avfilter_free(hqdn3d_ctx);
        hqdn3d_ctx = nullptr;
    }
    if (filter_graph != nullptr) {
        avfilter_graph_free(&filter_graph);
        filter_graph = nullptr;
    }
    if (!b3D_noise_reduction && !bEnableRotateFilter)
        goto errorA;


    time_base.den = 1;
    time_base.num = 25;

    filter_graph = avfilter_graph_alloc();
    if (!filter_graph) {
        return ret;
    }

    //m_codecCtx->pix_fmt,

    snprintf(args, sizeof(args),
             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
             m_decodedFrame->width, m_decodedFrame->height, AV_PIX_FMT_YUV420P,
             time_base.num, time_base.den, 0, 1);
    //
    ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                                       args, nullptr, filter_graph);
    if (ret < 0) {
        LOGE("Cannot create buffer sink");
        goto errorA;
    }


    ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                                       nullptr, nullptr, filter_graph);
    if (ret < 0) {
        goto errorA;

    }


    if (b3D_noise_reduction) {
        av_opt_set_int_list(buffersink_ctx, "pix_fmts", pix_fmts,
                            AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
        ret = avfilter_graph_create_filter(&hqdn3d_ctx, hqdn3d, "hqdn3d",
                                           filters_descr, nullptr, filter_graph);

        if (ret < 0) {
            goto errorA;
        }
    }

    if (bEnableRotateFilter) {
        ret = avfilter_graph_create_filter(&rotate_ctx, rotate, "rotate",
                                           filters2_descr, nullptr, filter_graph);
        if (ret < 0) {
            goto errorA;
        }

    }

    if (b3D_noise_reduction && bEnableRotateFilter) {
        avfilter_link(buffersrc_ctx, 0, hqdn3d_ctx, 0);
        avfilter_link(hqdn3d_ctx, 0, rotate_ctx, 0);
        avfilter_link(rotate_ctx, 0, buffersink_ctx, 0);
    } else if (b3D_noise_reduction && !bEnableRotateFilter) {
        avfilter_link(buffersrc_ctx, 0, hqdn3d_ctx, 0);
        avfilter_link(hqdn3d_ctx, 0, buffersink_ctx, 0);
    } else if (!b3D_noise_reduction && bEnableRotateFilter) {
        avfilter_link(buffersrc_ctx, 0, rotate_ctx, 0);
        avfilter_link(rotate_ctx, 0, buffersink_ctx, 0);
    }
/*
    avfilter_link(buffersrc_ctx, 0, hqdn3d_ctx, 0);
    if(rotate_ctx!= nullptr) {
        avfilter_link(hqdn3d_ctx, 0, rotate_ctx, 0);
        avfilter_link(rotate_ctx, 0, buffersink_ctx, 0);
    } else
    {
        avfilter_link(hqdn3d_ctx, 0, buffersink_ctx, 0);
    }
*/
    ret = avfilter_graph_config(filter_graph, nullptr);
    if (ret < 0) {
        goto errorA;
    }

    return ret;


    errorA:


    if (buffersink_ctx != nullptr) {
        avfilter_free(buffersink_ctx);
        buffersink_ctx = nullptr;
    }

    if (buffersrc_ctx != nullptr) {
        avfilter_free(buffersrc_ctx);
        buffersrc_ctx = nullptr;
    }

    if (rotate_ctx != nullptr) {
        avfilter_free(rotate_ctx);
        rotate_ctx = nullptr;
    }

    if (hqdn3d_ctx != nullptr) {
        avfilter_free(hqdn3d_ctx);
        hqdn3d_ctx = nullptr;
    }
    if (filter_graph != nullptr) {
        avfilter_graph_free(&filter_graph);
        filter_graph = nullptr;
    }

    return ret;
#else
    if(buffersrc_ctx!= nullptr) {
        avfilter_free(buffersrc_ctx);
        buffersrc_ctx = nullptr;
    }
    if(buffersink_ctx!= nullptr) {
        avfilter_free(buffersink_ctx);
        buffersink_ctx = nullptr;
    }
    if(filter_graph!= nullptr) {
        avfilter_graph_free(&filter_graph);
        filter_graph= nullptr;
    }

    char args[512];
    int ret = -1;
    const AVFilter *buffersrc  = avfilter_get_by_name("buffer");
    const AVFilter *buffersink = avfilter_get_by_name("buffersink");
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs  = avfilter_inout_alloc();

    AVRational time_base = m_codecCtx->time_base;
    time_base.den=1;
    time_base.num=25;
    enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE };

    filter_graph = avfilter_graph_alloc();
    if (!outputs || !inputs || !filter_graph) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

/* buffer video source: the decoded frames from the decoder will be inserted here. */
    snprintf(args, sizeof(args),
             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
             m_codecCtx->width, m_codecCtx->height, m_codecCtx->pix_fmt,
             time_base.num, time_base.den,
             m_codecCtx->sample_aspect_ratio.num, m_codecCtx->sample_aspect_ratio.den);
    ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                                       args, nullptr, filter_graph);
    if (ret < 0) {
        LOGE(TAG, "Cannot create buffer source\n");
        goto end;
    }
    /* buffer video sink: to terminate the filter chain. */
    ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                                       nullptr, nullptr, filter_graph);
    if (ret < 0) {
        LOGE(TAG, "Cannot create buffer sink\n");
        goto end;
    }
    outputs->name       = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx    = 0;
    outputs->next       = nullptr;

    inputs->name       = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = nullptr;

    if ((ret = avfilter_graph_parse_ptr(filter_graph, filters_descr,
                                        &inputs, &outputs, nullptr)) < 0)
        goto end;

    if ((ret = avfilter_graph_config(filter_graph, nullptr)) < 0)
        goto end;

    end:
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);
    return ret;
#endif
}

void C_FFMpegPlayer::F_ReleaseMediaCode1() {
    if (mMediaCodec != nullptr) {
        AMediaCodec_flush(mMediaCodec);
        AMediaCodec_stop(mMediaCodec);
        usleep(1000 * 100);
        AMediaCodec_delete(mMediaCodec);
        mMediaCodec = nullptr;
        LOGE("Release MediaCode!!!");
    }
}


bool C_FFMpegPlayer::F_InitMediaCodecDecord(int width, int height, uint8_t *sps1, int spsSize,
                                            uint8_t *pps1, int ppsSize) {
    const char *mine = "video/avc";
    if (mMediaCodec != nullptr) {
        AMediaCodec_stop(mMediaCodec);
        usleep(30 * 1000);
        AMediaCodec_delete(mMediaCodec);
        mMediaCodec = 0;
    }


    mColorFormat = -1;
    mMediaCodec = AMediaCodec_createDecoderByType(mine);
    if (mMediaCodec == nullptr) {
        return false;
    }
//    uint8_t *da = new uint8_t[spsSize+ppsSize];
//    memcpy(da,sps,spsSize);
//    memcpy(da+spsSize,pps,ppsSize);


    uint8_t *sps = new uint8_t[4 + spsSize];
    uint8_t *pps = new uint8_t[4 + ppsSize];
    sps[0] = pps[0] = 0;
    sps[1] = pps[1] = 0;
    sps[2] = pps[2] = 0;
    sps[3] = pps[3] = 1;
    memcpy(sps + 4, sps1, spsSize);
    memcpy(pps + 4, pps1, ppsSize);
    spsSize += 4;
    ppsSize += 4;

    mDecordWidth = width;
    mDecordHeight = height;

    media_status_t status = AMEDIA_ERROR_BASE;
    AMediaFormat *videoFormat = AMediaFormat_new();
    AMediaFormat_setString(videoFormat, "mime", mine);
    AMediaFormat_setInt32(videoFormat, AMEDIAFORMAT_KEY_WIDTH, width); // 视频宽度
    AMediaFormat_setInt32(videoFormat, AMEDIAFORMAT_KEY_HEIGHT, height); // 视频高度
    AMediaFormat_setBuffer(videoFormat, "csd-0", sps, spsSize); // sps
    AMediaFormat_setBuffer(videoFormat, "csd-1", pps, ppsSize); // pps

    delete[]sps;
    delete[]pps;

    status = AMediaCodec_configure(mMediaCodec, videoFormat, nullptr, nullptr, 0);
    if (status == AMEDIA_OK) {
        status = AMediaCodec_start(mMediaCodec);
        usleep(1000 * 200);
    }
    pts = 0;
    bFirst = true;
    AMediaCodec_flush(mMediaCodec);
    AMediaFormat_delete(videoFormat);
    return (status == AMEDIA_OK);

}

//bool C_FFMpegPlayer::F_DecorderData(char *bufData,int bufSize,bool bDisp)
//{
//    if(mMediaCodec== nullptr)
//    {
//        return false;
//    }
//
//
//
//
//    int nTy = bufData[4] & 0x0F;
//
//    LOGE("ABCDEFG : input packet");
//    size_t outsize;
//    bool bGetOK = false;
//    ssize_t bufidx = AMediaCodec_dequeueInputBuffer(mMediaCodec, 500);
//    if (bufidx >= 0)
//    {
//        // 获取buffer的索引
//        uint8_t *inputBuf = AMediaCodec_getInputBuffer(mMediaCodec, bufidx, &outsize);
//        if (inputBuf != nullptr && bufSize <= outsize)
//        {
//            //将待解码的数据copy到硬件中
//            memcpy(inputBuf, bufData, bufSize);
////            if(nTy == 0x07 || nTy == 0x08)
////            {
////                media_status_t status = AMediaCodec_queueInputBuffer(mMediaCodec, bufidx, 0,
////                                                                     bufSize, pts, AMEDIACODEC_BUFFER_FLAG_CODEC_CONFIG);
////            }
////            else
//            {
//                media_status_t status = AMediaCodec_queueInputBuffer(mMediaCodec, bufidx, 0,
//                                                                     bufSize, pts, 0);
//            }
//            pts+=10;
//            AMediaCodecBufferInfo info;
//            ssize_t outbufidx = AMediaCodec_dequeueOutputBuffer(mMediaCodec, &info, 1000);
//            while (true)
//            {
//                if (outbufidx >= 0)
//                {
//                    size_t outsize;
//                    uint8_t *outputBuf = AMediaCodec_getOutputBuffer(mMediaCodec, outbufidx,&outsize);
//
//                    if (outputBuf != nullptr)
//                    {
//                        if(bDisp) {
//                            AMediaFormat *bufferFormat = AMediaCodec_getOutputFormat(mMediaCodec);
//                            int W;
//                            int H;
//                            AMediaFormat_getInt32(bufferFormat, "color-format", &mColorFormat);
//                            AMediaFormat_getInt32(bufferFormat, "width", &W);
//                            AMediaFormat_getInt32(bufferFormat, "height", &H);
//                            AVFrame *frame = av_frame_alloc();
//
//                            frame->width = mDecordWidth;
//                            frame->height = mDecordHeight;
//                            frame->format = AV_PIX_FMT_YUV420P;
//                            int ret = av_image_alloc(
//                                    frame->data, frame->linesize, frame->width,
//                                    frame->height,
//                                    AV_PIX_FMT_YUV420P, 4);
//                            bool bOK = false;
//                            if (mColorFormat == 21)  //NV12
//                            {
//                                libyuv::NV12ToI420(outputBuf, W,
//                                                   outputBuf + (W * H), W,
//                                                   frame->data[0], frame->linesize[0],
//                                                   frame->data[1], frame->linesize[1],
//                                                   frame->data[2], frame->linesize[2],
//                                                   mDecordWidth, mDecordHeight);
//                                bOK = true;
//                            }
//                            if (mColorFormat == 19) //YUV420
//                            {
//                                libyuv::I420Copy(
//                                        outputBuf, W,
//                                        outputBuf + W * H, W / 2,
//                                        outputBuf + W * H + W / 2 * H / 2, W / 2,
//                                        frame->data[0], frame->linesize[0],
//                                        frame->data[1], frame->linesize[1],
//                                        frame->data[2], frame->linesize[2],
//                                        mDecordWidth, mDecordHeight);
//                                bOK = true;
//                            }
//                            if (bOK) {
////                            nBufferedCount++;
////                            if(nBufferedCount>6) {
////                                nBufferedCount = 6;
////
////                            }
//                                handleData(frame);
//                                LOGE("ABCDEFG : get frame");
//
//                            }
//                            av_freep(&frame->data[0]);
//                            av_frame_free(&frame);
//                        } else
//                        {
//                            LOGE("ABCDEFG : get frame");
//                        }
//
////                        if(info.flags & AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM)
////                        {
////                            LOGE("Flush");
////                            AMediaCodec_flush(mMediaCodec);
////                            break;
////                        }
////                        else
//                        {
//                            //LOGE(" NO Flush");
//                            AMediaCodec_releaseOutputBuffer(mMediaCodec, outbufidx, false);
//                            break;
//                            //outbufidx = AMediaCodec_dequeueOutputBuffer(mMediaCodec, &info, 500);
//                        }
//                    }
//
//                }
//                else if (outbufidx == AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED)
//                {
//                    outbufidx = AMediaCodec_dequeueOutputBuffer(mMediaCodec, &info, 500);
//                    LOGE(" 111 ");
//
//                }
//                else if (outbufidx == AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED) {
//                    outbufidx = AMediaCodec_dequeueOutputBuffer(mMediaCodec, &info, 500);
//                    LOGE(" 222 ");
//                }
//                else {
//                    //AMediaCodec_flush(mMediaCodec);
//                    LOGE(" 333 ");
////                    if(nTy == 7 || nTy == 8)
////                    {
////                        AMediaCodec_flush(mMediaCodec);
////                    }
//                    break;
//                }
//            }
//        }
//    }
//    else
//    {
//        AMediaCodec_flush(mMediaCodec);
//        LOGE("exit = %d", bufidx);
//    }
//    return  false;
//}

//void C_FFMpegPlayer::handleData(AVFrame *frame)
// {
//    int ret = 0;
//     m_codecCtx->width = frame->width;
//     m_codecCtx->height = frame->height;
//     m_codecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
//#if 1
//     InitMedia("");
//     if (bNeedChangedRotate) {
//         bNeedChangedRotate = false;
//         if (bEnableGsensor) {
//             F_SetRotate(nFilterRotate);
//         }
//     }
//
//     bool bfilter = false;
//     if ((b3D_noise_reduction || bEnableRotateFilter) && buffersrc_ctx != nullptr) {
//         ret = av_buffersrc_add_frame_flags(buffersrc_ctx, frame,
//                                            AV_BUFFERSRC_FLAG_KEEP_REF);
//         if (ret >= 0) {
//             ret = av_buffersink_get_frame(buffersink_ctx, m_decodedFrame);
//             if (ret >= 0)
//                 bfilter = true;
//                 ret = 0;
//         }
//     } else
//     {
//         bfilter = false;
//         ret = 0;
//         if(m_decodedFrame!= nullptr)
//         {
//             if(m_decodedFrame->width>0 || m_decodedFrame->height>0)
//             {
//                 {
//                     av_freep(&m_decodedFrame->data[0]);
//                     av_frame_free(&m_decodedFrame);
//                     m_decodedFrame = nullptr;
//                 }
//
//             }
//             else
//             {
//                 av_frame_free(&m_decodedFrame);
//             }
//         }
//         m_decodedFrame  = av_frame_alloc();
//         m_decodedFrame->width = frame->width;
//         m_decodedFrame->height = frame->height;
//         m_decodedFrame->format = frame->format;
//         int ret = av_image_alloc(
//                 m_decodedFrame->data, m_decodedFrame->linesize, m_decodedFrame->width,
//                 m_decodedFrame->height,
//                 AV_PIX_FMT_YUV420P, 4);
//         libyuv::I420Copy(frame->data[0],frame->linesize[0],
//                          frame->data[1],frame->linesize[1],
//                          frame->data[2],frame->linesize[2],
//                          m_decodedFrame->data[0],m_decodedFrame->linesize[0],
//                          m_decodedFrame->data[1],m_decodedFrame->linesize[1],
//                          m_decodedFrame->data[2],m_decodedFrame->linesize[2],
//                          frame->width,frame->height);
//     }
//
//     if (ret == 0)
//     {
//         _DispDecordData();
//         if(bfilter)
//         {
//             av_frame_unref(m_decodedFrame);
//             if ((b3D_noise_reduction || bEnableRotateFilter) && buffersrc_ctx != nullptr) {
//                     av_frame_unref(m3dFrame);
//             }
//         }
////         else
////         {
////             av_freep(&m_decodedFrame->data[0]);
////             av_frame_free(&m_decodedFrame);
////             m_decodedFrame = nullptr;
////         }
//     }
//#endif
//
// }





