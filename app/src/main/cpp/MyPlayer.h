//
// Created by 小米哥 on 2020/10/12.
//

#ifndef SYMADEMO_ANDROID_MYPLAYER_H
#define SYMADEMO_ANDROID_MYPLAYER_H


#include "FFmpegMusic.h"
#include "FFmpegVideo.h"

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

class MyPlayer {

private:

    void Recheck(AVFrame *pFrameYUV,AVFrame *pFrame);

    int  Play_B();
    void Play();


    FFmpegVideo ffmpegVideo;
    FFmpegMusic ffmpegMusic;

    AVFormatContext *pFormatCtx;
    int videoindex;
    int audioindex;

    AVCodecContext *pVideoCodecCtx = nullptr;
    AVCodecContext *pAudioCodecCtx = nullptr;


    bool   bIsDecordOK;
    bool   bReadDataOK;

    int    nReadInx;
    int    nLength;

    static void* Progress(void *data);



    pthread_t connectdid;


public:

    jobject  object;
    jmethodID Playduration_Mid;
    jmethodID PlayStatus_Mid;
    jmethodID PlayTime_mid;


    char  sPlayName[1024];
    int nSeekTime;
    MyPlayer();
    ~MyPlayer();
    volatile bool   bisPlaying;
    int ReleaseObject(void);

    void SentPlayduration_Mid(int i);
    void SentPlayStatus_Mid(int i);
    void SentPlayTime_mid(int i);

    void SentPlayTime_midTest(int i);


    void  F_SetCallBack(jobject  object1);

    int Play(char *bPath);
    int Stop();
    int Pause();
    void Seek(float fSec);

};


#endif //SYMADEMO_ANDROID_MYPLAYER_H
