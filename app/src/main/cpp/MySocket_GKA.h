//
// Created by AivenLau on 2016/12/1.
//

#ifndef SIMA_WIFI_MYSOCKET_GKA_H
#define SIMA_WIFI_MYSOCKET_GKA_H

#include <string>
#include "MySocketData.h"
#include "phone_rl_protocol.h"
#include "GP4225_FileStruct.h"

using namespace std;

typedef void (*Fuc_GetDAta)(MySocketData *dat);


typedef void (*Fuc_GetData_mjpeg)(char *dat,int nLen);

class MySocket_GKA {


    int ConnectA(string host, int port);
public:
    int buffLen;
    int nICType;


    bool  bNormalTCP;   //正常的读取 TCP，不是读取图传数据

    MySocket_GKA();

    ~MySocket_GKA();

    pthread_mutex_t m_mutex;

    int Connect(string host, int port);

    int DisConnect();

    int Write(MySocketData *dataA);

    int Read(MySocketData *data);

    int Read(MySocketData *data, int msTimeout);

    void StartReadThread(void);

    static void *ReadData(void *dat);
    static void *DispImageThread(void *dat);

    MySocketData RevData;
    //MySocketData RevSendData;

    void GetData(MySocketData *dat);

    Fuc_GetDAta fuc_getData;
    Fuc_GetData_mjpeg  fuc_getData_mjpeg;
    int socketfd;
    volatile  bool bConnected;
    volatile  bool bNeedRead;
    string host;
    int port;
    volatile  bool bFindHead;
    bool  bNotice;
    pthread_t readid;
    pthread_t dispid;


};


#endif //SIMA_WIFI_MYSOCKET_GKA_H
