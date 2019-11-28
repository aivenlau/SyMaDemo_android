//
// Created by Administrator on 2017/11/20.
//



#include <libyuv.h>
#include "FFmpegVideo.h"



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

void *videoPlay(void *args) {
    auto *ffmpegVideo = (FFmpegVideo *) args;
    //申请AVFrame
    auto *frame = av_frame_alloc();//分配一个AVFrame结构体,AVFrame结构体一般用于存储原始数据，指向解码后的原始帧

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
            AV_PIX_FMT_YUV420P, 4);


    //LOGE("转换成YUV格式");
    ffmpegVideo->swsContext = sws_getContext(ffmpegVideo->codec->width, ffmpegVideo->codec->height, ffmpegVideo->codec->pix_fmt,
                                             ffmpegVideo->codec->width, ffmpegVideo->codec->height, AV_PIX_FMT_YUV420P,
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

     start_time = av_gettime() /1000000.0;
    last_delay = 0;

    int frameCount;
    last_play = 0;


    bool bFirst = true;

//    ffmpegVideo->time_base.den = 20;

    while (ffmpegVideo->isPlay) {
        if (!ffmpegVideo->ffmpegMusic->bStartPlay) {
            av_usleep(1000 * 5);
            continue;
        }
        if (bFirst) {
            bFirst = false;
            start_time = ffmpegVideo->ffmpegMusic->audio_start_time;
        }

        ffmpegVideo->get(packet);
        {

            frameCount=0;
            if (avcodec_send_packet(ffmpegVideo->codec, packet) == 0) {
                if ((avcodec_receive_frame(ffmpegVideo->codec, frame)) == 0)
                {
                    frameCount = 1;
                }
            }
            if(!frameCount)
                continue;

            //avcodec_decode_video2(ffmpegVideo->codec, frame, &frameCount, packet);
            //if (!frameCount) {
            //    continue;
            //}


            if (frame->format == AV_PIX_FMT_YUVJ420P || frame->format == AV_PIX_FMT_YUV420P) {
                libyuv::I420Copy(frame->data[0], frame->linesize[0],
                                 frame->data[1], frame->linesize[1],
                                 frame->data[2], frame->linesize[2],
                                 yuv_frame->data[0], yuv_frame->linesize[0],
                                 yuv_frame->data[1], yuv_frame->linesize[1],
                                 yuv_frame->data[2], yuv_frame->linesize[2],
                                 yuv_frame->width, yuv_frame->height);

            } else if (frame->format == AV_PIX_FMT_YUV422P || frame->format == AV_PIX_FMT_YUVJ422P) {
                libyuv::I422ToI420(frame->data[0], frame->linesize[0],
                                   frame->data[1], frame->linesize[1],
                                   frame->data[2], frame->linesize[2],
                                   yuv_frame->data[0], yuv_frame->linesize[0],
                                   yuv_frame->data[1], yuv_frame->linesize[1],
                                   yuv_frame->data[2], yuv_frame->linesize[2],
                                   yuv_frame->width, yuv_frame->height);

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

            //video_clock =
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
            if (actual_delay < 0.01) {
                actual_delay = 0.01;
            }
            if (actual_delay > 1.5)
                actual_delay = 1.5;
            usleep( (uint32_t) ((actual_delay * 1000000) + 6000));




            if (gl_Frame != nullptr) {


//                if(gl_Frame->width != yuv_frame->width ||
//                   gl_Frame->height != yuv_frame->height)
//                {
//                    av_freep(&gl_Frame->data[0]);
//                    av_frame_free(&gl_Frame);
//
//                    gl_Frame = av_frame_alloc();
//                    gl_Frame->format = AV_PIX_FMT_YUV420P;
//                    gl_Frame->width = yuv_frame->width;
//                    gl_Frame->height = yuv_frame->height;
//                    av_image_alloc(
//                            gl_Frame->data, gl_Frame->linesize, yuv_frame->width,
//                            yuv_frame->height,
//                            AV_PIX_FMT_YUV420P, 4);
//
//                }


                gl_Frame->width = yuv_frame->width;
                gl_Frame->height = yuv_frame->height;
                gl_Frame->linesize[0] = yuv_frame->linesize[0];
                gl_Frame->linesize[1] = yuv_frame->linesize[1];
                gl_Frame->linesize[2] = yuv_frame->linesize[2];

                libyuv::I420Copy(yuv_frame->data[0], yuv_frame->linesize[0],
                                 yuv_frame->data[1], yuv_frame->linesize[1],
                                 yuv_frame->data[2], yuv_frame->linesize[2],
                                 gl_Frame->data[0], yuv_frame->linesize[0],
                                 gl_Frame->data[1], yuv_frame->linesize[1],
                                 gl_Frame->data[2], yuv_frame->linesize[2],
                                 yuv_frame->width, yuv_frame->height);
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
    pthread_exit(0);

}


FFmpegVideo::FFmpegVideo() {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

}

FFmpegVideo::~FFmpegVideo() {
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
}

void FFmpegVideo::setAvCodecContext(AVCodecContext *avCodecContext) {
    codec = avCodecContext;
}

int FFmpegVideo::put(AVPacket *avPacket) {
    //LOGE("插入队列 video");
    AVPacket *avPacket1 = (AVPacket *) av_mallocz(sizeof(AVPacket));
    //克隆
    if (av_packet_ref(avPacket1, avPacket)) {
        //克隆失败
        return 0;
    }
    //push的时候需要锁住，有数据的时候再解锁
    pthread_mutex_lock(&mutex);
    queue.push_back(avPacket1);//将packet压入队列
    //压入过后发出消息并且解锁
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    return 1;
}

int FFmpegVideo::get(AVPacket *avPacket) {
    pthread_mutex_lock(&mutex);
    while (isPlay) {
        if (!queue.empty() && isPause) {
            //如果队列中有数据可以拿出来
            if (av_packet_ref(avPacket, queue.front())) {
                break;
            }
            //取成功了，弹出队列，销毁packet
            AVPacket *packet2 = queue.front();
            queue.erase(queue.begin());
            av_free(packet2);
            break;
        } else {
            pthread_cond_wait(&cond, &mutex);
        }
    }
    //LOGE("解锁");
    pthread_mutex_unlock(&mutex);
    return 0;
}

void FFmpegVideo::play() {
    if (isPause == 0) {
        ;
    }
    clock = 0;
    isPlay = 1;
    isPause = 1;
    pthread_create(&playId, NULL, videoPlay, this);//开启begin线程

}

void FFmpegVideo::stop() {
    LOGE("VIDEO stop");

    pthread_mutex_lock(&mutex);
    isPlay = 0;
    //因为可能卡在 deQueue
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    pthread_join(playId, 0);

    for (int i = 0; i < queue.size(); ++i) {
        AVPacket *pkt = queue.front();
        queue.erase(queue.begin());
        LOGE("销毁音频帧%d",(int)queue.size());
        av_free(pkt);
    }

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
    if (isPause == 1) {
        isPause = 0;
    } else {
        isPause = 1;
        pthread_cond_signal(&cond);
    }
}
