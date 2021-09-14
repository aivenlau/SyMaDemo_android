//
// Created by Administrator on 2017/11/20.
//



#include <libyuv.h>
#include "FFmpegVideo.h"
#import  "MyPlayer.h"
#include "FFMpegPlayerCore.h"

/* no AV sync correction is done if below the minimum AV sync threshold */
#define AV_SYNC_THRESHOLD_MIN 0.01
/* AV sync correction is done if above the maximum AV sync threshold */
#define AV_SYNC_THRESHOLD_MAX 0.1
/* If a frame duration is longer than this, it will not be duplicated to compensate AV sync */
#define AV_SYNC_FRAMEDUP_THRESHOLD 0.1
/* no AV correction is done if too big error */
#define AV_NOSYNC_THRESHOLD 10.0


extern AVFrame *gl_Frame;

static void (*video_call)(AVFrame *frame);


void OnPlayIsStarting_Callback(int n);

extern C_FFMpegPlayer m_FFMpegPlayer;

extern MyPlayer *myPlayer;

void F_SentRevBmp(int32_t i);

void *videoPlay(void *args) {
    auto *ffmpegVideo = (FFmpegVideo *) args;

    //申请AVFrame
    auto *frame = av_frame_alloc();//分配一个AVFrame结构体,AVFrame结构体一般用于存储原始数据，指向解码后的原始帧
    auto *frame_hw = av_frame_alloc();

    auto *packet = (AVPacket *) av_mallocz(sizeof(AVPacket));

    //输出文件
    //FILE *fp = fopen(outputPath,"wb");


    AVFrame *yuv_frame = av_frame_alloc();//分配一个AVFrame结构体，指向存放转换成rgb后的帧


    yuv_frame->format = AV_PIX_FMT_YUV420P;
    yuv_frame->width = ffmpegVideo->codec->width;
    yuv_frame->height = ffmpegVideo->codec->height;
    av_image_alloc(
            yuv_frame->data, yuv_frame->linesize, yuv_frame->width,
            yuv_frame->height,
            AV_PIX_FMT_YUV420P, aligD);

//    frame->format = AV_PIX_FMT_YUV420P;
//    frame->width = ffmpegVideo->codec->width;
//    frame->height = ffmpegVideo->codec->height;
//    av_image_alloc(
//            frame->data, frame->linesize, frame->width,
//            frame->height,
//            AV_PIX_FMT_YUV420P, aligD);


    //LOGE("转换成YUV格式");
    ffmpegVideo->swsContext = sws_getContext(ffmpegVideo->codec->width, ffmpegVideo->codec->height,
                                             ffmpegVideo->codec->pix_fmt,
                                             ffmpegVideo->codec->width, ffmpegVideo->codec->height,
                                             AV_PIX_FMT_YUV420P,
                                             SWS_BICUBIC, nullptr, nullptr, nullptr);


    double last_play  //上一帧的播放时间
    , play             //当前帧的播放时间
    , last_delay    // 上一次播放视频的两帧视频间隔时间
    , delay         //两帧视频间隔时间
    , audio_clock //音频轨道 实际播放时间
    //,video_clock
    , diff   //音频帧与视频帧相差时间
    , sync_threshold
    , start_time  //从第一帧开始的绝对时间
    , pts
    , actual_delay//真正需要延迟时间
    ;

    //从第一帧开始的绝对时间
    last_delay = 0;
    start_time = av_gettime() / 1000000.0;

    int frameCount;
    last_play = 0;


    bool bFirst = true;

    int nStartPlay = 0;

//    ffmpegVideo->time_base.den = 20;


    int ret = -1;
    int64_t nPre = -1;
    int64_t nCurrent = -1;
    int nDelay = 1000 / ffmpegVideo->nFrameRate;
    int nFirst = 0;
    bool bD = false;
    while (ffmpegVideo->isPlay) {
        if (ffmpegVideo->ffmpegMusic == nullptr) {

            bD = false;

            if (nPre > 0) {
                nCurrent = av_gettime() / 1000;
                if (nCurrent - nPre < nDelay) {
                    continue;
                } else {
                    nPre = nCurrent;
                }
            } else {
                nPre = av_gettime() / 1000;
            }

            if (ffmpegVideo->get(packet) == 0) {
                usleep(1000 * 5);
                continue;
            }
            frameCount = 0;

            do {
                ret = avcodec_receive_frame(ffmpegVideo->codec, frame);
                if (ret >= 0) {
                    if (nStartPlay == 0) {
                        nStartPlay = 1;
                        OnPlayIsStarting_Callback(nStartPlay);
                    }

                    if (myPlayer != nullptr) {
                        myPlayer->SentPlayTime_mid(
                                (int) (pts * av_q2d(ffmpegVideo->time_base) * 10));
                    }
                    if (frame->format == AV_PIX_FMT_YUVJ420P ||
                        frame->format == AV_PIX_FMT_YUV420P) {
                        bD = true;
                        libyuv::I420ToABGR(frame->data[0], frame->linesize[0],
                                           frame->data[1], frame->linesize[1],
                                           frame->data[2], frame->linesize[2],
                                           m_FFMpegPlayer.Rgbabuffer, yuv_frame->width * 4,
                                           yuv_frame->width, yuv_frame->height);
                    } else if (frame->format == AV_PIX_FMT_YUV422P ||
                               frame->format == AV_PIX_FMT_YUVJ422P) {
                        bD = true;
                        libyuv::I422ToABGR(frame->data[0], frame->linesize[0],
                                           frame->data[1], frame->linesize[1],
                                           frame->data[2], frame->linesize[2],
                                           m_FFMpegPlayer.Rgbabuffer, yuv_frame->width * 4,
                                           yuv_frame->width, yuv_frame->height);
                    } else if (frame->format == AV_PIX_FMT_NV12) {
                        libyuv::NV21ToARGB(frame->data[0], frame->linesize[0],
                                           frame->data[1], frame->linesize[1],
                                           m_FFMpegPlayer.Rgbabuffer, yuv_frame->width * 4,
                                           yuv_frame->width, yuv_frame->height);
                        bD = true;
                    } else {
                        sws_scale(ffmpegVideo->swsContext,
                                  (const uint8_t *const *) frame->data,
                                  frame->linesize, 0,
                                  frame->height,
                                  yuv_frame->data, yuv_frame->linesize);
                    }

                    if (!bD) {
                        libyuv::I420ToABGR(yuv_frame->data[0], yuv_frame->linesize[0],
                                           yuv_frame->data[1], yuv_frame->linesize[1],
                                           yuv_frame->data[2], yuv_frame->linesize[2],
                                           m_FFMpegPlayer.Rgbabuffer, yuv_frame->width * 4,
                                           yuv_frame->width, yuv_frame->height);
                    }

                    int W1 = yuv_frame->width;
                    int H1 = yuv_frame->height;
                    F_SentRevBmp(W1 + H1 * 0x10000);

                }
            } while (ret != AVERROR(EAGAIN));
            usleep(800);
            ret = avcodec_send_packet(ffmpegVideo->codec, packet);
            usleep(500);

        } else {
            if (!ffmpegVideo->ffmpegMusic->bStartPlay) {
                usleep(1000 * 5);
            } else {
                if (bFirst) {
                    bFirst = false;
                    start_time = ffmpegVideo->ffmpegMusic->audio_start_time;
                }
                if (ffmpegVideo->get(packet) == 0) {
                    usleep(1000 * 5);
                    continue;
                }
                frameCount = 0;


                do {
                    ret = avcodec_receive_frame(ffmpegVideo->codec, frame);
                    while (ret == 0)
                    {
                        frameCount = 1;
                        bool bD = false;
                        if (frame->format == AV_PIX_FMT_YUVJ420P ||
                            frame->format == AV_PIX_FMT_YUV420P) {
                            libyuv::I420ToABGR(frame->data[0], frame->linesize[0],
                                               frame->data[1], frame->linesize[1],
                                               frame->data[2], frame->linesize[2],
                                               m_FFMpegPlayer.Rgbabuffer, yuv_frame->width * 4,
                                               yuv_frame->width, yuv_frame->height);
                            bD = true;

                        } else if (frame->format == AV_PIX_FMT_YUV422P ||
                                   frame->format == AV_PIX_FMT_YUVJ422P) {
                            libyuv::I422ToABGR(frame->data[0], frame->linesize[0],
                                               frame->data[1], frame->linesize[1],
                                               frame->data[2], frame->linesize[2],
                                               m_FFMpegPlayer.Rgbabuffer, yuv_frame->width * 4,
                                               yuv_frame->width, yuv_frame->height);
                            bD = true;

                        } else if (frame->format == AV_PIX_FMT_NV12) {
                            libyuv::NV21ToARGB(frame->data[0], frame->linesize[0],
                                               frame->data[1], frame->linesize[1],
                                               m_FFMpegPlayer.Rgbabuffer, yuv_frame->width * 4,
                                               yuv_frame->width, yuv_frame->height);
                            bD = true;
                        } else {
                            sws_scale(ffmpegVideo->swsContext,
                                      (const uint8_t *const *) frame->data,
                                      frame->linesize, 0,
                                      frame->height,
                                      yuv_frame->data, yuv_frame->linesize);
                        }

                        if ((pts = frame->pts) == AV_NOPTS_VALUE) {
                            pts = 0;
                        }

                        play = pts * av_q2d(ffmpegVideo->time_base);
                        play = ffmpegVideo->synchronize(frame, play);

                        delay = play - last_play;
                        if (delay <= 0 || delay > 1) {
                            delay = last_delay;
                        }
                        audio_clock = ffmpegVideo->ffmpegMusic->clock;
                        last_delay = delay;
                        last_play = play;
                        //音频与视频的时间差
                        diff = ffmpegVideo->clock - audio_clock;
                        //在合理范围外  才会延迟  加快
                        sync_threshold = (delay > 0.06 ? 0.06 : delay);
                        if (fabs(diff) < 10) {
                            if (diff <= -sync_threshold) {
                                delay = 0;
                            } else if (diff >= sync_threshold) {
                                delay = 2 * delay;
                            }
                        }
                        start_time += delay;
                        actual_delay = start_time - av_gettime() / (double) 1000000.0;
                        double dat = actual_delay;
                        if (actual_delay < 0.005) {
                            actual_delay = 0.005;
                        }
                        if (actual_delay > 1.5)
                            actual_delay = 1.5;
                        uint32_t dd = (actual_delay * 1000000) + 6000;
                        if (((int) (actual_delay * 100)) == 0) {
                            dd = 1;
                        }
                        usleep((uint32_t) dd);

                        if (nStartPlay == 0) {
                            nStartPlay = 1;
                            OnPlayIsStarting_Callback(nStartPlay);
                        }

                        if (myPlayer != nullptr) {
                            myPlayer->SentPlayTime_mid(
                                    (int) (pts * av_q2d(ffmpegVideo->time_base) * 10));
                        }


                        if (m_FFMpegPlayer.Rgbabuffer != nullptr) {
                            if (!bD) {
                                libyuv::I420ToABGR(yuv_frame->data[0], yuv_frame->linesize[0],
                                                   yuv_frame->data[1], yuv_frame->linesize[1],
                                                   yuv_frame->data[2], yuv_frame->linesize[2],
                                                   m_FFMpegPlayer.Rgbabuffer,
                                                   yuv_frame->width * 4,
                                                   yuv_frame->width, yuv_frame->height);
                            }
                        }

                        int W1 = yuv_frame->width;
                        int H1 = yuv_frame->height;
                        F_SentRevBmp(W1 + H1 * 0x10000);
                        ret = avcodec_receive_frame(ffmpegVideo->codec, frame);
                    }
                } while (ret != AVERROR(EAGAIN));
                usleep(800);
                ret = avcodec_send_packet(ffmpegVideo->codec, packet);
                usleep(500);


            }
        }
    }
    av_free(packet);
    av_frame_free(&frame);
    av_frame_free(&yuv_frame);
    sws_freeContext(ffmpegVideo->swsContext);
    size_t size = ffmpegVideo->queue.size();
    for (int i = 0; i < size; ++i) {
        AVPacket *pkt = ffmpegVideo->queue.front();
        av_free(pkt);
        ffmpegVideo->queue.erase(ffmpegVideo->queue.begin());
    }
    LOGE("VIDEO EXIT");
    if (nStartPlay != 0) {
        nStartPlay = 0;
        OnPlayIsStarting_Callback(nStartPlay);
    }
    pthread_exit(0);

}

