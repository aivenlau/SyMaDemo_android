//
// Created by AivenLau on 2016/12/1.
//

#include <linux/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <unistd.h>
#include <linux/in.h>
#include <linux/tcp.h>
#include <cerrno>
#include <sys/endian.h>
#include <arpa/inet.h>
#include <asm/ioctls.h>
#include <sys/socket.h>
#include "Defines.h"
#include "MySocket_GKA.h"
#include "phone_rl_protocol.h"
#include "JPEG_BUFFER.h"

#include <cstdio>
#include <pthread.h>

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


//#include <stdbool.h>

#ifdef __cplusplus
}
#endif


//#define   IC_NO         -1
//#define   IC_GK         0
//#define   IC_GP         1
//#define   IC_SN         2
//#define   IC_GKA        3
//#define   IC_GPRTSP     4
//#define   IC_GPH264     5
//#define   IC_GPRTP      6
//#define   IC_GPH264A    7
//#define   IC_GPRTPB      8
//#define   IC_GK_UDP      9
//
//#define   IC_GPRTPC      10


extern JPEG_BUFFER jpg0;

MySocket_GKA::MySocket_GKA() : socketfd(-1), bConnected(false), readid(-1), bFindHead(false), bNotice(false), nICType(IC_NO),fuc_getData_mjpeg(nullptr),fuc_getData(nullptr),
                               buffLen(1000),bNormalTCP(false),port(0)
{
    pthread_mutex_init(&m_mutex, nullptr);
    fuc_getData = nullptr;
    bConnected = false;
};

MySocket_GKA::~MySocket_GKA() {
    DisConnect();
};

int MySocket_GKA::DisConnect() {
    if (bConnected) {
        bConnected = false;
        usleep(1000 * 10);
        shutdown(socketfd, 2);
        close(socketfd);
        socketfd = -1;
        this->host = "";
        this->port = -1;
    }

    if (readid != -1) {
        void *ret = nullptr;
        pthread_join(readid, &ret);
        readid = -1;
    }

    return 0;
}

int MySocket_GKA::Connect(string host, int port) {
    if (ConnectA(host, port) < 0) {
        usleep(1000 * 10);
        DisConnect();
        return ConnectA(host, port);
    }
    return 0;
}

int MySocket_GKA::ConnectA(string host_, int port_) {
    const char *df = host.c_str();
    struct sockaddr_in dest_addr; //destnation ip info
    if (socketfd != -1 && bConnected) {
        return 1;
    }
    host = host_;
    port = port_;

    socketfd = socket(AF_INET, (int)((unsigned)SOCK_STREAM|(unsigned int)SOCK_CLOEXEC), IPPROTO_TCP);
    if (socketfd == -1) {
        bConnected = false;
        return -1;
    }
    int ret = 0;
    unsigned long ul = 1;

    struct timeval timeout;
    fd_set readset;
    fd_set writeset;

    int error = -1;
    int len = sizeof(int);

    int bTimeoutFlag = 0;

    int nRecvBufLen = 200 * 1024; //?????????32K
    int status = setsockopt(socketfd, SOL_SOCKET, SO_RCVBUF, (char *) &nRecvBufLen, sizeof(int));

    int nAddr = 1;
    setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &nAddr, sizeof(int));
    int nPorta = 1;
    setsockopt(socketfd, SOL_SOCKET, SO_REUSEPORT, &nPorta, sizeof(int));
    if(bNormalTCP)
    {
        int al = 1;
        setsockopt(socketfd, SOL_SOCKET, SO_KEEPALIVE, &al, sizeof(int));
        int al1 = 2;
        setsockopt(socketfd, SOL_SOCKET, TCP_KEEPIDLE, &al1, sizeof(int));
        int al2 = 1;
        setsockopt(socketfd, SOL_SOCKET, TCP_KEEPINTVL, &al2, sizeof(int));
    }

