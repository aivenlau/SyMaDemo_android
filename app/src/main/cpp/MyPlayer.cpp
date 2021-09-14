//
// Created by 小米哥 on 2020/10/12.
//

#include <libyuv/convert.h>
#include "MyPlayer.h"

MyPlayer::MyPlayer() : connectdid(-1), bisPlaying(false), bIsDecordOK(false), nSeekTime(-1), object(
        nullptr), Playduration_Mid(nullptr), PlayStatus_Mid(nullptr), PlayTime_mid(nullptr) {
    memset(sPlayName, 0, sizeof(sPlayName));

}

MyPlayer::~MyPlayer() {

}

JNIEnv *getJNIEnv(int *needsDetach);

extern JavaVM *gJavaVM;


void MyPlayer::SentPlayduration_Mid(int i) {
    if (object && Playduration_Mid) {
        int needsDetach = 0;
        JNIEnv *env = getJNIEnv(&needsDetach);
        if (env == nullptr) {
            return;
        }
        env->CallVoidMethod(object, Playduration_Mid, i);

        if (needsDetach)
            gJavaVM->DetachCurrentThread();
    }
}

void MyPlayer::SentPlayStatus_Mid(int i) {
    if (object && PlayStatus_Mid) {
        int needsDetach = 0;
        JNIEnv *env = getJNIEnv(&needsDetach);
        if (env == nullptr) {
            return;
        }
        env->CallVoidMethod(object, PlayStatus_Mid, i);

        if (needsDetach)
            gJavaVM->DetachCurrentThread();
    }
}

void MyPlayer::SentPlayTime_midTest(int i)
{
    if (nSeekTime!= 0)
    {

    }
}

void MyPlayer::SentPlayTime_mid(int i) {

    if (object!= nullptr && PlayTime_mid!= nullptr)
    {
        int needsDetach = 0;
        JNIEnv *env = getJNIEnv(&needsDetach);
        if (env == nullptr) {
            return;
        }
        env->CallVoidMethod(object, PlayTime_mid, i);

        if (needsDetach)
            gJavaVM->DetachCurrentThread();
    }

}

AVPixelFormat hw_pix_fmt = AV_PIX_FMT_NONE;

static enum AVPixelFormat get_hw_format(AVCodecContext *ctx,
                                        const enum AVPixelFormat *pix_fmts) {
    const enum AVPixelFormat *p;

    for (p = pix_fmts; *p != -1; p++) {
        if (*p == hw_pix_fmt)
            return *p;
    }

    fprintf(stderr, "Failed to get HW surface format.\n");
    return AV_PIX_FMT_NONE;
}

void MyPlayer::F_SetCallBack(jobject object1) {
    int needsDetach = 0;
    JNIEnv *env = getJNIEnv(&needsDetach);
    if (env == nullptr) {
        return;
    }
    if (object) {
        env->DeleteGlobalRef(object);
        object = nullptr;
    }

    object = env->NewGlobalRef(object1);

    jclass jcInfo = env->GetObjectClass(object);
    Playduration_Mid = env->GetMethodID(jcInfo, "Playduration", "(I)V");
    PlayStatus_Mid = env->GetMethodID(jcInfo, "PlayStatus", "(I)V");
    PlayTime_mid = env->GetMethodID(jcInfo, "PlayTime", "(I)V");

    if (needsDetach)
        gJavaVM->DetachCurrentThread();
}

int MyPlayer::Play_B() {
    int i;

    if (bisPlaying)
        return -100;


    //ffmpegVideo.player = this;
    bIsDecordOK = false;

    AVDictionary *optionsDict = nullptr;

    unsigned char *aviobuffer = nullptr;
    AVIOContext *avio = nullptr;
    pFormatCtx = nullptr;
    pVideoCodecCtx = nullptr;
    pAudioCodecCtx = nullptr;
    AVCodec *pCodec = nullptr;
    AVFrame *pFrame = nullptr;
    AVFrame *pFrameYUV = nullptr;
    AVPacket packet;

    struct SwsContext *img_convert_ctx;

    //int videoindex;
    //int audioindex;
    AVInputFormat *pInputFormat = nullptr;

    nReadInx = 0;

    av_log_set_level(AV_LOG_QUIET);

    pFormatCtx = avformat_alloc_context();

    if (avformat_open_input(&pFormatCtx, sPlayName, pInputFormat, nullptr) != 0) {
        bisPlaying = false;
        SentPlayStatus_Mid(-100);
        return -1;
    }

    if (avformat_find_stream_info(pFormatCtx, nullptr) < 0) {
        avformat_close_input(&pFormatCtx);
        avformat_free_context(pFormatCtx);
        pFormatCtx = nullptr;
        bisPlaying = false;
        SentPlayStatus_Mid(-100);
        return -2;
    }
    videoindex = -1;
    audioindex = -1;
    for (i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoindex = i;
        }
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioindex = i;
        }
    }

    //audioindex = -1;
    AVStream *stream;
    if (videoindex == -1) {
        LOGE("Didn't find a video stream.\n");
        avformat_close_input(&pFormatCtx);
        avformat_free_context(pFormatCtx);
        pFormatCtx = nullptr;
        bisPlaying = false;
        SentPlayStatus_Mid(-100);
        return -3;
    }
    stream = pFormatCtx->streams[videoindex];

    bool bH264 = false;
    hw_pix_fmt = AV_PIX_FMT_NONE;

    AVBufferRef *hw_device_ctx = nullptr;


