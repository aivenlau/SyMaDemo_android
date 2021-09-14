//
// Created by AivenLau on 2018/12/10.
//

#ifndef JH_HISI2_ANDROID_MYMP4_H
#define JH_HISI2_ANDROID_MYMP4_H

#include <string>
#include <pthread.h>
#include <stdbool.h>
#include "Defines.h"

#include "mp4v2.h"
#include "libyuv.h"
#include "MyFrame.h"
#include "MyMediaCoder.h"

#include <time.h>


#include <vector>
#include <queue>
#include <jni.h>
#include "faac.h"


#define   PCM_BUFFER_KEN  (1024*1024)

typedef unsigned char Byte;


using namespace std;

typedef std::queue<MyFrame *> WriteFrameQueue_h264;

class MyMp4 {



    bool MP4AacGetConfiguration(uint8_t** ppConfig,
                                uint32_t* pConfigLength,
                                uint8_t profile,
                                uint32_t samplingRate,
                                uint8_t channels);
    uint8_t MP4AdtsFindSamplingRateIndex(uint32_t samplingRate);

    int g711a_decode(void *pout_buf,int *pout_len,void * pin_buf,int in_len);




public:

    MyMp4();
    ~MyMp4();
    void StartWrite(void);

    uint64_t  getMs();

    bool      bConvert;

    uint64_t   T1;

    uint64_t   T1_V;


    int  nRecTime;

    bool  bG_Audio;
    bool  bRecording;
    string   sSavePath;
    string   sSavePathTmp;
    int    mMusicChannel;


    int     nRecordWidth;
    int     nRecordHeight;
    int     nFps;


    //Byte    _pbAACBuffer[1024*1024];

    //Byte    _pbPCMTmpBuffer[1024*1024];
    //Byte    _pbPCMBuffer[1024];
    //int     _nCount;

    MyFrame  *pFrameRecord;




    MP4TrackId videoTrackId;
    MP4TrackId  audioTrackId;
    MP4FileHandle fileHandle;
    pthread_t m_writeThread;


    pthread_mutex_t m_Frame_Queuelock;
    pthread_cond_t m_Frame_condition;

    pthread_mutex_t m_adjRecTime_lock; //= PTHREAD_MUTEX_INITIALIZER;

    WriteFrameQueue_h264 m_FrameQueue_H264;

    void ClearQueue();

    int AddMp4Video(uint8_t *sps, int len1, uint8_t *pps, int len2);

    int WriteMp4Frame(uint8_t *data, int nLen, bool b);
    bool  WriteAudio(jbyte * data,int nLen);

    static void *WriteThreadFunction(void *param);

    int writeVideo();


    int  StopSaveVideo();
    int  F_StartRecordMp4(const char * sPath,int nFps,int w,int h,bool bAudio);




    void F_AddAudioFrame(unsigned char * pcmBuffer,int nLen);
    void F_AddVideoFrame(int w,int h, char * data,int nlen);

    //void F_AddVideoFrameA(int w,int h, char * data,int nlen);



    int  F_GetRecordTime();


    void F_SetConvert(bool b);
    int F_InitMp4();

    int F_InitMp4_A(Byte *sps,int n1,Byte *pps,int n2);


    void InitfaacEnc();
    faacEncHandle hEncoder;
    faacEncConfigurationPtr pConfiguration;

    int nAudioFrame;
    unsigned int nChannels;
    unsigned int nPCMBitSize;
    unsigned long nInputSamples;
    unsigned long nSampleRate;
    unsigned long nMaxOutputBytes;
    int nPCMBufferSize;
    uint8_t *pbPCMBuffer;
    uint8_t *pbAACBuffer;
    int nCount;

    int Hisi_AddAudioFrame(uint8_t *databuffer,int nLen);





};


#endif //JH_HISI2_ANDROID_MYMP4_H