void FFmpegVideo::F_ClearQueue() {

    pthread_mutex_lock(&mutex);
    for (int i = 0; i < queue.size(); ++i) {
        AVPacket *pkt = queue[i];
        av_free(pkt);
    }
    queue.clear();
    pthread_mutex_unlock(&mutex);


}


FFmpegVideo::FFmpegVideo() : playId(-1), isPlay(false), nFrameRate(25),
                             isPause(false)  //,YUVBuffer(nullptr),bMediaCodeStart(false)
{
    pthread_mutex_init(&mutex, nullptr);
    pthread_cond_init(&cond, nullptr);
    const char *mine = "video/avc";
    //mMediaCodec = AMediaCodec_createDecoderByType(mine);
    pts = 0;
//    const char* mine = "video/avc";
//    mMediaCodec =  AMediaCodec_createDecoderByType(mine);

}

#if 0
//int64_t g_pts = 0.f;
//double g_play = 0.0f;
//double g_delay;
//double g_last_play = 0.0f;
//double g_audio_clock = 0.0f;
//double g_last_delay = 0.0f;
//double g_start_time = 0.0f;
//int g_nStartPlay = 0;

bool FFmpegVideo::F_InitMediaCoder(int width, int height, char *sps, int spsSize, char *pps,
                                   int ppsSize) {
    pts = 0;
    inx = 0;
    g_pts = 0;
    g_play = 0;
    g_delay = 0;
    g_last_play = 0;
    g_audio_clock = 0;
    g_last_delay = 0.0f;
    g_start_time = 0.0f;
    g_nStartPlay = 1;
    const char *mine = "video/avc";
    if (mMediaCodec == nullptr) {
        mMediaCodec = AMediaCodec_createDecoderByType(mine);
    } else
    {
        AMediaCodec_stop(mMediaCodec);
        AMediaCodec_delete(mMediaCodec);
        mMediaCodec= AMediaCodec_createDecoderByType(mine);
    }

    media_status_t status = AMEDIA_OK;
    mWidth = width;
    mHeight = height;
    AMediaFormat *videoFormat = AMediaFormat_new();
    AMediaFormat_setString(videoFormat, "mime", mine);
    AMediaFormat_setInt32(videoFormat, AMEDIAFORMAT_KEY_WIDTH, width); // 视频宽度
    AMediaFormat_setInt32(videoFormat, AMEDIAFORMAT_KEY_HEIGHT, height); // 视频高度
    AMediaFormat_setBuffer(videoFormat, "csd-0", sps, spsSize); // sps
    AMediaFormat_setBuffer(videoFormat, "csd-1", pps, ppsSize); // pps

    status = AMediaCodec_configure(mMediaCodec, videoFormat, nullptr, nullptr, 0);
    if (status == AMEDIA_OK) {
        status = AMediaCodec_start(mMediaCodec);
    }
    AMediaFormat_delete(videoFormat);
    g_last_play = 0.0f;
    return (status == AMEDIA_OK);
}