#if 1
    {

        /* find the video stream information */
        int ret=0;
        pCodec = avcodec_find_decoder_by_name("h264_mediacodec");
        if(pCodec!= nullptr) {
            for (i = 0;; i++) {
                const AVCodecHWConfig *config = avcodec_get_hw_config(pCodec, i);
                if (!config) {
                    fprintf(stderr, "Decoder %s does not support device type .\n",
                            pCodec->name);

                    break;
                }
                if (config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX &&
                    config->device_type == AV_HWDEVICE_TYPE_MEDIACODEC) {
                    hw_pix_fmt = config->pix_fmt;
                    break;
                }
            }
            if (hw_pix_fmt != AV_PIX_FMT_NONE) {
                pVideoCodecCtx = avcodec_alloc_context3(pCodec);
                if (pVideoCodecCtx != nullptr) {
                    ret = avcodec_parameters_to_context(pVideoCodecCtx, stream->codecpar);
                    ret = avcodec_open2(pVideoCodecCtx, pCodec, &optionsDict);
                    if (ret >= 0) {
                        pVideoCodecCtx->get_format = get_hw_format;
                        ret = av_hwdevice_ctx_create(&hw_device_ctx,
                                                     AV_HWDEVICE_TYPE_MEDIACODEC, nullptr,
                                                     nullptr, 0);
                        if(ret == 0) {
                            pVideoCodecCtx->hw_device_ctx = av_buffer_ref(hw_device_ctx);
                            bH264 = true;
                        }
                    }
                }
            }
        }
    }
#endif



