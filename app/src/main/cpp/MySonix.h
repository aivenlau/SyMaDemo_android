//
// Created by AivenLau on 2016/12/12.
//

#ifndef SIMA_WIFI_MYSONIX_H
#define SIMA_WIFI_MYSONIX_H


#ifdef __cplusplus
extern "C" {
#endif

#include "jh_wifi.h"

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/pixfmt.h"
#include "libavutil/imgutils.h"
#include "libavutil/time.h"
#include "libavutil/error.h"
#include "libavutil/frame.h"




#ifdef __cplusplus
}
#endif

#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>


#include  <vector>
#include  <list>
#include  "MyFrame.h"

#define COMMAND_TAG 0x42

#define COMMAND_GETCFG 0x02

#define COMMAND_START 0x76
#define COMMAND_STOP 0x77


#define COMMAND_TURN_LEFT 0x70
#define COMMAND_TURN_RIGHT 0x71
#define COMMAND_LEFT 0x72
#define COMMAND_RIGHT 0x73
#define COMMAND_UP 0x74
#define COMMAND_DOWN 0x75

#define COMMAND_SLEEP_TIME 50000

#define BUFFER_SIZE 1500
#define PACKET_SIZE 1456

#define COMMAND_SOCKET_PORT 30863
#define VIDEO_SOCKET_PORT 30865

using namespace std;
//extern int  PlatformDisplay_SN(char *pack,int nLen);

typedef void (*Fuc_GetDAtaSN)(char *dat, int nLen);

class MySonix {
public:
    MySonix();

    ~MySonix();

    bool F_GetResolution(void);



    void parsePacket(char *packeta, int length);

    MyFrame *merageMJPGHeader(char *header, int nHeadlen, char *data, int nLen);

    //void F_SendFrame(uint8_t *buffer, int nLen);

    void StartReceive(void);


    //static void *doReceive_Cfg(void *dat);
    static void *doReceive(void *dat);
    static void *doReceive_cmd(void *dat);


    static void *doDisplay(void *dat);

    int createVideoSocket(void);

    int createCommandSocket(void);

    void closeVideoSocket(void);

    void closeCommandSocket(void);

    void sendCommand(char *command, int length);

    void sendStop(void);

    void sendStart(void);

    void sendGetCfgCmd(void);

    void F_Set480(bool b);

    void  F_CreateRevSocket_and_Listen(void);


    bool isCancelled;
    int videofd;
    int commandfd;
    int uartCommandfd;
    int indexForPacket;

    int  udp_socket;

    int _qValue;
    pthread_t readid;
    pthread_t readid_cfg;
    pthread_t dispid;

    pthread_t rev_cmd_thread;
    vector<char> mjpgPacket;
    vector<char> mjpgFrame;
    bool bFlip;

    bool _alreadyBind;
    bool _isWaiting;
    bool b480;


    Fuc_GetDAtaSN fuc_getDAta;

    char QNumber;
    bool mIsFirstPacket;
    char *readBuff;
    char *readBuff_cfg;

    list<MyFrame *> myFrameList;

private:
    static pthread_mutex_t mutex;




/*
    if(m_decodedFrame == nullptr)
    {
        m_decodedFrame = av_frame_alloc();
        codec = avcodec_find_decoder(AV_CODEC_ID_H264);
        m_codecCtx = avcodec_alloc_context3(codec);
        m_codecCtx->codec_id = AV_CODEC_ID_H264;
        //m_parser = av_parser_init(AV_CODEC_ID_H264);
        //int ret = avcodec_open2(m_codecCtx, codec, nullptr);
        //if (ret != 0){
       // }
       // return 0;
    }

    return 0;
    */

};


#endif //SIMA_WIFI_MYSONIX_H
