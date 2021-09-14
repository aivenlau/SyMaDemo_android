//
// Created by Administrator on 2017/11/20.
//

#ifndef FFMPEGVIDEOPLAY_FFMPEGVIDEO_H
#define FFMPEGVIDEOPLAY_FFMPEGVIDEO_H

#import <queue>
#import<vector>
#import <pthread.h>
#import <unistd.h>
#import "FFmpegMusic.h"

#include "media/NdkMediaCodec.h"


class MyPlayer;

#ifdef __cplusplus
extern "C" {
#endif
#import <libavcodec/avcodec.h>
#import <libswresample/swresample.h>
#import <libswscale/swscale.h>
#import <libavformat/avformat.h>

#import <libavutil/imgutils.h>
#import <libavutil/time.h>
#ifdef __cplusplus
}
#endif

//#include <media/NdkMediaCodec.h>

class FFmpegVideo {

public:
    FFmpegVideo();

    ~FFmpegVideo();


    uint64_t pts;
    uint64_t inx;
    int  mWidth;
    int  mHeight;

    //bool   F_InitMediaCoder(int w,int h,char *sps,int nsps,char *pps,int npps);

    //int32_t  mColorFormat;

    //bool F_DecorderData(char *bufData,int bufSize,AVFrame *frame);

    //MyPlayer *player;

    int getSize();

    void setAvCodecContext(AVCodecContext *avCodecContext);//解码器上下文

    int put(AVPacket *avPacket);//压进队列
    int get(AVPacket *avPacket);//弹出队列

    void play();//播放
    void stop();//暂停
    void pause();//pause

    double synchronize(AVFrame *frame, double play);

    void setFFmepegMusic(FFmpegMusic *ffmpegMusic);

    void setPlayCall(void (*call)(AVFrame *frame));

    void F_ClearQueue();


public:

    //AMediaCodec* mMediaCodec;


    int index;//流索引
    bool isPlay;//是否正在播放
    bool isPause;//是否暂停
    pthread_t playId;//处理线程
    std::vector<AVPacket *> queue;//队列

    AVCodecContext *codec;//解码器上下文

    SwsContext *swsContext;
    //同步锁
    pthread_mutex_t mutex;
    //条件变量
    pthread_cond_t cond;

    uint8_t da;

    FFmpegMusic *ffmpegMusic;

    AVRational time_base;
    int      nFrameRate;
    //double  clock;
    double clock;


    ////////
//    int32_t    mColorFormat;
//    uint8_t  *YUVBuffer;
//    uint64_t  YUVBuffer_Len;
//    AMediaCodec* mMediaCodec;
//    bool     bMediaCodeStart;
//    int64_t pts;
//    int64_t  pts_;
//    bool  F_InitMediaCodecDecord(int width,int height,uint8_t *sps,int spsSize, uint8_t* pps,int ppsSize);
//    int   F_DecordPacket(AVPacket *packet);

};


#endif //FFMPEGVIDEOPLAY_FFMPEGVIDEO_H