bool FFmpegVideo::F_DecorderData(char *bufData, int bufSize, AVFrame *frame) {

    size_t outsize;
    if (frame == nullptr) {
        return false;
    }


    bool bGetOK = false;

    ssize_t bufidx = AMediaCodec_dequeueInputBuffer(mMediaCodec, 2000);
    if (bufidx >= 0) {
        // 获取buffer的索引
        uint8_t *inputBuf = AMediaCodec_getInputBuffer(mMediaCodec, bufidx, &outsize);
        if (inputBuf != nullptr && bufSize <= outsize) {
            //将待解码的数据copy到硬件中
            memcpy(inputBuf, bufData, bufSize);
            media_status_t status = AMediaCodec_queueInputBuffer(mMediaCodec, bufidx, 0, bufSize,
                                                                 inx, 0);
            inx++;

            AMediaCodecBufferInfo info;
            ssize_t outbufidx = AMediaCodec_dequeueOutputBuffer(mMediaCodec, &info, 2000);

            while (true) {
                if (outbufidx >= 0) {
                    size_t outsize;
                    uint8_t *outputBuf = AMediaCodec_getOutputBuffer(mMediaCodec, outbufidx,
                                                                     &outsize);
                    AMediaFormat *bufferFormat = AMediaCodec_getOutputFormat(mMediaCodec);


                    int W;
                    int H;
                    AMediaFormat_getInt32(bufferFormat, "color-format", &mColorFormat);
                    AMediaFormat_getInt32(bufferFormat, "width", &W);
                    AMediaFormat_getInt32(bufferFormat, "height", &H);


                    if (outputBuf != nullptr) {
                        //pts = info.presentationTimeUs;
                        //if(frame->width >=mWidth && frame->height >=mHeight)


                        bGetOK = true;
                        frame->pts = pts;
                        pts++;
                        if (mColorFormat == 21) {
                            libyuv::NV21ToARGB(outputBuf, W,
                                               outputBuf + (W * H),
                                               W,
                                               m_FFMpegPlayer.Rgbabuffer, mWidth * 4,
                                               mWidth, mHeight);
                        }
                        if (mColorFormat == 19) {

                            libyuv::I420ToABGR(outputBuf, W,
                                               outputBuf + W * H, W / 2,
                                               outputBuf + W * H +
                                               W / 2 * H / 2, W / 2,
                                               m_FFMpegPlayer.Rgbabuffer, mWidth * 4,
                                               mWidth, mHeight);
                        }


                        frame->repeat_pict = 0;
                        if (nFrameRate <= 0)
                            nFrameRate = 20;

                        frame->pts *= time_base.den / nFrameRate;
                        if ((g_pts = frame->pts) == AV_NOPTS_VALUE) {
                            g_pts = 0;
                        }

#if 1
                        g_play = g_pts * av_q2d(time_base);
                        g_audio_clock = ffmpegMusic->clock;
#else
                        g_play = g_pts * av_q2d(time_base);
                        g_play = synchronize(frame, g_play);
                        g_delay = g_play - g_last_play;
                        if (g_delay <= 0 || g_delay > 1) {
                            g_delay = g_last_play;
                        }
                        g_audio_clock = ffmpegMusic->clock;
                        g_last_delay = g_delay;
                        g_last_play = g_play;
//                        //音频与视频的时间差
                        double  diff = clock - g_audio_clock;
//                        //在合理范围外  才会延迟  加快
                        double sync_threshold = (g_delay > 0.06 ? 0.06 : g_delay);
                        if (fabs(diff) < 10) {
                            if (diff <= -sync_threshold) {
                                g_delay = 0;
                            } else if (diff >= sync_threshold) {
                                g_delay = 2 * g_delay;
                            }
                        }
                        g_start_time += g_delay;
                        double actual_delay = g_start_time - av_gettime() / (double) 1000000.0;
                        double dat = actual_delay;

                        if (actual_delay < 0.01) {
                            actual_delay = 0.01;
                        }
                        if (actual_delay > 1.5)
                            actual_delay = 1.5;
                        uint32_t  dd = (actual_delay * 1000000) + 8000;
//                        if(dat<0)
//                        {
//                            dd = 0;
//                        }
                        if(dd>0)
                            usleep((uint32_t) dd);
                        LOGE("delay %d",dd);
#endif

                        if (g_nStartPlay == 0) {
                            g_nStartPlay = 1;
                            OnPlayIsStarting_Callback(g_nStartPlay);
                        }


                        if (myPlayer != nullptr) {
                            myPlayer->SentPlayTime_mid((int) (g_pts * av_q2d(time_base) * 10));
                        }


                        int W1 = mWidth;
                        int H1 = mHeight;
                        F_SentRevBmp(W1 + H1 * 0x10000);


                        AMediaCodec_releaseOutputBuffer(mMediaCodec, outbufidx, false);
                        outbufidx = AMediaCodec_dequeueOutputBuffer(mMediaCodec, &info, 2000);
                        //return true;
                    }
                } else if (outbufidx == AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED) {
                    //public static final int COLOR_FormatYUV420PackedPlanar      = 20;    YUV420P
                    //public static final int COLOR_FormatYUV420SemiPlanar        = 21;     //NV12
//                    auto format = AMediaCodec_getOutputFormat(mMediaCodec);
//                    AMediaFormat_getInt32(format, "width", &mWidth);
//                    AMediaFormat_getInt32(format, "height", &mHeight);
//                    int32_t localColorFMT;
//                    AMediaFormat_getInt32(format, AMEDIAFORMAT_KEY_COLOR_FORMAT,
//                                          &localColorFMT);
//                    mColorFormat = localColorFMT;
                    outbufidx = AMediaCodec_dequeueOutputBuffer(mMediaCodec, &info, 2000);
                    LOGE("FORMAT CHANGED");

                } else if (outbufidx == AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED) {
                    outbufidx = AMediaCodec_dequeueOutputBuffer(mMediaCodec, &info, 2000);
                    LOGE("FORMAT CHANGED");
                } else {
                    break;
                }
            }
        }
    }
    return bGetOK;
}

