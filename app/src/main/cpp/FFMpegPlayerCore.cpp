//
// Created by generalplus_sa1 on 4/29/16.
//
#include <algorithm>
#include <string>
#include <iostream>
#include <cctype>

#include "FFMpegPlayerCore.h"
#include "Defines.h"
#include <vector>
#include <queue>


#include <jni.h>
#include <unistd.h>
#include <fcntl.h>
//#include <src/main/cpp/h264prase/h264_rbsp.h>
//#include <src/main/cpp/h264prase/sps_parse.h>
#include <sstream>

#include "MyMediaCoder.h"
#include "phone_rl_protocol.h"

extern MyMediaCoder myMediaCoder;

//

extern int nCameraDataRota;
extern int nDataType;

extern float nScal;
extern uint8_t nSDStatus;
extern bool bRealRecording;
extern uint32_t nRecFrameCount;
extern int nFrameCount;

extern int PlatformDisplay();

extern void F_CloseSocket();

typedef std::queue<AVFrame *> WriteFrameQueue;

typedef std::vector<AVFrame *>::iterator WriteFrameQueue_iterator;
typedef std::queue<MyFrame *> WriteFrameQueue_h264;
typedef std::vector<MyFrame *>::iterator WriteFrameQueue_iterator_h264;
typedef std::queue<MySocketData *> WriteFrameQueue_Disp;

WriteFrameQueue_h264 m_FrameQueue_H264;

int64_t time_out = 0;
int64_t time_out_dat = 5000;

#define OMX_COLOR_FormatYUV420SemiPlanar  21
#define OMX_COLOR_FormatYUV420Planar    19

bool firsttimeplay = true;


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
    strcpy(m_snapShotPath, path);
    m_bSaveSnapshot = true;
    return FFMPEGPLAYER_NOERROR;
}

int C_FFMpegPlayer::GetStatus() {
    return m_Status;
}

C_FFMpegPlayer::C_FFMpegPlayer() :
        m_Status(E_PlayerStatus_Stoped), nRecFps(20),
        m_formatCtx(nullptr), m_videoStream(0), m_codecCtx(nullptr), m_decodedFrame(nullptr),
        //My_EncodecodecCtx(nullptr),
        m_width(0), m_height(0),
        //m_sws_ctx(nullptr),
        //pFrameRGB(nullptr),
        //video_st(nullptr),
        //m_outsws_ctx(nullptr),
        m_bSaveSnapshot(false), m_bSaveVideo(false),

        m_writeThread(0), m_bOpenOK(false),  //AV_CODEC_ID_MPEG4
        bPause(true), nIC_Type(IC_GK),
        frame_c(nullptr),
        m_parser(nullptr), codec(nullptr), bFlip(false),
        nDisplayWidth(640), nDisplayHeight(360), nNeedRedraw(false), b3D(false),
        frame_b(nullptr), frame_a(nullptr), b480(false), bFollow(false), Rgbabuffer(nullptr), video(MP4_INVALID_TRACK_ID), fileHandle(MP4_INVALID_FILE_HANDLE), bIsH264(false), nSpsSet(0), nSecT(0),
        bContinue(true), b3DA(false), nfps(20), nErrorFrame(0), bStatWrite(false), YUVbuffer(nullptr), pFrameRecord(nullptr), nRecordWidth(640), pFrameYUV_D(nullptr),
        bMirror(false),pFrameYUV_Real(nullptr),nRecordHeight(360)
        {
    m_snapShotPath[0] = 0;  //= m_VideoPath[0]
    pthread_mutex_init(&m_Frame_Queuelock, nullptr);
    pthread_cond_init(&m_Frame_condition, nullptr);

    //pthread_mutex_init(&m_Frame_Queuelock_Display, nullptr);
    //pthread_cond_init(&m_Frame_condition_disp, nullptr);
    sRecordFileName = "";

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

    nRecordWidth = w;
    nRecordHeight = h;
    if (pFrameRecord != nullptr) {
        av_freep(&pFrameRecord->data[0]);
        av_frame_free(&pFrameRecord);
        pFrameRecord = nullptr;
    }
    pFrameRecord = av_frame_alloc();
    pFrameRecord->format = AV_PIX_FMT_YUV420P;
    pFrameRecord->width = nRecordWidth;
    pFrameRecord->height = nRecordHeight;
    int ret = av_image_alloc(
            pFrameRecord->data, pFrameRecord->linesize, nRecordWidth,
            nRecordHeight,
            AV_PIX_FMT_YUVJ420P, 4);

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

int C_FFMpegPlayer::InitMediaSN(void) {
    AVCodec *pCodec = nullptr;
    AVDictionary *optionsDict = nullptr;
    int err_code = 0;

    if (m_decodedFrame == nullptr) {
        F_InitFFfmpeg();
        av_log_set_level(AV_LOG_QUIET);

        m_decodedFrame = av_frame_alloc();
        codec = avcodec_find_decoder(AV_CODEC_ID_MJPEG);
        m_codecCtx = avcodec_alloc_context3(codec);
        m_codecCtx->codec_id = AV_CODEC_ID_MJPEG;
        m_codecCtx->pix_fmt = AV_PIX_FMT_YUVJ422P;
        m_codecCtx->time_base.den = 1;
        m_codecCtx->time_base.num = 1;
        int ret = avcodec_open2(m_codecCtx, codec, nullptr);
        if (ret != 0) {
            m_bOpenOK = false;
        }
        return 0;
    }

    if (m_codecCtx->width <= 0 || m_codecCtx->height <= 0) {
        return 0;
    }

    if (pFrameYUV_Real != nullptr) {
        if (pFrameYUV_Real->width != m_codecCtx->width || pFrameYUV_Real->height != m_codecCtx->height)
        {
            av_freep(&pFrameYUV_Real->data[0]);
            av_frame_free(&pFrameYUV_Real);
            pFrameYUV_Real = nullptr;
        }
        else {
            return 0;
        }

    }


    pix_format = AV_PIX_FMT_YUV420P;

    if (pFrameYUV_Real != nullptr)
    {
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
            pix_format, 4);

    pFrameYUV_Real->format = pix_format;
    pFrameYUV_Real->width = m_codecCtx->width;
    pFrameYUV_Real->height = m_codecCtx->height;
    ret = av_image_alloc(
            pFrameYUV_Real->data, pFrameYUV_Real->linesize, m_codecCtx->width,
            m_codecCtx->height,
            pix_format, 4);


    pFrameRecord = av_frame_alloc();
    pFrameRecord->format = pix_format;
    pFrameRecord->width = nRecordWidth;
    pFrameRecord->height = nRecordHeight;
    ret = av_image_alloc(
            pFrameRecord->data, pFrameRecord->linesize, nRecordWidth,
            nRecordHeight,
            pix_format, 4);


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
                       AV_PIX_FMT_YUV420P, 4);
    }


    if (frame_b == nullptr) {
        frame_b = av_frame_alloc();
        frame_b->format = AV_PIX_FMT_YUV420P;
        frame_b->width = m_codecCtx->width / 2;
        frame_b->height = m_codecCtx->height / 2;
        av_image_alloc(frame_b->data, frame_b->linesize, frame_b->width,
                       frame_b->height,
                       AV_PIX_FMT_YUV420P, 4);
    }
    m_bOpenOK = true;
    return 0;
}


