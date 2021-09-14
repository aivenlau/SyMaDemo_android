//
// Created by AivenLau on 2016/12/1.
//

extern int nWifiInterface;

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

void onGetVideoData(void);

MySocket_GKA::MySocket_GKA() : socketfd(-1), bConnected(false), readid(-1), bFindHead(false),
                               bNotice(false), nICType(IC_NO), fuc_getData_mjpeg(nullptr),
                               fuc_getData(nullptr),
                               buffLen(1024 * 4), bNormalTCP(false), port(0) {
    pthread_mutex_init(&m_mutex, nullptr);
    fuc_getData = nullptr;
    bConnected = false;
};

MySocket_GKA::~MySocket_GKA() {
    DisConnect();
};

int MySocket_GKA::DisConnect() {

    bNeedRead = false;
    if(readid!=-1)
    {
        pthread_join(readid,0);
        readid = -1;
    }
    close(socketfd);
    bConnected = false;
    socketfd = -1;
    host = "";
    port = -1;
    return 0;
}

int MySocket_GKA::Connect(string host, int port) {
    if (ConnectA(host, port) < 0) {
        //usleep(1000);
        //DisConnect();
        return ConnectA(host, port);
    }
    return 0;
}

//extern char wlan[256];


int MySocket_GKA::ConnectA(string host_, int port_) {
    const char *df = host.c_str();
    struct sockaddr_in dest_addr; //destnation ip info
    if (socketfd != -1 && bConnected) {
        return 1;
    }
    host = host_;
    port = port_;
    socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(nWifiInterface!=-1)
    {
        setsockopt(socketfd, IPPROTO_IP, SO_BINDTODEVICE, &nWifiInterface, sizeof(nWifiInterface));
    }
    LOGE("socketABC = %d",socketfd);
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


//    int nRecvBufLen = 200 * 1024; //设置为32K
//    int status = setsockopt(socketfd, SOL_SOCKET, SO_RCVBUF, (char *) &nRecvBufLen, sizeof(int));

    int nAddr = 1;
    //setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &nAddr, sizeof(int));
    int nPorta = 1;
    //setsockopt(socketfd, SOL_SOCKET, SO_REUSEPORT, &nPorta, sizeof(int));


    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(this->host.c_str());
    dest_addr.sin_port = htons(this->port);
    ioctl(socketfd, FIONBIO, &ul);
    bConnected = false;
    if (-1 == connect(socketfd, (struct sockaddr *) &dest_addr, sizeof(struct sockaddr))) {
        timeout.tv_sec = 0;
        timeout.tv_usec = 1000*200;  //1.500ms
        FD_ZERO(&writeset);
        FD_SET(socketfd, &writeset);
        ret = select(socketfd + 1, nullptr, &writeset, nullptr, &timeout);
        if(ret>0)
        {
            getsockopt(socketfd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *) &len);
            if (error == 0)
            {
                bConnected = true;
            }
        }
        //https://blog.csdn.net/nphyez/article/details/10268723
        /*
         select判断规则：

   1）如果select()返回0，表示在select()超时，超时时间内未能成功建立连接，也可以再次执行select()进行检测，如若多次超时，需返回超时错误给用户。

   2）如果select()返回大于0的值，则说明检测到可读或可写的套接字描述符。源自 Berkeley 的实现有两条与 select 和非阻塞 I/O 相关的规则：

        A) 当连接建立成功时，套接口描述符变成 可写（连接建立时，写缓冲区空闲，所以可写）

        B) 当连接建立出错时，套接口描述符变成 既可读又可写（由于有未决的错误，从而可读又可写）

        因此，当发现套接口描述符可读或可写时，可进一步判断是连接成功还是出错。这里必须将B）和另外一种连接正常的情况区分开，就是连接建立好了之后，
         服务器端发送了数据给客户端，此时select同样会返回非阻塞socket描述符既可读又可写。

        □对于Unix环境，可通过调用getsockopt来检测描述符集合是连接成功还是出错（此为《Unix Network Programming》一书中提供的方法，该方法在Linux环境上测试，发现是无效的）：
         经测试，Android 环境下是有效的

               A)如果连接建立是成功的，则通过getsockopt(sockfd,SOL_SOCKET,SO_ERROR,(char *)&error,&len) 获取的error 值将是0

               B)如果建立连接时遇到错误，则errno 的值是连接错误所对应的errno值，比如ECONNREFUSED，ETIMEDOUT 等

         */

    } else {
        bConnected = true;
    }
    int statusa = 0;

    ul = 0;
    ioctl(socketfd, FIONBIO, &ul); //重新将socket设置成阻塞模式

    int set = 1;

    if (bConnected) {
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
                data->nLen = 0;       //已经断开连接
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
    if ((nRet = (int) recv(socketfd, data->data, (size_t) data->nLen, 0)) != -1) {
        if (nRet <= 0) {
            int errs = errno;
            if (errs == EWOULDBLOCK) { ;
            } else {
                data->nLen = 0;       //已经断开连接
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

//void getNext(int next[])   //lengthP为模式串P的长度
//{
//    int j = 0, k = -1;//j为P串的下标，k用来记录该下标对应的next数组的值
//    next[0] = -1;//初始化0下标下的next数组值为-1
//    while (j < 4) { //对模式串进行扫描
//        if (k == -1 || pat[j] == pat[k]) {//串后缀与前缀没有相等的子串或者此时j下标下的字符与k下的字符相等。
//            j++;
//            k++;
//            next[j] = k;//设置next数组j下标的值为k
//        } else
//            k = next[k];//缩小子串的范围继续比较
//    }
//}
//
//int kmp(int k, int next[], int nTlen, uint8_t *T) {
//    int posP = 0, posT = k;              //posP和posT分别是模式串pat和目标串T的下标，先初始化它们的起始位置
//    //int lengthP= pat.length();    //lengthP是模式串pat长
//    int lengthT = nTlen;// T.length();        //lengthT是目标串T长
//    while (posP < 4 && posT < lengthT)    //对两串扫描
//    {
//        if (posP == -1 || pat[posP] == T[posT]) {                           //对应字符匹配
//            posP++;
//            posT++;
//        } else
//            posP = next[posP];        //失配时，用next数组值选择下一次匹配的位置
//    }
//    if (posP < 4)
//        return -1;
//    else
//        return posT - 4;//匹配成功
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
    if (_nJpgEnd >= 0) {

        for (uint32_t i = 0; i < _nJpgEnd; i++) {
            if (jpg0.buffer[i] == 0xFF && jpg0.buffer[i + 1] == 0xD8) {
                _nJpgStart = i;
                break;
            }
        }

        if (_nJpgStart >= 0) {
            return true;
        } else {
            jpg0.Clear();
            _nJpgStart = -1;
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

void F_SentError(int nError, const char *info);
//extern int nGetData_;



void *MySocket_GKA::ReadData(void *dat) {
    auto *self = (MySocket_GKA *)dat;
    int nRet;

    int Bufferlen = self->buffLen;
    if (!self->bNormalTCP) {
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
    nNexPos = 0;
    int next[4] = {0};
    auto *pbuffer = new uint8_t[500 * 1024];
    //nGetData_ = 0;
    while (self->bNeedRead)
    {
        struct timeval timeoutA = {0, 1000 * 10};
        FD_ZERO(&set); // 在使用之前总是要清空
        FD_SET(self->socketfd, &set); // 把socka放入要测试的描述符集中
        // 检测是否有套接口是否可读+1, &rfd, nullptr, nullptr, &timeoutA);// 检测是否有套接口是否可读
        nRet = select(self->socketfd + 1, &set, nullptr, nullptr,&timeoutA);
        if (nRet <= 0) // 超时
        {
            continue;
        }
        bzero(buffer, Bufferlen);
        if (!(FD_ISSET(self->socketfd, &set))) {
            continue;
        }
        nRet = (int) recv(self->socketfd, buffer, (size_t) Bufferlen, 0);
        if (nRet <= 0)
        {
            LOGE_B("Remote Sockect lose!!!!");
            close(self->socketfd);
            self->socketfd = -1;
            self->host = "";
            self->port = -1;
            self->bConnected = false;
            const char *ds = "Remote tcp  Sockect lose!!!!";
            F_SentError(0xFF, ds);
            break;
        }
        if (nRet > 0) {
            onGetVideoData();
            if (!self->bNormalTCP) {
                F_ResetRelinker();
                if (self->nICType == IC_GPRTPC)
                {
                    if (jpg0.AppendData(buffer, nRet)) {
                        if (F_FindJpg()) {
                            int nLen = jpg0.nCount;
                            int nJpgStart = _nJpgStart;
                            int nJpgEnd = _nJpgEnd;
                            uint8_t *buffer_ = jpg0.buffer;
                            if (self->fuc_getData_mjpeg != nullptr) {
                                self->fuc_getData_mjpeg((char *) (buffer_ + nJpgStart),
                                                        nJpgEnd - nJpgStart + 2);
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
                        self->RevData.nLen = (uint32_t) nRet;
                        memcpy(self->RevData.data, buffer, (size_t) nRet);
                        self->GetData(&self->RevData);

                    } else {
                        mydat.nLen = 0;
                        mydat.AppendData(buffer, (uint32_t) nRet);
                        self->GetData(&mydat);
                    }
                }
            } else {
                if (self->fuc_getData_mjpeg != nullptr) {
                    self->fuc_getData_mjpeg((char *) buffer, nRet);
                }
            }
        }

    }
    delete[]buffer;
    delete[]pbuffer;
    if (self == &GK_tcp_NoticeSocket) {
        LOGE("Exit NoticeSocket  Read Thread!!!");
    } else {
        LOGE("Exit DataSocket  Read Thread!!!");
    }
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
    int ret1 = 0;
    if (readid == -1) {
        bFindHead = true;
        RevData.nLen = 0;
        bNeedRead = true;
        ret = pthread_create(&readid, nullptr, ReadData, (void *) this); // 成功返回0，错误返回错误编号
        LOGE_B("readid1 = %d",(int)readid);
    }
    if (ret != 0) {
        readid = -1;
    }

}