//    bH264 = false;
    if (!bH264)
    {
        pCodec = avcodec_find_decoder(stream->codecpar->codec_id);

        if (pCodec == nullptr) {
            LOGE("Codec not found.");
            avformat_close_input(&pFormatCtx);
            avformat_free_context(pFormatCtx);
            pFormatCtx = nullptr;
            bisPlaying = false;
            SentPlayStatus_Mid(-100);
            return -4;
        }
        pVideoCodecCtx = avcodec_alloc_context3(pCodec);
        if (pVideoCodecCtx == nullptr) {
            LOGE("Codec not found.");
            avformat_close_input(&pFormatCtx);
            avformat_free_context(pFormatCtx);
            pFormatCtx = nullptr;
            bisPlaying = false;
            SentPlayStatus_Mid(-100);
            return -4;
        }
        int re = avcodec_parameters_to_context(pVideoCodecCtx, stream->codecpar);
        if (re < 0) {
            avformat_close_input(&pFormatCtx);
            avformat_free_context(pFormatCtx);
            pFormatCtx = nullptr;
            avcodec_free_context(&pVideoCodecCtx);
            SentPlayStatus_Mid(-100);
            return -5;
        }
        if (avcodec_open2(pVideoCodecCtx, pCodec, &optionsDict) < 0) {
            avformat_close_input(&pFormatCtx);
            avformat_free_context(pFormatCtx);
            pFormatCtx = nullptr;
            avcodec_free_context(&pVideoCodecCtx);
            SentPlayStatus_Mid(-100);
            return -5;
        }
    }


    uint8_t *sps = nullptr;
    int spsLen = 0;
    uint8_t *pps = nullptr;
    int ppsLen = 0;
    uint8_t *data_;
    int dataLen_;


    uint8_t *extradata = stream->codecpar->extradata;
    int nSzie = stream->codecpar->extradata_size;
    uint8_t *SPS1 = nullptr;
    int SPS1_Len = 0;
    uint8_t *PPS1 = nullptr;
    int PPS1_Len = 0;
    if (nSzie > 11 && extradata != nullptr)
    {
        int nx = 4;
        int n = extradata[6] * 0x100 + extradata[7];
        SPS1_Len = n + nx;
        SPS1 = new uint8_t[SPS1_Len];
        memcpy(SPS1 + nx, extradata + 8, n);
        if (nx >= 4) {
            SPS1[0] = 0;//(uint8_t )((SPS1_Len & 0xFF000000)>>24);
            SPS1[1] = 0;//(uint8_t )((SPS1_Len & 0x00FF0000)>>16);
            SPS1[2] = 0;//(uint8_t )((SPS1_Len & 0x0000FF00)>>8);
            SPS1[3] = 1;//(uint8_t )((SPS1_Len & 0x000000FF));

        }
        for (int i = 0; i < n; i++) {
            LOGE("= %02X = %d", extradata[8 + i], i);
        }
        LOGE("  --- ");
        for (int i = 0; i < SPS1_Len; i++) {
            LOGE("= %02X -- %d", SPS1[i], i);
        }

        int offset = 6 + 2 + 1 + n;
        n = extradata[offset] * 0x100 + extradata[offset + 1];
        PPS1_Len = n + nx;
        PPS1 = new uint8_t[PPS1_Len];
        memcpy(PPS1 + nx, extradata + offset + 2, n);
        if (nx >= 4) {
            PPS1[0] = 0;//(uint8_t )((PPS1_Len & 0xFF000000)>>24);
            PPS1[1] = 0;//(uint8_t )((PPS1_Len & 0x00FF0000)>>16);
            PPS1[2] = 0;//(uint8_t )((PPS1_Len & 0x0000FF00)>>8);
            PPS1[3] = 1;//(uint8_t )((PPS1_Len & 0x000000FF));
        }

        for (int i = 0; i < n; i++) {
            LOGE("= %02X = %d", extradata[offset + i], i);
        }
        LOGE("  --- ");
        for (int i = 0; i < PPS1_Len; i++) {
            LOGE("= %02X -- %d", PPS1[i], i);
        }




    }
    // ffmpegVideo.F_InitMediaCodecDecord(pVideoCodecCtx->width,pVideoCodecCtx->height,SPS1,SPS1_Len,PPS1,PPS1_Len);




    float times = stream->duration * av_q2d(stream->time_base);
    int du_Count = (int) times * 10;
    int du1 = 0;


    if (pVideoCodecCtx->width <= 0 || pVideoCodecCtx->height <= 0) {
        avformat_close_input(&pFormatCtx);
        avformat_free_context(pFormatCtx);
        pFormatCtx = nullptr;
        bisPlaying = false;
        avcodec_free_context(&pVideoCodecCtx);
        SentPlayStatus_Mid(-100);
        return -6;
    }




    if (audioindex != -1) {
        stream = pFormatCtx->streams[audioindex];
        pCodec = avcodec_find_decoder(stream->codecpar->codec_id);

        times = stream->duration * av_q2d(stream->time_base);

        du1 = (int) times * 10;

        if (pCodec == nullptr) {
            avformat_close_input(&pFormatCtx);
            avformat_free_context(pFormatCtx);
            pFormatCtx = nullptr;
            bisPlaying = false;
            avcodec_free_context(&pVideoCodecCtx);
            SentPlayStatus_Mid(-100);
            return -4;
        }

        pAudioCodecCtx = avcodec_alloc_context3(pCodec);
        avcodec_parameters_to_context(pAudioCodecCtx, stream->codecpar);
        if (avcodec_open2(pAudioCodecCtx, pCodec, &optionsDict) < 0) {
            //DEBUG_PRINT("avcodec_open2 failed!\n");
            avformat_close_input(&pFormatCtx);
            avformat_free_context(pFormatCtx);
            pFormatCtx = nullptr;
            bisPlaying = false;
            avcodec_free_context(&pVideoCodecCtx);
            avcodec_free_context(&pAudioCodecCtx);
            SentPlayStatus_Mid(-100);
            return -5;
        }
    }


    pFrame = av_frame_alloc();
    pFrameYUV = av_frame_alloc();

    pFrameYUV->format = AV_PIX_FMT_YUV420P;
    pFrameYUV->width = pVideoCodecCtx->width;
    pFrameYUV->height = pVideoCodecCtx->height;
    av_image_alloc(
            pFrameYUV->data, pFrameYUV->linesize, pVideoCodecCtx->width,
            pVideoCodecCtx->height,
            AV_PIX_FMT_YUV420P, 4);

    img_convert_ctx = sws_getContext(pVideoCodecCtx->width, pVideoCodecCtx->height,
                                     pVideoCodecCtx->pix_fmt,
                                     pVideoCodecCtx->width, pVideoCodecCtx->height,
                                     AV_PIX_FMT_YUV420P,
                                     SWS_BICUBIC, nullptr, nullptr, nullptr);

    int frame_rate = 25;

    stream = pFormatCtx->streams[videoindex];
    if (stream->r_frame_rate.den > 0) {
        frame_rate = stream->r_frame_rate.num / stream->r_frame_rate.den;
    } else if (pVideoCodecCtx->framerate.den > 0) {
        frame_rate = pVideoCodecCtx->framerate.num / pVideoCodecCtx->framerate.den;
    }


    if (audioindex != -1) {
        ffmpegMusic.stop();
        ffmpegMusic.index = audioindex;
        ffmpegMusic.setAvCodecContext(pAudioCodecCtx);
        ffmpegMusic.time_base = pFormatCtx->streams[audioindex]->time_base;
        // ffmpegMusic.play();
    }
    if (videoindex != -1) {
        ffmpegVideo.stop();
        ffmpegVideo.index = videoindex;
        if (audioindex != -1) {
            ffmpegVideo.setFFmepegMusic(&ffmpegMusic);
        } else {
            ffmpegVideo.setFFmepegMusic(nullptr);
        }
        ffmpegVideo.setAvCodecContext(pVideoCodecCtx);
        ffmpegVideo.time_base = pFormatCtx->streams[videoindex]->time_base;
        ffmpegVideo.nFrameRate = frame_rate;

//        if(pVideoCodecCtx->codec_id == AV_CODEC_ID_H264) {
//            ffmpegVideo.F_InitMediaCoder(pVideoCodecCtx->width, pVideoCodecCtx->height,
//                                         (char *) SPS1, SPS1_Len, (char *) PPS1, PPS1_Len);
//        }
    }

    if (nSeekTime < 0) {
        SentPlayduration_Mid(du_Count);
    }

    int rea = -1;
    if (nSeekTime > 0) {
        float seconds = nSeekTime / 100.0f;
        if (videoindex != -1) {
            int64_t ts = (int64_t) (seconds / av_q2d(ffmpegVideo.time_base));
            rea = avformat_seek_file(pFormatCtx, (int) videoindex, ts, ts, ts, AVSEEK_FLAG_FRAME);
            avcodec_flush_buffers(pVideoCodecCtx);
        }
        if (audioindex != -1) {
            int64_t ts = (int64_t) (seconds / av_q2d(ffmpegMusic.time_base));
            rea = avformat_seek_file(pFormatCtx, (int) audioindex, ts, ts, ts, AVSEEK_FLAG_FRAME);
            avcodec_flush_buffers(pAudioCodecCtx);
        }
        nSeekTime = -1;
    }

    int nCountFrame = 0;
    bisPlaying = true;

    if (audioindex != -1) {
        ffmpegMusic.play();
    }

    if (videoindex != -1) {
        ffmpegVideo.play();
    }

    int nDelay = 1000 / frame_rate;

    int64_t pre = -1;   //ms
    int32_t  cc = 0;
    while (bisPlaying) {
        int ret = 0;

        int frameCount = 0;


        if (ffmpegVideo.isPause) {
            usleep(1000 * 5);
            continue;
        }
        if (ffmpegVideo.getSize() > 600) {
            usleep(1000);
            continue;
        }

        ret = av_read_frame(pFormatCtx, &packet);
        if (ret != 0) {
            break;
        } else {
            if (packet.stream_index == videoindex) {
                if (!bIsDecordOK) {
                    bIsDecordOK = true;
                    SentPlayStatus_Mid(1);
                }
                nCountFrame = ffmpegVideo.put(&packet);
                LOGE("GET VIDEO Packet! %d",++cc);

            }
            if (packet.stream_index == audioindex) {
                if (!ffmpegMusic.bStartPlay) {
                    ffmpegMusic.bStartPlay = true;
                    ffmpegMusic.audio_start_time = av_gettime() / 1000000.0;
                }
                ffmpegMusic.put(&packet);
            }
            av_packet_unref(&packet);
        }


    }

    if (audioindex != -1) {
        while (bisPlaying && ffmpegMusic.getSize() > 0) //等待播放完毕，如果不是强行退出的话
        {
            usleep(1000 * 2);
        }
    } else {
        while (bisPlaying && ffmpegVideo.getSize() > 0) //等待播放完毕，如果不是强行退出的话
        {
            usleep(1000 * 2);
        }
    }
    ffmpegMusic.stop();
    ffmpegVideo.stop();

    if (nSeekTime < 0) {
        SentPlayStatus_Mid(0);
    }

    bisPlaying = false;
    sws_freeContext(img_convert_ctx);
    if (pVideoCodecCtx != nullptr) {
        if (pVideoCodecCtx->hw_device_ctx != nullptr) {
            av_buffer_unref(&pVideoCodecCtx->hw_device_ctx);
        }

        if (avcodec_is_open(pVideoCodecCtx))
            avcodec_close(pVideoCodecCtx);
        avcodec_free_context(&pVideoCodecCtx);
        pVideoCodecCtx = nullptr;

    }

    if (pAudioCodecCtx != nullptr) {
        if (avcodec_is_open(pAudioCodecCtx))
            avcodec_close(pAudioCodecCtx);
        avcodec_free_context(&pAudioCodecCtx);
        pAudioCodecCtx = nullptr;
    }


    if (SPS1) {
        delete[]SPS1;
    }
    if (PPS1) {
        delete[]PPS1;
    }


    if(hw_device_ctx!= nullptr) {
        av_buffer_unref(&hw_device_ctx);
        hw_device_ctx = nullptr;
    }

    avformat_close_input(&pFormatCtx);
    av_freep(&pFrameYUV->data[0]);
    av_frame_free(&pFrameYUV);
    return 0;
}