#endif

FFmpegVideo::~FFmpegVideo() {
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
}

void FFmpegVideo::setAvCodecContext(AVCodecContext *avCodecContext) {
    codec = avCodecContext;
}

int FFmpegVideo::put(AVPacket *avPacket) {
    //LOGE("插入队列 video");
    AVPacket *avPacket1 = av_packet_alloc();//(AVPacket *) av_mallocz(sizeof(AVPacket));
    //克隆
    if (av_packet_ref(avPacket1, avPacket)) {
        return 0;
    }
    //push的时候需要锁住，有数据的时候再解锁
    int nCount = 0;
    pthread_mutex_lock(&mutex);
    queue.push_back(avPacket1);//将packet压入队列
    //压入过后发出消息并且解锁
    nCount = queue.size();
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    return nCount;
}


int FFmpegVideo::getSize() {
    int nsize = 0;
    pthread_mutex_lock(&mutex);
    nsize = (int) queue.size();
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    return nsize;

}


int FFmpegVideo::get(AVPacket *avPacket) {
    pthread_mutex_lock(&mutex);
    int ret = 0;
    if (!queue.empty() && !isPause) {
        ret = 1;
        av_packet_ref(avPacket, queue.front());
        AVPacket *packet2 = queue.front();
        queue.erase(queue.begin());
        av_free(packet2);
    }

//    int ret = 0;
//    while (isPlay) {
//        if (!queue.empty() && !isPause)
//        {
//            //如果队列中有数据可以拿出来
//            if (av_packet_ref(avPacket, queue.front())!=0) {
//                ret = 1;
//            }
//            //取成功了，弹出队列，销毁packet
//            AVPacket *packet2 = queue.front();
//            queue.erase(queue.begin());
//            av_free(packet2);
//            break;
//        }
////        else {
////            pthread_cond_wait(&cond, &mutex);
////        }
//    }
    pthread_mutex_unlock(&mutex);
    return ret;
}

