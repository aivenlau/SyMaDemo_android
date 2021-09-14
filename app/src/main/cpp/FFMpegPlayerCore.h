//
// Created by generalplus_sa1 on 4/29/16.
//
#ifndef FFMPEGTEST_C_FFMPEGPLAYER_H
#define FFMPEGTEST_C_FFMPEGPLAYER_H
//----------------------------------------------------------------------

#include <vector>
#include <queue>

typedef unsigned char Byte;


#include "media/NdkMediaCodec.h"

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
#include "libavfilter/avfilter.h"
#include "libavfilter/buffersrc.h"
#include "libavfilter/buffersink.h"

#ifdef __cplusplus
}
#endif

#include "parsesps.h"

#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include "MySocketData.h"

#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <semaphore.h>
#include "media/NdkMediaCodec.h"

//#include "mp4v2.h"
#include "MyFrame.h"
#include "libyuv.h"

#include "MyMaskImg.h"


//#include "h264_rbsp.h"
//#include "sps_parse.h"
//#include "pps_parse.h"
//#include "myOpenCV.h"


#define  FFMPEG3_3_3


//----------------------------------------------------------------------
#define FFMPEGPLAYER_NOERROR                     0
#define FFMPEGPLAYER_INITMEDIAFAILED             1
#define FFMPEGPLAYER_MEDIAISPLAYING              2
#define FFMPEGPLAYER_CREATESAVESTREAMFAILED      3
#define FFMPEGPLAYER_SAVESNAPSHOTFAILED          4
#define FFMPEGPLAYER_SAVEVIDEOFAILED             5


typedef unsigned char byte;

#define   DATA_Type_NO     0
#define   DATA_Type_H264    1
#define   DATA_Type_MJPEG   2


#define  bit0_OnLine            1
#define  bit1_LocalRecording    2
//#define  SD_Ready               4
//#define  SD_Recroding           8
#define  SD_Photo               0x10




//IC_GK = 0,      //192.168.234.X
//IC_GP,          //192.168.25.X
//IC_SN,          //192.168.123.X
//IC_GKA,        //175.16.10.X
//IC_GPRTSP,   //192.168.26.X
//IC_GPH264,   //192.168.27.X
//IC_GPRTP,    //192.168.28.X   GP
//IC_GPRTPB,   //192.168.29.X   JH
//IC_GPH264A,   //192.168.30.X
//IC_GPRTPC     192.168.31.1

//#define   D_H264file



extern bool bInitMediaA;


typedef std::queue<AVFrame *> WriteFrameQueue;
typedef std::vector<AVFrame *>::iterator WriteFrameQueue_iterator;
typedef std::queue<MyFrame *> WriteFrameQueue_h264;
typedef std::vector<MyFrame *>::iterator WriteFrameQueue_iterator_h264;
typedef std::queue<MySocketData *> WriteFrameQueue_Disp;
typedef std::queue<AVPacket *> RevFrameQueue;

typedef enum {
    E_PlayerStatus_Stoped,
    E_PlayerStatus_Playing,
    E_PlayerStatus_Stoping,

} E_PlayerStatus;


typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} RGBA_STRUCT;

typedef struct {
    int nWidth;
    int hHeight;
    int *pLenL;
    int *pLenR;
} Mask_img;


typedef struct{
    char *data;
    int nLen;
}MyPacket;

//----------------------------------------------------------------------
class C_FFMpegPlayer {


    bool MP4AacGetConfiguration(uint8_t **ppConfig,
                                uint32_t *pConfigLength,
                                uint8_t profile,
                                uint32_t samplingRate,
                                uint8_t channels);

    uint8_t MP4AdtsFindSamplingRateIndex(uint32_t samplingRate);

public:
    bool  bNeedAdd; //需要添加到解码线程
    E_PlayerStatus m_Status;
    uint64_t nErrorFrame;

    C_FFMpegPlayer();

    ~C_FFMpegPlayer();

//    MP4TrackId video;
//    MP4TrackId  music;
//    MP4FileHandle fileHandle;

    bool bIsH264;
    int nSpsSet;
    bool bStatWrite;
    bool bStarDecord;
//    std::string  sRecordFileName;
//    std::string  sRecordFileName_tmp;

#ifdef  D_H264file
    int     h64fileHandle;
    std::string  sH264FileName;
#endif


    bool bAdjFocus;
    int nAdjX;
    int nAdjY;

    int nRecordWidth;
    int nRecordHeight;

//    int nRecordWidth_rel;
//    int nRecordHeight_rel;

    void F_ReSetRecordWH(int w, int h);

    void F_InitFFfmpeg(void);

    bool b480;
    bool bFollow;
    bool bContinue;
    unsigned char *Rgbabuffer;
    unsigned char *YUVbuffer;
    int nSecT;


    int nDisplayWidth;
    int nDisplayHeight;
    bool nNeedRedraw;