int MyPlayer::Play(char *bPath) {
    if (bPath == nullptr)
        return -1;
    int nLen = strlen(bPath);
    if (nLen >= 1023)
        return -1;
    memset(sPlayName, 0, sizeof(sPlayName));
    memcpy(sPlayName, bPath, strlen(bPath));

    if (connectdid != -1) {
        bisPlaying = false;
        pthread_join(connectdid, 0);
        connectdid = -1;
    }
    bIsDecordOK = false;
    nSeekTime = -1;
    pthread_create(&connectdid, nullptr, Progress, (void *) this);
    return 0;

}


void MyPlayer::Play() {
    if (connectdid != -1) {
        bisPlaying = false;
        pthread_join(connectdid, 0);
        connectdid = -1;
    }
    bIsDecordOK = false;
    pthread_create(&connectdid, nullptr, Progress, (void *) this);

}

void MyPlayer::Recheck(AVFrame *pFrameYUV, AVFrame *pFrame) {
    if (pFrameYUV != nullptr) {
        if (pFrameYUV->width != pFrame->width || pFrameYUV->height != pFrame->height) {
            av_freep(&pFrameYUV->data[0]);
            av_frame_free(&pFrameYUV);
            pFrameYUV = nullptr;
        }
    }
    if (pFrameYUV == nullptr) {
        pFrameYUV = av_frame_alloc();
        pFrameYUV->format = AV_PIX_FMT_YUV420P;
        pFrameYUV->width = pFrame->width;
        pFrameYUV->height = pFrame->height;
        av_image_alloc(
                pFrameYUV->data, pFrameYUV->linesize, pFrame->width,
                pFrame->height,
                AV_PIX_FMT_YUV420P, aligD);
    }
}