void FFmpegVideo::play() {
//    if (isPause == 0) {
//        ;
//    }
    clock = 0;
    isPlay = false;
    isPause = false;
    if (playId != -1) {
        pthread_join(playId, 0);
        playId = -1;
    }
    isPlay = true;
    pthread_create(&playId, nullptr, videoPlay, this);//开启begin线程

}

void FFmpegVideo::stop() {
    LOGE("VIDEO stop");

    pthread_mutex_lock(&mutex);
    isPlay = false;
    //因为可能卡在 deQueue
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    if (playId != -1) {
        pthread_join(playId, 0);
        playId = -1;
    }

    for (int i = 0; i < queue.size(); ++i) {
        AVPacket *pkt = queue.front();
        queue.erase(queue.begin());
        LOGE("销毁音频帧%d", (int) queue.size());
        av_free(pkt);
    }

//    if (mMediaCodec != nullptr) {
//        media_status_t status = AMediaCodec_stop(mMediaCodec);
//        if (status == AMEDIA_OK) {
//            AMediaCodec_delete(mMediaCodec);
//            mMediaCodec = nullptr;
//        }
//    }

    LOGE("VIDEO close");

}

double FFmpegVideo::synchronize(AVFrame *frame, double play) {
    //clock是当前播放的时间位置
    if (play != 0)
        clock = play;
    else //pst为0 则先把pts设为上一帧时间
        play = clock;
    //可能有pts为0 则主动增加clock
    //frame->repeat_pict = 当解码时，这张图片需要要延迟多少
    //需要求出扩展延时：
    //extra_delay = repeat_pict / (2*fps) 显示这样图片需要延迟这么久来显示
    double repeat_pict = frame->repeat_pict;
    //使用AvCodecContext的而不是stream的
    double frame_delay = av_q2d(codec->time_base);
    //如果time_base是1,25 把1s分成25份，则fps为25
    //fps = 1/(1/25)
    double fps = 1 / frame_delay;
    //pts 加上 这个延迟 是显示时间
    double extra_delay = repeat_pict / (2 * fps);
    double delay = extra_delay + frame_delay;
    clock += delay;
    return play;
}

