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

class FFmpegVideo {
public:
    FFmpegVideo();
    ~FFmpegVideo();
    void setAvCodecContext(AVCodecContext *avCodecContext);//解码器上下文

    int put(AVPacket *avPacket);//压进队列
    int get(AVPacket *avPacket);//弹出队列

    void play();//播放
    void stop();//暂停
    void pause();//pause

    double synchronize(AVFrame *frame, double play);

    void setFFmepegMusic(FFmpegMusic *ffmpegMusic);

    void setPlayCall(void (*call)(AVFrame* frame));


public:
    int index;//流索引
    int isPlay=-1;//是否正在播放
    int isPause=-1;//是否暂停
    pthread_t playId;//处理线程
    std::vector<AVPacket*> queue;//队列

    AVCodecContext *codec;//解码器上下文

    SwsContext* swsContext;
    //同步锁
    pthread_mutex_t mutex;
    //条件变量
    pthread_cond_t cond;

    uint8_t da;

    FFmpegMusic *ffmpegMusic;

    AVRational time_base;
    //double  clock;
    double   clock;

};



#endif //FFMPEGVIDEOPLAY_FFMPEGVIDEO_H