int MyPlayer::Pause() {
    ffmpegVideo.pause();
    ffmpegMusic.pause();
    {
        if (ffmpegVideo.isPause) {
            SentPlayStatus_Mid(2);
        } else {
            SentPlayStatus_Mid(1);
        }
    }
    if (ffmpegVideo.isPause)
        usleep(1000 * 300);
    return 0;
}

int MyPlayer::Stop() {
    if (!bisPlaying)
        return 0;
    if (ffmpegVideo.isPause) {
        Pause();
        usleep(1000 * 20);
    }
    bisPlaying = false;

    if (connectdid != -1) {
        pthread_join(connectdid, 0);
        connectdid = -1;
    }

    ffmpegMusic.F_ClearQueue();
    ffmpegVideo.F_ClearQueue();


    return 0;
}

int MyPlayer::ReleaseObject(void) {
    if (object) {
        int needsDetach = 0;
        JNIEnv *env = getJNIEnv(&needsDetach);
        if (env == nullptr) {
            object = nullptr;
            return 0;
        }
        if (object) {
            env->DeleteGlobalRef(object);
            object = nullptr;
        }
        if (needsDetach)
            gJavaVM->DetachCurrentThread();
    }
    memset(sPlayName, 0, sizeof(sPlayName));
    return 0;
}


void MyPlayer::Seek(float seconds) {
    int nSec = (int) (seconds * 100);
    nSeekTime = nSec;
    Stop();
    nSeekTime = nSec;
    Play();


}

void *MyPlayer::Progress(void *data) {
    MyPlayer *myPlayer = (MyPlayer *) data;
    myPlayer->Play_B();
    return nullptr;
}