/*
    int keepAlive = 1;   // ??????keepalive??????. ?????????: 0(??????)
    int keepIdle = 5;   // ?????????60??????????????????????????????,???????????????. ?????????:7200(s)
    int keepInterval = 1;   // ???????????????????????????????????????5???. ?????????:75(s)
    int keepCount = 2;   // ?????????????????????. ?????????????????????????????????..?????????:9(???)
    setsockopt(socketfd, SOL_SOCKET, SO_KEEPALIVE, (void*)&keepAlive, sizeof(keepAlive));
    setsockopt(socketfd, SOL_TCP, TCP_KEEPIDLE, (void*)&keepIdle, sizeof(keepIdle));
    setsockopt(socketfd, SOL_TCP, TCP_KEEPINTVL, (void*)&keepInterval, sizeof(keepInterval));
    setsockopt(socketfd, SOL_TCP, TCP_KEEPCNT, (void*)&keepCount, sizeof(keepCount));
*/

    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(this->port);
    dest_addr.sin_addr.s_addr = inet_addr(this->host.c_str());
    ioctl(socketfd, FIONBIO, &ul);
    usleep(1000);
    bConnected = false;
    if (-1 == connect(socketfd, (struct sockaddr *) &dest_addr, sizeof(struct sockaddr)))
    {
        timeout.tv_sec = 1;
        timeout.tv_usec = 1500;  //1.500ms
        FD_ZERO(&writeset);
        FD_SET(socketfd, &writeset);
        ret = select(socketfd + 1, nullptr, &writeset, nullptr, &timeout);
        if (ret == 0)              //??????0??????????????????????????????????????????timeout??????
        {
            getsockopt(socketfd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *) &len);
            if (error == 0)          // ??????????????????????????????????????????????????????
            {
                bTimeoutFlag = 1;
               // LOGE("Not Connect timeout");
            } else {
                LOGE("Not Connect host error");
            }
        } else if (ret == -1)      // ??????-1??? ???????????????????????????????????????errno
        {
            LOGE("Not Connect host error1");

        } else {
            bConnected = true;
        }
    } else {
        bConnected = true;
    }
    int statusa = 0;

    ul = 0;
    ioctl(socketfd, FIONBIO, &ul); //?????????socket?????????????????????
    int on = 1;
    statusa = setsockopt(socketfd, IPPROTO_TCP, TCP_QUICKACK, (void *) &on, sizeof(on));
    //  LOGE("TCP_QUICKACK status = %d",statusa);
    on = 0;
    statusa = setsockopt(socketfd, IPPROTO_TCP, TCP_NODELAY, (void *) &on, sizeof(on));
    // LOGE("TCP_NODELAY status = %d",statusa);
    on = 1;
    statusa = setsockopt(socketfd, IPPROTO_TCP, TCP_DEFER_ACCEPT, (void *) &on, sizeof(on));


    int set = 1;

    if (bConnected) {
        setsockopt(socketfd, SOL_SOCKET, MSG_NOSIGNAL, (void *) &set, sizeof(int));
        return 0;
    } else {
        LOGE("not connect!!!!!");
        close(socketfd);
        socketfd = -1;
        bConnected = false;
        this->host = "";
        this->port = -1;
        return -1;
    }
}

int MySocket_GKA::Write(MySocketData *data) {
    struct timeval timeoutA = {0, 1000 * 10};     //10ms
    if (!bConnected)
        return -1;
    setsockopt(socketfd, SOL_SOCKET, SO_SNDTIMEO, (char *) &timeoutA, sizeof(struct timeval));
    ssize_t ret = send(socketfd, data->data, (size_t) data->nLen, 0);
    if (ret < data->nLen)
        return -1;
    return 0;
}

int MySocket_GKA::Read(MySocketData *data, int msTimeout) {
    ssize_t nRet;

    uint8_t *bufferA = data->data;
    int nLen = data->nLen;
    uint32_t nCount = 0;
    int64_t start = av_gettime();
    while (nLen > 0) {
        struct timeval timeoutA = {0, 1000 * 20};     //20ms
        setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeoutA, sizeof(struct timeval));
        nRet = recv(socketfd, bufferA, (size_t) nLen, 0);
        if (nRet > 0) {
            nLen -= nRet;
            bufferA += nRet;
            nCount += nRet;
        } else if (nRet == 0) {
            int errs = errno;
            if (errs == EWOULDBLOCK) { ;
            } else {
                data->nLen = 0;       //??????????????????
                return -1;
            }
        }
        if (msTimeout > 0) {
            int64_t current = av_gettime();
            if (current - start >= (msTimeout * 1000)) {
                break;
            }
        }
    }
    data->nLen = nCount;
    return data->nLen;
}