    bool F_RecreateEnv(void);

    void F_DispSurface(void);


    int InitMediaH264(void);

    int InitMediaMjpeg(void);

    AVPacket *F_GetPacket(void);

    void F_Adj_Resolution(void) const;




    //int decodeAndRender_GKA_B(MySocketData *dat);

    void F_StartDispThread();

    void DispPaket(void *data);
    int DecodeH264(MySocketData *dat);
    int DecodeMjpeg(char *data, int nLen);

    //int decodeAndRender_RTP(char *data, int nLen);


    uint8_t nIC_Type;



    int InitMedia(const char *path);

    int PlayMedia();

    int Stop(bool bStopVideo=true);

    int SaveSnapshot(const char *path);

    int GetStatus(void);

    int SaveVideo(const char *path, bool bisH264 = false);

    int StopSaveVideo();

    int AddMp4Video(uint8_t *sps, int len1, uint8_t *pps, int len2);

    int WriteMp4Frame(uint8_t *data, int nLen, bool b);

    static void *decodeThreadFunction(void *param);

    static void *WriteThreadFunction(void *param);

    void ClearQueue();


    AVCodecContext *m_codecCtx;
    AVCodecContext *m_codecCtx_T;

    void _DispDecordData(void);


private:

    sps_info_struct  spsinfo;

    int CloseVideo();

    int EncodeSnapshot();

    int decodeAndRender();

    int writeVideo();


    int SetYUVFrame(AVFrame *yunframe);




    AVCodecParserContext *m_parser;
    bool   bGetSps;
    bool   bGetPps;
    Byte    *sps;
    Byte    *pps;
    int     spsLen;

    bool    bFirst = false;

    int   ppsLen;
    //AVCodec *codec;

    bool bPause;


    AVFormatContext *m_formatCtx;
    int m_videoStream;

    struct SwsContext *img_convert_ctx;

    int m_width;
    int m_height;

    pthread_t m_decodeThread;
    pthread_t m_writeThread;

    pthread_t m_dispid;


    AVPixelFormat pix_format;









    bool m_bSaveSnapshot;



    //void F_DispH264NoBuffer(MySocketData *data);

    void F_AdjUV(int8_t u, int8_t v);

public:

    AVFrame *pFrameresolution;


    AVFrame *pFrameYUV_Real;
    AVFrame *pFrameYUV_Square;

    AVFrame *pFrameYUV;
    AVFrame *m_decodedFrame;
    AVFrame *pFrameRecord;
    AVFrame *pFrameYUV_D;
    AVFrame *frame_a;
    AVFrame *frame_b;
    AVFrame *frame_c;
    AVFrame *frame_SnapBuffer;

    void F_MakeCircul(AVFrame *frame);

    void F_AdjDarkcorner();

    bool F_WriteAudio(jbyte *data, int nLen);

    int Releaseffmpeg(void);

    char m_snapShotPath[256];
    char m_MediaPath[256];

    int nfps;
    int nRecFps;
    bool m_bOpenOK;
    bool bFlip;
    bool b3D;
    bool b3DA;

    bool bMirror;

    MySocketData keyFrame;
    AVPacket packet;

public:


    pthread_mutex_t m_Frame_Queuelock;
//    pthread_cond_t m_Frame_condition;

    bool m_bSaveVideo;
    //sem_t sem_Rec;


    pthread_mutex_t m_SetResulotion_mutex_t;

    pthread_mutex_t m_Rev_Queuelock;
    pthread_cond_t m_Rev_condition;

    WriteFrameQueue_h264 m_FrameQueue_H264;
    RevFrameQueue    m_RevFrameQueue;



    MyMaskImg maskImg;

    AVFilterGraph *filter_graph;
    AVFilterContext *buffersrc_ctx;
    AVFilterContext *buffersink_ctx;

    AVFilterContext *hqdn3d_ctx;
    AVFilterContext *rotate_ctx;

    int F_SetRotate(float n); //旋转角度
    int init_filters(const char *filters_descr, const char *filters2_descr);

    //MediaCodec


    int mDecordWidth;
    int mDecordHeight;

    AVFrame  *mediacoder_frame;

    int64_t  pts;
    int  mColorFormat;
    AMediaCodec* mMediaCodec;
    bool F_InitMediaCodecDecord(int width,int height,uint8_t *sps,int spsSize, uint8_t* pps,int ppsSize);
//    bool F_DecorderData(char *bufData,int bufSize,bool bDisp=true);
    //void handleData(AVFrame *m_decodedFrame);
    void F_ReleaseMediaCode1();

    int  nBufferedCount;
//    MyPacket myPacket[10];
//    MyPacket myPacketA;
    volatile bool   bNeedDisp;
    int    nDispInx;


};


#endif //FFMPEGTEST_C_FFMPEGPLAYER_H