int C_FFMpegPlayer::InitMediaGK(void) {
    AVCodec *pCodec = nullptr;
    AVDictionary *optionsDict = nullptr;
    int err_code = 0;
    m_bOpenOK = false;

    AVDictionary *avCodecOptions = nullptr;

    if (m_decodedFrame == nullptr) {

        F_InitFFfmpeg();
        av_log_set_level(AV_LOG_QUIET);
        m_decodedFrame = av_frame_alloc();
//        codec = avcodec_find_decoder_by_name("h264_mediacodec");//mpeg硬解码
//        if(codec== nullptr)
        codec = avcodec_find_decoder(AV_CODEC_ID_H264);
        m_codecCtx = avcodec_alloc_context3(codec);






        int ret = avcodec_open2(m_codecCtx, nullptr, nullptr);

        //avcodec_parameters_from_context(&para,m_codecCtx);


        m_parser = av_parser_init(codec->id);  //AV_CODEC_ID_H264);
        return 0;
    }


    if (m_codecCtx->width <= 0 || m_codecCtx->height <= 0) {
        return 0;
    }


    if (pFrameYUV_Real != nullptr) {
        if (pFrameYUV_Real->width != m_codecCtx->width || pFrameYUV_Real->height != m_codecCtx->height)
        {
            av_freep(&pFrameYUV_Real->data[0]);
            av_frame_free(&pFrameYUV_Real);
            pFrameYUV_Real = nullptr;
        }
        else {
            return 0;
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
            pix_format, 4);


    pFrameYUV_Real->format = pix_format;
    pFrameYUV_Real->width = m_codecCtx->width;
    pFrameYUV_Real->height = m_codecCtx->height;
    ret = av_image_alloc(
            pFrameYUV_Real->data, pFrameYUV_Real->linesize, m_codecCtx->width,
            m_codecCtx->height,
            pix_format, 4);

    /*
    pFrameRGB->format = disp_pix_format;//AV_PIX_FMT_BGR24;
    pFrameRGB->width = m_codecCtx->width;
    pFrameRGB->height = m_codecCtx->height;
    ret = av_image_alloc(
            pFrameRGB->data, pFrameRGB->linesize, nDisplayWidth,
            nDisplayHeight,
            disp_pix_format, 4);
    */

    pFrameRecord = av_frame_alloc();
    pFrameRecord->format = pix_format;
    pFrameRecord->width = nRecordWidth;
    pFrameRecord->height = nRecordHeight;
    ret = av_image_alloc(
            pFrameRecord->data, pFrameRecord->linesize, nRecordWidth,
            nRecordHeight,
            pix_format, 4);

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
                       AV_PIX_FMT_YUV420P, 4);
    }


    if (frame_b == nullptr) {
        frame_b = av_frame_alloc();
        frame_b->format = AV_PIX_FMT_YUV420P;
        frame_b->width = m_codecCtx->width / 2;
        frame_b->height = m_codecCtx->height / 2;
        av_image_alloc(frame_b->data, frame_b->linesize, frame_b->width,
                       frame_b->height,
                       AV_PIX_FMT_YUV420P, 4);
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
                    AV_PIX_FMT_BGR24, 4);

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