int MySocket_GKA::Read(MySocketData *data) {
    int nRet;
    struct timeval timeoutA = {0, 1000 * 50};     //20ms
    setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeoutA, sizeof(struct timeval));
    if ((nRet = (int)recv(socketfd, data->data, (size_t)data->nLen, 0)) != -1) {
        if (nRet <= 0) {
            int errs = errno;
            if (errs == EWOULDBLOCK) { ;
            } else {
                data->nLen = 0;       //??????????????????
                return -1;
            }
        } else {
            return nRet;
        }
    }
    return -1;
}
//0XF3B0A4B8

//int MySocket_GKA::FindHead(MySocketData *dat, int pos) {
//
//    int ix;
//    if (dat->nLen < 4)
//        return -1;
//    uint8_t *pdata = dat->data;
//    uint32_t magic = 0x01000000;
//    uint32_t data = 0;
//    for (ix = pos; ix <= dat->nLen - 4; ix++) {
//        data = *((uint32_t *) (pdata + ix));
//        if (magic == data) {
//            return ix;
//        }
//    }
//    return -1;
//}

void F_ResetRelinker();

extern int64_t nCheckT_pre;


extern MySocket_GKA GK_tcp_NoticeSocket;


uint8_t pat[] = {0, 0, 0, 1};

//void getNext(int next[])   //lengthP????????????P?????????
//{
//    int j = 0, k = -1;//j???P???????????????k??????????????????????????????next????????????
//    next[0] = -1;//?????????0????????????next????????????-1
//    while (j < 4) { //????????????????????????
//        if (k == -1 || pat[j] == pat[k]) {//???????????????????????????????????????????????????j?????????????????????k?????????????????????
//            j++;
//            k++;
//            next[j] = k;//??????next??????j???????????????k
//        } else
//            k = next[k];//?????????????????????????????????
//    }
//}
//
//int kmp(int k, int next[], int nTlen, uint8_t *T) {
//    int posP = 0, posT = k;              //posP???posT??????????????????pat????????????T?????????????????????????????????????????????
//    //int lengthP= pat.length();    //lengthP????????????pat???
//    int lengthT = nTlen;// T.length();        //lengthT????????????T???
//    while (posP < 4 && posT < lengthT)    //???????????????
//    {
//        if (posP == -1 || pat[posP] == T[posT]) {                           //??????????????????
//            posP++;
//            posT++;
//        } else
//            posP = next[posP];        //???????????????next???????????????????????????????????????
//    }
//    if (posP < 4)
//        return -1;
//    else
//        return posT - 4;//????????????
//}

void F_SetRelinkerT(int nMs);

int nNexPos = 0;


int32_t _nJpgStart;
int32_t _nJpgEnd;

//uint8_t  *mypbuffer = new uint8_t[500*1024];

bool F_FindJpg(void) {
    if (jpg0.nCount < 4)
        return false;

    _nJpgStart = -1;
    _nJpgEnd = -1;



    for (uint32_t i = 0; i < jpg0.nCount; i++) {
        if (jpg0.buffer[i] == 0xFF && jpg0.buffer[i + 1] == 0xD9) {
            _nJpgEnd = i;
            break;
        }
    }
    if(_nJpgEnd >= 0)
    {

        for (uint32_t i = 0; i < _nJpgEnd; i++) {
            if (jpg0.buffer[i] == 0xFF && jpg0.buffer[i + 1] == 0xD8) {
                _nJpgStart = i;
                break;
            }
        }

        if(_nJpgStart>=0)
        {
            return true;
        }
        else
        {
            jpg0.Clear();
            _nJpgStart=-1;
            _nJpgEnd = -1;

        }
    }
    return false;

    /*
    if (_nJpgStart >= 0 && _nJpgEnd >= 0)
    {
        if(_nJpgEnd > _nJpgStart)
        {
            return true;
        }
        else
        {

        }
    }
     */
    return false;
}

void F_SentError(int nError,const char  *info);