void FFmpegVideo::setFFmepegMusic(FFmpegMusic *ffmpegMusic) {
    this->ffmpegMusic = ffmpegMusic;

}

void FFmpegVideo::setPlayCall(void (*call)(AVFrame *)) {
    video_call = call;
}

void FFmpegVideo::pause() {
    LOGE("执行了pause");
    isPause = !isPause;
    pthread_cond_signal(&cond);
//    if (isPause == 1) {
//        isPause = 0;
//    } else {
//        isPause = 1;
//        pthread_cond_signal(&cond);
//    }
}

//MediaCode NDK
void dd() {
    AMediaCodec *mMediaCodec;

}

/*
bool  FFmpegVideo::F_InitMediaCodecDecord(int width,int height,uint8_t *sps,int spsSize, uint8_t* pps,int ppsSize)
{
    if(mMediaCodec==nullptr)
        return false;
    if(bMediaCodeStart)
        AMediaCodec_stop(mMediaCodec);
    bMediaCodeStart = false;
    mColorFormat = -1;

     uint8_t *da = new uint8_t[spsSize+ppsSize];
     memcpy(da,sps,spsSize);
     memcpy(da+spsSize,pps,ppsSize);


    char *b = (char *)(AMEDIAFORMAT_KEY_WIDTH);
    AMediaFormat* videoFormat = AMediaFormat_new();
    AMediaFormat_setString(videoFormat, AMEDIAFORMAT_KEY_MIME, "video/avc");
    AMediaFormat_setInt32(videoFormat, AMEDIAFORMAT_KEY_WIDTH, width); // 视频宽度
    AMediaFormat_setInt32(videoFormat, AMEDIAFORMAT_KEY_HEIGHT, height); // 视频高度
    AMediaFormat_setBuffer(videoFormat, "csd-0", sps, spsSize); // sps
    AMediaFormat_setBuffer(videoFormat, "csd-1", pps, ppsSize); // pps
    AMediaFormat_setInt32(videoFormat,AMEDIAFORMAT_KEY_COLOR_FORMAT,2135033992);
    media_status_t status = AMediaCodec_configure(mMediaCodec,videoFormat, nullptr, nullptr,0);
    delete []da;
    if(status!=AMEDIA_OK)
    {
        return false;
    }
    status = AMediaCodec_start(mMediaCodec);
    if(status == AMEDIA_OK)
    {
        if(YUVBuffer!= nullptr)
        {
            delete []YUVBuffer;


        }
        YUVBuffer_Len = 1920*1080*3;
        YUVBuffer = new uint8_t[YUVBuffer_Len];
        bMediaCodeStart = true;
    }
    pts = 0;
    return (status == AMEDIA_OK);


}

int   FFmpegVideo::F_DecordPacket(AVPacket *packet)
{
      if(packet== nullptr)
          return 0;
      if(packet->size==0)
          return 0;

      size_t outSize = 0;
      int bufSize = packet->size;
      uint8_t  *bufData = packet->data;
      int   len = bufData[3]+bufData[2]*0x100+bufData[1]*0x10000+bufData[0]*0x1000000;
      bufData[0]=0;
      bufData[1]=0;
      bufData[2]=0;
      bufData[3]=1;
      uint8_t  da = bufData[4]&0x1F;

    ssize_t bufidx = AMediaCodec_dequeueInputBuffer(mMediaCodec, 2000);
    if (bufidx >= 0) {
        // 获取buffer的索引
        uint8_t *inputBuf = AMediaCodec_getInputBuffer(mMediaCodec, bufidx, &outSize);
        if (inputBuf != nullptr && bufSize <= outSize) {
            int ss = (int)(outSize<=bufSize?outSize:bufSize);
            memcpy(inputBuf, bufData, ss);
            int64_t pts1 = packet->pts;
            pts_ = pts*(1000000/25);
            media_status_t status = AMediaCodec_queueInputBuffer(mMediaCodec, bufidx, 0, ss,
                                                                 pts_, 0);
            pts++;
            if(status != AMEDIA_OK)
            {
                return  0;
            }

            AMediaCodecBufferInfo info;
            ssize_t outbufidx = AMediaCodec_dequeueOutputBuffer(mMediaCodec, &info, 1000*100);
            if (outbufidx >=0){
                size_t outsize;
                uint8_t* outputBuf = AMediaCodec_getOutputBuffer(mMediaCodec, outbufidx, &outsize);
                if (outputBuf != nullptr){
                    int aa = outsize<=YUVBuffer_Len?outsize:YUVBuffer_Len;
                    memcpy(YUVBuffer,outputBuf,aa);
                    AMediaCodec_releaseOutputBuffer(mMediaCodec, outbufidx, info.size != 0);
                    return 1;
                }
            }
            else {
                switch (outbufidx) {
                    case AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED:
                    {
                        int nW,nH;
                        auto format = AMediaCodec_getOutputFormat(mMediaCodec);
                        AMediaFormat_getInt32(format, "width", &nW);
                        AMediaFormat_getInt32(format, "height", &nH);
                        AMediaFormat_getInt32(format, AMEDIAFORMAT_KEY_COLOR_FORMAT, &mColorFormat);
                       // AMediaFormat *mColorFormat = getTTFormatFromMC(localColorFMT);
                        int32_t stride = 0;
                        AMediaFormat_getInt32(format, "stride", &stride);
                        if(stride == 0) {
                            stride = nW;
                        }
                        return 0;
                    }
                    case AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED:
                        break;
                    default:
                        break;
                }
                return 0;
            }


        }
    }
    else
    {
        return 0;
    }
    return  0;

}
 */