int C_FFMpegPlayer::InitMedia(const char *a_path) {

    string spath = a_path;
    transform(spath.begin(), spath.end(), spath.begin(), ::tolower);
    const char *path = spath.c_str();
    int r1 = spath.find("http://");
    int r2 = spath.find("rtsp:");
    int r3 = spath.find("file:");

    if(r1 != 0 && r2 != 0 && r3 != 0)
    {
        if(nDataType == DATA_Type_H264)
        {
            InitMediaGK();
            F_RecreateEnv();
            return 0;
        }
        if(nDataType == DATA_Type_MJPEG)
        {
            InitMediaSN();
            F_RecreateEnv();
            return 0;
        }
        if(nDataType != DATA_Type_NO)
        {
            return -1;
        }
    }


/*
    if (nIC_Type == IC_GK_UDP) {
        InitMediaGK();
        F_RecreateEnv();
        return 0;
    }

    if (nIC_Type == IC_GKA) {

        InitMediaGK();
        F_RecreateEnv();
        return 0;
    }
    if (nIC_Type == IC_GPH264A || nIC_Type == IC_RTLH264) {
        InitMediaGK();
        F_RecreateEnv();
        return 0;
    }
    if (nIC_Type == IC_SN) {
        InitMediaSN();
        F_RecreateEnv();
        return 0;
    }

    if (nIC_Type == IC_GPRTP || nIC_Type == IC_GPRTPB || nIC_Type == IC_GPRTPC) {

        nDisplayHeight = 480;
        nDisplayWidth = 640;
        InitMediaSN();
        F_RecreateEnv();
        return 0;
    }
*/
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

    if ((err_code = avformat_open_input(&m_formatCtx, path, nullptr, nullptr)) != 0) {
        av_strerror(err_code, buffer, 1023);
        DEBUG_PRINT("avformat_open_input %s failed!", path);
        DEBUG_PRINT("%s", buffer);
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
            DEBUG_PRINT("avformat_find_stream_info failed!\n");
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
        //if (m_formatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)  //ByAiven
        if (m_formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoindex = i;
            m_videoStream = i;
            break;
        }
    if (videoindex == -1) {
        LOGE("Didn't find a video stream.\n");
        avformat_close_input(&m_formatCtx);
        avformat_free_context(m_formatCtx);
        m_formatCtx = nullptr;
        return -1;
    }

    AVStream *stream = m_formatCtx->streams[videoindex];



//    if(stream->codecpar->codec_id == AV_CODEC_ID_H264) {
//        pCodec = avcodec_find_decoder_by_name("h264_mediacodec");//mpeg硬解码
//    }
//    else if(stream->codecpar->codec_id == AV_CODEC_ID_HEVC) {
//        pCodec = avcodec_find_decoder_by_name("hevc_mediacodec");//mpeg硬解码
//    }
//    else if(stream->codecpar->codec_id == AV_CODEC_ID_MPEG4){
//        pCodec = avcodec_find_decoder_by_name("mpeg4_mediacodec");//mpeg硬解码
//    }
//    if(pCodec== nullptr) {
        pCodec = avcodec_find_decoder(stream->codecpar->codec_id);
//    }


    if (pCodec == nullptr) {
        DEBUG_PRINT("Unsupported codec!\n");
        avformat_close_input(&m_formatCtx);
        avformat_free_context(m_formatCtx);
        m_formatCtx = nullptr;
        return FFMPEGPLAYER_INITMEDIAFAILED; // Codec not found
    }

    m_codecCtx = avcodec_alloc_context3(pCodec);
    avcodec_parameters_to_context(m_codecCtx, stream->codecpar);
    F_ResetCheckT(5000);
    if (avcodec_open2(m_codecCtx, pCodec, &optionsDict) < 0) {
        DEBUG_PRINT("avcodec_open2 failed!\n");
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

    if(pFrameYUV_Real!=nullptr)
    {
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
            pix_format, 4);

    frame_SnapBuffer = av_frame_alloc();

    frame_SnapBuffer->format = pix_format;
    frame_SnapBuffer->width = m_codecCtx->width;
    frame_SnapBuffer->height = m_codecCtx->height;
    ret = av_image_alloc(
            frame_SnapBuffer->data, frame_SnapBuffer->linesize, m_codecCtx->width,
            m_codecCtx->height,
            pix_format, 4);

    pFrameRecord = av_frame_alloc();
    pFrameRecord->format = pix_format;
    pFrameRecord->width = nRecordWidth;
    pFrameRecord->height = nRecordHeight;
    ret = av_image_alloc(
            pFrameRecord->data, pFrameRecord->linesize, nRecordWidth,
            nRecordHeight,
            pix_format, 4);

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
                       AV_PIX_FMT_YUV420P, 4);
    }


    if (frame_b == nullptr) {
        frame_b = av_frame_alloc();
        frame_b->format = AV_PIX_FMT_YUV420P;
        frame_b->width = m_codecCtx->width / 2;
        frame_b->height = m_codecCtx->height / 2;
        av_image_alloc(frame_b->data, frame_b->linesize, frame_b->width,
                       frame_b->height,
                       AV_PIX_FMT_YUV420P, 4);
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
    return FFMPEGPLAYER_NOERROR;
}

//----------------------------------------------------------------------
int C_FFMpegPlayer::Stop() {
#ifdef D_H264file
    if(h64fileHandle!=-1)
        {
            close(h64fileHandle);
            h64fileHandle = -1;
        }
#endif
    m_Status = E_PlayerStatus_Stoping;
    m_bSaveVideo = false;
    usleep(1000 * 50);
    F_ResetCheckT(10);
    usleep(1000 * 20);
    //if (nIC_Type == IC_GKA || nIC_Type == IC_SN || nIC_Type == IC_GPRTP || nIC_Type == IC_GPH264A || nIC_Type == IC_GPRTPB || nIC_Type == IC_GK_UDP)
    if (nIC_Type == IC_GPH264 ||
        nIC_Type == IC_GPRTSP) { ;
    } else {
        Releaseffmpeg();
    }
    m_bOpenOK = false;
    return FFMPEGPLAYER_NOERROR;
}

//----------------------------------------------------------------------
extern pthread_mutex_t m_mp4Write_lock;

int C_FFMpegPlayer::WriteMp4Frame(uint8_t *data, int nLen, bool b) {

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


extern bool bG_Audio;


int C_FFMpegPlayer::AddMp4Video(uint8_t *sps, int len1, uint8_t *pps, int len2) {

    if (fileHandle != MP4_INVALID_FILE_HANDLE) {
        video = MP4AddH264VideoTrack(fileHandle, 90000, (uint32_t) (90000 / nRecFps), (uint16_t) nRecordWidth, (uint16_t) nRecordHeight, sps[1], sps[2], sps[3], 3);
        if (video == MP4_INVALID_TRACK_ID) {
            MP4Close(fileHandle, 0);
            fileHandle = MP4_INVALID_FILE_HANDLE;
            return -1;
        } else {
            MP4AddH264SequenceParameterSet(fileHandle, video, sps, (uint16_t) len1);
            MP4AddH264PictureParameterSet(fileHandle, video, pps, (uint16_t) len2);
            MP4SetVideoProfileLevel(fileHandle, 0x7F);
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
            MP4AacGetConfiguration(&pConfig, &configLength, profile, samplesPerSecond, channelConfig);
            MP4SetTrackESConfiguration(fileHandle, music, pConfig, configLength);
            free(pConfig);
        }

        return 0;
    }
    return -1;

}

void F_SetRecordAudio(int n);

int C_FFMpegPlayer::SaveVideo(const char *path, bool bisH264) {

    bisH264 = true;
    bRealRecording = false;
    nRecFrameCount = 0;

    if (bG_Audio) {
        F_SetRecordAudio(1);
    } else {
        F_SetRecordAudio(0);
    }

    music = MP4_INVALID_TRACK_ID;
    bStatWrite = false;
    this->bIsH264 = bisH264;
    sRecordFileName = path;
    sRecordFileName_tmp = path;
    sRecordFileName_tmp = sRecordFileName_tmp + "_.tmp";
    if (m_codecCtx == nullptr) {
        this->bIsH264 = false;
        fileHandle = MP4_INVALID_FILE_HANDLE;
        return -1;
    }
    if (m_codecCtx->width <= 0 || m_codecCtx->height <= 0) {
        this->bIsH264 = false;
        fileHandle = MP4_INVALID_FILE_HANDLE;
        return -1;
    }

    //if (bisH264)
    {


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
        pthread_create(&m_writeThread, nullptr, WriteThreadFunction, this);
        return 0;
    }
    return FFMPEGPLAYER_NOERROR;
}


//----------------------------------------------------------------------
void C_FFMpegPlayer::ClearQueue() {
    pthread_mutex_lock(&m_Frame_Queuelock);
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
    if (m_bSaveVideo && fileHandle != MP4_INVALID_FILE_HANDLE && music != MP4_INVALID_TRACK_ID) {
        bool bre;
        pthread_mutex_lock(&m_mp4Write_lock);
         bre = MP4WriteSample(fileHandle, music, (const uint8_t *) data, (uint32_t) nLen, MP4_INVALID_DURATION, 0, 1);
        pthread_mutex_unlock(&m_mp4Write_lock);
        return  bre;
    } else {
        return false;
    }
}

void C_FFMpegPlayer::F_DispH264NoBuffer(MySocketData *data) {
    {
        bCanDisplay = false;
        decodeAndRender_GKA_B(data);
    }
}


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

int C_FFMpegPlayer::decodeAndRender_GKA(MySocketData *data) {
    F_ResetRelinker();
    if (data->nLen < 4)
        return 0;
    F_DispH264NoBuffer(data);
    return 0;
}


int F_Decord_JAVA(uint8 *data, int nLen);

void C_FFMpegPlayer::F_StartDispThread() {

    m_Status = E_PlayerStatus_Playing;

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
extern int g_sdkVersion;
int nCount_fps = -1;
extern int64_t nPreTimeA;

int C_FFMpegPlayer::decodeAndRender_GKA_B(MySocketData *data) {

    F_ResetRelinker();
    if (data->nLen < 4)
        return 0;
#if 1

    int size = 0;
    int len;

    int in_len = (int) data->nLen;
    uint8_t *outbuff = nullptr;
    uint8_t *in_data;
    in_data = data->data;
    while (in_len) {
        len = av_parser_parse2(m_parser, m_codecCtx, &outbuff, &size, in_data, in_len, 0, 0, 0);
        in_data += len;
        in_len -= len;
        if (size > 0) {
            AVPacket packetA = {0};
            av_init_packet(&packetA);
            packetA.data = outbuff;
            packetA.size = size;
            if (avcodec_send_packet(m_codecCtx, &packetA) == 0) {
                if ((avcodec_receive_frame(m_codecCtx, m_decodedFrame)) == 0) {
//                    AVCodecParameters para;
//                    avcodec_parameters_from_context(&para,m_codecCtx);

                    _DispDecordData();
                }
            }
            av_packet_unref(&packetA);
        }
    }
    return 0;

#else

#endif
}


void C_FFMpegPlayer::_DispDecordData(void) {
//    nDisplayWidth = m_codecCtx->width;
//    nDisplayHeight = m_codecCtx->height;
    InitMediaGK();
    F_RecreateEnv();

    if (m_decodedFrame->format == AV_PIX_FMT_YUVJ420P || m_decodedFrame->format == AV_PIX_FMT_YUV420P) {
        libyuv::I420Copy(m_decodedFrame->data[0], m_decodedFrame->linesize[0],
                         m_decodedFrame->data[1], m_decodedFrame->linesize[1],
                         m_decodedFrame->data[2], m_decodedFrame->linesize[2],
                         pFrameYUV_Real->data[0], pFrameYUV_Real->linesize[0],
                         pFrameYUV_Real->data[1], pFrameYUV_Real->linesize[1],
                         pFrameYUV_Real->data[2], pFrameYUV_Real->linesize[2],
                         m_decodedFrame->width, m_decodedFrame->height);
    } else if (m_decodedFrame->format == AV_PIX_FMT_YUV422P || m_decodedFrame->format == AV_PIX_FMT_YUVJ422P) {
        libyuv::I422ToI420(m_decodedFrame->data[0], m_decodedFrame->linesize[0],
                           m_decodedFrame->data[1], m_decodedFrame->linesize[1],
                           m_decodedFrame->data[2], m_decodedFrame->linesize[2],
                           pFrameYUV_Real->data[0], pFrameYUV_Real->linesize[0],
                           pFrameYUV_Real->data[1], pFrameYUV_Real->linesize[1],
                           pFrameYUV_Real->data[2], pFrameYUV_Real->linesize[2],
                           m_decodedFrame->width, m_decodedFrame->height);
    }
    else{
        if (img_convert_ctx != nullptr)
        {
            sws_scale(img_convert_ctx,
                      (const uint8_t *const *) m_decodedFrame->data,
                      m_decodedFrame->linesize, 0,
                      m_codecCtx->height,
                      pFrameYUV_Real->data, pFrameYUV_Real->linesize);
        }
    }

    SetYUVFrame(pFrameYUV_Real);
}




int C_FFMpegPlayer::decodeAndRender_RTP(char *data, int nLen) {


    return decodeAndRender_SN(data, nLen);

}

//GPH264

int C_FFMpegPlayer::decodeAndRender_SN(char *data, int nLen) {
    if (nLen <= 0) {
        return 0;
    }

    uint8_t *in_data = (uint8_t *) data;// new uint8_t[in_len];
    int ret;
    AVPacket packet = {0};
    av_init_packet(&packet);
    packet.data = in_data;
    packet.size = nLen;
    if (avcodec_send_packet(m_codecCtx, &packet) == 0) {
        if (avcodec_receive_frame(m_codecCtx, m_decodedFrame) != 0) {
            ret = -1;
        } else {
            ret = 0;
        }
    } else {
        ret = -1;
    }

    if (ret == 0)
    {

        m_Status = E_PlayerStatus_Playing;

        InitMediaSN();

        if (m_decodedFrame->format == AV_PIX_FMT_YUVJ420P || m_decodedFrame->format == AV_PIX_FMT_YUV420P) {
            libyuv::I420Copy(m_decodedFrame->data[0], m_decodedFrame->linesize[0],
                             m_decodedFrame->data[1], m_decodedFrame->linesize[1],
                             m_decodedFrame->data[2], m_decodedFrame->linesize[2],
                             pFrameYUV_Real->data[0], pFrameYUV_Real->linesize[0],
                             pFrameYUV_Real->data[1], pFrameYUV_Real->linesize[1],
                             pFrameYUV_Real->data[2], pFrameYUV_Real->linesize[2],
                             m_decodedFrame->width, m_decodedFrame->height);
        } else if (m_decodedFrame->format == AV_PIX_FMT_YUV422P || m_decodedFrame->format == AV_PIX_FMT_YUVJ422P) {
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
    av_packet_unref(&packet);
    return 0;

}

int C_FFMpegPlayer::SetYUVFrame(AVFrame *yunframe) {

    if (yunframe != pFrameYUV_Real) {
        pFrameYUV_Real = yunframe;
    }
    if (pFrameYUV_Real->width <= 0 || pFrameYUV_Real->height <= 0) {
        return -1;
    }

    int www1 = pFrameYUV_Real->width;
    int hhh1 = pFrameYUV_Real->height;
    if(nCameraDataRota == 90 || nCameraDataRota == 270)
    {
        www1 = pFrameYUV_Real->height;
        hhh1 = pFrameYUV_Real->width;
    }


    if (pFrameYUV != nullptr)
    {
        if (pFrameYUV->width != www1 || pFrameYUV->height != hhh1) {
            av_freep(&pFrameYUV->data[0]);
            av_frame_free(&pFrameYUV);
            pFrameYUV = nullptr;
        }
    }

    if (pFrameYUV == nullptr)
    {
        pFrameYUV = av_frame_alloc();
        pFrameYUV->format = AV_PIX_FMT_YUV420P;
        pFrameYUV->width = www1;
        pFrameYUV->height = hhh1;
        av_image_alloc(
                pFrameYUV->data, pFrameYUV->linesize, pFrameYUV->width,
                pFrameYUV->height,
                AV_PIX_FMT_YUV420P, 4);
    }


    if(nCameraDataRota!=0)
    {

        int rot =  libyuv::kRotate0;
        if(nCameraDataRota == 90 )
        {
            rot = libyuv::kRotate90;
        }
        if(nCameraDataRota == 180 )
        {
            rot = libyuv::kRotate180;
        }
        if(nCameraDataRota == 270 )
        {
            rot = libyuv::kRotate270;
        }

        libyuv::I420Rotate(pFrameYUV_Real->data[0], pFrameYUV_Real->linesize[0],
                           pFrameYUV_Real->data[1], pFrameYUV_Real->linesize[1],
                           pFrameYUV_Real->data[2], pFrameYUV_Real->linesize[2],
                           pFrameYUV->data[0], pFrameYUV->linesize[0],
                           pFrameYUV->data[1], pFrameYUV->linesize[1],
                           pFrameYUV->data[2], pFrameYUV->linesize[2],
                           pFrameYUV_Real->width, pFrameYUV_Real->height,
                           (libyuv::RotationMode)rot);

    }
    else
    {
         libyuv::I420Copy(pFrameYUV_Real->data[0], pFrameYUV_Real->linesize[0],
                          pFrameYUV_Real->data[1], pFrameYUV_Real->linesize[1],
                          pFrameYUV_Real->data[2], pFrameYUV_Real->linesize[2],
                          pFrameYUV->data[0], pFrameYUV->linesize[0],
                          pFrameYUV->data[1], pFrameYUV->linesize[1],
                          pFrameYUV->data[2], pFrameYUV->linesize[2],
                          pFrameYUV_Real->width, pFrameYUV_Real->height);
    }


    nDisplayWidth = pFrameYUV->width;
    nDisplayHeight = pFrameYUV->height;


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
                    AV_PIX_FMT_YUV420P, 4);
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
                              pFrameYUV->data[0], nSize,
                              pFrameYUV->data[1], nSize >> 1,
                              pFrameYUV->data[2], nSize >> 1,
                              lx, ly,
                              pFrameYUV_D->width, pFrameYUV_D->height,
                              pFrameYUV->width, pFrameYUV->height,
                              libyuv::kRotate0, libyuv::FOURCC_I420);
    }


    if (frame_SnapBuffer != nullptr) {
        if (frame_SnapBuffer->width != pFrameYUV->width || frame_SnapBuffer->height != pFrameYUV->height) {
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
                AV_PIX_FMT_YUV420P, 4);
    }

    if (bMirror) {
        if (frame_a != nullptr) {
            if(frame_a->width!=pFrameYUV->width || frame_a->height!=pFrameYUV->height) {
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
                           AV_PIX_FMT_YUV420P, 4);
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
            if(frame_a->width!=pFrameYUV->width || frame_a->height!=pFrameYUV->height) {
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
                           AV_PIX_FMT_YUV420P, 4);
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
            if(frame_b->width!=pFrameYUV->width || frame_b->height!=pFrameYUV->height) {
                av_freep(&frame_b->data[0]);
                av_frame_free(&frame_b);
                frame_b = nullptr;
            }
        }

        if (frame_b == nullptr) {
            frame_b = av_frame_alloc();
            frame_b->format = AV_PIX_FMT_YUV420P;
            frame_b->width = pFrameYUV->width/2;
            frame_b->height = pFrameYUV->height/2;
            av_image_alloc(frame_b->data, frame_b->linesize, pFrameYUV->width/2,
                           pFrameYUV->height/2,
                           AV_PIX_FMT_YUV420P, 4);
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



    if (m_decodedFrame != nullptr)
    {
        libyuv::I420Copy(pFrameYUV->data[0], pFrameYUV->linesize[0],
                         pFrameYUV->data[1], pFrameYUV->linesize[1],
                         pFrameYUV->data[2], pFrameYUV->linesize[2],
                         frame_SnapBuffer->data[0], frame_SnapBuffer->linesize[0],
                         frame_SnapBuffer->data[1], frame_SnapBuffer->linesize[1],
                         frame_SnapBuffer->data[2], frame_SnapBuffer->linesize[2],
                         frame_SnapBuffer->width, frame_SnapBuffer->height);

    }
    bCanDisplay = true;
    PlatformDisplay();

    return 0;

}

int C_FFMpegPlayer::decodeAndRender() {
    if (nIC_Type == IC_SN)
        return 0;
    int frameFinished = 0;
    AVPacket packet;
    int ret = 0;
    time_out = getCurrentTime();
    firsttimeplay = true;
    F_ResetCheckT(0);
    while (m_Status == E_PlayerStatus_Playing)
    {
        if (av_read_frame(m_formatCtx, &packet) < 0)
            break;
        time_out = getCurrentTime();
        firsttimeplay = true;


        if (packet.stream_index == m_videoStream)
        {
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

                if(pFrameYUV_Real!= nullptr)
                {
                    if(pFrameYUV_Real->width !=m_codecCtx->width || pFrameYUV_Real->height !=m_codecCtx->height)
                    {
                        av_freep(&pFrameYUV_Real->data[0]);
                        av_frame_free(&pFrameYUV_Real);
                        pFrameYUV_Real = nullptr;
                    }
                }
                if(pFrameYUV_Real == nullptr)
                {
                    pFrameYUV_Real->format = AV_PIX_FMT_YUV420P;
                    pFrameYUV_Real->width = m_codecCtx->width;
                    pFrameYUV_Real->height = m_codecCtx->height;
                    av_image_alloc(
                            pFrameYUV_Real->data, pFrameYUV_Real->linesize, pFrameYUV_Real->width,
                            pFrameYUV_Real->height,
                            AV_PIX_FMT_YUV420P, 4);
                }

                if (m_decodedFrame->format == AV_PIX_FMT_YUVJ420P || m_decodedFrame->format == AV_PIX_FMT_YUV420P) {
                    libyuv::I420Copy(m_decodedFrame->data[0], m_decodedFrame->linesize[0],
                                     m_decodedFrame->data[1], m_decodedFrame->linesize[1],
                                     m_decodedFrame->data[2], m_decodedFrame->linesize[2],
                                     pFrameYUV_Real->data[0], pFrameYUV_Real->linesize[0],
                                     pFrameYUV_Real->data[1], pFrameYUV_Real->linesize[1],
                                     pFrameYUV_Real->data[2], pFrameYUV_Real->linesize[2],
                                     m_decodedFrame->width, m_decodedFrame->height);
                } else if (m_decodedFrame->format == AV_PIX_FMT_YUV422P || m_decodedFrame->format == AV_PIX_FMT_YUVJ422P) {
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
    m_bSaveVideo = false;
    bRealRecording = false;


    usleep(1000 * 50);
    F_SetRecordAudio(0);
    if (m_writeThread != 0) {
        pthread_join(m_writeThread, &ret);
        m_writeThread = 0;
    }


    if (pthread_mutex_trylock(&m_Frame_Queuelock) == 0) {
        pthread_cond_signal(&m_Frame_condition);
        pthread_mutex_unlock(&m_Frame_Queuelock);
    }

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

int C_FFMpegPlayer::writeVideo() {
    m_bSaveVideo = true;
    int64_t T1 = av_gettime() / 1000;
    int64_t T2 = av_gettime() / 1000;
    int nDelay = 1000 / nRecFps;
    if (bIsH264) {
        int nLen = 0;
        MyFrame *pMyFrame = nullptr;
        bool bNeedDelete = false;
        while (m_bSaveVideo && m_Status == E_PlayerStatus_Playing) {
            pMyFrame = nullptr;
            struct timespec timespec1;
            timespec1.tv_sec = 0;
            timespec1.tv_nsec = 1000 * 1000 * 5;
            pthread_mutex_lock(&m_Frame_Queuelock);
            pthread_cond_timedwait(&m_Frame_condition, &m_Frame_Queuelock, &timespec1);
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
                T2 = av_gettime() / 1000;
                int daa = (int) (T2 - T1);
                if (daa < nDelay) {
                    usleep((useconds_t) ((nDelay - daa) * 1000));
                }
                T1 = av_gettime() / 1000;
            } else {
                usleep(5 * 1000); //delay 10ms  让出CPU给其他线程
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
    DEBUG_PRINT("CloseVideo\n");
    ClearQueue();
    return FFMPEGPLAYER_NOERROR;
}


int nSteppp = 0;

void F_SentRevBmp(int32_t wh);
void F_SentGestureBmp(int32_t wh);

extern int nBufferLen;
int encord_colorformat;


AVPacket *C_FFMpegPlayer::F_GetPacket(void) {
    int ret, got_output;
    if (pFrameYUV == nullptr)
        return nullptr;
    if (m_codecCtx == nullptr)
        return nullptr;
        if (m_FrameQueue_H264.size() < 8) {
            if (m_bSaveVideo) {
                AVFrame *frame_rec;
                if (pFrameYUV->width != nRecordWidth || pFrameYUV->height != nRecordHeight) {
                    libyuv::I420Scale(pFrameYUV->data[0], pFrameYUV->linesize[0],
                                      pFrameYUV->data[1], pFrameYUV->linesize[1],
                                      pFrameYUV->data[2], pFrameYUV->linesize[2],
                                      pFrameYUV->width, pFrameYUV->height,
                                      pFrameRecord->data[0], pFrameRecord->linesize[0],
                                      pFrameRecord->data[1], pFrameRecord->linesize[1],
                                      pFrameRecord->data[2], pFrameRecord->linesize[2],
                                      pFrameRecord->width, pFrameRecord->height,
                                      libyuv::kFilterLinear);
                    frame_rec = pFrameRecord;
                } else {
                    frame_rec = pFrameYUV;
                }

                int HH = nRecordHeight;
                int H2 = HH >> 1;

                int WW = nRecordWidth;
                int W2 = WW >> 1;

                uint8_t *srcY = frame_rec->data[0];
                uint8_t *srcU = frame_rec->data[1];
                uint8_t *srcV = frame_rec->data[2];

                uint32_t nCount = 0;
                auto *myfram = new MyFrame();

                nCount += frame_rec->linesize[0] * HH;
                nCount += frame_rec->linesize[1] * H2;
                nCount += frame_rec->linesize[2] * H2;
                myfram->data = new uint8_t[nCount];
                myfram->nLen = nCount;
                unsigned char *bufY = myfram->data;
                unsigned char *bufU = bufY + WW * HH;
                unsigned char *bufV = bufU + (W2 * H2);

                if (encord_colorformat == OMX_COLOR_FormatYUV420SemiPlanar)   //YUV420 或者 NV2 主要是Android  硬编码，有些机型支持的 数据格式只有OMX_COLOR_FormatYUV420SemiPlanar
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
                m_FrameQueue_H264.push(myfram);
                pthread_cond_signal(&m_Frame_condition);
                pthread_mutex_unlock(&m_Frame_Queuelock);
            }
        }

    if (m_bSaveSnapshot)
    {
        EncodeSnapshot();
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

int C_FFMpegPlayer::EncodeSnapshot() {


    int WW = nRecordWidth;
    int HH = nRecordHeight;
    if (bPhotoSet) {
        WW = nPhotoW;
        HH = nPhotoH;
    }

    AVCodec *codec;
    AVCodecContext *cSnap = nullptr;
    int ret, got_output;
    AVPacket pkt;

    m_bSaveSnapshot = false;

    codec = avcodec_find_encoder(AV_CODEC_ID_MJPEG);
    if (!codec) {
        return FFMPEGPLAYER_SAVESNAPSHOTFAILED;
    }
    cSnap = avcodec_alloc_context3(codec);
    if (!cSnap) {
        return FFMPEGPLAYER_SAVESNAPSHOTFAILED;
    }


    cSnap->codec_type = AVMEDIA_TYPE_VIDEO;
    cSnap->width = WW;   // 1280;//m_codecCtx->width;
    cSnap->height = HH; //720;//m_codecCtx->height;
    cSnap->time_base.num = 1;//m_codecCtx->time_base.num;
    cSnap->time_base.den = 1;//m_codecCtx->time_base.den;
    cSnap->pix_fmt = AV_PIX_FMT_YUVJ420P;  //  pix_format;//m_codecCtx->pix_fmt;





    if ((ret = avcodec_open2(cSnap, codec, nullptr)) < 0) {
        DEBUG_PRINT("Could not open codecc\n");
        return FFMPEGPLAYER_SAVESNAPSHOTFAILED;
    }

    av_init_packet(&pkt);
    pkt.data = nullptr;
    pkt.size = 0;
    ret = 0;
    //if (ret != 0) {
    //    return FFMPEGPLAYER_SAVESNAPSHOTFAILED;
    //}
    AVFrame *frame_Snap = nullptr;

    bool bAlloc = false;

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
                pix_format, 4);


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

        uint8 *pbufferA = new uint8[WW * 4 * HH];
        uint8 *pbuffer = pbufferA;

        libyuv::I420ToABGR(frame_Snap->data[0], frame_Snap->linesize[0],
                           frame_Snap->data[1], frame_Snap->linesize[1],
                           frame_Snap->data[2], frame_Snap->linesize[2],
                           pbuffer, frame_Snap->width * 4,
                           frame_Snap->width, frame_Snap->height);


        RGBA_STRUCT df = {0, 0, 0, 0};

        {
            int r, g, b;
            if (nDispStyle == 2) {
                df = {255, 0, 0, 20};
            } else if (nDispStyle == 3) {
                df = {255, 255, 0, 20};
            } else if (nDispStyle == 4) {
                df = {0, 255, 0, 20};
            } else if (nDispStyle == 5) {
                df = {128, 69, 9, 50};
            } else if (nDispStyle == 6) {
                df = {0, 0, 255, 20};
            }

            RGBA_STRUCT *buffer;
            uint8 dat = 0;
            float ap = df.a / 100.0f;
            for (int y = 0; y < WW; y++) {
                for (int x = 0; x < HH; x++) {
                    buffer = (RGBA_STRUCT *) pbuffer;
                    if (nDispStyle == 1) {
                        dat = (uint8) (((*buffer).r * 38 + (*buffer).g * 75 + (*buffer).b * 15) >> 7);
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
                        (*buffer).r = (uint8) r;
                        (*buffer).g = (uint8) g;
                        (*buffer).b = (uint8) b;
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

    if (avcodec_send_frame(cSnap, frame_Snap) == 0) {
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


    if (ret < 0) {
        DEBUG_PRINT("Error encoding frame\n");
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
            return FFMPEGPLAYER_SAVESNAPSHOTFAILED;
        }
    }

    av_packet_unref(&pkt);
    avcodec_close(cSnap);
    avcodec_free_context(&cSnap);
    F_OnSave2ToGallery_mid(0);
    return FFMPEGPLAYER_NOERROR;
}

//----------------------------------------------------------------------
int C_FFMpegPlayer::Releaseffmpeg() {


    m_Status = E_PlayerStatus_Stoped;


    avcodec_close(m_codecCtx);
    avformat_close_input(&m_formatCtx);

    sws_freeContext(img_convert_ctx);
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
        if (pFrameYUV_Real != nullptr)
        {
            av_freep(&pFrameYUV_Real->data[0]);
            av_frame_free(&pFrameYUV_Real);
            pFrameYUV_Real = nullptr;
        }

        if (pFrameYUV != nullptr)
        {
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

        if(pGestureFrame!= nullptr)
        {
            if (pGestureFrame->width != ww || pGestureFrame->height != hh)
            {
                av_freep(&pGestureFrame->data[0]);
                av_frame_free(&pGestureFrame);
                pGestureFrame = nullptr;
            }
        }
        if(pGestureFrame == nullptr)
        {
            pGestureFrame = av_frame_alloc();
            pGestureFrame->width = ww;
            pGestureFrame->height = hh;
            av_image_alloc(pGestureFrame->data, pGestureFrame->linesize, ww,
                           hh,
                           AV_PIX_FMT_YUV420P, 4);
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
        }
        else
        {
            libyuv::I420ToABGR(pFrameYUV->data[0], pFrameYUV->linesize[0],
                               pFrameYUV->data[1], pFrameYUV->linesize[1],
                               pFrameYUV->data[2], pFrameYUV->linesize[2],
                               Rgbabuffer, pFrameYUV->width * 4,
                               pFrameYUV->width, pFrameYUV->height);
        }
        F_SentGestureBmp(ww + hh * 0x10000);
    } else
        {
        if(pGestureFrame!= nullptr) {
            av_freep(&pGestureFrame->data[0]);
            av_frame_free(&pGestureFrame);
            pGestureFrame = nullptr;
        }
    }
#endif
    if (bSentRevBMP_bak)
    {
        if (Rgbabuffer != nullptr)
        {
            int W1 = pFrameYUV->width;
            int H1 = pFrameYUV->height;
            if (bTransferSize && pTranFrame != nullptr)
            {
                if (pFrameYUV->width != pTranFrame->width || pFrameYUV->height != pTranFrame->height)   //转换为 设定的尺寸，然后传给应用层。
                {

                    libyuv::I420Scale(pFrameYUV->data[0], pFrameYUV->linesize[0],
                                      pFrameYUV->data[1], pFrameYUV->linesize[1],
                                      pFrameYUV->data[2], pFrameYUV->linesize[2],
                                      pFrameYUV->width, pFrameYUV->height,
                                      pTranFrame->data[0], pTranFrame->linesize[0],
                                      pTranFrame->data[1], pTranFrame->linesize[1],
                                      pTranFrame->data[2], pTranFrame->linesize[2],
                                      nTransferWidth, nTransferHeight,
                                      libyuv::kFilterLinear);

                    libyuv::I420ToABGR(pTranFrame->data[0], pTranFrame->linesize[0],
                                       pTranFrame->data[1], pTranFrame->linesize[1],
                                       pTranFrame->data[2], pTranFrame->linesize[2],
                                       Rgbabuffer, pTranFrame->width * 4,
                                       pTranFrame->width, pTranFrame->height);
                    W1 = pTranFrame->width;
                    H1 = pTranFrame->height;
                }

            } else {
                libyuv::I420ToABGR(pFrameYUV->data[0], pFrameYUV->linesize[0],
                                   pFrameYUV->data[1], pFrameYUV->linesize[1],
                                   pFrameYUV->data[2], pFrameYUV->linesize[2],
                                   Rgbabuffer, pFrameYUV->width * 4,
                                   pFrameYUV->width, pFrameYUV->height);
            }
            F_SentRevBmp(W1 + H1 * 0x10000);
        }
        if (bSentRevBMP)
            return;
    }

        if (gl_Frame != nullptr)
        {
            if ((nSDStatus & bit1_LocalRecording) && !bRealRecording) {
                bRealRecording = true;
                //nRecStartTime = av_gettime() / 1000;
            }
//            if(gl_Frame->width != pFrameYUV->width ||
//                    gl_Frame->height != pFrameYUV->height)
//            {
//                av_freep(&gl_Frame->data[0]);
//                av_frame_free(&gl_Frame);
//
//                gl_Frame = av_frame_alloc();
//                gl_Frame->format = AV_PIX_FMT_YUV420P;
//                gl_Frame->width = pFrameYUV->width;
//                gl_Frame->height = pFrameYUV->height;
//                av_image_alloc(
//                        gl_Frame->data, gl_Frame->linesize, pFrameYUV->width,
//                        pFrameYUV->height,
//                        AV_PIX_FMT_YUV420P, 4);
//
//            }

            //pTmpAvFrame
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