void *MySocket_GKA::ReadData(void *dat) {
    auto *self = (MySocket_GKA *) dat;
    int nRet;

    int Bufferlen = self->buffLen;
    if(!self->bNormalTCP) {
        if (self->nICType == IC_GPRTPC) {
            Bufferlen = 8192;
            jpg0.Clear();
        }
    }

    auto *buffer = new uint8_t[Bufferlen];
    self->RevData.nLen = 0;
    MySocketData mydat;
    MySocketData mydatA;
    fd_set set;
//    bool bStartab = false;
//    int on = 1;
//
//    //F_SetRelinkerT(3000);
    nNexPos = 0;
    int next[4] = {0};
    auto  *pbuffer = new uint8_t[500*1024];
    while (self->bConnected) {
        struct timeval timeoutA = {0, 1000*5};
        //int nError;
        FD_ZERO(&set); // ??????????????????????????????
        // ????????????select
        FD_SET(self->socketfd, &set); // ???socka?????????????????????????????????
        nRet = select(self->socketfd + 1, &set, nullptr, nullptr, &timeoutA);  // ????????????????????????????????????+1, &rfd, nullptr, nullptr, &timeoutA);// ????????????????????????????????????
        if (nRet <= 0) // ??????
        {
            continue;
        }


        bzero(buffer, Bufferlen);if (!(FD_ISSET(self->socketfd, &set))) {
            continue;
        }
        nRet = (int)recv(self->socketfd, buffer, (size_t)Bufferlen, 0);
        if (nRet <= 0) {
                LOGE_B("Remote Sockect lose!!!!");
                shutdown(self->socketfd, 2);
                close(self->socketfd);
                self->socketfd = -1;
                self->host = "";
                self->port = -1;
                self->bConnected = false;
                const char *ds = "Remote tcp  Sockect lose!!!!";
                F_SentError(0xFF,ds);
                break;

        }
        if (nRet > 0) {
            if(!self->bNormalTCP)
            {
                F_ResetRelinker();
                if (self->nICType == IC_GPRTPC) {
                    if (jpg0.AppendData(buffer, nRet)) {
                        if (F_FindJpg()) {
                            int nLen = jpg0.nCount;
                            int nJpgStart = _nJpgStart;
                            int nJpgEnd = _nJpgEnd;
                            uint8_t *buffer_ = jpg0.buffer;
                            if (self->fuc_getData_mjpeg != nullptr) {
                                self->fuc_getData_mjpeg((char *) (buffer_ + nJpgStart), nJpgEnd - nJpgStart + 2);
                            }
                            if (nLen - nJpgEnd - 2 > 0) {
                                memcpy(pbuffer, buffer_ + nJpgEnd + 2, nLen - nJpgEnd - 2);
                                jpg0.Clear();
                                jpg0.AppendData(pbuffer, nLen - nJpgEnd - 2);

                            } else {
                                jpg0.Clear();
                            }
                            _nJpgStart = -1;
                            _nJpgEnd = -1;
                        }
                    } else {
                        LOGE("Not Find!!!");
                        jpg0.Clear();
                        _nJpgStart = -1;
                        _nJpgEnd = -1;
                    }

                } else {

                    if (self->bNotice) {
                        if (nRet > self->RevData.nSize) {
                            LOGE("errorA");
                        }
                        self->RevData.nLen =(uint32_t)nRet;
                        memcpy(self->RevData.data, buffer,(size_t)nRet);
                        self->GetData(&self->RevData);
                        usleep(100);

                    }
                    else {
                        mydat.nLen = 0;
                        mydat.AppendData(buffer, (uint32_t) nRet);
                        self->GetData(&mydat);
                    }
                }
            }else
            {
                if (self->fuc_getData_mjpeg != nullptr) {
                    self->fuc_getData_mjpeg((char *) buffer, nRet);

                }
            }
        }

    }
    if(buffer!=nullptr)
    {
        delete []buffer;
    }
    if(pbuffer!=nullptr)
    {
        delete []pbuffer;
    }
    if (self == &GK_tcp_NoticeSocket) {
        LOGE("Exit NoticeSocket  Read Thread!!!");
    } else {
        LOGE("Exit DataSocket  Read Thread!!!");
    }
    self->readid = -1;
    return nullptr;

}

//void MySocket_GKA::GetData_A(uint8_t *dataA,int nLenA)
//{
//
//
//}

void MySocket_GKA::GetData(MySocketData *dat) {
    if (fuc_getData != nullptr) {
        fuc_getData(dat);
    }
}

void MySocket_GKA::StartReadThread(void) {
    int ret = 0;
    if (readid == -1) {
        bFindHead = true;
        RevData.nLen = 0;
        ret = pthread_create(&readid, nullptr, ReadData, (void *) this); // ????????????0???????????????????????????

    }
    if (ret != 0) {
        readid = -1;
    }

}