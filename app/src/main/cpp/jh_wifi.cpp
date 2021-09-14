/*
 *
 * 致敬雷霄骅
 * https://blog.csdn.net/leixiaohua1020
 *
 */

#include "MyPlayer.h"

#include "jh_wifi.h"
#include "Defines.h"


#ifdef __cplusplus
extern "C" {
#endif

#include "libavcodec/jni.h"
#include "speex/speex_preprocess.h"
#include "speex/speex_echo.h"
#ifdef __cplusplus
}
#endif


#include "MyMp4.h"




#include <jni.h>

#include "RTL_DownLoad.h"

#include "JPEG_BUFFER.h"
#include "myRTP.h"
#include <pthread.h>
#include "FFMpegPlayerCore.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <cstdlib>
#include <fcntl.h>

#include <ctime>


#include <string>

#include<unistd.h>
#include<arpa/inet.h>


#include <sys/ioctl.h>


#include "MySocket_GKA.h"
#include <algorithm>
#include <cctype>
#include<net/if.h>

#include <vector>
#include <sys/socket.h>
#include <sstream>
#include <sys/system_properties.h>

#include <cstdlib>
#include <cstdio>
#include <cstdarg>

#include <dirent.h>


#include "MySonix.h"
#include "MyDownLoad_GKA.h"
#include "jh_wifi.h"
#include "phone_rl_protocol.h"

#include "MyMediaCoder.h"
#include "JH_TestInfo.h"


#include "GP4225_FileStruct.h"
#include "OnLinePlayer.h"
#include "MyRevBuffer.h"
//#include "XJH264.h"




#include <netinet/tcp.h>
#include <android/bitmap.h>


#include "JH_GetPics.h"

using namespace std;

#define COMMAND_TAG 0x42

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
//#define PACKET_SIZE 1460

MyMediaCoder myMediaCoder;


#define COMMAND_SOCKET_PORT 30863
#define VIDEO_SOCKET_PORT 30865

//#define  GPH264_IP   "192.168.27.1"


//#define  GPRTP_IP   "192.168.27.1"

//#define  GPRTSP_IP   "192.168.26.1"
//#define  GP_IP   "192.168.25.1"

//#define  GKA_IP   "175.16.10.2"


pthread_t rev_Udp_thread = -1;
int rev_udp_socket = -1;



pthread_t rev_Udp_thread20000 = -1;
pthread_t rev_Udp_thread20001 = -1;


int udp_socket20000 = -1;
int udp_socket20001 = -1;

volatile bool bNeedExitReadUDP = false;


pthread_mutex_t mOpenCamera_Lock = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t m_mp4Write_lock = PTHREAD_MUTEX_INITIALIZER;

//XJH264  *xjh264=nullptr;

uint32_t nUdpInx = 0;

std::string sServerIP = "192.168.254.1";



bool bFilterChanged = true;
bool b3D_noise_reduction = false;
//bool b3D_noise_reduction_bak = false;
bool bEnableRotateFilter = false;


int nAdjDelay = 110;

volatile bool bNoTimeout = false;

__volatile int64_t nCheckT_pre;
volatile bool bInit = false;
volatile bool bNeedStop = true;

typedef unsigned char byte;


//volatile  bool bRecord720P = false;

//typedef struct {
//    char str[40];
//    pthread_t Pthread_id;
//} Thumb_threadINFO;


unsigned char *cmd_buffer = nullptr;

int nDelayDisplayT = 0;

int nSdStatus_GP = 0;


int g_sdkVersion = 0;


volatile bool bGesture = false;

JavaVM *gJavaVM;

//static jobject gInterfaceObject;

pthread_mutex_t m_mutex;
C_FFMpegPlayer m_FFMpegPlayer;

//JH_GK_UDP m_JhGK_udp;

MySocket_GKA mysocket;


RTL_DownLoad mRTL_DownLoad;

pthread_mutex_t check_Lock;

jmethodID onProgressGetPictureFromVideo_mid;
jmethodID onUdpRevData_mid;
jmethodID status_mid;
jmethodID key_mid;
jmethodID gp_status_mid;
jmethodID dir_mid;
jmethodID Receive_mid;
jmethodID Gesture_mid;
jmethodID onReadRtlData_mid;
jmethodID G_StartAudio_mid;
jmethodID GetWifiData_mid;
jmethodID GetIP_mid;
jmethodID GetThunb_mid;
jmethodID GetDownLoad_mid;
jmethodID RevTestInfo_mid;
jmethodID Save2ToGallery_mid;
jmethodID F_InitEncoder_mid;
jmethodID offerEncoder_mid;
jmethodID F_CloseEncoder_mid;


jmethodID OnGetAngle_mid;

jmethodID OnPlayIsStarting_Callback_mid;


jmethodID onAdjFocus_mid;


jmethodID onGetVideoData_mid;


jmethodID  onGetLedPwm_mid;
jmethodID  onGetBattery_mid;


jclass objclass;


typedef union {
    struct {
        uint16_t Vcm_Srtc: 2;
        uint16_t Vcm_Step: 2;
        uint16_t Vcm_Current: 10;
        uint16_t Vcm_Flag: 1;
        uint16_t Vcm_PD: 1;
    } b;
    uint16_t VcmAdjVal;
} VCM_FUNC;


VCM_FUNC mVCM_func;


//bool bStoped = false;

uint8_t cmd[20];



//jobject surfaceA = nullptr;

char sCustomer[256];

//int64_t disp_no = -1;
//int64_t start_time = -1;

int getDir_PhotOrVideo = 0;
int getFiles_PhotoPrVideo = 0;

unsigned char *buffer = nullptr;
unsigned char *bufferYUV = nullptr;
int nBufferLen = 0;
//int nBufferLenYUV = 0;
const char *kInterfacePath = "com/joyhonest/wifination/wifination";
const char *fly_classpath = "com/joyhonest/wifination/fly_cmd";

//const char *mediaCodePath = "android/media/MediaCodec";

int PlatformDisplay();

void F_StartReadUdp20000();

int naPlay();

void F_ResetRelinker();

int Init_GKA();

//int localsocket = -1;
//int nStep = -1;

char sPlayPath[256];

//bool bFollow = false;

volatile bool bRocordWHisSeted = false;
volatile bool bGoble_Flip = false;
jbyte TestInfo[1024];

#define  bit0_OnLine            1
#define  bit1_LocalRecording    2
//#define  SD_Ready               4
//#define  SD_Recroding           8
#define  SD_Photo               0x10

#define  Bit_Receive            0x80


volatile bool bSaveByName = false;

typedef enum {
    Status_Connected = 1,
    LocalRecording = 2,
    SD_Ready = 4,
    SD_Recording = 8,
    SD_SNAP = 0x10,
    SSID_CHANGED = 0x20
} SD_STATUS;


//#define   IC_GK         0
//#define   IC_GP         1
//#define   IC_SN         2

//char wlan[250];
int  nWifiInterface = -1;

bool bTCP = false;
int  nPort = -1;

int nICType = IC_NO;
int nDataType = DATA_Type_H264;

float nScal = 1.0f;

uint8_t nSDStatus = 0;
uint8_t nSDstatus_bak = -1;

int F_SendStatus2Jave();

int F_GetSDCardStatus(const char *sUrl);

void F_StartReadSdStatus();

void *F_ReadSdStaturThread(void *dat);

void *F_ReadUdp8001_Thread(void *dat);

void F_SetRelinkerT(int nSec);

void F_StopPlay();

pthread_t nReadStatusThread;
pthread_t nReadUdpThread;

volatile bool bStartCheckSD = false;

volatile bool bStartRead25000 = false;
volatile bool bNeedSentHear = false;
int Read_sock = -1;
int Read_25000sock = -1;
char buff25000[101];

volatile bool bDebug = false;

int SendDir2java(const char *sName);

int F_SD_Stop_Recrod();

unsigned char *bufer = nullptr;

int sockfd = -1;

void DisConnect(bool bNoNormal);

void F_ResetCheckT(int n);

int F_ReadAck(int ms);

int send_cmd_udp(uint8_t *msg, int nLen, const char *host, uint16_t port);

extern int encord_colorformat;

int nCameraDataRota = 0; //从摄像头数据旋转度数。


MySonix m_MySonix;

////GP_H264A

MySocket_GKA GP_tcp_VideoSocket;

//////== gka

MySocket_GKA GK_tcp_SendSocket;
MySocket_GKA GK_tcp_DataSocket;
MySocket_GKA GK_tcp_NoticeSocket;
MySocket_GKA GK_tcp_SearchSocket;


volatile bool bNeedExit = false;

int GPRTP_UDP_SOCKET;
pthread_t GPRTP_rev_thread = -1;
//pthread_t GPRTP_progress_thread=-1;


volatile bool bGKACmd_UDP = true;
volatile bool bGKA_ConnOK = false;


//pthread_t rev_Udp_thread20000 = -1;






//int nFrameCount = 0;
//int nFrameFps = 0;
pthread_t thread_CheckFps = -1;

pthread_mutex_t m_sendudp_lock = PTHREAD_MUTEX_INITIALIZER;


//pthread_cond_t m_checkFrame_cond = PTHREAD_COND_INITIALIZER;
//pthread_mutex_t m_checkFrame_lock = PTHREAD_MUTEX_INITIALIZER;


pthread_mutex_t m_Tcp_Cmd_lock = PTHREAD_MUTEX_INITIALIZER;


pthread_mutex_t m_gl_lock = PTHREAD_MUTEX_INITIALIZER;


int32_t session_id = 0;
volatile bool bIsConnect = false;
pthread_t heartid = -1;
pthread_t checkLinkid = -1;
//bool bCheckLink = false;

T_REQ_MSG req_msg;


int nPhotoW = 640;
int nPhotoH = 360;
bool bPhotoSet = false;  //  如果 为 true 表示拍照时用  nPhotoW   nPhotoH




uint32_t nRecFrameCount = 0;
volatile bool bRealRecording = false;

//pthread_mutex_t m_adjRecTime_lock = PTHREAD_MUTEX_INITIALIZER;


//JNIEXPORT jint JNICALL
//Java_com_joyhonest_wifination_wifination_naGetRecordTime(JNIEnv *env, jclass type) {
//
//    // TODO
//    int32_t ret = 0;
//    float df = 1000.0f / m_FFMpegPlayer.nRecFps;
//    pthread_mutex_lock(&m_adjRecTime_lock);
//    ret = (int32_t) (nRecFrameCount * df);
//    pthread_mutex_unlock(&m_adjRecTime_lock);
//    return (jint) ret;
//}


int send_cmd_tcp(uint8_t *msg, int nLen, char *host, uint16_t port) {
    //if (!mysocket.bConnected)
//    {
//        mysocket.Connect(host, port);
//    }
//
//    if (mysocket.bConnected)
//    {
//        MySocketData data;
//        data.AppendData(msg, nLen);
//        mysocket.Write(&data);
//        mysocket.DisConnect();
//        LOGE("socket closed!!!!");
//    }

    MySocket_GKA mysocketA;
    mysocketA.Connect(host, port);
    if (mysocketA.bConnected) {
        MySocketData data;
        data.AppendData(msg, nLen);
        mysocketA.Write(&data);
        mysocketA.DisConnect();
    }

    return 0;
}


void F_CloseReadSocket() {
    if (Read_sock != -1) {
        close(Read_sock);
        Read_sock = -1;
        LOGE("Close ReadSocket");
    }
}


int send_cmd_udp(uint8_t *msg, int nLen, const char *host, uint16_t port) {
    if (msg == nullptr)
        return -1;

    pthread_mutex_lock(&m_sendudp_lock);
    int nRet = 0;


    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(host);
    server_addr.sin_port = htons(port);


    struct sockaddr_in loc_addr;//本机地址

    bzero(&loc_addr, sizeof(loc_addr));

    loc_addr.sin_family = AF_INET;//协议
    //loc_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    //in_addr_t  da =
    loc_addr.sin_addr.s_addr =  inet_addr(sServerIP.c_str());  // htons(INADDR_ANY);
    loc_addr.sin_port = htons(10901);


    /* 创建socket */
    int res = 0;
    int cmd_udp_socket = -1;
    if (cmd_udp_socket < 0)
        cmd_udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (cmd_udp_socket < 0) {
        LOGE("Create Socket Failed_a:");
        nRet = -1;
        goto ret1;
    }

    if(nWifiInterface!=-1) {
        setsockopt(cmd_udp_socket, IPPROTO_IP, SO_BINDTODEVICE, &nWifiInterface,
                          sizeof(nWifiInterface));
    }

    if ((res = sendto(cmd_udp_socket, msg, (size_t) nLen, 0, (struct sockaddr *) &server_addr,
               sizeof(server_addr))) < 0)
    {
        close(cmd_udp_socket);
        cmd_udp_socket = -1;
        nRet = -1;
        goto ret1;
    }
    close(cmd_udp_socket);
    ret1:
    pthread_mutex_unlock(&m_sendudp_lock);
    return nRet;

}


int split(const string &str, vector<string> &ret_, string sep = ",") {
    if (str.empty()) {
        return 0;
    }
    string tmp;
    string::size_type pos_begin = str.find_first_not_of(sep);
    string::size_type comma_pos = 0;

    while (pos_begin != string::npos) {
        comma_pos = str.find(sep, pos_begin);
        if (comma_pos != string::npos) {
            tmp = str.substr(pos_begin, comma_pos - pos_begin);
            pos_begin = comma_pos + sep.length();
        } else {
            tmp = str.substr(pos_begin);
            pos_begin = comma_pos;
        }

        if (!tmp.empty()) {
            ret_.push_back(tmp);
            tmp.clear();
        }
    }
    return 0;
}

void F_StopReadSdStatus() {
    if (nICType == IC_GK) {

        if (bStartCheckSD) {
            bStartCheckSD = false;
            void *ret = nullptr;
            pthread_join(nReadStatusThread, &ret);
        }
    }
}


void F_StartReadSdStatus() {
    nSDstatus_bak = 0;
    if (bStartCheckSD) {
        F_SendStatus2Jave();
        return;
    }
    if (nICType == IC_GK) {
        bStartCheckSD = true;
        pthread_create(&nReadStatusThread, nullptr, F_ReadSdStaturThread, &nReadStatusThread);
        pthread_create(&nReadUdpThread, nullptr, F_ReadUdp8001_Thread, &nReadUdpThread);
        F_SendStatus2Jave();
    }
    if (nICType == IC_GP) {

    }
}


void *F_ReadUdp8001_Thread(void *dat) {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8001);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    F_CloseReadSocket();

    if ((Read_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) < 0)) {
        LOGE("open udp:8001 error");
        return nullptr;
    }

    if(nWifiInterface!=-1) {
        setsockopt(Read_sock, SOL_SOCKET, SO_BINDTODEVICE, &nWifiInterface, sizeof(nWifiInterface));
    }

    int res = (int) (::bind(Read_sock, (struct sockaddr *) &addr, sizeof(addr)));
    if (res < 0) {
        LOGE("bind udp:8001 error");
        return nullptr;
    }

    char buff[101];
    struct sockaddr_in clientAddr;
    int n;
    int len = sizeof(clientAddr);

    struct timeval tv_out;
    fd_set readset;
    int ret = -1;
    int error = -1;
    while (bStartCheckSD) {
        memset(buff, 0, 101 * sizeof(char));
        {
            unsigned long ul = 1;
            ioctl(Read_sock, FIONBIO, &ul); //设置为非阻塞模式
            ret = -1;
            tv_out.tv_sec = 0;//TIMEOUT_TIME;
            tv_out.tv_usec = 50 * 1000;
            FD_ZERO(&readset);
            FD_SET(Read_sock, &readset);
            n = 0;
#if 1
            {
                ret = select(Read_sock + 1, &readset, nullptr, nullptr, &tv_out);
                if (ret == 0)              //返回0，代表在描述词状态改变已超过timeout时间
                {
                    getsockopt(Read_sock, SOL_SOCKET, SO_ERROR, &error, (socklen_t *) &len);
                    if (error == 0)          // 超时，可以做更进一步的处理，如重试等
                    {
                        //bTimeoutFlag = 1;
                        //LOGE("Connect timeout!\n");
                    } else {
                        //LOGE("Cann't connect to server!\n");
                    }
                    //close(sock);
                    ret = -4;
                } else if (ret == -1)      // 返回-1， 有错误发生，错误原因存在于errno
                {
                    LOGE("Cann't connect to server!\n");
                    //close(sock);
                    ret = -4;
                } else                      // 成功，返回描述词状态已改变的个数
                {
                    // LOGE("Connect success!\n");
                    if (FD_ISSET(Read_sock, &readset)) {
                        socklen_t td;
                        n = (int) recvfrom(Read_sock, buff, (size_t) 100, 0,
                                           (struct sockaddr *) &clientAddr, (socklen_t *) &td);
                    }
                    ret = 1;

                }
            }
#endif

            if (n > 0) {

                buff[n] = 0;
                char *df = &buff[12];
                string s = df;
                transform(s.begin(), s.end(), s.begin(), ::toupper);
                if (s == "SNAP") {
                    nSDStatus |= SD_Photo;
                    F_SendStatus2Jave();
                    nSDStatus &= (SD_Photo ^ 0xFFFF);
                }
            }
        }
    }
    F_CloseReadSocket();

    return nullptr;
}

void *F_ReadSdStaturThread(void *dat) {
    int i = 0;
    while (bStartCheckSD) {
        if ((i & 0x07) == 0) {
            if (nICType == IC_GK) {
                const char *str = "http://192.168.234.1/web/cgi-bin/hi3510/getsdcareInfo.cgi?";
                F_GetSDCardStatus(str);
            }
        }
        i++;
        usleep(1000 * 125);     //125ms
    }
    bStartCheckSD = false;
    return nullptr;
}

int F_GetSDCardStatus(const char *sUrl) {

    const char *host = "192.168.234.1";
    int port = 80;
    int socket_handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_handle < 0) {
        LOGE("error:1");
        return -1;
    }


    if(nWifiInterface!=-1) {
        setsockopt(socket_handle, IPPROTO_IP, SO_BINDTODEVICE, &nWifiInterface,
                          sizeof(nWifiInterface));
    }

    //  setsockopt(socket_handle, SOL_SOCKET, SO_BINDTODEVICE, wlan, strlen(wlan));
    struct sockaddr_in loc_addr;//本机地址
    loc_addr.sin_family = AF_INET;//协议
    loc_addr.sin_addr.s_addr = htons(INADDR_ANY);
    loc_addr.sin_port = htons(INADDR_ANY);

    int res = (int) ::bind(socket_handle, (const struct sockaddr *) &loc_addr,
                           sizeof(struct sockaddr_in));
    if (res < 0) {
        close(socket_handle);
        LOGE("error 2:");
        return -1;
    }
    struct sockaddr_in serv_add;//服务器地址
    serv_add.sin_family = AF_INET;
    serv_add.sin_addr.s_addr = inet_addr(host);
    serv_add.sin_port = htons(80);
    //if(connect(socket_handle,(const struct sockaddr*)&serv_add,sizeof(struct sockaddr_in))<0){
    //    close(socket_handle);
    //    LOGE("error 3:");
    //    return -1;
    // }

    int ret = -1;
    int error = -1;
    int len = 0;
    struct timeval timeout;
    fd_set readset, writeset;
    unsigned long ul = 1;
    ioctl(socket_handle, FIONBIO, &ul); //设置为fei阻塞模式

    bool  bConnected = false;

    if (connect(socket_handle, (const struct sockaddr *) &serv_add, sizeof(struct sockaddr_in)) ==
        -1) {
        timeout.tv_sec = 2;//TIMEOUT_TIME;
        timeout.tv_usec = 0;
        FD_ZERO(&writeset);
        FD_SET(socket_handle, &writeset);
        ret = select(socket_handle + 1, nullptr, &writeset, nullptr, &timeout);
        if(ret>0)
        {
            getsockopt(socket_handle, SOL_SOCKET, SO_ERROR, &error, (socklen_t *) &len);
            if (error == 0)
            {
                bConnected = true;
            }
        }
        //https://blog.csdn.net/nphyez/article/details/10268723

    } else {
        bConnected = true;

    }
    ul = 0;
    ioctl(socket_handle, FIONBIO, &ul); //设置为阻塞模式
    if(!bConnected)
        return 0;

    string sHead = "GET ";
    sHead += sUrl;
    sHead += " HTTP/1.1\r\nHost:192.168.234.1:80\r\n\r\n";
    char *head = (char *) sHead.c_str();//"GET http://192.168.0.66/HttpServer/GetTest.php?name=lang&age=20 HTTP/1.1\r\nHost:192.168.0.66:80\r\n\r\n";

    struct timeval timeoutA = {3, 0};
    //设置发送超时
    setsockopt(socket_handle, SOL_SOCKET, SO_SNDTIMEO, (char *) &timeoutA, sizeof(struct timeval));
//设置接收超时
    setsockopt(socket_handle, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeoutA, sizeof(struct timeval));

    if (send(socket_handle, head, strlen(head), 0) < 0) { //发送头部
        close(socket_handle);
        LOGE("error 4:");
        return -1;
    }
    char *result = (char *) malloc(sizeof(char));
    char *temp_result = (char *) malloc(sizeof(char));;
    size_t SIZE = sizeof(char) * 1024;
    char *cache = (char *) malloc(SIZE);
    //int len = 0;
    memset(result, 0x00, sizeof(char));
    memset(temp_result, 0x00, sizeof(char));
    memset(cache, 0x00, SIZE);
    const char *strA;
    while ((len = (int) recv(socket_handle, cache, SIZE, 0)) > 0) {
        vector<string> vector1;
        vector<string>::iterator it;
        split(cache, vector1, "\r\n");
        string sdStatus;
        sdStatus.clear();
        int ix = 0;
        if (vector1.size() > 2) {
            sdStatus.clear();
            sdStatus = vector1[2];
        }

        if (sdStatus.size() > 0) {
            sdStatus.erase(sdStatus.begin(), std::find_if(sdStatus.begin(), sdStatus.end(),
                                                          std::not1(std::ptr_fun<int, int>(
                                                                  std::isspace))));
            sdStatus.erase(std::find_if(sdStatus.rbegin(), sdStatus.rend(),
                                        std::not1(std::ptr_fun<int, int>(std::isspace))).base(),
                           sdStatus.end());
            strA = sdStatus.c_str();

            if (sdStatus == "sdstatus=\"Ready\";") {
                nSDStatus |= SD_Ready;
                nSDStatus &= (SD_Recording ^ 0xFF);
                LOGE("SD_Ready");
            } else if (sdStatus == "var rec=ok;" || sdStatus == "var rec=off;" ||
                       sdStatus == "var rec=on;" || sdStatus == "sdstatus=\"Recing\";") {
                nSDStatus |= SD_Recording;
                nSDStatus |= SD_Ready;
                LOGE("SD_Ready");
            } else {
                nSDStatus &= (SD_Ready ^ 0xFF);
                nSDStatus &= (SD_Recording ^ 0xFF);
            }
            if (nSDstatus_bak != nSDStatus) {
                F_SendStatus2Jave();
                nSDstatus_bak = nSDStatus;
            }
        }
    }
    if (result != nullptr)
        free(result);
    if (temp_result != nullptr)
        free(temp_result);
    if (cache != nullptr)
        free(cache);
    close(socket_handle);

    return 0;

}


pthread_t checkRelinker = -1;

int naStop(void);

int _naInit_(const char *ps);


int nRelinker_T = 6 * 1000;

void F_SetRelinkerT(int nMs) {

    nRelinker_T = nMs;
}

void F_StopRecordAll(void);

int naInit_Re(void);


int naStop_B(void);

int naInit_Re_B(void);

void *checkRelinkerThrad(void *param) {
    bool re = true;
    pthread_detach(pthread_self());
    while (re) {
#if 0
        if (!bNoTimeout) {
            if (bInit && !bNeedStop) {
                int64_t current = av_gettime() / 1000;
                int dat = (int) (current - nCheckT_pre);
                if (nRelinker_T != 0) {
                    if (dat > (int) nRelinker_T) {
                        bInit = false;
                        bRealRecording = false;
                        naStop_B();
                        usleep(1000 * 500);
                        if (!bNeedStop) {
                            LOGE_B("Relinker!!!!  %d  dat = %d", (int) nRelinker_T, dat);
                            naInit_Re_B();
                            nCheckT_pre = av_gettime() / 1000;
                        }
                    }
                }
            }
        }
#endif
        usleep(1000 * 10);
    }
    return nullptr;
}

void F_SetartCheckRelinker(void) {
    pthread_create(&checkRelinker, nullptr, checkRelinkerThrad, nullptr);
};


extern AVFrame *gl_Frame;

AVFrame *gl_Frame1;

int nGSInx = 0;

int32_t GX[100];
int32_t GY[100];
int32_t GZ[100];

//int32_t GX_[100];
//int32_t GY_[100];
//int32_t GZ_[100];


volatile bool bNeedRead20000 = false;
volatile bool bNeedRead20001 = false;

void F_InitGSensorData() {
    nGSInx = 0;
    memset(GX, 0, 20);
    memset(GY, 0, 20);
    memset(GZ, 0, 20);
}

void F_StartReadUdp20001();


void JNI_OnUnload(JavaVM *vm, void *reserved) {

    bNeedRead20000 = false;
    bNeedRead20001 = false;
}

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    gJavaVM = vm;
    b3D_noise_reduction = true;
    //b3D_noise_reduction_bak = true;

    int xx = av_jni_set_java_vm(vm, nullptr);      //（位于libavcodec/jni.h）


    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        DEBUG_PRINT("Failed to get the environment using GetEnv()");
        return -1;
    }
    bNoTimeout = false;


    memset(TestInfo, 0, 1024);
    char m_szSdkVer[PROP_VALUE_MAX];
    __system_property_get("ro.build.version.sdk", m_szSdkVer);
    g_sdkVersion = atoi(m_szSdkVer); //21  5.0
    g_sdkVersion = 21;


    pthread_mutex_init(&check_Lock, nullptr);
    F_SetartCheckRelinker();
    memset(sCustomer, 0, 256);
//    jclass mediaCode_classa = env->FindClass(mediaCodePath);
//    mediaCode_Class = (jclass) (env->NewGlobalRef(mediaCode_classa));

    jclass data_Clazz = env->FindClass(kInterfacePath);
    if (data_Clazz != nullptr) {

        objclass = (jclass) (env->NewGlobalRef(data_Clazz));
        Save2ToGallery_mid = env->GetStaticMethodID(data_Clazz, "OnSave2ToGallery",
                                                    "(Ljava/lang/String;I)V");
        if (Save2ToGallery_mid == nullptr) {
            DEBUG_PRINT("No Save2ToGallery_mid");
        }

        onGetVideoData_mid = env->GetStaticMethodID(data_Clazz, "onGetVideoData","()V");


        //G_getIP = env->GetStaticMethodID(data_Clazz, "G_getIP", "()I");

        onAdjFocus_mid = env->GetStaticMethodID(data_Clazz, "onAdjFocus", "(I)V");


        OnPlayIsStarting_Callback_mid = env->GetStaticMethodID(data_Clazz,
                                                               "OnPlayIsStarting_Callback", "(I)V");


        OnGetAngle_mid = env->GetStaticMethodID(data_Clazz, "OnGetAngle", "(F)V");

        onReadRtlData_mid = env->GetStaticMethodID(data_Clazz, "onReadRtlData", "([B)V");

        F_InitEncoder_mid = env->GetStaticMethodID(data_Clazz, "F_InitEncoder", "(IIII)I");
        offerEncoder_mid = env->GetStaticMethodID(data_Clazz, "offerEncoder", "([BI)V");
        F_CloseEncoder_mid = env->GetStaticMethodID(data_Clazz, "F_CloseEncoder", "()V");


        onGetLedPwm_mid = env->GetStaticMethodID(data_Clazz, "onGetLedPwm", "(I)V");
        onGetBattery_mid = env->GetStaticMethodID(data_Clazz, "onGetBattery", "(I)V");
        RevTestInfo_mid = env->GetStaticMethodID(data_Clazz, "RevTestInfo", "([B)V");
        if (RevTestInfo_mid == nullptr) {
            DEBUG_PRINT("No RevTestInfo_mid");
        }

        GetIP_mid = env->GetStaticMethodID(data_Clazz, "getIP", "()I");


        GetWifiData_mid = env->GetStaticMethodID(data_Clazz, "OnGetWifiData", "([B)V");
        if (GetWifiData_mid == nullptr) {
            DEBUG_PRINT("No OnGetWifiData");
            DEBUG_PRINT("No OnGetWifiData");
        }
        G_StartAudio_mid = env->GetStaticMethodID(data_Clazz, "G_StartAudio", "(I)Z");


        status_mid = env->GetStaticMethodID(data_Clazz, "OnStatusChamnge", "(I)V");
        if (status_mid == nullptr) {
            DEBUG_PRINT("4");
            //return -8000;
        }

        dir_mid = env->GetStaticMethodID(data_Clazz, "GetFiles", "([B)V");
        if (dir_mid == nullptr) {
            DEBUG_PRINT("6");
        }

        Receive_mid = env->GetStaticMethodID(data_Clazz, "ReceiveBmp", "(I)V");
        if (Receive_mid == nullptr) {
            DEBUG_PRINT("8");
        }

        Gesture_mid = env->GetStaticMethodID(data_Clazz, "GestureBmp", "(I)V");


        GetThunb_mid = env->GetStaticMethodID(data_Clazz, "GetThumb", "([BLjava/lang/String;)V");
        if (GetThunb_mid == nullptr) {
            DEBUG_PRINT("10");
        }

        GetDownLoad_mid = env->GetStaticMethodID(data_Clazz, "DownloadFile_callback",
                                                 "(ILjava/lang/String;I)V");
        if (GetDownLoad_mid == nullptr) {
            DEBUG_PRINT("12");
        }

        key_mid = env->GetStaticMethodID(data_Clazz, "OnKeyPress", "(I)V");
        if (key_mid == nullptr)
        {
            DEBUG_PRINT("14");
        }


        onProgressGetPictureFromVideo_mid = env->GetStaticMethodID(data_Clazz, "onProgressGetPictureFromVideo", "([BI)V");

        onUdpRevData_mid = env->GetStaticMethodID(data_Clazz, "onUdpRevData", "([BI)V");
        if (onUdpRevData_mid == nullptr) {
            DEBUG_PRINT("15");
        }


        gp_status_mid = nullptr;

        gp_status_mid = env->GetStaticMethodID(data_Clazz, "OnGetGP_Status", "(I)V");
        if (gp_status_mid == nullptr) {
            DEBUG_PRINT("16");
        }

        if (gl_Frame == nullptr) {
            gl_Frame = av_frame_alloc();
            gl_Frame->format = AV_PIX_FMT_YUV420P;
            gl_Frame->width = 1920;
            gl_Frame->height = 1080;
            av_image_alloc(
                    gl_Frame->data, gl_Frame->linesize, gl_Frame->width,
                    gl_Frame->height,
                    AV_PIX_FMT_YUV420P, aligD);
        }
    }
//    bNeedRead20000 = true;
//    bNeedRead20001 = true;
//    F_StartReadUdp20000();
//    F_StartReadUdp20001();


    return JNI_VERSION_1_6;
}

JNIEnv *getJNIEnv(int *needsDetach);

//int64_t  nGetData_ = 0;
int64_t   nPPPReT = -1;
void onGetVideoData(void)
{
    if(nPPPReT==-1)
    {
        nPPPReT = av_gettime()/1000;
    }
    int64_t nRt = av_gettime()/1000;

    if(nRt-nPPPReT>=50) {
        nPPPReT = nRt;
        {
            if (onGetVideoData_mid != nullptr) {
                int needsDetach = 0;
                JNIEnv *evn = getJNIEnv(&needsDetach);
                if (evn == nullptr) {
                    return;
                }
                evn->CallStaticVoidMethod(objclass, onGetVideoData_mid);
                if (needsDetach)
                    gJavaVM->DetachCurrentThread();
            }
        }
    }
}

int nGKA_StreamNo = 2;
bool bAdjFps = true;


void F_GetData_SN(char *data, int nLen);

void F_GetData_SNA(char *data, int nLen);

int F_InitSN();

int Connect_gk(void);

bool F_GetResolution_A(void);


/*
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naGetMenuFile(JNIEnv *env, jclass type, jint nLanguage) {




}
*/




int F_GetIpfor4Bytes(int n1, int n2, int n3) {

    //   return (i & 0xFF) + "." + ((i >> 8) & 0xFF) + "." + ((i >> 16) & 0xFF) + "." + ((i >> 24) & 0xFF);
    int ip = 0;
    ip += n1;
    ip += n2 * 0x100;
    ip += n3 * 0x10000;
    return ip;

}

int naInit_Re(void);


//FILE *testFile = nullptr;

int64_t nPreTimeA = 0;

int32_t F_GetIP(void);


void F_AdjIcType(int type)
{
    if (type == F_GetIpfor4Bytes(192, 168, 234)) {
        nICType = IC_GK;
        sServerIP = "192.168.234.1";
        nDataType = DATA_Type_H264;
    } else if (type == F_GetIpfor4Bytes(192, 168, 123)) {
        nICType = IC_SN;
        sServerIP = "192.168.123.1";
        nDataType = DATA_Type_MJPEG;
    } else if (type == F_GetIpfor4Bytes(175, 16, 10)) {
        nICType = IC_GKA;
        sServerIP = "175.16.10.2";
        nDataType = DATA_Type_H264;
    } else if (type == F_GetIpfor4Bytes(192, 168, 25)) {
        nICType = IC_GP;
        sServerIP = "192.168.25.1";
        nDataType = DATA_Type_MJPEG;
    } else if (type == F_GetIpfor4Bytes(192, 168, 26)) {
        nICType = IC_GPRTSP;
        sServerIP = "192.168.26.1";
        nDataType = DATA_Type_MJPEG;
    } else if (type == F_GetIpfor4Bytes(192, 168, 27)) {
        nICType = IC_GPH264;
        sServerIP = "192.168.27.1";
        nDataType = DATA_Type_H264;
    } else if (type == F_GetIpfor4Bytes(192, 168, 28)) {
        nICType = IC_GPRTP;
        sServerIP = "192.168.28.1";
        nDataType = DATA_Type_MJPEG;
    } else if (type == F_GetIpfor4Bytes(192, 168, 29)) {
        nICType = IC_GPRTPB;
        sServerIP = "192.168.29.1";
        nDataType = DATA_Type_MJPEG;
    } else if (type == F_GetIpfor4Bytes(192, 168, 30)) {
        nICType = IC_GPH264A;
        sServerIP = "192.168.30.1";
        nDataType = DATA_Type_H264;
    } else if (type == F_GetIpfor4Bytes(192, 168, 31)) {
        nICType = IC_GPRTPC;
        sServerIP = "192.168.31.1";
        nDataType = DATA_Type_MJPEG;
    } else if (type == F_GetIpfor4Bytes(192, 168, 32)) {
        nICType = IC_RTLH264;
        sServerIP = "192.168.32.1";
        nDataType = DATA_Type_H264;
    } else if (type == F_GetIpfor4Bytes(192, 168, 33)) {
        nICType = IC_GPRTPB;
        sServerIP = "192.168.33.1";         //图传用29，但支持SD卡
        nDataType = DATA_Type_MJPEG;
    } else if (type == F_GetIpfor4Bytes(192, 168, 34)) {
        nICType = IC_GPH264_34;
        sServerIP = "192.168.34.1";         //通33，   但图传为H264格式
        nDataType = DATA_Type_H264;
    }
    else {
        nICType = IC_NO;
        sServerIP = "127.0.0.1";
    }

}




void F_SetnRTL_RTC_Time() {
    int len = 0;

    char sHead[] = "AMEBA:DISCOVER:";
    uint8_t data[1000];
    memset(data, 0, 1000);

    len = (int) strlen(sHead);
    memcpy(data, sHead, len);

    int64_t time = av_gettime() / 1000;
    sprintf((char *) (data + len), "%lld", (long long) time); //将100转为16进制表示的字符串。
    len = (int) strlen((char *) data);
    len += 1;
    char *pData = (char *) data;
    LOGE("%s", pData);
    send_cmd_udp(data, len, sServerIP.c_str(), 49152);

}

int F_GetIPByJava(void);


void F_GetServerIP(void) {
    //if(nICType != IC_NO)  //已经获取到了 IP信息
    //    return;

    int32_t ip = F_GetIP();
    if (ip <= 0) {
        ip = (int32_t) F_GetIPByJava();
        F_AdjIcType(ip & 0x00FFFFFF);
    }


}




void deleteAllMark(std::string &s, const std::string &mark) {
    size_t nSize = mark.size();
    while (1) {
        size_t pos = s.find(mark);    //  尤其是这里
        if (pos == string::npos) {
            return;
        }

        s.erase(pos, nSize);
    }
}


int _naInit_(const char *pFileName) {

    // memset(wlan, 0, sizeof(wlan));
    nPreTimeA = 0;
    GPRTP_UDP_SOCKET = -1;
    std::string::size_type idx;
    nUdpInx = 0;
    memset(sPlayPath, 0, sizeof(char) * 256);
    bInit = false;
    std::string sServerIP1="";
    if (pFileName == nullptr) {
        nCheckT_pre = av_gettime() / 1000;
        F_SetRelinkerT(500);
        bInit = true;
        //return -1;
    }
    sServerIP1 = pFileName;
    deleteAllMark(sServerIP1, " "); //清除空格

    int nPathLen = (int) sServerIP1.length();

    if (nPathLen > 256) {
        nCheckT_pre = av_gettime() / 1000;
        F_SetRelinkerT(500);
        bInit = true;
        return -1;
    }
    memset(sPlayPath, 0, 256);

    if (nPathLen != 0) {
        memcpy(sPlayPath, sServerIP1.c_str(), (size_t) nPathLen);
    }

    transform(sServerIP1.begin(), sServerIP1.end(), sServerIP1.begin(), ::tolower);


//IC_GK = 0,      //192.168.234.X
//IC_SN,          //192.168.123.X
//IC_GKA,             //175.16.10.X
//IC_GP,          //192.168.25.X
//IC_GPRTSP,   //192.168.26.X

//IC_GPH264,   //192.168.27.X
//IC_GPRTP,    //192.168.28.X
//IC_GPH264A,   //192.168.30.X
//IC_GPRTPB,   //192.168.29.X

//    int ip = F_GetIPByJava();
//    if(ip==0) {
//        F_GetIP();
//    } else
//    {
//        F_AdjIcType(ip&0x00FFFFFF);
//
//    }


    nICType = IC_NO;
    F_GetServerIP();
    if (sPlayPath[0] == '/') {
        nICType = IC_FILES;
        nDataType = DATA_Type_NO;
    }

    if (nICType == IC_NO) {
        LOGE("No Model!");
        return -100;
    }
    if (nICType == IC_GPH264)// && sServerIP1.length()<10)
    {
        sServerIP1 = "rtsp://192.168.27.1:8080/?action=stream";
        memset(sPlayPath, 0, 256);
        nPathLen = (int) sServerIP1.length();
        memcpy(sPlayPath, sServerIP1.c_str(), (size_t) nPathLen);
    }


    if (sServerIP == "192.168.25.1" || sServerIP == "192.168.26.1") {
        idx = sServerIP1.find("rtsp://192.168.26.1");
        if (idx != std::string::npos) {
            nICType = IC_GPRTSP;
            sServerIP = "192.168.26.1";
        }

        idx = sServerIP1.find("rtsp://192.168.25.1");
        if (idx != std::string::npos) {
            nICType = IC_GPRTSP;
            sServerIP = "192.168.25.1";
        }

        idx = sServerIP1.find("http://192.168.26.1");
        if (idx != std::string::npos) {
            nICType = IC_GP;
            sServerIP = "192.168.26.1";
        }

        idx = sServerIP1.find("http://192.168.25.1");
        if (idx != std::string::npos) {
            nICType = IC_GP;
            sServerIP = "192.168.25.1";
        }
    }

    m_FFMpegPlayer.nIC_Type = (uint8_t) nICType;
    nSDStatus = 0;
    bInit = false;
    nCheckT_pre = av_gettime() / 1000;
    F_SetRelinkerT(1000 * 4);
    return naInit_Re();

}


void *doCheckFrameFps(void *v) {
//    int xx = 0;
//    //struct timespec timespec1;
//    LOGE("Start Check DispFps");
//    while (!bNeedExit) {
//        xx++;
//        if (xx >= 49)  //1Sec
//        {
//            xx = 0;
//            pthread_mutex_lock(&m_checkFrame_lock);/*锁住互斥量*/
//            nFrameFps = nFrameCount;
//            nFrameCount = 0;
//            pthread_mutex_unlock(&m_checkFrame_lock);/*解锁互斥量*/
//        }
//        usleep(1000 * 20);
//    }
//    nFrameCount = 0;
//    nFrameFps = 0;
//    thread_CheckFps = -1;
//    LOGE("Exit Check DispFps");
    return nullptr;
}


void F_StartheckFps(void)  //计算每秒收到的帧数
{
//    nFrameCount = 0;
//    nFrameFps = 0;
//    {
//        thread_CheckFps = pthread_create(&thread_CheckFps, nullptr, doCheckFrameFps,
//                                         (void *) nullptr); // 成功返回0，错误返回错误编号
//    }


}

void F_Rec_RTP_Data_Service();

int Connect_GPH264(void);


//bool bInitMedia = false;
bool bInitMediaA = false;


void F_SentRTPStop();

int naInit_Re_B(void) {
    int ret = 0;
    uint8_t msg[20];
    bNeedExit = false;


/*
    myMediaCoder.F_CloseDecoder();
    if (udp_socket20000 > 0 || rev_Udp_thread20000 != -1) {
        if (udp_socket20000 > 0) {
            close(udp_socket20000);
            udp_socket20000 = -1;
        }
        rev_Udp_thread20000 = -1;
    }
    if (GPRTP_UDP_SOCKET > 0 || GPRTP_rev_thread != -1) {
        if (GPRTP_UDP_SOCKET > 0) {
            close(GPRTP_UDP_SOCKET);
            GPRTP_UDP_SOCKET = -1;
        }
        GPRTP_rev_thread = -1;
    }
*/

    //F_StartReadUdp20000();
    if (nICType == IC_GPRTPC) {
        // bStoped = true;
        bInit = true;
        if (GP_tcp_VideoSocket.bConnected) {
            return -1;
        }

        nCheckT_pre = av_gettime() / 1000;
        F_SetRelinkerT(1000 * 5);
        m_FFMpegPlayer.nfps = 20;
        m_FFMpegPlayer.nDisplayHeight = 480;  //

        F_SentRTPStop();
        usleep(1000 * 30);
        F_SentRTPStop();
        usleep(1000 * 30);
        F_SentRTPStop();
        usleep(1000 * 20);

        //F_Rec_RTP_Data_Service();

        msg[0] = 'J';
        msg[1] = 'H';
        msg[2] = 'C';
        msg[3] = 'M';
        msg[4] = 'D';
        msg[5] = 0x10;
        msg[6] = 0x00;
        send_cmd_udp(msg, 7, sServerIP.c_str(), 20000);
        usleep(1000 * 25);
        msg[0] = 'J';
        msg[1] = 'H';
        msg[2] = 'C';
        msg[3] = 'M';
        msg[4] = 'D';
        msg[5] = 0x20;
        msg[6] = 0x00;
        msg[7] = 0x00;
        msg[8] = 0x00;
        msg[9] = 0x00;
        msg[10] = 0x00;

        send_cmd_udp(msg, 11, sServerIP.c_str(), 20000);
        usleep(1000 * 10);

        msg[0] = 'J';
        msg[1] = 'H';
        msg[2] = 'C';
        msg[3] = 'M';
        msg[4] = 'D';
        msg[5] = 0xD0;
        msg[6] = 0x01;
        send_cmd_udp(msg, 7, sServerIP.c_str(), 20000);
        usleep(1000 * 15);
        msg[0] = 'J';
        msg[1] = 'H';
        msg[2] = 'C';
        msg[3] = 'M';
        msg[4] = 'D';
        msg[5] = 0xD0;
        msg[6] = 0x01;
        send_cmd_udp(msg, 7, sServerIP.c_str(), 20000);
        usleep(1000 * 10);

        if (Connect_GPH264() < 0) {
            ret = -1;
        } else
            bInit = true;
        return 0;


    }
    if (nICType == IC_GPH264A || nICType == IC_RTLH264 || nICType == IC_GPH264_34) {

        // bStoped = true;
        bInit = true;
        if (GP_tcp_VideoSocket.bConnected) {
            return -1;
        }
        m_FFMpegPlayer.nErrorFrame = 0;
        m_FFMpegPlayer.nfps = 20;
        if (Connect_GPH264() < 0) {
            ret = -1;
        }
        return ret;
    } else if (nICType == IC_GKA) {

        // bStoped = true;
        bInit = true;
        int ret = 0;
        if (GK_tcp_SendSocket.bConnected) {

            DEBUG_PRINT("1-----");
            return ret;
        }
        if (sPlayPath[0] == '1') {
            m_FFMpegPlayer.nDisplayWidth = 640;//1280;
            m_FFMpegPlayer.nDisplayHeight = 360;//720;
            nGKA_StreamNo = 1;
        } else {
            m_FFMpegPlayer.nDisplayWidth = 640;
            m_FFMpegPlayer.nDisplayHeight = 360;
            nGKA_StreamNo = 2;
        }
        F_ResetRelinker();
        F_SetRelinkerT(5000);
        ret = 0;
        if (Connect_gk() < 0) {
            F_SetRelinkerT(100);
            ret = -1;
        }
        if (ret >= 0) {
            // F_RecRP_RTSP_Status_Service();  //GKA
        }
        return ret;

    } else if (nICType == IC_SN) {
        m_FFMpegPlayer.nfps = 25;
        if (m_MySonix.createCommandSocket() == 0) {
            m_MySonix.sendStop();
            bool re;

            re = F_GetResolution_A();

            if (re) {
                m_FFMpegPlayer.nDisplayWidth = 640;
                m_FFMpegPlayer.nDisplayHeight = 480;
                m_FFMpegPlayer.b480 = true;
            } else {
                re = F_GetResolution_A();
                if (re) {
                    m_FFMpegPlayer.nDisplayWidth = 640;
                    m_FFMpegPlayer.nDisplayHeight = 480;
                    m_FFMpegPlayer.b480 = true;
                } else {
                    re = m_MySonix.F_GetResolution();
                    if (re) {
                        m_FFMpegPlayer.nDisplayWidth = 640;
                        m_FFMpegPlayer.nDisplayHeight = 480;
                        m_FFMpegPlayer.b480 = true;
                    } else {
                        m_FFMpegPlayer.nDisplayWidth = 640;
                        m_FFMpegPlayer.nDisplayHeight = 360;
                        m_FFMpegPlayer.b480 = false;
                    }
                }
            }
            m_MySonix.F_Set480(re);
            int32_t res;
            m_MySonix.F_CreateRevSocket_and_Listen();
            m_MySonix.createVideoSocket();

#ifdef D_DEBUG
            m_MySonix.fuc_getDAta = F_GetData_SNA;
#else
            m_MySonix.fuc_getDAta = F_GetData_SN;
#endif

            m_MySonix.StartReceive();
            m_MySonix.sendStart();
            F_SendStatus2Jave();
            //  F_RecRP_RTSP_Status_Service();  //SN


            msg[0] = 'J';
            msg[1] = 'H';
            msg[2] = 'C';
            msg[3] = 'M';
            msg[4] = 'D';
            msg[5] = 0x20;
            msg[6] = 0x00;
            msg[7] = 0x00;
            msg[8] = 0x00;
            msg[9] = 0x00;
            msg[10] = 0x00;

            send_cmd_udp(msg, 11, sServerIP.c_str(), 20000);
            usleep(1000 * 20);
            msg[0] = 'J';
            msg[1] = 'H';
            msg[2] = 'C';
            msg[3] = 'M';
            msg[4] = 'D';
            msg[5] = 0x20;
            msg[6] = 0x00;
            msg[7] = 0x00;
            msg[8] = 0x00;
            msg[9] = 0x00;
            msg[10] = 0x00;

            send_cmd_udp(msg, 11, sServerIP.c_str(), 20000);
            bInit = true;
        }
        return 0;
    } else if (nICType == IC_GPRTSP || nICType == IC_GPRTP || nICType == IC_GPRTPB) {

        nCheckT_pre = av_gettime() / 1000;
        F_SetRelinkerT(1000 * 5);
        m_FFMpegPlayer.nfps = 20;
        m_FFMpegPlayer.nDisplayHeight = 480;  //
        m_FFMpegPlayer.nDisplayWidth = 640;
        //  F_RecRP_RTSP_Status_Service();              //GP
        if (nICType == IC_GPRTP ||
            nICType == IC_GPRTPB)       //自定义RTP    IC_GPRTP 凌通的自定义RTP    IC_GPRTPB 乐信的自定义RTP
        {

            //m_FFMpegPlayer.InitMedia("");

            F_SentRTPStop();
            usleep(1000 * 30);
            F_SentRTPStop();
            usleep(1000 * 30);
            F_SentRTPStop();
            usleep(1000 * 20);

            F_Rec_RTP_Data_Service();

            msg[0] = 'J';
            msg[1] = 'H';
            msg[2] = 'C';
            msg[3] = 'M';
            msg[4] = 'D';
            msg[5] = 0x10;
            msg[6] = 0x00;
            send_cmd_udp(msg, 7, sServerIP.c_str(), 20000);
            usleep(1000 * 25);
            msg[0] = 'J';
            msg[1] = 'H';
            msg[2] = 'C';
            msg[3] = 'M';
            msg[4] = 'D';
            msg[5] = 0x20;
            msg[6] = 0x00;
            msg[7] = 0x00;
            msg[8] = 0x00;
            msg[9] = 0x00;
            msg[10] = 0x00;

            send_cmd_udp(msg, 11, sServerIP.c_str(), 20000);
            usleep(1000 * 10);

            msg[0] = 'J';
            msg[1] = 'H';
            msg[2] = 'C';
            msg[3] = 'M';
            msg[4] = 'D';
            msg[5] = 0xD0;
            msg[6] = 0x01;
            send_cmd_udp(msg, 7, sServerIP.c_str(), 20000);
            usleep(1000 * 15);
            msg[0] = 'J';
            msg[1] = 'H';
            msg[2] = 'C';
            msg[3] = 'M';
            msg[4] = 'D';
            msg[5] = 0xD0;
            msg[6] = 0x01;
            send_cmd_udp(msg, 7, sServerIP.c_str(), 20000);
            usleep(1000 * 10);
            bInit = true;
            return 0;
        }
    }
    if (nICType == IC_GPH264) {

        m_FFMpegPlayer.nfps = 20;
        m_FFMpegPlayer.nDisplayHeight = 360;
        m_FFMpegPlayer.nDisplayWidth = 640;
        //    F_RecRP_RTSP_Status_Service();     //GP


        msg[0] = 'J';
        msg[1] = 'H';
        msg[2] = 'C';
        msg[3] = 'M';
        msg[4] = 'D';
        msg[5] = 0x10;
        msg[6] = 0x00;
        send_cmd_udp(msg, 7, sServerIP.c_str(), 20000);
        usleep(1000 * 25);
        msg[0] = 'J';
        msg[1] = 'H';
        msg[2] = 'C';
        msg[3] = 'M';
        msg[4] = 'D';
        msg[5] = 0x20;
        msg[6] = 0x00;
        msg[7] = 0x00;
        msg[8] = 0x00;
        msg[9] = 0x00;
        msg[10] = 0x00;

        send_cmd_udp(msg, 11, sServerIP.c_str(), 20000);
        usleep(1000 * 10);


    } else {
        m_FFMpegPlayer.nDisplayHeight = 360;  //
        m_FFMpegPlayer.nDisplayWidth = 640;

    }

    bInit = false;
    nCheckT_pre = av_gettime() / 1000;
    F_SetRelinkerT(1000 * 5);
    bInit = true;
    m_FFMpegPlayer.InitMedia(sPlayPath);
    naPlay();  //RTSP
    return 0;
}


void F_GP_InitA(void) {
    uint8_t msg[20];
    msg[0] = 'J';
    msg[1] = 'H';
    msg[2] = 'C';
    msg[3] = 'M';
    msg[4] = 'D';
    msg[5] = 0x10;
    msg[6] = 0x00;
    send_cmd_udp(msg, 7, sServerIP.c_str(), 20000);
    usleep(1000 * 25);

    msg[0] = 'J';
    msg[1] = 'H';
    msg[2] = 'C';
    msg[3] = 'M';
    msg[4] = 'D';
    msg[5] = 0x20;
    msg[6] = 0x00;
    send_cmd_udp(msg, 7, sServerIP.c_str(), 20000);
    usleep(1000 * 25);

    msg[0] = 'J';
    msg[1] = 'H';
    msg[2] = 'C';
    msg[3] = 'M';
    msg[4] = 'D';
    msg[5] = 0xD0;
    msg[6] = 0x01;
    send_cmd_udp(msg, 7, sServerIP.c_str(), 20000);
    usleep(1000 * 25);

    msg[0] = 'J';
    msg[1] = 'H';
    msg[2] = 'C';
    msg[3] = 'M';
    msg[4] = 'D';
    msg[5] = 0xD0;
    msg[6] = 0x01;
    send_cmd_udp(msg, 7, sServerIP.c_str(), 20000);
    usleep(1000 * 5);
}


int naInit_Re(void) {


    bNeedRead20000 = true;
    bNeedRead20001 = true;
    F_StartReadUdp20000();
    F_StartReadUdp20001();




    bFilterChanged = true;
    bInitMediaA = false;
    //nFrameCount = 0;

    m_FFMpegPlayer.nSecT = 0;
    m_FFMpegPlayer.nSpsSet = 0;
    m_FFMpegPlayer.nSpsSet = 0;
    bNeedExit = false;
    //F_StartheckFps();   //开始检查 每秒接受多少帧，从而来判断图像释放卡顿
    uint8_t msg[50];
    nCheckT_pre = av_gettime() / 1000;

    nSDStatus = 0;
    m_FFMpegPlayer.bStarDecord = false;
    m_FFMpegPlayer.StopSaveVideo();


    bInit = false;
    nCheckT_pre = av_gettime() / 1000;
    F_SetRelinkerT(4000);
    pthread_mutex_init(&m_mutex, nullptr);
    bInit = true;

    int i32Ret = -1;


    if (nICType == IC_GPH264A || nICType == IC_RTLH264 || nICType == IC_GPH264_34) {

        int ret = 0;

        F_GP_InitA();

        if (GP_tcp_VideoSocket.bConnected) {
            return -1;
        }
        m_FFMpegPlayer.nErrorFrame = 0;
        m_FFMpegPlayer.nfps = 20;
        m_FFMpegPlayer.nDisplayWidth = 640;
        m_FFMpegPlayer.nDisplayHeight = 360;


        msg[0] = 'J';
        msg[1] = 'H';
        msg[2] = 'C';
        msg[3] = 'M';
        msg[4] = 'D';
        msg[5] = 0x10;
        msg[6] = 0x00;
        send_cmd_udp(msg, 7, sServerIP.c_str(), 20000);

        m_FFMpegPlayer.InitMedia("");

        if (Connect_GPH264() < 0) {
            ret = -1;
        }
        return ret;
    }


    if (nICType == IC_GKA) {
        bGKA_ConnOK = false;
        // bStoped = true;
        int ret = 0;
        if (GK_tcp_SendSocket.bConnected) {

            DEBUG_PRINT("1-----");
            return ret;
        }


        if (sPlayPath[0] == '1') {
            m_FFMpegPlayer.nDisplayWidth = 640;//1280;
            m_FFMpegPlayer.nDisplayHeight = 360;//720;
            nGKA_StreamNo = 1;
        } else {
            m_FFMpegPlayer.nDisplayWidth = 640;
            m_FFMpegPlayer.nDisplayHeight = 360;
            nGKA_StreamNo = 2;
        }

        m_FFMpegPlayer.InitMedia(sPlayPath);
        F_ResetRelinker();
        F_SetRelinkerT(5000);
        ret = 0;
        if (Connect_gk() < 0) {
            F_SetRelinkerT(100);
            ret = -1;
        }
        if (ret >= 0) {
            // F_RecRP_RTSP_Status_Service();  //GKA
        }

        return ret;

    }
    if (nICType == IC_SN) {
        m_FFMpegPlayer.nfps = 25;
        if (m_MySonix.createCommandSocket() == 0) {
            m_MySonix.sendStop();
            bool re;

            re = F_GetResolution_A();

            if (re) {
                m_FFMpegPlayer.nDisplayWidth = 640;
                m_FFMpegPlayer.nDisplayHeight = 480;
                m_FFMpegPlayer.b480 = true;
            } else {
                re = F_GetResolution_A();
                if (re) {
                    m_FFMpegPlayer.nDisplayWidth = 640;
                    m_FFMpegPlayer.nDisplayHeight = 480;
                    m_FFMpegPlayer.b480 = true;
                } else {
                    re = m_MySonix.F_GetResolution();
                    if (re) {
                        m_FFMpegPlayer.nDisplayWidth = 640;
                        m_FFMpegPlayer.nDisplayHeight = 480;
                        m_FFMpegPlayer.b480 = true;
                    } else {
                        m_FFMpegPlayer.nDisplayWidth = 640;
                        m_FFMpegPlayer.nDisplayHeight = 360;
                        m_FFMpegPlayer.b480 = false;
                    }
                }
            }
            m_MySonix.F_Set480(re);
            int32_t res;
            m_MySonix.F_CreateRevSocket_and_Listen();
            m_MySonix.createVideoSocket();

#ifdef D_DEBUG
            m_MySonix.fuc_getDAta = F_GetData_SNA;
#else
            m_MySonix.fuc_getDAta = F_GetData_SN;
#endif

            m_MySonix.StartReceive();
            m_FFMpegPlayer.InitMedia("");
            m_MySonix.sendStart();
            nSDStatus = 0;//|=bit0_OnLine;
            F_SendStatus2Jave();
            //    F_RecRP_RTSP_Status_Service(); //SN

            msg[0] = 'J';
            msg[1] = 'H';
            msg[2] = 'C';
            msg[3] = 'M';
            msg[4] = 'D';
            msg[5] = 0x20;
            msg[6] = 0x00;
            msg[7] = 0x00;
            msg[8] = 0x00;
            msg[9] = 0x00;
            msg[10] = 0x00;

            send_cmd_udp(msg, 11, sServerIP.c_str(), 20000);
            usleep(1000 * 20);
            msg[0] = 'J';
            msg[1] = 'H';
            msg[2] = 'C';
            msg[3] = 'M';
            msg[4] = 'D';
            msg[5] = 0x20;
            msg[6] = 0x00;
            msg[7] = 0x00;
            msg[8] = 0x00;
            msg[9] = 0x00;
            msg[10] = 0x00;

            send_cmd_udp(msg, 11, sServerIP.c_str(), 20000);
        }
        return 0;
    }
    if (nICType == IC_GPRTPC) {
        nCheckT_pre = av_gettime() / 1000;
        F_SetRelinkerT(1000 * 5);
        m_FFMpegPlayer.nfps = 20;
        m_FFMpegPlayer.nDisplayHeight = 360;  //
        m_FFMpegPlayer.nDisplayWidth = 640;

        //先发stop

        F_SentRTPStop();
        usleep(1000 * 30);
        F_SentRTPStop();
        usleep(1000 * 30);
        F_SentRTPStop();
        usleep(1000 * 20);
        m_FFMpegPlayer.InitMedia("");
        //F_Rec_RTP_Data_Service();
        msg[0] = 'J';
        msg[1] = 'H';
        msg[2] = 'C';
        msg[3] = 'M';
        msg[4] = 'D';
        msg[5] = 0x20;
        msg[6] = 0x00;
        msg[7] = 0x00;
        msg[8] = 0x00;
        msg[9] = 0x00;
        msg[10] = 0x00;
        send_cmd_udp(msg, 11, sServerIP.c_str(), 20000);
        usleep(1000 * 50);
        msg[0] = 'J';
        msg[1] = 'H';
        msg[2] = 'C';
        msg[3] = 'M';
        msg[4] = 'D';
        msg[5] = 0x20;
        msg[6] = 0x00;
        msg[7] = 0x00;
        msg[8] = 0x00;
        msg[9] = 0x00;
        msg[10] = 0x00;
        send_cmd_udp(msg, 11, sServerIP.c_str(), 20000);
        usleep(1000 * 40);
        msg[0] = 'J';
        msg[1] = 'H';
        msg[2] = 'C';
        msg[3] = 'M';
        msg[4] = 'D';
        msg[5] = 0x10;
        msg[6] = 0x00;
        send_cmd_udp(msg, 7, sServerIP.c_str(), 20000);
        usleep(1000 * 10);

        msg[0] = 'J';
        msg[1] = 'H';
        msg[2] = 'C';
        msg[3] = 'M';
        msg[4] = 'D';
        msg[5] = 0xD0;
        msg[6] = 0x01;
        send_cmd_udp(msg, 7, sServerIP.c_str(), 20000);
        usleep(1000 * 15);
        msg[0] = 'J';
        msg[1] = 'H';
        msg[2] = 'C';
        msg[3] = 'M';
        msg[4] = 'D';
        msg[5] = 0xD0;
        msg[6] = 0x01;
        send_cmd_udp(msg, 7, sServerIP.c_str(), 20000);
        usleep(1000 * 10);

        if (Connect_GPH264() < 0) {

        } else {

        }

        return 0;


    }

    if (nICType == IC_GPRTSP || nICType == IC_GPRTP || nICType == IC_GPRTPB) {
        nCheckT_pre = av_gettime() / 1000;
        F_SetRelinkerT(1000 * 5);
        m_FFMpegPlayer.nfps = 20;
        m_FFMpegPlayer.nDisplayHeight = 360;  //
        m_FFMpegPlayer.nDisplayWidth = 640;
        //   F_RecRP_RTSP_Status_Service();              //GP
        if (nICType == IC_GPRTP ||
            nICType == IC_GPRTPB)       //自定义RTP    IC_GPRTP 凌通的自定义RTP    IC_GPRTPB 乐信的自定义RTP
        {
            //  F_SentRTPStop();
            //  F_SentRTPStop();
            m_FFMpegPlayer.InitMedia("");

            msg[0] = 'J';
            msg[1] = 'H';
            msg[2] = 'C';
            msg[3] = 'M';
            msg[4] = 'D';
            msg[5] = 0x20;
            msg[6] = 0x00;
            msg[7] = 0x00;
            msg[8] = 0x00;
            msg[9] = 0x00;
            msg[10] = 0x00;

            send_cmd_udp(msg, 11, sServerIP.c_str(), 20000);

            usleep(1000 * 5);
//            msg[0] = 'J';
//            msg[1] = 'H';
//            msg[2] = 'C';
//            msg[3] = 'M';
//            msg[4] = 'D';
//
//            msg[5] = 0x20;
//            msg[6] = 0x00;
//            msg[7] = 0x00;
//            msg[8] = 0x00;
//            msg[9] = 0x00;
//            msg[10] = 0x00;

            send_cmd_udp(msg, 11, sServerIP.c_str(), 20000);
            usleep(1000 * 5);

            msg[0] = 'J';
            msg[1] = 'H';
            msg[2] = 'C';
            msg[3] = 'M';
            msg[4] = 'D';
            msg[5] = 0x10;
            msg[6] = 0x00;
            send_cmd_udp(msg, 7, sServerIP.c_str(), 20000);
            usleep(1000 * 5);

            msg[0] = 'J';
            msg[1] = 'H';
            msg[2] = 'C';
            msg[3] = 'M';
            msg[4] = 'D';
            msg[5] = 0xD0;
            msg[6] = 0x01;
            send_cmd_udp(msg, 7, sServerIP.c_str(), 20000);

            usleep(1000 * 5);
            msg[0] = 'J';
            msg[1] = 'H';
            msg[2] = 'C';
            msg[3] = 'M';
            msg[4] = 'D';
            msg[5] = 0xD0;
            msg[6] = 0x01;
            send_cmd_udp(msg, 7, sServerIP.c_str(), 20000);
        } else {
            msg[0] = 'J';             //标准RTSP
            msg[1] = 'H';
            msg[2] = 'C';
            msg[3] = 'M';
            msg[4] = 'D';
            msg[5] = 0x10;
            msg[6] = 0x00;
            send_cmd_udp(msg, 7, sServerIP.c_str(), 20000);
            usleep(1000 * 25);
            msg[0] = 'J';
            msg[1] = 'H';
            msg[2] = 'C';
            msg[3] = 'M';
            msg[4] = 'D';
            msg[5] = 0x20;
            msg[6] = 0x00;
            msg[7] = 0x00;
            msg[8] = 0x00;
            msg[9] = 0x00;
            msg[10] = 0x00;
            send_cmd_udp(msg, 11, sServerIP.c_str(), 20000);
            //usleep(1000 * 10);
        }

        F_Rec_RTP_Data_Service();
        return 0;

    } else if (nICType == IC_GPH264) {

        m_FFMpegPlayer.nfps = 20;

        m_FFMpegPlayer.nDisplayHeight = 360;
        m_FFMpegPlayer.nDisplayWidth = 640;

        msg[0] = 'J';
        msg[1] = 'H';
        msg[2] = 'C';
        msg[3] = 'M';
        msg[4] = 'D';
        msg[5] = 0x10;
        msg[6] = 0x00;
        send_cmd_udp(msg, 7, sServerIP.c_str(), 20000);
        usleep(1000 * 25);
        msg[0] = 'J';
        msg[1] = 'H';
        msg[2] = 'C';
        msg[3] = 'M';
        msg[4] = 'D';
        msg[5] = 0x20;
        msg[6] = 0x00;
        msg[7] = 0x00;
        msg[8] = 0x00;
        msg[9] = 0x00;
        msg[10] = 0x00;
        send_cmd_udp(msg, 11, sServerIP.c_str(), 20000);
        usleep(1000 * 10);

    } else {
        m_FFMpegPlayer.nDisplayHeight = 360;  //
        m_FFMpegPlayer.nDisplayWidth = 640;
    }

    //GPRTSP  GPH264  朗通的方案

    bInit = false;
    nCheckT_pre = av_gettime() / 1000;
    F_SetRelinkerT(1000 * 5);
    bInit = true;
    i32Ret = m_FFMpegPlayer.InitMedia(sPlayPath);
    if (i32Ret == 0) {
        naPlay();  //RTSP
    }
    return i32Ret;
}

void F_StartReadUdp20000();

void naStop_(void);

volatile bool bOpened = false;

bool bInitA = false;

JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naStop(JNIEnv *env, jclass type) {

    pthread_mutex_lock(&mOpenCamera_Lock);
    nWifiInterface = -1;
    naStop_();
    bOpened = false;
    bInitA = false;
    pthread_mutex_unlock(&mOpenCamera_Lock);
    return 0;
}


JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naInit(JNIEnv *env, jclass type, jstring pFileName_) {

    if (bOpened)
        return 0;

    const char *pFileName = env->GetStringUTFChars(pFileName_, 0);
    bNeedStop = false;
    F_InitGSensorData();

    pthread_mutex_lock(&mOpenCamera_Lock);

    bOpened = true;
    bInitA = true;
    int i32Ret = _naInit_(pFileName);

    pthread_mutex_unlock(&mOpenCamera_Lock);

    env->ReleaseStringUTFChars(pFileName_, pFileName);

    return i32Ret;
}

//int F_PlaySN();

bool F_GetResolution_A(void) {
    uint8_t cmd[2];
    cmd[0] = 0xA6;
    cmd[1] = 0x6A;
    //send_cmd_udp_fd(m_MySonix.uartCommandfd, cmd, 2, sServerIP.c_str(), 30864);
    send_cmd_udp(cmd, 2, sServerIP.c_str(), 30864);
    usleep(1000);
    bool b480 = false;
    char readBuff_cfg[150];
    ssize_t nbytes;
    int size;
    struct sockaddr_in servaddr;
    bzero((char *) &servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(30864);
    size = sizeof(servaddr);
    struct timeval timeoutA = {0, 1000 * 210};     //210ms
    int nError;
    memset(readBuff_cfg, 0, 100);
    setsockopt(m_MySonix.uartCommandfd, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeoutA,
               sizeof(timeoutA));
    if ((nbytes = recvfrom(m_MySonix.uartCommandfd, readBuff_cfg, 100, 0,
                           (struct sockaddr *) &servaddr, (socklen_t *) &size)) < 0) {
        b480 = false;
    } else {
        if (nbytes >= 48) {
            unsigned char *p = (unsigned char *) readBuff_cfg;
            int n = readBuff_cfg[41] + readBuff_cfg[42] * 0x100;
            if (n == 480) {
                b480 = true;
            } else {
                b480 = false;
            }
        } else {
            b480 = false;
        }
    }

    return b480;
}


bool bGKA_Play = false;

int naPlay(void) {
    nCheckT_pre = av_gettime() / 1000;
    F_SetRelinkerT(4000);
    return m_FFMpegPlayer.PlayMedia();
}

JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naPlay(JNIEnv *env, jclass type) {

    //nCheckT_pre = av_gettime()*1000;
    //return m_FFMpegPlayer.PlayMedia();
    // return naPlay();

    return 0;
}




int naStop_B(void) {
    int ret = 0;
    bNeedExit = true;
    usleep(1000 * 50);


    if (GPRTP_rev_thread != -1) {
        void *ret = nullptr;
        pthread_join(GPRTP_rev_thread, &ret);
        GPRTP_rev_thread = -1;
    }


    //GPRTP_Data
    if (GPRTP_UDP_SOCKET > 0) {
        close(GPRTP_UDP_SOCKET);
        GPRTP_UDP_SOCKET = -1;
    }
    //GPRTP_rev_thread = -1;


    if (nICType == IC_GPH264A || nICType == IC_RTLH264 || nICType == IC_GPH264_34) {
        bInit = false;
        //bNeedExit = true;
        //usleep(1000 * 50);
        DisConnect(true);
        //ret  = m_FFMpegPlayer.Stop();
        nSDStatus &= bit0_OnLine ^ 0xFF;
        F_SendStatus2Jave();
        return ret;
    } else if (nICType == IC_GKA) {
        F_SendStatus2Jave();
        DisConnect(true);
        usleep(1000 * 50);
        nSDStatus &= bit0_OnLine ^ 0xFF;
        F_SendStatus2Jave();
    } else if (nICType == IC_GK_UDP) {
        F_SendStatus2Jave();
        //  m_JhGK_udp.Stop();
        //ret = m_FFMpegPlayer.Stop();
        return 0;
    } else if (nICType == IC_SN) {

        F_SendStatus2Jave();
        m_MySonix.sendStop();
        m_MySonix.closeCommandSocket();
        m_MySonix.closeVideoSocket();
        return 0;
    } else if (nICType == IC_GPRTPC) {
        bInit = false;
        //bNeedExit = true;
        //usleep(1000 * 50);
        DisConnect(true);
        nSDStatus &= bit0_OnLine ^ 0xFF;
        F_SendStatus2Jave();
    } else if (nICType == IC_GPRTP || nICType == IC_GPRTPB) {
        F_SendStatus2Jave();
        return 0;
    } else if (nICType == IC_GP) {
        naStop();
    } else if (nICType == IC_GPRTSP) {
        naStop();
    }

    return -1;
}


MySocket_GKA _tcp_Socket;
OnLinePlayer RevPlay;

int naStop(void) {
    int ret = 0;
    bNeedExit = true;
    DisConnect(true);
    usleep(1000*10);
    if (GPRTP_rev_thread != -1) {
        void *ret = nullptr;
        pthread_join(GPRTP_rev_thread, &ret);
        GPRTP_rev_thread = -1;
    }

    if (GPRTP_UDP_SOCKET > 0) {
        close(GPRTP_UDP_SOCKET);
        GPRTP_UDP_SOCKET = -1;
    }

    RevPlay.Stop();
    mRTL_DownLoad.disConnected();
    _tcp_Socket.DisConnect();
    bInit = false;
    nSDStatus = 0;
    //m_FFMpegPlayer.Stop();
    myMediaCoder.F_CloseDecoder();

    if (nICType == IC_GPH264A || nICType == IC_RTLH264 || nICType == IC_GPH264_34) {
        F_SendStatus2Jave();
        ret = m_FFMpegPlayer.Stop();
        return ret;

    }
    if (nICType == IC_GK_UDP) {
        F_SendStatus2Jave();
        ret = m_FFMpegPlayer.Stop();
        return 0;
    }

    if (nICType == IC_GKA) {

        F_SendStatus2Jave();
        ret = m_FFMpegPlayer.Stop();
        return ret;
    }
    if (nICType == IC_SN) {
        F_SendStatus2Jave();
        m_MySonix.sendStop();
        m_MySonix.closeCommandSocket();
        m_MySonix.closeVideoSocket();
        m_FFMpegPlayer.Stop();
        return 0;
    }
    if (nICType == IC_GPRTPC) {
        F_SentRTPStop();
        F_SendStatus2Jave();
        m_FFMpegPlayer.Stop();
        return 0;

    }
    if (nICType == IC_GPRTP || nICType == IC_GPRTPB) {
        F_SentRTPStop();
        F_SendStatus2Jave();
        m_FFMpegPlayer.Stop();
        return 0;
    }
    if (nICType == IC_GP) {
        /*
        int i=0;
        cmd[i++] = 'G';
        cmd[i++] = 'P';
        cmd[i++] = 'S';
        cmd[i++] = 'O';
        cmd[i++] = 'C';
        cmd[i++] = 'K';
        cmd[i++] = 'E';
        cmd[i++] = 'T';
        cmd[i++] = 0x01;
        cmd[i++] = 0x00;        //2byte  Socket_CmdType
        cmd[i++] = 0x00;        // Socket_Mode ID
        cmd[i++] = 0x00;        // Socket_Cmd_ID    0=set mode
        cmd[i++] = 0x03;       // 0 record mode  1  capture mode 2 = playback  3 menu
        send_cmd_tcp(cmd, i,GP_IP,8081);
         */
    }
    mysocket.DisConnect();
    F_ResetCheckT(1);
    nSDStatus = 0;
    F_SendStatus2Jave();
    m_FFMpegPlayer.Stop();

    /*
    int needsDetach = 0;
    JNIEnv *evn = getJNIEnv(&needsDetach);
    if (evn == nullptr) {
        return 0;
    }
    if (needsDetach)
        gJavaVM->DetachCurrentThread();
      */
    return 0;
}

void naStop_();


JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naStatus(JNIEnv *env, jclass type) {
    return m_FFMpegPlayer.GetStatus();
}

JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naSaveSnapshot(JNIEnv *env, jclass type,
                                                        jstring pFileName_) {
    const char *pFileName = env->GetStringUTFChars(pFileName_, 0);


    int ret = m_FFMpegPlayer.SaveSnapshot(pFileName);

    env->ReleaseStringUTFChars(pFileName_, pFileName);
    return ret;
}

JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naSaveVideo(JNIEnv *env, jclass type, jstring pFileName_) {
    const char *pFileName = env->GetStringUTFChars(pFileName_, 0);

    int ret = m_FFMpegPlayer.SaveVideo(pFileName);
    nSDStatus |= bit1_LocalRecording;
    F_SendStatus2Jave();
    env->ReleaseStringUTFChars(pFileName_, pFileName);
    return ret;
}

JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naStopSaveVideo(JNIEnv *env, jclass type) {
    nSDStatus &= (bit1_LocalRecording ^ 0xFF);
    F_SendStatus2Jave();
    m_FFMpegPlayer.StopSaveVideo();


    return 0;
}


////////


JNIEnv *getJNIEnv(int *needsDetach) {
    JNIEnv *env = nullptr;
    *needsDetach = 0;
    if (gJavaVM->GetEnv((void **) &env, JNI_VERSION_1_6) < JNI_OK) {
        int status = gJavaVM->AttachCurrentThread(&env, nullptr);
        if (status < 0) {
            return nullptr;
        }
        *needsDetach = 1;
    }
    return env;
}


void F_SentGp_Status2Jave(int nStatus) {
    int needsDetach = 0;
    JNIEnv *evn = getJNIEnv(&needsDetach);
    if (evn == nullptr) {
        return;
    }
    if (gp_status_mid != nullptr) {
        evn->CallStaticVoidMethod(objclass, gp_status_mid, nStatus);
    }
    if (needsDetach)
        gJavaVM->DetachCurrentThread();

}

int F_SendKey2Jave(int nKey) {
    int needsDetach = 0;
    JNIEnv *evn = getJNIEnv(&needsDetach);
    if (evn == nullptr) {
        return -1;
    }
    if (key_mid != nullptr) {
        evn->CallStaticVoidMethod(objclass, key_mid, nKey);
    }
    if (needsDetach)
        gJavaVM->DetachCurrentThread();
    return 0;

}


void F_OnSave2ToGallery_mid(int n) {
    int needsDetach = 0;
    JNIEnv *evn = getJNIEnv(&needsDetach);
    if (evn == nullptr) {
        return;
    }
    if (Save2ToGallery_mid != nullptr) {
        if (n == 0) {
            jstring str = evn->NewStringUTF((char *) (m_FFMpegPlayer.m_snapShotPath));
            evn->CallStaticVoidMethod(objclass, Save2ToGallery_mid, str, n);
            evn->DeleteLocalRef(str);
        } else {
//            jstring str = evn->NewStringUTF(m_FFMpegPlayer.sRecordFileName.c_str());
//            evn->CallStaticVoidMethod(objclass, Save2ToGallery_mid, str, n);
//            evn->DeleteLocalRef(str);
        }
    }
    if (needsDetach)
        gJavaVM->DetachCurrentThread();

}


//////////测试信息
extern int  nWifiInterface;
int32_t F_GetIP() {


    nICType = IC_NO;
    int socket, i;
    int numif;
    struct ifconf ifc;
    struct ifreq *ifr;
    int32_t ip = -5;
    memset(&ifc, 0, sizeof(ifc));
    ifc.ifc_ifcu.ifcu_req = nullptr;
    ifc.ifc_len = 0;
    if ((socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        return -1;
    }

    if (ioctl(socket, SIOCGIFCONF, &ifc) < 0) {
        return -2;
    }
    byte *ffp = new byte[ifc.ifc_len];
    ifr = (struct ifreq *) ffp;
    if (ifr == nullptr) {
        return -3;
    }
    ifc.ifc_ifcu.ifcu_req = ifr;
    if (ioctl(socket, SIOCGIFCONF, &ifc) < 0) {
        delete[]ffp;
        return -4;
    }
    numif = ifc.ifc_len / sizeof(struct ifreq);

    int ss = 0;
    bool bFind = false;
    for (i = 0; i < numif; i++) {
        struct ifreq *r = &ifr[i];
        struct sockaddr_in *sin = (struct sockaddr_in *) &r->ifr_addr;
        char *a = inet_ntoa(sin->sin_addr);
        ip = (int32_t) sin->sin_addr.s_addr;
        F_AdjIcType(ip & 0x00FFFFFF);
        if (nICType != IC_NO) {
            char *pp = r->ifr_name;
            if (pp) {
                nWifiInterface = if_nametoindex(pp);
                //nWifiInterface = -1;
//                int i = strlen(pp);
//                if (i < 256) {
//                    memset(wlan, 0, sizeof(wlan));
//                     memcpy(wlan, pp, i);
//                }
            }
            bFind = true;
            break;
        }
    }
    delete[]ffp;
    close(socket);
    if (bFind)
        return ip;
    else
        return 0;

    /*

    int needsDetach = 0;
    int nTt=IC_NO;
    JNIEnv *evn = getJNIEnv(&needsDetach);
    if (evn == nullptr) {
        return nTt;
    }

    if (G_getIP != nullptr) {
        nTt  = evn->CallStaticIntMethod(objclass,G_getIP);

    }
    if (needsDetach)
        gJavaVM->DetachCurrentThread();
    return nTt;
     */
}

int onGetLedPwm(int i)
{
    int needsDetach = 0;
    JNIEnv *evn = getJNIEnv(&needsDetach);
    if (evn == nullptr) {
        return -1;
    }
    if (onGetLedPwm_mid != nullptr) {
        evn->CallStaticVoidMethod(objclass, onGetLedPwm_mid, i);
    }
    if (needsDetach)
        gJavaVM->DetachCurrentThread();
    return 0;

}

int F_SentTestInfo() {

    int needsDetach = 0;
    JNIEnv *evn = getJNIEnv(&needsDetach);
    if (evn == nullptr) {
        return -1;
    }
    if (RevTestInfo_mid != nullptr) {
        jbyteArray jbarray = evn->NewByteArray(1024);
        evn->SetByteArrayRegion(jbarray, 0, 1024, TestInfo);

        evn->CallStaticVoidMethod(objclass, RevTestInfo_mid, jbarray);

        evn->DeleteLocalRef(jbarray);

    }
    if (needsDetach)
        gJavaVM->DetachCurrentThread();
    return 0;
};

int F_SendStatus2Jave() {
    int needsDetach = 0;
    JNIEnv *evn = getJNIEnv(&needsDetach);
    if (evn == nullptr) {
        return -1;
    }
    if (status_mid != nullptr) {
        evn->CallStaticVoidMethod(objclass, status_mid, nSDStatus);
    }
    if (needsDetach)
        gJavaVM->DetachCurrentThread();
    return 0;
}


void F_CloseSocket() {
    mysocket.DisConnect();
}


void F_OnGetWifiData(byte *data, int nLen) {
    int needsDetach = 0;
    if (data == nullptr)
        return;
    if (data == nullptr)
        return;
    if (nLen == 0)
        return;

    JNIEnv *evn = getJNIEnv(&needsDetach);
    if (evn == nullptr) {
        return;
    }
    int nCount = nLen;
    if (nCount > 0) {
        jbyteArray jbarray = evn->NewByteArray(nCount);
        jbyte *jy = (jbyte *) data;
        evn->SetByteArrayRegion(jbarray, 0, nCount, jy);
        if (GetWifiData_mid != nullptr) {
            evn->CallStaticVoidMethod(objclass, GetWifiData_mid, jbarray);
        }
        evn->DeleteLocalRef(jbarray);
    }

    if (needsDetach)
        gJavaVM->DetachCurrentThread();
}


bool F_SetRecordAudio(int n) {
    int needsDetach = 0;

    bool re = false;

    JNIEnv *evn = getJNIEnv(&needsDetach);
    if (evn == nullptr) {
        return re;
    }

    if (G_StartAudio_mid != nullptr) {
        //re = evn->CallStaticVoidMethod(objclass, G_StartAudio_mid, n);
        re = evn->CallStaticBooleanMethod(objclass, G_StartAudio_mid, n);
    }

    if (needsDetach)
        gJavaVM->DetachCurrentThread();
    return re;
}

int SendDir2java(const char *sName) {
    int needsDetach = 0;
    if (sName == nullptr)
        return -2;
    JNIEnv *evn = getJNIEnv(&needsDetach);
    if (evn == nullptr) {
        return -1;
    }
    int nCount = strlen(sName);
    if (nCount > 0) {
        jbyteArray jbarray = evn->NewByteArray(nCount);
        jbyte *jy = (jbyte *) sName;
        evn->SetByteArrayRegion(jbarray, 0, nCount, jy);
        if (dir_mid != nullptr) {
            evn->CallStaticVoidMethod(objclass, dir_mid, jbarray);
        }
        evn->DeleteLocalRef(jbarray);
    }

    if (needsDetach)
        gJavaVM->DetachCurrentThread();

    return 0;


}


/*
void F_SetSurface()
{
        if (surfaceA != nullptr && m_FFMpegPlayer.nativeWindow == nullptr)
        {
            int needsDetach = 0;
            JNIEnv *evn = getJNIEnv(&needsDetach);
            if (evn == nullptr) {
                return;
            }

            m_FFMpegPlayer.nativeWindow = ANativeWindow_fromSurface(evn, surfaceA);

            if (needsDetach)
                gJavaVM->DetachCurrentThread();

            ANativeWindow_setBuffersGeometry(m_FFMpegPlayer.nativeWindow,
                                             m_FFMpegPlayer.nDisplayWidth,
                                             m_FFMpegPlayer.nDisplayHeight,
                                             WINDOW_FORMAT_RGBA_8888);
        }

}
 */

void F_ResetRelinker() {
    nCheckT_pre = av_gettime() / 1000;
}


void F_CloseEncoder() {
    if (F_CloseEncoder_mid != nullptr) {
        int needsDetach = 0;
        JNIEnv *evn = getJNIEnv(&needsDetach);
        if (evn == nullptr) {
            return;
        }
        evn->CallStaticVoidMethod(objclass, F_CloseEncoder_mid);
        if (needsDetach)
            gJavaVM->DetachCurrentThread();
    }
}

void offerEncoder(uint8_t *data, int nLen) {
    if (offerEncoder_mid != nullptr) {
        int needsDetach = 0;
        JNIEnv *evn = getJNIEnv(&needsDetach);
        if (evn == nullptr) {
            return;
        }

        jbyteArray jbarray = evn->NewByteArray(nLen);
        evn->SetByteArrayRegion(jbarray, 0, nLen, (jbyte *) data);
        evn->CallStaticVoidMethod(objclass, offerEncoder_mid, jbarray, nLen);
        evn->DeleteLocalRef(jbarray);
        if (needsDetach)
            gJavaVM->DetachCurrentThread();
        return;
    }
}


void OnPlayIsStarting_Callback(int n) {
    if (OnPlayIsStarting_Callback_mid != nullptr) {
        int needsDetach = 0;
        JNIEnv *evn = getJNIEnv(&needsDetach);
        if (evn == nullptr) {
            return;
        }
        evn->CallStaticVoidMethod(objclass, OnPlayIsStarting_Callback_mid, n);
        if (needsDetach)
            gJavaVM->DetachCurrentThread();
        return;
    }
}

void F_OnGetAngle(float a) {
    if (OnGetAngle_mid != nullptr) {
        int needsDetach = 0;
        JNIEnv *evn = getJNIEnv(&needsDetach);
        if (evn == nullptr) {
            return;
        }
        evn->CallStaticVoidMethod(objclass, OnGetAngle_mid, a);
        if (needsDetach)
            gJavaVM->DetachCurrentThread();
        return;
    }
}

void F_onReadRtlData(uint8_t *data, int nLen) {
    if (onReadRtlData_mid != nullptr) {
        int needsDetach = 0;
        JNIEnv *evn = getJNIEnv(&needsDetach);
        if (evn == nullptr) {
            return;
        }
        jbyteArray jbarray = evn->NewByteArray(nLen);
        evn->SetByteArrayRegion(jbarray, 0, nLen, (jbyte *) data);
        evn->CallStaticVoidMethod(objclass, onReadRtlData_mid, jbarray);
        evn->DeleteLocalRef(jbarray);

        if (needsDetach)
            gJavaVM->DetachCurrentThread();
        return;
    }
}

bool F_InitEncoder(int w, int h, int bitfrate, int fps) {
    if (F_InitEncoder_mid != nullptr) {
        int needsDetach = 0;
        JNIEnv *evn = getJNIEnv(&needsDetach);
        if (evn == nullptr) {
            return false;
        }
        bool bOK = true;
        encord_colorformat = evn->CallStaticIntMethod(objclass, F_InitEncoder_mid, w, h, bitfrate,
                                                      fps);
        if (encord_colorformat == 0) {
            bOK = false;
            encord_colorformat = OMX_COLOR_FormatYUV420Planar;
        }
        if (encord_colorformat == 0x7FFFF) {
            bOK = false;
            encord_colorformat = OMX_COLOR_FormatYUV420Planar;
        }


        if (needsDetach)
            gJavaVM->DetachCurrentThread();

        return bOK;
    }
    return true;
}

int F_GetIPByJava(void) {
    if (GetIP_mid != nullptr) {
        int nIp = 0;
        int needsDetach = 0;
        JNIEnv *evn = getJNIEnv(&needsDetach);
        if (evn == nullptr) {
            return 0;
        }
        nIp = evn->CallStaticIntMethod(objclass, GetIP_mid);
        if (needsDetach)
            gJavaVM->DetachCurrentThread();
        return nIp;
    }
    return 0;
}


void F_SentGestureBmp(int32_t wh) {
    int needsDetach = 0;
    JNIEnv *evn = getJNIEnv(&needsDetach);
    if (evn == nullptr) {
        return;
    }
    if (Gesture_mid != nullptr) {
        evn->CallStaticVoidMethod(objclass, Gesture_mid, wh);
    }

    if (needsDetach)
        gJavaVM->DetachCurrentThread();

}


volatile  bool  bSent = false;

void F_SentRevBmp(int32_t wh) {
    int needsDetach = 0;
    if (!bSent)
    {
        bSent = true;
        JNIEnv *evn = getJNIEnv(&needsDetach);
        if (evn == nullptr) {
            bSent = false;
            return;
        }
        if (Receive_mid != nullptr) {
            evn->CallStaticVoidMethod(objclass, Receive_mid, wh);
        }


        if (needsDetach)
             gJavaVM->DetachCurrentThread();
        bSent = false;
    }

}


int PlatformDisplay() {
    nCheckT_pre = av_gettime() / 1000;

    if (((nSDStatus & bit0_OnLine) == 0) && bInit) {
        nSDStatus |= bit0_OnLine;
        F_SendStatus2Jave();
    }
    nSDStatus |= bit0_OnLine;
    bGoble_Flip = false;

    //pthread_mutex_lock(&m_checkFrame_lock);/*锁住互斥量*/
    //nFrameCount++;
    //pthread_mutex_unlock(&m_checkFrame_lock);/*解锁互斥量*/
    m_FFMpegPlayer.F_GetPacket();
    return 0;
#if 0
    int needsDetach = 0;
    JNIEnv *evn = getJNIEnv(&needsDetach);
    if (evn == nullptr) {
        return -1;
    }

    AVPacket *packet = m_FFMpegPlayer.F_GetPacket();
    if (packet != nullptr) {
        unsigned long nCount = packet->size;

        jbyteArray jbarray = evn->NewByteArray(nCount);
        jbyte *jy = (jbyte *) packet->data;  //BYTE强制转换成Jbyte；
        evn->SetByteArrayRegion(jbarray, 0, nCount, jy);
        if(image_mid!=nullptr) {
            evn->CallStaticVoidMethod(objclass, image_mid, jbarray);
        }
        av_packet_unref(packet);
        evn->DeleteLocalRef(jbarray);
    }
    if (needsDetach)
        gJavaVM->DetachCurrentThread();
#endif
}

JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naSentCmd(JNIEnv *env, jclass type, jbyteArray cmd_,
                                                   jint nLen1) {

    jbyte *cmd_b = env->GetByteArrayElements(cmd_, nullptr);

    int nLen = nLen1;
    //int len_array = env->GetArrayLength(cmd_);
    //if(nLen>len_array)
    //    nLen = len_array;
    //int n = n1;

    uint16_t ncheck = 0;
    uint8_t cmd[200];
    uint8_t msg[200];
#if 0
    F_OnGetWifiData((byte *)cmd_b,n);

#else
    if (nICType == IC_GK) {

        if (nLen > 40 - 12) {
            env->ReleaseByteArrayElements(cmd_, cmd_b, 0);
            return -1;
        }
        int i = 0;
        int x = 0;
        cmd[i++] = 0x5b;     //0
        cmd[i++] = 0x52;
        cmd[i++] = 0x74;
        cmd[i++] = 0x3e;

        cmd[i++] = (uint8_t) (12 + nLen);
        cmd[i++] = 0;

        cmd[i++] = 1;
        cmd[i++] = 0;

        cmd[i++] = 0xe0;
        cmd[i++] = 0x00;
        cmd[i++] = 0;          //10
        cmd[i++] = 0;          //11
        for (x = 0; x < nLen; x++) {
            cmd[i++] = (uint8_t) cmd_b[x];
        }
        uint16_t checksum = 0;
        for (x = 0; x < i; x++) {
            checksum += cmd[x];
        }
        while ((checksum >> 8) != 0) {
            checksum = (uint16_t) ((checksum & 0xFF) + (checksum >> 8));
        }
        cmd[10] = (uint8_t) (checksum ^ 0x00FF);

        send_cmd_udp(cmd, i, sServerIP.c_str(), 9001);
    } else if (nICType == IC_GP) {
        cmd[0] = 0xA5;
        cmd[1] = 0x5A;
        cmd[2] = (uint8_t) (nLen + 2);
        int x = 0;
        for (x = 0; x < nLen; x++) {
            if (x < 40)
                cmd[x + 3] = (uint8_t) cmd_b[x];
        }
        ncheck = 0;
        for (int i = 0; i < nLen + 3; i++) {
            ncheck += cmd[i];
        }

        cmd[nLen + 3] = (uint8_t) ncheck;
        cmd[nLen + 4] = (uint8_t) (ncheck >> 8);

        send_cmd_udp(cmd, nLen + 5, sServerIP.c_str(), 25000);
    } else if (nICType == IC_GPRTSP || nICType == IC_GPRTP || nICType == IC_GPRTPB ||
               nICType == IC_GPH264A || nICType == IC_RTLH264 || nICType == IC_GPRTPC ||
               nICType == IC_GPH264 || nICType == IC_GPH264_34) {

        if (cmd_b[0] == 'J' &&
            cmd_b[1] == 'H' &&
            cmd_b[2] == 'C' &&
            cmd_b[3] == 'M' &&
            cmd_b[4] == 'D') {
            if (cmd_b[5] == 0x20) {
                msg[0] = 'J';
                msg[1] = 'H';
                msg[2] = 'C';
                msg[3] = 'M';
                msg[4] = 'D';
                msg[5] = 0x20;
                msg[6] = 0x00;
                msg[7] = 0x00;
                msg[8] = 0x00;
                msg[9] = 0x00;
                msg[10] = 0x00;
                send_cmd_udp(msg, 11, sServerIP.c_str(), 20000);
            }
        } else {

            cmd[0] = 0xA5;
            cmd[1] = 0x5A;
            cmd[2] = (uint8_t) (nLen + 2);
            int x = 0;
            for (x = 0; x < nLen; x++) {
                if (x < 40)
                    cmd[x + 3] = (uint8_t) cmd_b[x];
            }
            ncheck = 0;
            for (int i = 0; i < nLen + 3; i++) {
                ncheck += cmd[i];
            }
            cmd[nLen + 3] = (uint8_t) ncheck;
            cmd[nLen + 4] = (uint8_t) (ncheck >> 8);

            send_cmd_udp(cmd, nLen + 5, sServerIP.c_str(), 25000);
        }
    }
//    else if (nICType == IC_GPH264) {
//        if (cmd_b[0] == 'J' &&
//            cmd_b[1] == 'H' &&
//            cmd_b[2] == 'C' &&
//            cmd_b[3] == 'M' &&
//            cmd_b[4] == 'D') {
//            if (cmd_b[5] == 0x20) {
//                msg[0] = 'J';
//                msg[1] = 'H';
//                msg[2] = 'C';
//                msg[3] = 'M';
//                msg[4] = 'D';
//                msg[5] = 0x20;
//                msg[6] = 0x00;
//                msg[7] = 0x00;
//                msg[8] = 0x00;
//                msg[9] = 0x00;
//                msg[10] = 0x00;
//
//                send_cmd_udp(msg, 11, sServerIP.c_str(), 20000);
//            }
//
//        } else {
//
//            cmd[0] = 0xA5;
//            cmd[1] = 0x5A;
//            cmd[2] = n + 2;
//            int x = 0;
//            for (x = 0; x < n; x++) {
//                if (x < 40)
//                    cmd[x + 3] = cmd_b[x];
//            }
//            ncheck = 0;
//            for (int i = 0; i < n + 3; i++) {
//                ncheck += cmd[i];
//            }
//            cmd[n + 3] = (uint8_t) ncheck;
//            cmd[n + 4] = (uint8_t) (ncheck >> 8);
//
//            send_cmd_udp(cmd, n + 5, sServerIP.c_str(), 25000);
//        }
//    }
    else if (nICType == IC_SN) {

        if (cmd_b[0] == 'J' &&
            cmd_b[1] == 'H' &&
            cmd_b[2] == 'C' &&
            cmd_b[3] == 'M' &&
            cmd_b[4] == 'D') {
            if (cmd_b[5] == 0x20) {
                msg[0] = 'J';
                msg[1] = 'H';
                msg[2] = 'C';
                msg[3] = 'M';
                msg[4] = 'D';
                msg[5] = 0x10;
                msg[6] = 0x00;
                send_cmd_udp(msg, 7, sServerIP.c_str(), 20000);

            }
        } else {

            cmd[0] = 0xA5;
            cmd[1] = 0x5A;
            cmd[2] = nLen + 2;
            int x = 0;
            for (x = 0; x < nLen; x++) {
                if (x < 40)
                    cmd[x + 3] = cmd_b[x];
            }
            ncheck = 0;
            for (int i = 0; i < nLen + 3; i++) {
                ncheck += cmd[i];
            }
            cmd[nLen + 3] = (uint8_t) ncheck;
            cmd[nLen + 4] = (uint8_t) (ncheck >> 8);
            //send_cmd_udp_fd(m_MySonix.uartCommandfd, cmd, nLen + 5, sServerIP.c_str(), 30864);
            send_cmd_udp(cmd, nLen + 5, sServerIP.c_str(), 30864);

        }
    } else if (nICType == IC_GKA) {

        if (bGKACmd_UDP) {
            T_NET_UTP_PTZ_CONTROL udp;
            nUdpInx++;
            udp.seq = nUdpInx;
            udp.sid = session_id;
            udp.flag = 0x12345678;
            udp.size = nLen;
            memcpy(udp.ptz_cmd, cmd_b, nLen);
            send_cmd_udp((uint8_t *) (&udp), sizeof(T_NET_UTP_PTZ_CONTROL), sServerIP.c_str(),
                         0x7105);
        } else {
            if (bGKA_ConnOK) {
                T_NET_CMD_MSG Cmd;
                Cmd.type = CMD_PTZ_CONTROL;
                Cmd.session_id = session_id;
                T_NET_PTZ_CONTROL ptz;
                bzero(&ptz, sizeof(T_NET_PTZ_CONTROL));
                ptz.size = nLen;
                memcpy(ptz.ptz_cmd, cmd_b, nLen);
                MySocketData data;
                data.AppendData((uint8_t *) &Cmd, sizeof(T_NET_CMD_MSG));
                data.AppendData((uint8_t *) &ptz, sizeof(T_NET_PTZ_CONTROL));
                pthread_mutex_lock(&m_Tcp_Cmd_lock);
                GK_tcp_SendSocket.Write(&data);
                pthread_mutex_unlock(&m_Tcp_Cmd_lock);
            }
        }


    }
#endif
    env->ReleaseByteArrayElements(cmd_, cmd_b, 0);

    return 0;
}


JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naStartCheckSDStatus(JNIEnv *env, jclass type,
                                                              jboolean bStart) {

    if (bStart) {
        F_StartReadSdStatus();
    } else {
        F_StopReadSdStatus();
    }
    return 0;
}


int F_SendHttpComd(string spath) {
    LOGE("Video Cmd _Start_1");
    if (nICType == IC_GK) {
        LOGE("Video Cmd _Start_2");
        const char *host = "192.168.234.1";
        //const char *url = "http://192.168.234.1";
        int port = 80;
        int socket_handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (socket_handle < 0) {
            LOGE("Video Cmd _Start_3");
            return -1;
        }

        if(nWifiInterface!=-1) {
            setsockopt(socket_handle, IPPROTO_IP, SO_BINDTODEVICE, &nWifiInterface,
                              sizeof(nWifiInterface));
        }
        //  setsockopt(socket_handle, SOL_SOCKET, SO_BINDTODEVICE, wlan, strlen(wlan));

        struct sockaddr_in serv_add;//服务器地址
        serv_add.sin_family = AF_INET;
        serv_add.sin_addr.s_addr = inet_addr(host);
        serv_add.sin_port = htons(port);


        fd_set readset, writeset;
        struct timeval timeout;
        unsigned long ul = 1;
        int error = -1, len = sizeof(int);
        int bTimeoutFlag = 0;
        int ret = 0;

        /*Setting socket to non-blocking mode */
        ioctl(sockfd, FIONBIO, &ul);
        if (connect(socket_handle, (const struct sockaddr *) &serv_add,
                    sizeof(struct sockaddr_in)) == -1) {
            timeout.tv_sec = 3;//TIMEOUT_TIME;
            timeout.tv_usec = 0;
            FD_ZERO(&writeset);
            FD_SET(sockfd, &writeset);
            ret = select(sockfd + 1, nullptr, &writeset, nullptr, &timeout);
            if (ret == 0)              //返回0，代表在描述词状态改变已超过timeout时间
            {
                getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *) &len);
                if (error == 0)          // 超时，可以做更进一步的处理，如重试等
                {
                    bTimeoutFlag = 1;

                    LOGE("Connect timeout!");
                } else {
                    LOGE("Cann't connect to server!");
                }
                close(socket_handle);
                return -4;
            } else if (ret == -1)      // 返回-1， 有错误发生，错误原因存在于errno
            {
                LOGE("Cann't connect to server!");
                close(socket_handle);
                return -4;
            } else                      // 成功，返回描述词状态已改变的个数
            {
                LOGE("Connect success!");
                ret = 1;
            }
        } else {
            printf("Connect success!\n");
            ret = 1;
        }
        ul = 0;
        ioctl(sockfd, FIONBIO, &ul); //设置为阻塞模式
        string str =
                "GET http://192.168.234.1" + spath + " HTTP/1.1\r\nHost:192.168.234.1:80\r\n\r\n";

        char *head = (char *) str.c_str();//"GET http://192.168.234.1/web/cgi-bin/hi3510/snap.cgi?&-getpic&-chn=0 HTTP/1.1\r\nHost:192.168.234.1:80\r\n\r\n";
        DEBUG_PRINT("Cmd_Str %s", head);
        if (send(socket_handle, head, strlen(head), 0) < 0) { //发送头部
            close(socket_handle);
            LOGE("err -  abc");
            return -3;
        } else {
            LOGE("OK -  abc111");
            close(socket_handle);
            return 0;
        }
    }

    return 0;

}

JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naRemoteSnapshot(JNIEnv *env, jclass type) {
    LOGE("Video Cmd _Photo");
    if (nICType == IC_GK) {
        LOGE("Video Cmd _Photo2");
        return F_SendHttpComd("/web/cgi-bin/hi3510/snap.cgi?&-getpic&-chn=0");
    } else if (nICType == IC_GP) {

        return 0;
    }
    return -100;

}

JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naRemoteSaveVideo(JNIEnv *env, jclass type) {
    DEBUG_PRINT("Video Cmd _Video %d", nICType);
    if (nICType == IC_GK) {
        DEBUG_PRINT("Video Cmd _Video2");
        return F_SendHttpComd("/web/cgi-bin/hi3510/switchrec.cgi?-chn=11");
    } else {
        F_SD_Stop_Recrod();
        return 0;
    }
    return -100;

}

/*
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSN_1WriteFrame(JNIEnv *env, jclass type,
                                                          jbyteArray data_, jint nLen) {
    uint8_t *data = (uint8_t *) (env->GetByteArrayElements(data_, nullptr));

    m_FFMpegPlayer.F_SendFrame((uint8_t *) data, nLen);

    env->ReleaseByteArrayElements(data_, (jbyte *) data, 0);
}
*/
int F_ReadUdpPort25000();


uint8_t F_rand() {
    //int i= time(0);
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    long i = tv.tv_sec * 1000000 + tv.tv_usec;
    return (uint8_t) i;
}

JNIEXPORT jboolean JNICALL
Java_com_joyhonest_wifination_wifination_naCheckDevice(JNIEnv *env, jclass type) {
    int re = -1;
    const char *host = sServerIP.c_str();
    uint8_t msg[22];
    uint8_t mac_addr[6];
    uint8_t mac_addr_ep[6] = {0x17, 0x28, 0x39, 0x4a, 0x5b, 0x6c};
    int nRead;
    int ix;
    if (m_FFMpegPlayer.nIC_Type == IC_SN || m_FFMpegPlayer.nIC_Type == IC_GP) {
        if (m_FFMpegPlayer.nIC_Type == IC_GP)
            host = sServerIP.c_str();
        //srand(unsigned(time(0)));
        //nStep = 1;
        msg[0] = 'L'; // LEXIN
        msg[1] = 'E';
        msg[2] = 'X';
        msg[3] = 'I';
        msg[4] = 'N';
        msg[5] = 0x01;
        msg[6] = 0x0c;
        for (ix = 0; ix < 12; ix++) {
            msg[7 + ix] = F_rand();
        }
        uint16_t checksum = 0;
        for (ix = 0; ix < 19; ix++) {
            checksum += msg[ix];
        }
        msg[19] = (uint8_t) checksum;
        msg[20] = (uint8_t) (checksum >> 8);
        re = send_cmd_udp(msg, 21, host, 25000);
        nRead = F_ReadUdpPort25000();
        if (nRead != 21) {
            return false;
        }
        for (ix = 7; ix < 7 + 6; ix++) {
            mac_addr[ix - 7] = buff25000[ix] - mac_addr_ep[ix - 7];
        }
        uint32_t Encrypt_L;
        uint16_t Encrypt_H;
        Encrypt_L = (((mac_addr[0] + mac_addr[3]) ^ (mac_addr[1] + mac_addr[5]) ^
                      (mac_addr[2] + mac_addr[4]))
                     * (mac_addr[0] + mac_addr[1] + mac_addr[2] + mac_addr[3] + mac_addr[4] +
                        mac_addr[5])) + 0xbc924357;
        Encrypt_H = (((mac_addr[0] + mac_addr[3]) * (mac_addr[1] + mac_addr[5]) *
                      (mac_addr[2] + mac_addr[4])) ^ 0x2917) * 0xb329;
        msg[5] = 0x02;
        uint8_t *dat = (uint8_t *) &Encrypt_L;
        for (ix = 7; ix < 7 + 4; ix++) {
            msg[ix] = *dat;
            dat++;
        }
        dat = (uint8_t *) &Encrypt_H;

        msg[11] = *dat;
        dat++;
        msg[12] = *dat;
        for (ix = 13; ix < 13 + 6; ix++) {
            msg[ix] = F_rand();
        }
        checksum = 0;
        for (ix = 0; ix < 19; ix++) {
            checksum += msg[ix];
        }
        msg[19] = (uint8_t) checksum;
        msg[20] = (uint8_t) (checksum >> 8);
        re = send_cmd_udp(msg, 21, host, 25000);
        nRead = F_ReadUdpPort25000();
        if (nRead != 21) {
            return false;
        }
    }
    return true;


}


void F_CloseRead25000Socket() {
    if (Read_25000sock != -1) {
        close(Read_25000sock);
        Read_25000sock = -1;
        LOGE("Cloe Read_25000sock");
    }
}


int F_ReadUdpPort25000() {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(25000);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    F_CloseRead25000Socket();

    if ((Read_25000sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) < 0)) {
        LOGE("open udp:8001 error");
        return -1;
    }

    if(nWifiInterface!=-1) {
        setsockopt(Read_25000sock, IPPROTO_IP, SO_BINDTODEVICE, &nWifiInterface,
                          sizeof(nWifiInterface));
    }

    int res = (int) ::bind(Read_25000sock, (struct sockaddr *) &addr, sizeof(addr));
    if (res < 0) {
        LOGE("bind udp:8001 error");
        return -1;
    }


    struct sockaddr_in clientAddr;
    int n = -1;
    int len = sizeof(clientAddr);

    struct timeval tv_out;
    fd_set readset;
    int ret = -1;
    int error = -1;
//    while(bStartRead25000)
    {
        memset(buff25000, 0, 101 * sizeof(char));
        {
            unsigned long ul = 1;
            ioctl(Read_25000sock, FIONBIO, &ul); //设置为非阻塞模式
            ret = -1;
            tv_out.tv_sec = 1;//TIMEOUT_TIME;
            tv_out.tv_usec = 100 * 1000;
            FD_ZERO(&readset);
            FD_SET(Read_25000sock, &readset);
            n = -1;
            {
                ret = select(Read_25000sock + 1, &readset, nullptr, nullptr, &tv_out);
                if (ret == 0) {
                    getsockopt(Read_25000sock, SOL_SOCKET, SO_ERROR, &error, (socklen_t *) &len);
                    if (error == 0)          // 超时，可以做更进一步的处理，如重试等
                    {
                        //bTimeoutFlag = 1;
                        LOGE("udp25000 timeout!\n");
                    } else {
                        //LOGE("Cann't connect to server!\n");
                    }
                    ret = -4;
                } else if (ret == -1)      // 返回-1， 有错误发生，错误原因存在于errno
                {
                    LOGE("Cann't connect to server! 25000");
                    ret = -4;
                } else                      // 成功，返回描述词状态已改变的个数
                {
                    if (FD_ISSET(Read_25000sock, &readset)) {
                        socklen_t tdlen;
                        n = (int) ::recvfrom(Read_25000sock, buff25000, (size_t) 100, (int) 0,
                                             (struct sockaddr *) &clientAddr, (socklen_t *) &tdlen);
                    }
                    ret = 1;
                }
            }
            if (n > 0) {


            }
        }
    }
    F_CloseRead25000Socket();
    return n;

}
/////////////////////////



void F_CloseStream(void) {
    MySocketData data;
    T_NET_CMD_MSG Cmd;
    Cmd.type = CMD_CLOSE_STREAM;
    Cmd.session_id = session_id;
    T_NET_STREAM_CONTROL control;
    control.stream_type = nGKA_StreamNo;
    data.AppendData((uint8_t *) &Cmd, sizeof(T_NET_CMD_MSG));
    data.AppendData((uint8_t *) &control, sizeof(T_NET_STREAM_CONTROL));
    GK_tcp_SendSocket.Write(&data);
}

void DisConnect(bool bNoNormal) {
    bIsConnect = false;
    if (GK_tcp_DataSocket.bConnected) {
        GK_tcp_DataSocket.DisConnect();
    }

    if (GK_tcp_SendSocket.bConnected) {
        GK_tcp_SendSocket.DisConnect();
    }

    if (GP_tcp_VideoSocket.bConnected) {
        GP_tcp_VideoSocket.DisConnect();
    }

    if (GK_tcp_NoticeSocket.bConnected) {
        GK_tcp_NoticeSocket.DisConnect();
    }
    if (GK_tcp_SearchSocket.bConnected) {
        GK_tcp_SearchSocket.DisConnect();
    }
    bNeedExit = true;
    if (heartid != -1) {
        void *ret = nullptr;
        pthread_join(heartid, &ret);
        heartid = -1;
    }
    if (checkLinkid != -1) {
        void *ret1 = nullptr;
        pthread_join(checkLinkid, &ret1);
        checkLinkid = -1;
    }
}


int F_ReadAck(int ms) {
    req_msg.ret = -1;
    MySocketData data;
    data.MemSet(-1);
    data.nLen = sizeof(T_REQ_MSG);
    GK_tcp_SendSocket.Read(&data, ms);
    memcpy(&req_msg, data.data, sizeof(T_REQ_MSG));
    return req_msg.ret;
}

int F_Login(void) {
    MySocketData data;
    T_NET_CMD_MSG Cmd;
    Cmd.session_id = 0;
    Cmd.type = CMD_LOGIN;

    data.AppendData((uint8_t *) &Cmd, sizeof(T_NET_CMD_MSG));
    T_NET_LOGIN user;
    memset(user.passwd, 0, 100);
    memset(user.user, 0, 100);
    int nLen = strlen(sCustomer);
    if (nLen > 1) {
        memcpy(user.user, sCustomer, nLen);
    }
    data.AppendData((uint8_t *) &user, sizeof(T_NET_LOGIN));
    GK_tcp_SendSocket.Write(&data);
    return F_ReadAck(1000 * 1);

}

int F_AdjTime() {
    T_NET_CMD_MSG Cmd;
    MySocketData data;
    if (session_id == 0) {
        return -1;
    }
    Cmd.session_id = session_id;
    Cmd.type = CMD_ADJUST_TIME;
    data.AppendData((uint8_t *) &Cmd, sizeof(T_NET_CMD_MSG));


    T_NET_DATE_TIME date;
    time_t tt = time(nullptr);//这句返回的只是一个时间cuo
    tm *t = localtime(&tt);

    date.usYear = (uint16_t) (t->tm_year + 1900);
    date.usMonth = (uint16_t) (t->tm_mon + 1);
    date.usDay = (uint16_t) t->tm_mday;
    date.ucHour = (unsigned char) t->tm_hour;
    date.ucMin = (unsigned char) t->tm_min;
    date.ucSec = (unsigned char) t->tm_sec;

    data.AppendData((uint8_t *) &date, sizeof(T_NET_DATE_TIME));
    GK_tcp_SendSocket.Write(&data);
    return F_ReadAck(1000 * 1);


}

int F_OpenDataSocket() {

    if (GK_tcp_DataSocket.Connect(sServerIP.c_str(), 0x7102) < 0) {
        return -1;
    } else {
        MySocketData data;
        T_NET_CMD_MSG Cmd;
        Cmd.type = CMD_DATA_SOCK;
        Cmd.session_id = session_id;
        T_NET_STREAM_CONTROL control;
        control.stream_type = nGKA_StreamNo;
        data.AppendData((uint8_t *) &Cmd, sizeof(T_NET_CMD_MSG));
        data.AppendData((uint8_t *) &control, sizeof(T_NET_STREAM_CONTROL));
        GK_tcp_DataSocket.Write(&data);

        req_msg.ret = -1;
        data.nLen = sizeof(T_REQ_MSG);
        GK_tcp_DataSocket.Read(&data, 1000);
        memcpy(&req_msg, data.data, sizeof(T_REQ_MSG));


        if (req_msg.ret < 0) {
            GK_tcp_DataSocket.DisConnect();
        }
        return req_msg.ret;
    }

}


int F_OpenVideoStream() {

    MySocketData data;
    T_NET_CMD_MSG Cmd;
    Cmd.type = CMD_OPEN_STREAM;
    Cmd.session_id = session_id;

    T_NET_STREAM_CONTROL control;
    control.stream_type = nGKA_StreamNo;
    data.AppendData((uint8_t *) &Cmd, sizeof(T_NET_CMD_MSG));
    data.AppendData((uint8_t *) &control, sizeof(T_NET_STREAM_CONTROL));
    GK_tcp_SendSocket.Write(&data);
    return F_ReadAck(1000);


}


void F_GetData(MySocketData *data);

void F_GetData_Notice(MySocketData *data);
//void F_GetData_SN(MySocketData *data);


void F_StartReadData(void) {
    GK_tcp_DataSocket.fuc_getData = F_GetData;
    GK_tcp_DataSocket.StartReadThread();
}


void F_StartReadNotice(void) {
    GK_tcp_NoticeSocket.buffLen = sizeof(int);
    GK_tcp_NoticeSocket.fuc_getData = F_GetData_Notice;
    GK_tcp_NoticeSocket.StartReadThread();
}

int F_CloseAllStream(void) {
    MySocketData data;
    T_NET_CMD_MSG Cmd;
    T_NET_STREAM_CONTROL control;

    Cmd.type = CMD_CLOSE_STREAM;
    Cmd.session_id = session_id;

//    control.stream_type = 0;
//    data.nLen = 0;
//    data.AppendData((uint8_t*)&Cmd, sizeof(T_NET_CMD_MSG));
//    data.AppendData((uint8_t*)&control, sizeof(T_NET_STREAM_CONTROL));
//    GK_tcp_SendSocket.Write(&data);
//    F_ReadAck();
//    Cmd.type=CMD_CLOSE_STREAM;
//    Cmd.session_id = session_id;

//    control.stream_type = 1;
//    data.nLen = 0;
//    data.AppendData((uint8_t*)&Cmd, sizeof(T_NET_CMD_MSG));
//    data.AppendData((uint8_t*)&control, sizeof(T_NET_STREAM_CONTROL));
//    GK_tcp_SendSocket.Write(&data);
//    F_ReadAck();
//    Cmd.type=CMD_CLOSE_STREAM;
//    Cmd.session_id = session_id;

    control.stream_type = nGKA_StreamNo;
    data.nLen = 0;
    data.AppendData((uint8_t *) &Cmd, sizeof(T_NET_CMD_MSG));
    data.AppendData((uint8_t *) &control, sizeof(T_NET_STREAM_CONTROL));
    GK_tcp_SendSocket.Write(&data);
    return F_ReadAck(1000 * 1);

}

void F_CMD_FORCE_I() {
    MySocketData data;
    data.nLen = 0;
    T_NET_CMD_MSG Cmd;
    Cmd.session_id = session_id;
    Cmd.type = CMD_FORCE_I;

    T_NET_STREAM_CONTROL streeam;
    streeam.stream_type = nGKA_StreamNo;
    data.AppendData((uint8_t *) &Cmd, sizeof(T_NET_CMD_MSG));
    data.AppendData((uint8_t *) &streeam, sizeof(T_NET_STREAM_CONTROL));
    GK_tcp_SendSocket.Write(&data);
}

void F_AdjStatus(int nStatus);


int F_GetSDStatus() {
    MySocketData data;
    data.nLen = 0;
    T_NET_CMD_MSG Cmd;
    T_NET_CONFIG config;
    Cmd.session_id = session_id;
    Cmd.type = CMD_GET_CONFIG;
    config.type = CONFIG_SD_CARD;
    config.res = 0;
    data.AppendData((uint8_t *) &Cmd, sizeof(T_NET_CMD_MSG));
    data.AppendData((uint8_t *) &config, sizeof(T_NET_CONFIG));
    GK_tcp_SendSocket.Write(&data);

    data.nLen = sizeof(T_REQ_MSG) + sizeof(T_NET_SDCARD_INFO);
    int nLength = GK_tcp_SendSocket.Read(&data, 1000);
    if (nLength == data.nLen) {
        T_REQ_MSG *msg = (T_REQ_MSG *) data.data;
        if (msg->ret == 0) {
            T_NET_SDCARD_INFO *info = (T_NET_SDCARD_INFO *) (data.data + sizeof(T_REQ_MSG));
            return info->sd_status;
        }
    }
    return -10;
}

/*
void F_GetSDStatus() {
    MySocketData data;
    data.nLen = 0;
    T_NET_CMD_MSG Cmd;
    T_NET_CONFIG config;
    Cmd.session_id = session_id;
    Cmd.type = CMD_GET_CONFIG;
    config.type = CONFIG_SD_CARD;
    config.res = 0;
    data.AppendData((uint8_t *) &Cmd, sizeof(T_NET_CMD_MSG));
    data.AppendData((uint8_t *) &config, sizeof(T_NET_CONFIG));
    GK_tcp_SendSocket.Write(&data);

    data.nLen = sizeof(T_REQ_MSG) + sizeof(T_NET_SDCARD_INFO);
    int nLength = GK_tcp_SendSocket.Read(&data, 300);
    if (nLength == data.nLen) {
        T_REQ_MSG *msg = (T_REQ_MSG *) data.data;
        if (msg->ret == 0) {
            T_NET_SDCARD_INFO *info = (T_NET_SDCARD_INFO *) (data.data + sizeof(T_REQ_MSG));
            F_AdjStatus(info->sd_status);
        }
    }

}
 */


void *F_SentHearbeat_A(void *para) {
    int bas = 0;
    int nD = 0;
    while (bIsConnect && !bNeedExit) {
        nD++;
        if ((nD % 25) == 0) {
            if (bNeedSentHear) {
                if ((bas & 0xF) == 0) {
                    MySocketData data;
                    T_NET_CMD_MSG Msg;
                    Msg.session_id = session_id;
                    Msg.type = CMD_KEEP_LIVE;
                    data.AppendData((uint8_t *) &Msg, sizeof(T_NET_CMD_MSG));
                    pthread_mutex_lock(&m_Tcp_Cmd_lock);
                    GK_tcp_SendSocket.Write(&data);
                    pthread_mutex_unlock(&m_Tcp_Cmd_lock);
                    F_ReadAck(200);
                }
            }
        }
        usleep(1000 * 10);  //250
        bas++;
    }
    //heartid = -1;
    LOGE("Exit HeardBeat Thread!");
    return nullptr;
}


void F_StartSendHeart(void) {
    bNeedSentHear = true;
    int ret = pthread_create(&heartid, nullptr, F_SentHearbeat_A, nullptr); // 成功返回0，错误返回错误编号

}


void *F_DisplayThread(void *para) {

    return nullptr;
}


string F_GetFirewareVer() {
    T_NET_VIDEO_INFO video_info;
    T_NET_CMD_MSG Cmd;
    T_NET_CONFIG config;

    MySocketData data;
    data.nLen = 0;
    Cmd.session_id = session_id;
    Cmd.type = CMD_GET_CONFIG;
    config.type = CONFIG_VENDOR_INFO;
    config.res = 0;
    data.AppendData((uint8_t *) &Cmd, sizeof(T_NET_CMD_MSG));
    data.AppendData((uint8_t *) &config, sizeof(T_NET_CONFIG));
    GK_tcp_SendSocket.Write(&data);
    data.nLen = sizeof(T_REQ_MSG);
    int nLength = GK_tcp_SendSocket.Read(&data, 1000);
    if (nLength == data.nLen) {
        T_REQ_MSG *msg = (T_REQ_MSG *) data.data;
        memcpy(&req_msg, msg, sizeof(T_REQ_MSG));
        if (msg->ret == 0) {
            data.nLen = sizeof(GK_NET_VENDOR_CFG);
            int nLength = GK_tcp_SendSocket.Read(&data, 250);
            GK_NET_VENDOR_CFG *info = (GK_NET_VENDOR_CFG *) (data.data);
            string sver = string((char *) (info->firmware_version));
            return sver;
        }
    }
    return "";
}

T_NET_VIDEO_INFO video_info;

extern int g_sdkVersion;

int F_SetFpsBySDK(int nChancel) {
    if (video_info.width == 0) {
        LOGE("SetFPS : no info!");
        return -1;
    }

    int dfps = 20;

    if (g_sdkVersion >= 21) {
        dfps = 15;
    } else {
        dfps = 13;
    }

/*
    video_info.stream_type = 1;   // 0 为主码流， 1为子码流
    video_info.fps = dfps;           // 帧率
    video_info.i_interval = dfps;    // I 帧间隔
    video_info.brc_mode = 0;      // 0: CBR 固定码率; 1: VBR 可变码率
    video_info.cbr_avg_bps = 0;   // 固定码率
    video_info.vbr_min_bps = 0;   // 可变码率下限
    video_info.vbr_max_bps = 0;   // 可变码率上限
    video_info.width=1280;         // 分辨率宽
    video_info.height = 720;        // 分辨率高
*/
    video_info.fps = dfps;
    video_info.i_interval = dfps;
    m_FFMpegPlayer.nfps = dfps;

    {
        //SetFps 15;
        //T_NET_VIDEO_INFO  video_info;
        T_NET_CMD_MSG Cmd;
        T_NET_CONFIG config;
        MySocketData data;
        data.nLen = 0;
        Cmd.session_id = session_id;
        Cmd.type = CMD_SET_CONFIG;
        config.type = CONFIG_VIDEO;
        config.res = nChancel;
        data.AppendData((uint8_t *) &Cmd, sizeof(T_NET_CMD_MSG));
        data.AppendData((uint8_t *) &config, sizeof(T_NET_CONFIG));
        data.AppendData((uint8_t *) &video_info, sizeof(T_NET_VIDEO_INFO));
        GK_tcp_SendSocket.Write(&data);
        data.nLen = sizeof(T_REQ_MSG);
        int nLength = GK_tcp_SendSocket.Read(&data, 1000);
        if (nLength == data.nLen) {
            T_REQ_MSG *msg = (T_REQ_MSG *) data.data;
            if (msg->ret == 0) {

                LOGE_B("Set Fps = %d OK", dfps);
            }
            return msg->ret;
        }
    }
    return -1;
}

int F_GetFps(int nChancel) {

    video_info.width = 0;

    T_NET_CMD_MSG Cmd;
    T_NET_CONFIG config;

    MySocketData data;
    data.nLen = 0;
    Cmd.session_id = session_id;
    Cmd.type = CMD_GET_CONFIG;
    config.type = CONFIG_VIDEO;
    config.res = nChancel;
    data.AppendData((uint8_t *) &Cmd, sizeof(T_NET_CMD_MSG));
    data.AppendData((uint8_t *) &config, sizeof(T_NET_CONFIG));
    GK_tcp_SendSocket.Write(&data);

    data.nLen = sizeof(T_REQ_MSG);
    int nLength = GK_tcp_SendSocket.Read(&data, 1000);
    if (nLength == data.nLen) {
        T_REQ_MSG *msg = (T_REQ_MSG *) data.data;
        if (msg->ret == 0) {
            data.nLen = sizeof(T_NET_VIDEO_INFO);
            int nLength = GK_tcp_SendSocket.Read(&data, 1000);
            T_NET_VIDEO_INFO *info = (T_NET_VIDEO_INFO *) (data.data);
            LOGE_B("Get Fps = %d", info->fps);
            if (info->fps > 30) {
                info->fps = 30;
            }
            if (info->fps < 10) {
                info->fps = 10;
            }
            m_FFMpegPlayer.nfps = info->fps;
            memcpy(&video_info, info, sizeof(T_NET_VIDEO_INFO));
            return 0;
        }

    }
    return -1;
}

string sver = "0";

void F_ReadClearSocket() {
    MySocketData data;
    data.SetSize(1024);
    data.nLen = 1024;
    GK_tcp_SendSocket.Read(&data, 300);
    memcpy(&req_msg, data.data, sizeof(T_REQ_MSG));

}

void F_ClsoeSocket() {
    GK_tcp_SendSocket.DisConnect();
    GK_tcp_NoticeSocket.DisConnect();
    GK_tcp_DataSocket.DisConnect();
    bIsConnect = false;
}

int Connect_gk_c(void) {
    LOGE_B("----------Start-----111111");
    LOGE_B(" ");
    LOGE_B(" ");
    if (GK_tcp_SendSocket.Connect(sServerIP.c_str(), 0x7102) < 0) {
        return -1;
    }

    if (GK_tcp_NoticeSocket.Connect(sServerIP.c_str(), 0x7102) < 0) {
        return -2;
    }

    GK_tcp_NoticeSocket.bNotice = true;

    LOGE_B("Login");
    if (F_Login() < 0) {
        F_ClsoeSocket();
        LOGE("Error 1");
        return -1;
    }
    session_id = req_msg.session_id;


    LOGE_B("Time");
    if (F_AdjTime() < 0) {
        F_ClsoeSocket();
        LOGE("Error 2");
        return -1;
    }

    LOGE_B("CloseStream");
    if (F_CloseAllStream() < 0) {
        F_ClsoeSocket();
        LOGE_B("Error 3");
        return -1;
    }


    //F_ReadClearSocket();
    //F_GetFps(nGKA_StreamNo);
    LOGE_B("GetFps");
    if (F_GetFps(nGKA_StreamNo) < 0) {
        F_ClsoeSocket();
        LOGE_B("Error 5");
        return -1;
    }

    if (bAdjFps) {
        if (nGKA_StreamNo == 1) {
            LOGE_B("SetFps");
            if (F_SetFpsBySDK(nGKA_StreamNo) < 0) {
                F_ClsoeSocket();
                LOGE_B("Error 6");
                return -1;
            }

            LOGE_B("GetFps2");
            if (F_GetFps(nGKA_StreamNo) != 0) {
                F_ClsoeSocket();
                LOGE_B("Error 7");
                return -1;
            }
        }
    }

    LOGE_B("Get Ver");
    sver = F_GetFirewareVer(); //SYMA_X5UW
    if (req_msg.ret != 0) {
        F_ClsoeSocket();
        LOGE_B("Error 8");
        return -1;
    }
    LOGE_B("OpenDataSocket");
    if (F_OpenDataSocket() < 0) {
        F_ClsoeSocket();
        LOGE("Error 9");
        return -1;
    } else {
        F_StartReadData();
    }
    m_FFMpegPlayer.F_StartDispThread();
    LOGE_B("OpenStream");
    if (F_OpenVideoStream() < 0) {
        F_ClsoeSocket();
        LOGE_B("Error 10");
        return -1;
    }
    LOGE_B("ForceI");
    F_CMD_FORCE_I();
    if (F_ReadAck(1000) != 0) {
        F_ClsoeSocket();
        LOGE_B("Error 11");
        return -1;
    }
    bIsConnect = true;
    //if(0)
    {

        LOGE_B("Open NoticeSocket");
        MySocketData data;
        T_NET_CMD_MSG Cmd;
        Cmd.session_id = session_id;
        Cmd.type = CMD_NOTICE_SOCK;
        data.nLen = 0;
        data.AppendData((uint8_t *) &Cmd, sizeof(T_NET_CMD_MSG));
        GK_tcp_NoticeSocket.Write(&data);  //writeData:sendData withTimeout:100 tag:Tag_Notify];
        data.nLen = sizeof(T_REQ_MSG);
        data.MemSet(-1);
        GK_tcp_NoticeSocket.Read(&data, 1000);
        memcpy(&req_msg, data.data, sizeof(T_REQ_MSG));
        if (req_msg.ret != 0) {
            F_ClsoeSocket();
            LOGE_B("Error 12");
            return -1;
        }
        F_StartReadNotice();
    }
    F_StartSendHeart();


    LOGE_B("Get Sd Status");
    int nSD = 0;
    if ((nSD = F_GetSDStatus()) == -10) {
        F_ClsoeSocket();
        LOGE_B("Error 4");
        //return -1;
        req_msg.ret = 0;
    } else {
        F_AdjStatus(nSD);
    }

    LOGE_B("----------Cmd  End-----");
    bGKA_ConnOK = true;
    return req_msg.ret;
}

/*
int Connect_gk_A(void) {
    bIsConnect = false;
    if (GK_tcp_SendSocket.bConnected) {
        GK_tcp_SendSocket.DisConnect();
    }
    if (GK_tcp_DataSocket.bConnected) {
        GK_tcp_DataSocket.DisConnect();
    }
    if (GK_tcp_NoticeSocket.bConnected) {
        GK_tcp_NoticeSocket.DisConnect();
    }
    if (heartid != -1) {
        void *ret = nullptr;
        pthread_join(heartid, &ret);
        heartid = -1;
    }
    return Connect_gk_c();

}
*/

int Connect_gk_B(void) {
    //nRelinkTime = 0;
    DisConnect(true);
    return Connect_gk_c();
}

void F_GetData_SN(char *data, int nLen);
void F_DispPacket(void *data);

int Connect_GPH264(void) {
    DEBUG_PRINT("2-----");
    if (GP_tcp_VideoSocket.Connect(sServerIP, 8080) < 0) {
        return -1;
    }
    if (nICType == IC_RTLH264) {
        usleep(1000 * 100);
        F_SetnRTL_RTC_Time();
        usleep(1000 * 100);
        F_SetnRTL_RTC_Time();
    }
    if (nICType == IC_GPRTPC) {
        GP_tcp_VideoSocket.fuc_getData_mjpeg = F_GetData_SN;
        GP_tcp_VideoSocket.nICType = IC_GPRTPC;
    } else {
        GP_tcp_VideoSocket.fuc_getData = F_GetData;
        GP_tcp_VideoSocket.nICType = IC_NO;
    }
    GP_tcp_VideoSocket.StartReadThread();
    m_FFMpegPlayer.F_StartDispThread();
    return 0;
}

int Connect_gk(void) {
    int ret = Connect_gk_B();
    return ret;
}


void F_DispPacket(void *data)
{
    m_FFMpegPlayer.DispPaket(data);
}

void F_GetData(MySocketData *data) {
    m_FFMpegPlayer.DecodeH264(data);
}

void F_GetData_SN(char *data, int nLen) {
    m_FFMpegPlayer.DecodeMjpeg(data, nLen);
}


AVFrame *m3dFrame = av_frame_alloc();
AVFrame *m3dFrame_YUV420 = nullptr;

/*
#define  bit0_OnLine            1
#define  bit1_LocalRecording    2
#define  SD_Ready               4
#define  SD_Recroding           8
#define  SD_Photo               0x10
*/

void F_AdjStatus(int nStatus) {
    int status = nStatus;
    //nSDStatus &= 0xE3;
    if (status & 0x01) {
        nSDStatus |= SD_Ready;
        LOGE("SD_Ready");
    } else {
        LOGE("SD_Remove");
        nSDStatus &= (SD_Ready ^ 0xFFFF);
        nSDStatus &= (SD_SNAP ^ 0xFFFF);
        nSDStatus &= (SD_Recording ^ 0xFFFF);
    }
    if (nSDStatus & SD_Ready) {
        if (status & SD_Recording) {
            LOGE("SD_Start Recode %d", status);
            nSDStatus |= SD_Recording;
        } else {
            nSDStatus &= (SD_Recording ^ 0xFFFF);
        }
        if (status & SD_SNAP) {
            LOGE("SD_SNAP Start");
            nSDStatus |= SD_SNAP;
        } else {
            LOGE("SD_SNAP Stop");
            nSDStatus &= (SD_SNAP ^ 0xFFFF);
        }
    }
    F_SendStatus2Jave();

}


void F_GetData_Notice(MySocketData *data) {
    //nRelinkTime = 0;
    int *p = (int *) data->data;
    int nStatus = *p;
    F_AdjStatus(nStatus);

    //m_FFMpegPlayer.DecodeH264(data);
}

void F_GetData_SNA(char *data, int nLen) {
    nSDStatus |= 0x80;
    F_SendStatus2Jave();
    nSDStatus &= 0x7F;
}


JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetFlip(JNIEnv *env, jclass type, jboolean b) {

    // TODO
    if (nSDStatus & bit0_OnLine) {
        bGoble_Flip = false;
    } else {
        bGoble_Flip = b;
    }
    m_FFMpegPlayer.bFlip = b;

}

//////////// GKA

pthread_t GetDirThreadId = -1;
pthread_t GetFilesThreadId = -1;

string sDirPath;


//JH_Item *jh_item_current =nullptr;
//JH_Item *jh_item_photo  = new JH_Item();
//JH_Item *jh_item_video  = new JH_Item();




void *F_InitPhotos_Items(void *dat) {
    int nType = *((int *) dat);
    static int ret = 0;
    MySocketData data;
    SendDir2java("---start");


    T_REQ_MSG *pmsg;
    T_REQ_MSG msg;
    T_NET_CMD_MSG Cmd;
    T_NET_CONFIG config;
    T_NET_SD_SNAP_FILE_LIST dir_list;
    T_NET_SD_SNAP_DIR_INFO dir_info;
    if (nType == 0)  //Photo
    {
        //jh_item_current = jh_item_photo;
        //jh_item_current->sName="Photo";
        config.type = CONFIG_SD_SNAP_FILE_LIST;
        //jh_item_current->nType = nType;
    } else {
        //jh_item_current = jh_item_video;
        //jh_item_current->sName="Video";
        config.type = CONFIG_SD_REC_FILE_LIST;
        //jh_item_current->nType = 1;
    }
    //jh_item_current->Clean();

    if (!GK_tcp_SearchSocket.bConnected) {
        if (GK_tcp_SearchSocket.Connect(sServerIP.c_str(), 0x7102) < 0) {
            ret = -1;
            GetDirThreadId = -1;
            SendDir2java("---End");
            return &ret;
        }
        Cmd.session_id = session_id;
        Cmd.type = CMD_SEARCH_SOCK;
        data.nLen = 0;
        data.AppendData((uint8_t *) &Cmd, sizeof(T_NET_CMD_MSG));
        GK_tcp_SearchSocket.Write(&data);

    }

    int nLen;


    data.nLen = 0;
    Cmd.session_id = session_id;
    Cmd.type = CMD_GET_CONFIG;

    config.res = 0;

    dir_list.type = 255;
    dir_list.file_num = 0;
    dir_list.send_buf = 0;
    //nullptr;
    //2000-1-1 00:00:00  - 2100-1-1 00:00:00           获取所有文件
    dir_list.begin_time.dwYear = 2000;
    dir_list.begin_time.dwMonth = 1;
    dir_list.begin_time.dwDay = 1;
    dir_list.begin_time.dwHour = 0;
    dir_list.begin_time.dwMinute = 0;
    dir_list.begin_time.dwSecond = 0;

    dir_list.end_time.dwYear = 2099;
    dir_list.end_time.dwMonth = 12;
    dir_list.end_time.dwDay = 31;
    dir_list.end_time.dwHour = 23;
    dir_list.end_time.dwMinute = 59;
    dir_list.end_time.dwSecond = 59;

    data.SetSize(1024);
    GK_tcp_SearchSocket.Read(&data, 100);
    data.SetSize(1024);
    GK_tcp_SearchSocket.Read(&data, 100);
    data.Clean();
    data.AppendData((uint8_t *) &Cmd, sizeof(T_NET_CMD_MSG));
    data.AppendData((uint8_t *) &config, sizeof(T_NET_CONFIG));
    data.AppendData((uint8_t *) &dir_list, sizeof(T_NET_SD_SNAP_FILE_LIST));

    GK_tcp_SearchSocket.Write(&data);

    data.SetSize(sizeof(T_REQ_MSG));
    int nret = GK_tcp_SearchSocket.Read(&data, 5000);

    if (nret == sizeof(T_REQ_MSG)) {
        pmsg = (T_REQ_MSG *) (data.data);
        if (pmsg->ret == 0) {

            nLen = sizeof(T_NET_SD_SNAP_FILE_LIST);
            data.SetSize(nLen);
            nret = GK_tcp_SearchSocket.Read(&data, 500);
            if (nret == nLen) {
                T_NET_SD_SNAP_FILE_LIST *dir_listA = (T_NET_SD_SNAP_FILE_LIST *) data.data;
                int nCount = dir_listA->file_num;
                if (nCount > 0) {
                    nLen = sizeof(T_NET_SD_SNAP_FILE_INFO);
                    for (int ix = 0; ix < nCount; ix++) {
                        data.SetSize(nLen);
                        nret = GK_tcp_SearchSocket.Read(&data, 50);
                        T_NET_SD_SNAP_DIR_INFO *pInfo;
                        if (nret == nLen) {
                            pInfo = (T_NET_SD_SNAP_DIR_INFO *) data.data;
                            SendDir2java(pInfo->path);
                        } else {

                        }
                    }
                }
            }
        }
    }
    SendDir2java("---End");
    GK_tcp_SearchSocket.DisConnect();
    GetDirThreadId = -1;
    return nullptr;
}

JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naGetPhotoDir(JNIEnv *env, jclass type) {

    if (nICType != IC_GKA) {
        return -2;
    }
    getDir_PhotOrVideo = 0;     //Photo
    int ret = 0;
    if (GetDirThreadId < 0)
        ret = pthread_create(&GetDirThreadId, nullptr, F_InitPhotos_Items,
                             (void *) (&getDir_PhotOrVideo));  //成功返回0，错误返回错误编号
    return ret;

}

JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naGetVideoDir(JNIEnv *env, jclass type) {

    if (nICType != IC_GKA) {
        return -2;
    }
    getDir_PhotOrVideo = 1; //Video
    int ret = 0;
    if (GetDirThreadId < 0)
        ret = pthread_create(&GetDirThreadId, nullptr, F_InitPhotos_Items,
                             (void *) (&getDir_PhotOrVideo)); // 成功返回0，错误返回错误编号
    return ret;
}

JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetdispRect(JNIEnv *env, jclass type, jint w, jint h) {
    if (w != m_FFMpegPlayer.nDisplayWidth && h != m_FFMpegPlayer.nDisplayHeight) {
        m_FFMpegPlayer.nNeedRedraw = true;
        m_FFMpegPlayer.nDisplayWidth = w;
        m_FFMpegPlayer.nDisplayHeight = h;
        /*
        if (m_FFMpegPlayer.My_EncodecodecCtx != nullptr) {
            avcodec_close(m_FFMpegPlayer.My_EncodecodecCtx);
            avcodec_free_context(&m_FFMpegPlayer.My_EncodecodecCtx);
            m_FFMpegPlayer.My_EncodecodecCtx = nullptr;
        }
         */
    }
}


int F_InitFiles_Items(void *dat) {
    int nType = *((int *) dat);
    static int ret = 0;
    MySocketData data;
    SendDir2java("---start");

    int nYear = 2000;
    int nMonth = 1;
    int nDay = 1;

    T_NET_CMD_MSG Cmd;
    T_NET_CONFIG config;
    T_NET_SD_SNAP_FILE_LIST file_list;
    //T_NET_SD_SNAP_FILE_INFO fileinfo;

    MySocket_GKA serchSocket;

    if (serchSocket.Connect(sServerIP.c_str(), 0x7102) < 0) {
        ret = -1;
        GetFilesThreadId = -1;
        SendDir2java("---End");
        return -1;
    }

    Cmd.session_id = session_id;
    Cmd.type = CMD_SEARCH_SOCK;
    data.nLen = 0;
    data.AppendData((uint8_t *) &Cmd, sizeof(T_NET_CMD_MSG));
    serchSocket.Write(&data);

    Cmd.session_id = session_id;
    Cmd.type = CMD_GET_CONFIG;

    if (nType == 0)
        config.type = CONFIG_SD_SNAP_FILE_LIST;
    else
        config.type = CONFIG_SD_REC_FILE_LIST;

    config.res = 0;
    file_list.type = 255;
    file_list.file_num = 0;
    file_list.send_buf = 0;//nullptr;

    //2000-1-1 00:00:00  - 2100-1-1 00:00:00           获取所有文件
    file_list.begin_time.dwYear = 2000;
    file_list.begin_time.dwMonth = 1;
    file_list.begin_time.dwDay = 1;
    file_list.begin_time.dwHour = 0;
    file_list.begin_time.dwMinute = 0;
    file_list.begin_time.dwSecond = 0;

    file_list.end_time.dwYear = 2099;
    file_list.end_time.dwMonth = 12;
    file_list.end_time.dwDay = 31;
    file_list.end_time.dwHour = 23;
    file_list.end_time.dwMinute = 59;
    file_list.end_time.dwSecond = 59;


    data.nLen = 0;
    data.AppendData((uint8_t *) &Cmd, sizeof(T_NET_CMD_MSG));
    data.AppendData((uint8_t *) &config, sizeof(T_NET_CONFIG));
    data.AppendData((uint8_t *) &file_list, sizeof(T_NET_SD_SNAP_FILE_LIST));
    serchSocket.Write(&data);

    int nLen = sizeof(T_REQ_MSG);
    data.SetSize(nLen);
    int nret = serchSocket.Read(&data, 6000);
    if (nret == nLen) {
        T_REQ_MSG *msg = (T_REQ_MSG *) data.data;
        if (msg->ret == 0) {
            nLen = sizeof(T_NET_SD_SNAP_FILE_LIST);
            data.SetSize(nLen);
            nret = serchSocket.Read(&data, 500);
            {
                if (nret == nLen) {
                    T_NET_SD_SNAP_FILE_LIST *file = (T_NET_SD_SNAP_FILE_LIST *) data.data;
                    int nCountFile = file->file_num;
                    if (nCountFile > 0) {
                        for (int ix = 0; ix < nCountFile; ix++) {
                            nLen = sizeof(T_NET_SD_SNAP_FILE_INFO);
                            data.SetSize(nLen);
                            nret = serchSocket.Read(&data, 300);
                            if (nret == nLen) {
                                T_NET_SD_SNAP_FILE_INFO *pfileInfo = (T_NET_SD_SNAP_FILE_INFO *) data.data;
                                if (bGKACmd_UDP) {
                                    if (pfileInfo->size > 2048) {
                                        std::stringstream streean;
                                        streean << pfileInfo->size;
                                        string str = streean.str();
                                        string sName_ = pfileInfo->name;
                                        sName_ = sName_ + "--";
                                        sName_ = sName_ + str;
                                        SendDir2java(sName_.c_str());
                                    }
                                } else {
                                    std::stringstream streean;
                                    streean << pfileInfo->size;
                                    string str = streean.str();
                                    string sName_ = pfileInfo->name;
                                    sName_ = sName_ + "--";
                                    sName_ = sName_ + str;
                                    SendDir2java(sName_.c_str());
                                }
                            } else {
                                if (nret == -1)  //Socket Close
                                {
                                    break;
                                }
                            }
                        }
                    }

                }
            }
        }
    }
    serchSocket.DisConnect();
    GetFilesThreadId = -1;
    SendDir2java("---End");
    return 0;
}


JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naGetFiles(JNIEnv *env, jclass type,
                                                    jint nType) {

    getFiles_PhotoPrVideo = nType;
    if (nICType != IC_GKA) {
        return -2;
    }
    F_InitFiles_Items((void *) (&getFiles_PhotoPrVideo));
    return 0;
}


bool bGoble_3D = false;


JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSet3DA(JNIEnv *env, jclass type, jboolean b) {

    if (nSDStatus & bit0_OnLine) {
        bGoble_3D = false;
    } else {
        bGoble_3D = b;
    }
    m_FFMpegPlayer.b3D = b;
    m_FFMpegPlayer.b3DA = false;

}


JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSet3D(JNIEnv *env, jclass type, jboolean b) {


    if (nSDStatus & bit0_OnLine) {
        bGoble_3D = false;
    } else {
        bGoble_3D = b;
    }
    m_FFMpegPlayer.b3D = b;
    m_FFMpegPlayer.b3DA = b;

}

JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naGetSettings(JNIEnv *env, jclass type) {

    // TODO
    jint setting = 0;
    if (m_FFMpegPlayer.bFlip)
        setting |= 0x01;
    if (m_FFMpegPlayer.b3D)
        setting |= 0x02;

    return setting;

}

//SD卡怕照

int F_SD_Snap() {
    MySocketData data;
    data.nLen = 0;
    T_NET_CMD_MSG Cmd;
    Cmd.session_id = session_id;
    Cmd.type = CMD_SNAP_TO_SD;
    T_NET_STREAM_CONTROL contrul;
    contrul.stream_type = 0;
    data.AppendData((uint8_t *) &Cmd, sizeof(T_NET_CMD_MSG));
    data.AppendData((uint8_t *) &contrul, sizeof(T_NET_STREAM_CONTROL));
    pthread_mutex_lock(&m_Tcp_Cmd_lock);
    GK_tcp_SendSocket.Write(&data);
    pthread_mutex_unlock(&m_Tcp_Cmd_lock);
    return 0;
}

//SD卡录像
int F_SD_Start_Recrod() {
    if (nSDStatus & SD_Recording)
        return 0;

    MySocketData data;
    data.nLen = 0;
    T_NET_CMD_MSG Cmd;
    Cmd.session_id = session_id;
    Cmd.type = CMD_SD_REC_START;
    T_NET_STREAM_CONTROL contrul;
    contrul.stream_type = 0;//self.nSetStream;
    data.AppendData((uint8_t *) &Cmd, sizeof(T_NET_CMD_MSG));
    data.AppendData((uint8_t *) &contrul, sizeof(T_NET_STREAM_CONTROL));
    pthread_mutex_lock(&m_Tcp_Cmd_lock);
    GK_tcp_SendSocket.Write(&data);
    F_ReadAck(100);
    pthread_mutex_unlock(&m_Tcp_Cmd_lock);
    return 0;
}

//停止SD卡录像

int F_SD_Stop_Recrod(void) {
    //if ((nSDStatus & SD_Recording) == 0)
    //    return 0;

    MySocketData data;
    data.nLen = 0;
    T_NET_CMD_MSG Cmd;
    Cmd.session_id = session_id;
    Cmd.type = CMD_SD_REC_STOP;
    T_NET_STREAM_CONTROL contrul;
    contrul.stream_type = 0;//self.nSetStream;
    data.AppendData((uint8_t *) &Cmd, sizeof(T_NET_CMD_MSG));
    data.AppendData((uint8_t *) &contrul, sizeof(T_NET_STREAM_CONTROL));
    pthread_mutex_lock(&m_Tcp_Cmd_lock);
    GK_tcp_SendSocket.Write(&data);
    F_ReadAck(100);
    pthread_mutex_unlock(&m_Tcp_Cmd_lock);
    return 0;
}


JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naSnapPhoto(JNIEnv *env, jclass type, jstring pFileName_,
                                                     jint PhoneOrSD) {
    const char *pFileName = env->GetStringUTFChars(pFileName_, 0);
    int ret = 0;
    if ((nSDStatus & bit0_OnLine) == 0) {
        env->ReleaseStringUTFChars(pFileName_, pFileName);
        return -1;
    }

    if (nSDStatus & bit1_LocalRecording) { ;
    } else {

        if (!bRocordWHisSeted) {
            {
                m_FFMpegPlayer.F_ReSetRecordWH(m_FFMpegPlayer.nDisplayWidth,
                                               m_FFMpegPlayer.nDisplayHeight);
            }
        }

    }


    if (PhoneOrSD == 0) //Phone
    {
        ret = m_FFMpegPlayer.SaveSnapshot(pFileName);
    } else if (PhoneOrSD == 1)  //SD
    {
        if (nICType == IC_GK) {
            LOGE("Video Cmd _Photo2 GK");
            F_SendHttpComd("/web/cgi-bin/hi3510/snap.cgi?&-getpic&-chn=0");
        }
        if (nICType == IC_GKA) {
            LOGE("Video Cmd _Photo2 GKA");
            F_SD_Snap();
        }
        if (nICType == IC_GPRTSP || nICType == IC_GPRTP || nICType == IC_GPRTPB ||
            nICType == IC_GPH264 || nICType == IC_GPH264_34) {
            uint8_t msg[7];
            msg[0] = 'J';
            msg[1] = 'H';
            msg[2] = 'C';
            msg[3] = 'M';
            msg[4] = 'D';
            msg[5] = 0x00;
            msg[6] = 0x01;
            send_cmd_udp(msg, 7, sServerIP.c_str(), 20000);
        }
    } else                    //SD and Phone
    {
        ret = m_FFMpegPlayer.SaveSnapshot(pFileName);
        if (nICType == IC_GK) {
            LOGE("Video Cmd _Photo2 GK");
            F_SendHttpComd("/web/cgi-bin/hi3510/snap.cgi?&-getpic&-chn=0");
        }
        if (nICType == IC_GKA) {
            LOGE("Video Cmd _Photo2 GKA");
            F_SD_Snap();
        }
        if (nICType == IC_GPRTSP || nICType == IC_GPRTP || nICType == IC_GPRTPB ||
            nICType == IC_GPH264 || nICType == IC_GPH264_34) {
            uint8_t msg[7];
            msg[0] = 'J';
            msg[1] = 'H';
            msg[2] = 'C';
            msg[3] = 'M';
            msg[4] = 'D';
            msg[5] = 0x00;
            msg[6] = 0x01;
            send_cmd_udp(msg, 7, sServerIP.c_str(), 20000);
        }
    }
    env->ReleaseStringUTFChars(pFileName_, pFileName);
    return ret;
}


JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naStartRecordA(JNIEnv *env, jclass type,
                                                        jstring pFileName_,
                                                        jint PhoneOrSD) {

    if (PhoneOrSD == 1)  //SD
    {
        if (nICType == IC_GK) {
            if ((nSDStatus & SD_Recording) == 0)
                F_SendHttpComd("/web/cgi-bin/hi3510/switchrec.cgi?-chn=11");
        } else if (nICType == IC_GKA) {
            F_SD_Start_Recrod();
        } else {

            {
                LOGE_B("Record SD Cmd");
                uint8_t msg[7];
                msg[0] = 'J';
                msg[1] = 'H';
                msg[2] = 'C';
                msg[3] = 'M';
                msg[4] = 'D';
                msg[5] = 0x00;
                msg[6] = 0x02;
                send_cmd_udp(msg, 7, sServerIP.c_str(), 20000);
            }
        }
        return 0;
    }


    if (nSDStatus & bit1_LocalRecording) { ;//return -1;
    } else {

        if (!bRocordWHisSeted) {
            m_FFMpegPlayer.F_ReSetRecordWH(m_FFMpegPlayer.nDisplayWidth,
                                           m_FFMpegPlayer.nDisplayHeight);
        }

    }

    int nFps = m_FFMpegPlayer.nRecFps;

    int bitrate;
//    if (m_FFMpegPlayer.nRecordWidth <= 640) {
//        bitrate = 2 * 1000 * 1000;
//    } else {
//        bitrate = (6 * 1000 * 1000);
//    }


//    if(bUseX264)
//    {
//        bitrate = (2000); //Uint 1K
//        if (xjh264 != nullptr) {
//            delete xjh264;
//            xjh264 = nullptr;
//        }
//
//        xjh264 = new XJH264;
//        xjh264->initX264Encode(m_FFMpegPlayer.nRecordWidth, m_FFMpegPlayer.nRecordHeight, nFps,bitrate,
//                              H264DataCallBackFunc);
//    }
    //else
    {

        long aa = m_FFMpegPlayer.nRecordWidth * m_FFMpegPlayer.nRecordHeight;
        if (aa <= 640 * 480) {
            bitrate = (1000 * 2500);
        } else if (aa <= 1280 * 720) {
            bitrate = (1000 * 3500);
        } else if (aa <= 1920 * 1080) {
            bitrate = (1000 * 6000);
        } else  if (aa <= 2560 * 1440){
            bitrate = (1000 * 7500);
        }
        else
        {
            bitrate = (1000 * 15000);
        }

        //bitrate =m_FFMpegPlayer.nRecordWidth* m_FFMpegPlayer.nRecordHeight*2;
        if (!myMediaCoder.F_InitEncoder(m_FFMpegPlayer.nRecordWidth, m_FFMpegPlayer.nRecordHeight,
                                        bitrate, nFps)) {
            return -1;
        }
    }

/*
    const char *pFileName = env->GetStringUTFChars(pFileName_, 0);
    bool bH264 = true;
    int ret = 0;
    if ((nSDStatus & bit0_OnLine) == 0) {
        env->ReleaseStringUTFChars(pFileName_, pFileName);
        return -1;
    }
*/
    if (PhoneOrSD == 0) //Phone
    {
        LOGE_B("Record Phone Cmd");
        if (nSDStatus & bit1_LocalRecording) { ;
        } else {

            //int ret = m_FFMpegPlayer.SaveVideo(pFileName, bH264);
            int ret = m_FFMpegPlayer.SaveVideo(nullptr, true);

            nSDStatus |= bit1_LocalRecording;
            F_SendStatus2Jave();
        }
    } else                    //SD and Phone
    {
        if (nSDStatus & bit1_LocalRecording) { ;

        } else {

            LOGE_B("Record Phone & sd Cmd 1");
            //int ret = m_FFMpegPlayer.SaveVideo(pFileName, bH264);
            int ret = m_FFMpegPlayer.SaveVideo(nullptr, true);
            nSDStatus |= bit1_LocalRecording;
            F_SendStatus2Jave();
        }


        if (nICType == IC_GK) {
            if ((nSDStatus & SD_Recording) == 0)
                F_SendHttpComd("/web/cgi-bin/hi3510/switchrec.cgi?-chn=11");
        } else if (nICType == IC_GKA) {
            F_SD_Start_Recrod();
        } else {
            //
            LOGE_B("Record Phone & sd Cmd 2");
            uint8_t msg[7];
            msg[0] = 'J';
            msg[1] = 'H';
            msg[2] = 'C';
            msg[3] = 'M';
            msg[4] = 'D';
            msg[5] = 0x00;
            msg[6] = 0x02;
            send_cmd_udp(msg, 7, sServerIP.c_str(), 20000);
        }
    }
    //env->ReleaseStringUTFChars(pFileName_, pFileName);
    //return ret;
    return 0;

}


JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naStopRecord(JNIEnv *env, jclass type, jint nType) {

    //TODO
    if (nType == 0) //Phone
    {
        m_FFMpegPlayer.StopSaveVideo();
        nSDStatus &= (bit1_LocalRecording ^ 0xFF);
        F_CloseEncoder();
    } else if (nType == 1)  //SD
    {
        if (nICType == IC_GK) {
            if (nSDStatus & SD_Recording)
                F_SendHttpComd("/web/cgi-bin/hi3510/switchrec.cgi?-chn=11");
        } else if (nICType == IC_GKA) {
            F_SD_Stop_Recrod();
        } else //if(nType == IC_GPRTSP || nICType == IC_GPRTP || nICType == IC_GPRTPB || nType == IC_GPH264)
        {

            LOGE_B("Stop SD Cmd");
            uint8_t msg[7];
            msg[0] = 'J';
            msg[1] = 'H';
            msg[2] = 'C';
            msg[3] = 'M';
            msg[4] = 'D';
            msg[5] = 0x00;
            msg[6] = 0x03;
            send_cmd_udp(msg, 7, sServerIP.c_str(), 20000);
        }
        nSDStatus = nSDStatus & (SD_Recording ^ 0xFFFF);
        F_SendStatus2Jave();
        return;

    } else                    //SD and Phone
    {
        m_FFMpegPlayer.StopSaveVideo();

        if (nICType == IC_GK) {
            if (nSDStatus & SD_Recording)
                F_SendHttpComd("/web/cgi-bin/hi3510/switchrec.cgi?-chn=11");
        } else if (nICType == IC_GKA) {
            F_SD_Stop_Recrod();
        } else //if (nType == IC_GPRTSP || nICType == IC_GPRTP || nICType == IC_GPRTPB) {
        {
            //if(nSdStatus_GP & 0x0100)
            {
                LOGE_B("Stop SD Cmd 111");
                uint8_t msg[7];
                msg[0] = 'J';
                msg[1] = 'H';
                msg[2] = 'C';
                msg[3] = 'M';
                msg[4] = 'D';
                msg[5] = 0x00;
                msg[6] = 0x03;
                send_cmd_udp(msg, 7, sServerIP.c_str(), 20000);
            }
        }
        nSDStatus &= (bit1_LocalRecording ^ 0xFF);
        F_CloseEncoder();

    }
    usleep(1000 * 100);
    F_SetRecordAudio(0);

    myMediaCoder.F_CloseEncoder();
    F_SendStatus2Jave();
}

void F_StopRecordAll() {

    if (nICType == IC_GK) {
        DEBUG_PRINT("Video Cmd _Video2");
        if (nSDStatus & SD_Recording)
            F_SendHttpComd("/web/cgi-bin/hi3510/switchrec.cgi?-chn=11");
    } else if (nICType == IC_GKA) {
        F_SD_Stop_Recrod();
    } else
//        if (nICType == IC_GPRTSP || nICType == IC_GPRTP || nICType == IC_GPRTPB ||
//               nICType == IC_GPH264 || nICType == IC_GPH264A || nICType == IC_RTLH264 ||
//               nICType == IC_GPH264_34)
    {
        uint8_t msg[7];
        msg[0] = 'J';
        msg[1] = 'H';
        msg[2] = 'C';
        msg[3] = 'M';
        msg[4] = 'D';
        msg[5] = 0x00;
        msg[6] = 0x03;
        send_cmd_udp(msg, 7, sServerIP.c_str(), 20000);

    }
    m_FFMpegPlayer.StopSaveVideo();
    nSDStatus &= (bit1_LocalRecording ^ 0xFF);
    F_SendStatus2Jave();
    myMediaCoder.F_CloseEncoder();
    F_CloseEncoder();
    F_SetRecordAudio(0);

}

JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naStopRecord_1All(JNIEnv *env, jclass type) {
    F_StopRecordAll();
    return 0;

}

JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naGetSessionId(JNIEnv *env, jclass type) {

    return session_id;

}


JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetCustomer(JNIEnv *env, jclass type,
                                                       jstring sCustomer_) {
    const char *sCustomerA = env->GetStringUTFChars(sCustomer_, 0);
    int nLen = strlen(sCustomerA);
    if (nLen > 254)
        return;

    memcpy(sCustomer, sCustomerA, (size_t) nLen);

    // TODO

    env->ReleaseStringUTFChars(sCustomer_, sCustomerA);
}

JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetDirectBuffer(JNIEnv *env, jclass type,
                                                           jobject bufferA,
                                                           jint nLen) {
    buffer = (unsigned char *) env->GetDirectBufferAddress(bufferA);
    m_FFMpegPlayer.Rgbabuffer = buffer;
    nBufferLen = nLen - 2048;
    cmd_buffer = buffer + nBufferLen;
    memset(cmd_buffer, 0, 2048);

}

JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetDirectBufferYUV(JNIEnv *env, jclass type,
                                                              jobject bufferA,
                                                              jint nLen) {

    bufferYUV = (unsigned char *) env->GetDirectBufferAddress(bufferA);
    m_FFMpegPlayer.YUVbuffer = bufferYUV;
    //nBufferLenYUV = nLen;

}

JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetFollow(JNIEnv *env, jclass type, jboolean bFollowA) {

    m_FFMpegPlayer.bFollow = bFollowA;
    //bFollow = bFollowA;
}

JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetContinue(JNIEnv *env, jclass type) {
    m_FFMpegPlayer.bContinue = true;
    // TODO

}

#if 0
jstring stoJstring(JNIEnv* env, const char* pat)
{
    //定义java String类 strClass
    jclass strClass = env->FindClass("java/lang/String");
    //获取String(byte[],String)的构造器,用于将本地byte[]数组转换为一个新String
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    //建立byte数组
    jbyteArray bytes = env->NewByteArray(strlen(pat));
    //将char* 转换为byte数组
    (env)->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte*) pat);
    // 设置String, 保存语言类型,用于byte数组转换至String时的参数
    jstring encoding = env->NewStringUTF("GB2312");

    //将byte数组转换为java String,并输出
    jstring  str =  (jstring)env->NewObject(strClass, ctorID, bytes, encoding);
    env->DeleteLocalRef(bytes);
    env->DeleteLocalRef(encoding);
    return str;
    /*
    jclass strClass = env->FindClass("Ljava/lang/String;");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jbyteArray bytes = env->NewByteArray(strlen(pat));
    env->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte*)pat);
    jstring encoding = env->NewStringUTF("utf-8");
    return (jstring)env->NewObject(strClass, ctorID, bytes, encoding);
*/

}
#endif


int SendThumb2java(uint8_t *dat, uint32_t nLen, const char *sName) {
    int needsDetach = 0;
    if (sName == nullptr)
        return -2;
    JNIEnv *evn = getJNIEnv(&needsDetach);
    if (evn == nullptr) {
        return -1;
    }
    int nCount = strlen(sName);
    if (nCount > 0) {
        //jstring  str = stoJstring(evn,sName);
        jstring str = evn->NewStringUTF(sName);
        jbyte *jy = nullptr;
        if (dat == nullptr) {
            nLen = 0;
            if (GetThunb_mid != nullptr) {
                evn->CallStaticVoidMethod(objclass, GetThunb_mid, nullptr, str);
            }
        } else {
            jy = (jbyte *) dat;

            jbyteArray jbarray = evn->NewByteArray(nLen);
            evn->SetByteArrayRegion(jbarray, 0, nLen, jy);

            if (GetThunb_mid != nullptr) {
                evn->CallStaticVoidMethod(objclass, GetThunb_mid, jbarray, str);
            }
            evn->DeleteLocalRef(jbarray);
        }
        evn->DeleteLocalRef(str);

    }
    if (needsDetach)
        gJavaVM->DetachCurrentThread();
    return 0;


}


int F_GetThumb(uint8_t *data, uint32_t nLen, const char *filename) {
    MySocketData *imagedata = new MySocketData;
    AVPacket packet_abc;
    av_new_packet(&packet_abc, nLen);


    memcpy(packet_abc.data, data, nLen);
    AVFrame *m_decodedFrame_abc = av_frame_alloc();
    AVCodec *codec_abc = avcodec_find_decoder(AV_CODEC_ID_H264);
    AVCodecContext *m_codecCtx_abc = avcodec_alloc_context3(codec_abc);
    m_codecCtx_abc->codec_id = AV_CODEC_ID_H264;
    struct SwsContext *img_convert_ctxBmp_abc;
    AVFrame *pFrameRGB_abc;

    int ret = avcodec_open2(m_codecCtx_abc, codec_abc, nullptr);

    uint8_t *bufffff = nullptr;
    uint32_t nDataLen = 0;


    if (ret == 0) {

        if (avcodec_send_packet(m_codecCtx_abc, &packet_abc) == 0) {
            if (avcodec_receive_frame(m_codecCtx_abc, m_decodedFrame_abc) != 0) {
                ret = -1;
            } else {
                ret = 0;
            }
        } else {
            ret = -1;
        }
        if (ret < 0) {
            SendThumb2java(nullptr, 0, filename);
            return -1;
        }

        /*
        int frameFinished;
        ret = avcodec_decode_video2(m_codecCtx_abc, m_decodedFrame_abc, &frameFinished,
                                    &packet_abc);
        if (ret < 0) {
            SendThumb2java(nullptr, 0, filename);
            return -1;
        }
         */

        img_convert_ctxBmp_abc = sws_getContext(m_codecCtx_abc->width, m_codecCtx_abc->height,
                                                AV_PIX_FMT_YUV420P,
                                                160,// m_codecCtx_abc->width / 2,
                                                90,//m_codecCtx_abc->height / 2,
                                                AV_PIX_FMT_RGBA,
                                                SWS_BILINEAR, nullptr, nullptr, nullptr); //
        pFrameRGB_abc = av_frame_alloc();

        pFrameRGB_abc->format = AV_PIX_FMT_RGBA;
        pFrameRGB_abc->width = 160;//m_codecCtx_abc->width / 2; 160*4
        pFrameRGB_abc->height = 90;//m_codecCtx_abc->height / 2;
        av_image_alloc(pFrameRGB_abc->data, pFrameRGB_abc->linesize, pFrameRGB_abc->width,
                       pFrameRGB_abc->height,
                       AV_PIX_FMT_RGBA, aligD);
        sws_scale(img_convert_ctxBmp_abc,
                  (const uint8_t *const *) m_decodedFrame_abc->data,
                  m_decodedFrame_abc->linesize, 0, m_decodedFrame_abc->height,
                  pFrameRGB_abc->data, pFrameRGB_abc->linesize);
        ret = 0;
        if (ret == 0) {
            bufffff = pFrameRGB_abc->data[0];
            nDataLen = (uint32_t) (pFrameRGB_abc->linesize[0] * pFrameRGB_abc->height);
            SendThumb2java(bufffff, nDataLen, filename);
        }
        avcodec_free_context(&m_codecCtx_abc);
        av_freep(&pFrameRGB_abc->data[0]);
        av_frame_free(&pFrameRGB_abc);
        av_frame_free(&m_decodedFrame_abc);
        av_packet_unref(&packet_abc);
        return 0;
    } else {
        avcodec_free_context(&m_codecCtx_abc);
        av_frame_free(&m_decodedFrame_abc);
        SendThumb2java(nullptr, 0, filename);
    }
    return -1;
}

#if 0
vector<pthread_t> vec;


void F_ClearThreadID(pthread_t thread_id) {
    vector<pthread_t>::iterator it;
    for (it = vec.begin(); it != vec.end(); it++) {

        if (thread_id == *it) {
            vec.erase(it);
            break;
        }
    }
}

#endif


std::string GetLastPathName(std::string in) {
    std::string::size_type index = in.find_last_of('/');
    if (index == -1)
        return in;
    return std::string(in.begin() + index + 1, in.end());
}

std::string GetExtensionName(std::string in) {
    std::string::size_type index = in.find_last_of('.');
    return std::string(in.begin() + index + 1, in.end());
}


JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naSetMenuFilelanguage(JNIEnv *env, jclass type,
                                                               jint nLanguage) {

    // TODO
    uint8_t n = (uint8_t) nLanguage;

    F_GetServerIP();

    int i = 0;
    cmd[i++] = 'U';
    cmd[i++] = 'D';
    cmd[i++] = 'P';
    cmd[i++] = 'S';
    cmd[i++] = 'O';
    cmd[i++] = 'C';
    cmd[i++] = 'K';
    cmd[i++] = 'E';
    cmd[i++] = 'T';
    cmd[i++] = n;
    send_cmd_udp(cmd, i, sServerIP.c_str(), 25010);
    return 0;
}


int PackLen = 1024 * 256;


void F_CallDownloadCallBack(int nPercentage, const char *sFileName, int nError) {
    int needsDetach = 0;
    JNIEnv *evn = getJNIEnv(&needsDetach);
    if (evn == nullptr) {
        return;
    }
    if (GetDownLoad_mid != nullptr) {
        jstring str = evn->NewStringUTF(sFileName);
        evn->CallStaticVoidMethod(objclass, GetDownLoad_mid, nPercentage, str, nError);
        evn->DeleteLocalRef(str);
    }
    if (needsDetach)
        gJavaVM->DetachCurrentThread();
}


std::list<MyDownLoad_GKA *> DownLoadSocketList;

std::list<MySocket_GKA *> DownThmbSocketList;


JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naDownloadFile(JNIEnv *env, jclass type, jstring sPath_,
                                                        jstring dPath_) {
    const char *sPath = env->GetStringUTFChars(sPath_, 0);
    const char *dPath = env->GetStringUTFChars(dPath_, 0);
    MyDownLoad_GKA *download = new MyDownLoad_GKA();
    DownLoadSocketList.push_back(download);
    int ret = download->DownLoadFile(sPath, dPath, session_id);
    env->ReleaseStringUTFChars(sPath_, sPath);
    env->ReleaseStringUTFChars(dPath_, dPath);
    return ret;
}


#if 1

JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naCancelDownload(JNIEnv *env, jclass type) {

    list<MyDownLoad_GKA *>::iterator it;
    for (it = DownLoadSocketList.begin(); it != DownLoadSocketList.end(); it++) {
        MyDownLoad_GKA *download = *it;
        download->CancelDownLod();
    }
    DownLoadSocketList.clear();
    return 0;

}

#endif

#if 0
void *F_GetThumb_(void *dat) {
    Thumb_threadINFO *info = (Thumb_threadINFO *) dat;
    pthread_t thread_id = info->Pthread_id;
    char *filename = (char *) info->str;
    static int ret = 0;
    while (thread_id == -1) { ;
    }
    if (thread_id == -2)
        return nullptr;
    uint32_t nLen = strlen(filename);
    if (nLen > 39) {
        ret = -1;
        F_ClearThreadID(thread_id);
        SendThumb2java(nullptr, 0, filename);
        delete info;
        return &ret;
    }

    MySocketData data;

    T_NET_CMD_MSG Cmd;
    T_NET_CONFIG config;
    MySocket_GKA serchSocket;

    if (serchSocket.Connect("175.16.10.2", 0x7102) < 0) {
        ret = -1;
        F_ClearThreadID(thread_id);
        SendThumb2java(nullptr, 0, filename);
        delete info;
        return &ret;
    }


    T_REQ_MSG msg;

    unsigned char *imgdat = nullptr;
    int nret;
    T_NET_SD_REC_THUMB_LIST thumb;
    memset(thumb.file_name, 0, 40);
    memcpy(thumb.file_name, filename, nLen);
    thumb.send_buf = 0;
    thumb.thumb_size = 0;

    Cmd.session_id = session_id;
    Cmd.type = CMD_SEARCH_SOCK;
    serchSocket.Write(&data);
    usleep(3000);


    Cmd.session_id = session_id;
    Cmd.type = CMD_GET_CONFIG;
    config.type = CONFIG_SD_GET_REC_THUMB;
    config.res = 0;

    data.nLen = 0;
    data.AppendData((uint8_t *) &Cmd, sizeof(T_NET_CMD_MSG));
    data.AppendData((uint8_t *) &config, sizeof(T_NET_CONFIG));
    serchSocket.Write(&data);
    data.nLen = 0;
    data.AppendData((uint8_t *) &thumb, sizeof(T_NET_SD_REC_THUMB_LIST));
    serchSocket.Write(&data);
    usleep(3000);

    nLen = sizeof(T_REQ_MSG);
    data.SetSize(nLen);
    nret = serchSocket.Read(&data, 2100);
    if (nret == nLen) {
        memcpy(&msg, data.data, nLen);
        if (msg.ret == 0) {
            memset(thumb.file_name, 0, 40);
            thumb.send_buf = 0;
            thumb.thumb_size = 0;
            nLen = sizeof(T_NET_SD_REC_THUMB_LIST);
            data.SetSize(nLen);
            nret = serchSocket.Read(&data, 2000);
            if (nret == nLen) {
                memcpy(&thumb, data.data, nLen);
                nLen = thumb.thumb_size;
                data.SetSize(nLen);
                nret = serchSocket.Read(&data, 5000);
                if (nret == nLen && nret > 0) {
                    F_GetThumb(data.data, nLen, filename);
                    serchSocket.DisConnect();
                    ret = 0;
                    F_ClearThreadID(thread_id);
                    delete info;
                    return &ret;
                }
            }
        }
    }
    serchSocket.DisConnect();
    ret = 0;
    F_ClearThreadID(thread_id);
    SendThumb2java(nullptr, 0, filename);
    delete info;
    return &ret;

}
#endif

JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naGetThumb(JNIEnv *env, jclass type, jstring filename_) {
    const char *filenameB = env->GetStringUTFChars(filename_, 0);
    if (nICType != IC_GKA) {
        env->ReleaseStringUTFChars(filename_, filenameB);
        return -100;
    }
    string sd = GetLastPathName(filenameB);
    const char *filename = sd.c_str();
    uint32_t nLen = strlen(filename);
    if (nLen > 39) {
        //SendThumb2java(nullptr, 0, filename);
        env->ReleaseStringUTFChars(filename_, filenameB);
        return -1;
    }

    MySocketData data;

    T_REQ_MSG msg;
    T_NET_CMD_MSG Cmd;
    T_NET_CONFIG config;
    T_NET_SD_REC_THUMB_LIST thumb;

    MySocket_GKA *serchSocket = new MySocket_GKA();

    if (serchSocket->Connect(sServerIP.c_str(), 0x7102) < 0) {
        env->ReleaseStringUTFChars(filename_, filenameB);
        return -1;
    }


    DownThmbSocketList.push_back(serchSocket);

    Cmd.session_id = session_id;
    Cmd.type = CMD_SEARCH_SOCK;
    data.nLen = 0;
    data.AppendData((uint8_t *) &Cmd, sizeof(T_NET_CMD_MSG));
    serchSocket->Write(&data);

    Cmd.session_id = session_id;
    Cmd.type = CMD_GET_CONFIG;
    config.type = CONFIG_SD_GET_REC_THUMB;
    config.res = 0;

    data.nLen = 0;
    data.AppendData((uint8_t *) &Cmd, sizeof(T_NET_CMD_MSG));
    data.AppendData((uint8_t *) &config, sizeof(T_NET_CONFIG));
    serchSocket->Write(&data);


    memset(&thumb, 0, sizeof(T_NET_SD_REC_THUMB_LIST));
    memcpy(thumb.file_name, filename, nLen);

    data.nLen = 0;
    data.AppendData((uint8_t *) &thumb, sizeof(T_NET_SD_REC_THUMB_LIST));

    serchSocket->Write(&data);


    nLen = sizeof(T_REQ_MSG);
    data.SetSize(nLen);
    int nret = serchSocket->Read(&data, 2500);
    if (nret == nLen) {
        memcpy(&msg, data.data, nLen);
        if (msg.ret == 0) {
            memset(&thumb, 0, sizeof(T_NET_SD_REC_THUMB_LIST));
            nLen = sizeof(T_NET_SD_REC_THUMB_LIST);
            data.SetSize(nLen);
            nret = serchSocket->Read(&data, 2100);
            if (nret == nLen) {
                memcpy(&thumb, data.data, nLen);
                nLen = thumb.thumb_size;
                data.SetSize(nLen);
                nret = serchSocket->Read(&data, 8000);
                if (nret == nLen && nret > 0) {
                    F_GetThumb(data.data, nLen, filename);
                    serchSocket->DisConnect();
                    env->ReleaseStringUTFChars(filename_, filenameB);
                    return 0;
                }
            }
        }
    }
    serchSocket->DisConnect();
    env->ReleaseStringUTFChars(filename_, filenameB);
    return -1;

}

JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naCancelGetThumb(JNIEnv *env, jclass type) {

    list<MySocket_GKA *>::iterator it;

    for (it = DownThmbSocketList.begin(); it != DownThmbSocketList.end(); it++) {
        MySocket_GKA *socket = *it;
        socket->DisConnect();
    }
    DownThmbSocketList.clear();
    return 0;

}


JNIEXPORT jboolean JNICALL
Java_com_joyhonest_wifination_wifination_isPhoneRecording(JNIEnv *env, jclass type) {

    if (nSDStatus & bit1_LocalRecording)
        return true;
    else
        return false;

}


JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naGetGP_1RTSP_1Status(JNIEnv *env, jclass type) {


    F_GetServerIP();
    //F_StartReadUdp20000();
    uint8_t msg[20];
    {
//        msg[0] = 'J';
//        msg[1] = 'H';
//        msg[2] = 'C';
//        msg[3] = 'M';
//        msg[4] = 'D';
//        msg[5] = 0x10;
//        msg[6] = 0x00;
//        send_cmd_udp(msg, 7, sServerIP.c_str(), 20000);
//        usleep(1000 * 20);

        msg[0] = 'J';
        msg[1] = 'H';
        msg[2] = 'C';
        msg[3] = 'M';
        msg[4] = 'D';
        msg[5] = 0x20;
        msg[6] = 0x00;
        msg[7] = 0x00;
        msg[8] = 0x00;
        msg[9] = 0x00;
        msg[10] = 0x00;

        send_cmd_udp(msg, 11, sServerIP.c_str(), 20000);
        usleep(1000 * 20);

    }

    return 0;

}


#if 1

void *doReceive_Udp20000(void *dat);

void *doReceive_rtp(void *dat);

void *doProgress_rtp(void *dat);

void F_SentRTPStop() {
    uint8_t msg[7];
    msg[0] = 'J';
    msg[1] = 'H';
    msg[2] = 'C';
    msg[3] = 'M';
    msg[4] = 'D';
    msg[5] = 0xD0;
    msg[6] = 0x02;
    send_cmd_udp(msg, 7, sServerIP.c_str(), 20000);
}

void F_SentRTPHeartBeep() {
    uint8_t msg[7];
    msg[0] = 'J';
    msg[1] = 'H';
    msg[2] = 'C';
    msg[3] = 'M';
    msg[4] = 'D';
    msg[5] = 0xD0;
    msg[6] = 0x01;
    send_cmd_udp(msg, 7, sServerIP.c_str(), 20000);
}


void F_Rec_RTP_Data_Service() {


    //F_StartReadUdp20000();

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 1000 * 1;
    struct sockaddr_in myaddr;
//    if (GPRTP_rev_thread != -1) {
//        return;
//    }

    if (GPRTP_UDP_SOCKET > 0) {
        close(GPRTP_UDP_SOCKET);
        GPRTP_UDP_SOCKET = -1;
        //return;
    }

    GPRTP_UDP_SOCKET = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);


    if(nWifiInterface!=-1) {
        setsockopt(GPRTP_UDP_SOCKET, IPPROTO_IP, SO_BINDTODEVICE, &nWifiInterface,
                          sizeof(nWifiInterface));
    }

    /*
    if (GPRTP_UDP_SOCKET > 0) {
        setsockopt(GPRTP_UDP_SOCKET, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    } else {
        return;
    }
     */
    int value = 1;
    int status;

    status = setsockopt(GPRTP_UDP_SOCKET, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));
    status = setsockopt(GPRTP_UDP_SOCKET, SOL_SOCKET, SO_REUSEPORT, &value, sizeof(value));


    /*
    int nRecvBuf = 400 * 1024;       //设置成
    int len = sizeof(nRecvBuf);

    setsockopt(GPRTP_UDP_SOCKET, SOL_SOCKET, SO_RCVBUF, (const char *) &nRecvBuf, sizeof(nRecvBuf));
     */

    bzero((char *) &myaddr, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(10900);
    int res = (int) ::bind(GPRTP_UDP_SOCKET, (struct sockaddr *) &myaddr, sizeof(myaddr));
    if (res < 0) {
        DEBUG_PRINT("10900 bind failed!");
        shutdown(GPRTP_UDP_SOCKET, 0);
        close(GPRTP_UDP_SOCKET);
        GPRTP_UDP_SOCKET = -1;
        return;
    }
    int ret = -1;
    if (GPRTP_rev_thread == -1) {
        ret = pthread_create(&GPRTP_rev_thread, nullptr, doReceive_rtp,
                             (void *) nullptr); // 成功返回0，错误返回错误编号
        if (ret != 0) {
            GPRTP_rev_thread = -1;
        }
    }

}

//void F_RecRP_RTSP_Status_Service()
void F_StartReadUdp20000()
{
    struct timeval tv;

    tv.tv_sec = 0;
    tv.tv_usec = 1000 * 10;
    struct sockaddr_in myaddr;

    if (udp_socket20000 > 0) {
        return;
    }

    udp_socket20000 = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if(nWifiInterface!=-1) {
        setsockopt(udp_socket20000, IPPROTO_IP, SO_BINDTODEVICE, &nWifiInterface,sizeof(nWifiInterface));
    }

    bzero((char *) &myaddr, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(20000);

    int value = 1;
    int status;
    //status = setsockopt(udp_socket20000, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));
    //status = setsockopt(udp_socket20000, SOL_SOCKET, SO_REUSEPORT, &value, sizeof(value));
    int ad = sizeof(myaddr);
    int ac = sizeof(struct sockaddr);

    int res = (int) ::bind(udp_socket20000, (struct sockaddr *) &myaddr, sizeof(myaddr));

    if (res < 0) {
        DEBUG_PRINT("udp_socket20000 bind failed!");
        shutdown(udp_socket20000, 0);
        close(udp_socket20000);
        udp_socket20000 = -1;
        bNeedRead20000 = false;
        return;
    }

    int ret = -1;
    //if (rev_Udp_thread20000 == -1)
    {
        ret = pthread_create(&rev_Udp_thread20000, nullptr, doReceive_Udp20000,
                             (void *) nullptr); // 成功返回0，错误返回错误编号
        if (ret != 0) {
            close(udp_socket20000);
            udp_socket20000 = -1;
            rev_Udp_thread20000 = -1;
            bNeedRead20000 = false;
        }
    }
}



bool less_second(const myRTP *m1, const myRTP *m2) {
    return m1->nIndex < m2->nIndex;
}


#define   buffer_Len  1450

uint8_t readRtpBuffer[buffer_Len];
char *jpgbuffer = nullptr;


///
JPEG_BUFFER jpg0;
JPEG_BUFFER jpg1;
JPEG_BUFFER jpg2;


JPEG_BUFFER *F_ClearJpegBuffer();

JPEG_BUFFER *F_FindJpegBuffer(int njpginx) {
    if (jpg0.nJpegInx == njpginx)
        return &jpg0;
    else if (jpg1.nJpegInx == njpginx)
        return &jpg1;
    else if (jpg2.nJpegInx == njpginx)
        return &jpg2;
    else if (jpg0.nJpegInx == 0) {
        jpg0.Clear();
        return &jpg0;
    } else if (jpg1.nJpegInx == 0) {
        jpg1.Clear();
        return &jpg1;
    } else if (jpg2.nJpegInx == 0) {
        jpg2.Clear();
        return &jpg2;
    } else
        return F_ClearJpegBuffer(); //清除最下inx的JPEG

}


int m_InxArray[3];

void sortA2() {
    int i, j, temp;
    int length = 3;
    for (i = 0; i < length; ++i) {
        for (j = length - 1; j > i; --j) {
            if (m_InxArray[j] < m_InxArray[j - 1]) {
                temp = m_InxArray[j];
                m_InxArray[j] = m_InxArray[j - 1];
                m_InxArray[j - 1] = temp;
            }
        }
    }
}


JPEG_BUFFER *F_ClearJpegBuffer() {
    int ix0 = jpg0.nJpegInx;
    int ix1 = jpg1.nJpegInx;
    int ix2 = jpg2.nJpegInx;



//    if (ix0 > ix1) {
//        if (ix1 > ix2) {
//            jpg2.Clear();
//            return &jpg2;
//        } else {
//            jpg1.Clear();
//            return &jpg1;
//        }
//    } else {
//        if (ix0 < ix2) {
//            jpg0.Clear();
//            return &jpg0;
//        } else {
//            jpg2.Clear();
//            return &jpg2;
//        }
//    }



    m_InxArray[0] = ix0;
    m_InxArray[1] = ix1;
    m_InxArray[2] = ix2;
    sortA2();

    if (m_InxArray[0] == ix0) {
        jpg0.Clear();
        return &jpg0;
    } else if (m_InxArray[0] == ix1) {
        jpg1.Clear();
        return &jpg1;
    } else {
        jpg2.Clear();
        return &jpg2;
    }

}


list<myRTP *> list0;
list<myRTP *> list1;
list<myRTP *> list2;
list<myRTP *> list3;
list<myRTP *> list4;

list<myRTP *> *listA = nullptr;

void F_ClearList(list<myRTP *> *listB) {
    list<myRTP *>::iterator it;
    if (listB != nullptr) {
        while (!listB->empty()) {
            it = listB->begin();
            if ((*it) != nullptr) {
                delete (*it);
                (*it) = nullptr;
            }
            listB->pop_front();
        }
        listB->clear();
    }
}

int nMinxInx = 0;


void Swap(uint32_t A[], int i, int j) {
    int temp = A[i];
    A[i] = A[j];
    A[j] = temp;
}

void BubbleSort(uint32_t A[], int n) {
    for (int j = 0; j < n - 1; j++)         // 每次最大元素就像气泡一样"浮"到数组的最后
    {
        for (int i = 0; i < n - 1 - j; i++) // 依次比较相邻的两个元素,使较大的那个向后移
        {
            if (A[i] > A[i + 1])            // 如果条件改成A[i] >= A[i + 1],则变为不稳定的排序算法
            {
                Swap(A, i, i + 1);
            }
        }
    }
}

list<myRTP *> *FindList(uint32_t ix) {
    uint32_t id0 = 0;
    uint32_t id1 = 0;
    uint32_t id2 = 0;
    uint32_t id3 = 0;
    uint32_t id4 = 0;


    if (list0.empty())
        return &list0;
    else if (list1.empty())
        return &list1;
    else if (list2.empty())
        return &list2;
    else if (list3.empty())
        return &list3;
    else if (list4.empty())
        return &list4;
    else {

        list<myRTP *>::iterator it;
        it = list0.begin();
        id0 = (*it)->nTimestamp;
        it = list1.begin();
        id1 = (*it)->nTimestamp;
        it = list2.begin();
        id2 = (*it)->nTimestamp;
        it = list3.begin();
        id3 = (*it)->nTimestamp;
        it = list4.begin();
        id4 = (*it)->nTimestamp;
        if (id0 == ix)
            return &list0;
        if (id1 == ix)
            return &list1;
        if (id2 == ix)
            return &list2;
        if (id3 == ix)
            return &list3;
        if (id4 == ix)
            return &list4;

        uint32_t df[5] = {id0, id1, id2, id3, id4};

        BubbleSort(df, 5);
        int minx = df[0];
        if (minx == id0) {
            F_ClearList(&list0);
            return &list0;
        }
        if (minx == id1) {
            F_ClearList(&list1);
            return &list1;
        }
        if (minx == id2) {
            F_ClearList(&list2);
            return &list2;
        }
        if (minx == id3) {
            F_ClearList(&list3);
            return &list3;
        }
        if (minx == id4) {
            F_ClearList(&list4);
            return &list4;
        }


/*

         if(id0<id1)
         {
             if(id0<id2)
             {
                 F_ClearList(&list0);
                 return &list0;
             }
             else
             {
                 F_ClearList(&list2);
                 return &list2;
             }
         }
         else
         {
             if(id1<id2)
             {
                 F_ClearList(&list1);
                 return &list1;
             } else
             {
                 F_ClearList(&list2);
                 return &list2;
             }
         }
         */
    }
    return nullptr;
}


void *doReceive_rtp(void *dat) {


    LOGE("Start Thread RecData-111");
    struct sockaddr_in servaddr;
    memset(readRtpBuffer, 0, buffer_Len);
    fd_set rfd;     // 读描述符集
    int nRet = -1;
    int LEN_Buffer = 1024 * 1024;

    if (nICType == IC_GPRTP) {
        if (jpgbuffer != nullptr) {
            delete[]jpgbuffer;
            jpgbuffer = nullptr;
        }
        jpgbuffer = new char[LEN_Buffer];
    }


    int nInx = 0;
    int nLen = 0;


    list<myRTP *>::iterator it;
    list<myRTP *>::iterator it1;

    uint8_t *buff = nullptr;


    int nCurrent = 0;
    bool bIsOk = true;

    int endIx = 0;

    int nPreIx = 0;
    int64_t nCheckT_Current = av_gettime() / 1000;


    jpg0.Clear();
    jpg1.Clear();
    jpg2.Clear();

    uint16_t jpginx = 0;
    uint16_t jpg_pack_count = 0;
    uint8_t jpg_udp_inx = 0;


    struct timeval tv = {0, 100};     //
    int size;
    int64_t nTime1_pre = av_gettime() / 1000;
    int64_t nTime1_current;
    F_SentRTPHeartBeep();
    size = sizeof(servaddr);
    //nGetData_ = 0;
    while (!bNeedExit) {
        nTime1_current = av_gettime() / 1000;
        if (nTime1_current - nTime1_pre > (1000 * 2)) {
            F_SentRTPHeartBeep();
            nTime1_pre = nTime1_current;
        }
        tv.tv_sec = 0;
        tv.tv_usec = 1000 * 10;
        FD_ZERO(&rfd);      // 在使用之前总是要清空
        FD_SET(GPRTP_UDP_SOCKET, &rfd); // 把socka放入要测试的描述符集中
        nRet = select(GPRTP_UDP_SOCKET + 1, &rfd, nullptr, nullptr, &tv);// 检测是否有套接口是否可读
        if (nRet > 0) {
            int nRecEcho = 0;
            if (FD_ISSET(GPRTP_UDP_SOCKET, &rfd)) {
                nRecEcho = (int) recvfrom(GPRTP_UDP_SOCKET, readRtpBuffer, sizeof(readRtpBuffer), 0,
                                          (sockaddr *) &servaddr, (socklen_t *) &size);
                if(nRecEcho>0)
                    onGetVideoData();
                if (nICType == IC_GPRTPB) {
                    if (nRecEcho >= 9) {
                        nCheckT_pre = av_gettime() / 1000;
                        jpginx = (uint16_t) (readRtpBuffer[1] * 0x100 + readRtpBuffer[0]);
                        jpg_pack_count = (uint8_t) readRtpBuffer[2];
                        jpg_udp_inx = (uint8_t) readRtpBuffer[3];
                        if (jpg_udp_inx >= 0xFF)
                            continue;
                        JPEG_BUFFER *jpg = F_FindJpegBuffer(jpginx);
                        if (jpg->nJpegInx == 0 || jpg->nJpegInx == jpginx) {
                            jpg->nJpegInx = jpginx;
                        } else {
                            LOGE("Find packed error!");
                        }
                        if (jpg_udp_inx * (1450 - 8) + (1450 - 8) < LEN_Buffer) {
                            if (jpg->mInx[jpg_udp_inx] == 0) {
                                jpg->mInx[jpg_udp_inx] = 1;
                                memcpy(jpg->buffer + jpg_udp_inx * (1450 - 8), readRtpBuffer + 8,
                                       1450 - 8);
                                jpg->nCount++;
                            } else {
                                LOGE("Duplicate  packet  %d of %d", jpg_udp_inx, jpginx);
                            }
                            if (jpg->nCount >= jpg_pack_count) {
                                bool bOK = true;
                                for (int ix = 0; ix < jpg_pack_count; ix++) {
                                    if (jpg->mInx[ix] == 0) {
                                        bOK = false;
                                        break;
                                    }
                                }
                                if (bOK) {
                                    if(!bNeedExit) {
                                        m_FFMpegPlayer.DecodeMjpeg((char *) (jpg->buffer),
                                                                   jpg_pack_count * (1450 - 8));
                                    }

                                } else {
                                    LOGE("receive error!");
                                }
                                jpg->Clear();
                            }

                        }

                    }
                } else if (nICType == IC_GPRTP) {
                    if (nRecEcho > 20) {
                        nCheckT_pre = av_gettime() / 1000;
                        uint32_t timnapA = (uint32_t) (readRtpBuffer[7] + readRtpBuffer[6] * 0x100 +
                                                       readRtpBuffer[5] * 0x10000 +
                                                       readRtpBuffer[4] * 0x1000000);
                        uint16_t nIndex = (uint16_t) (readRtpBuffer[2] * 0x100 + readRtpBuffer[3]);
                        listA = FindList(timnapA);
                        bool bFind = false;
                        for (it = listA->begin(); it != listA->end(); it++) {
                            if ((*it)->nIndex == nIndex) {
                                bFind = true;
                                break;
                            }
                        }
                        if (!bFind) {
                            myRTP *rtp = new myRTP((uint8_t *) readRtpBuffer, nRecEcho);
                            rtp->nTimestamp = timnapA;
                            listA->push_back(rtp);
                        }


                        for (int xx = 0; xx < 5; xx++) {

                            listA = nullptr;
                            if (xx == 0) {
                                for (it = list0.begin(); it != list0.end(); it++) {
                                    if ((*it)->mBuffer[1] & 0x80) {
                                        listA = &list0;
                                        break;
                                    }
                                }
                            }
                            if (xx == 1) {
                                for (it = list1.begin(); it != list1.end(); it++) {
                                    if ((*it)->mBuffer[1] & 0x80) {
                                        listA = &list1;
                                        break;
                                    }
                                }
                            }
                            if (xx == 2) {
                                for (it = list2.begin(); it != list2.end(); it++) {
                                    if ((*it)->mBuffer[1] & 0x80) {
                                        listA = &list2;
                                        break;
                                    }
                                }
                            }
                            if (xx == 3) {
                                for (it = list3.begin(); it != list3.end(); it++) {
                                    if ((*it)->mBuffer[1] & 0x80) {
                                        listA = &list3;
                                        break;
                                    }
                                }
                            }
                            if (xx == 4) {
                                for (it = list4.begin(); it != list4.end(); it++) {
                                    if ((*it)->mBuffer[1] & 0x80) {
                                        listA = &list4;
                                        break;
                                    }
                                }
                            }


                            if (listA != nullptr && !listA->empty()) {
                                listA->sort(greater<myRTP *>());
                                int ix = 0;
                                bFind = false;
                                for (it = listA->begin(); it != listA->end(); it++) {
                                    if ((*it)->mBuffer[20] == 0xFF &&
                                        (*it)->mBuffer[21] == 0xD8) {
                                        bFind = true;
                                        break;
                                    }
                                    ix++;
                                }

                                if (bFind) {
                                    ix = 0;
                                    bool bOK = true;
                                    uint16_t nstart = (*it)->nIndex;
                                    for (; it != listA->end(); it++) {
                                        if (ix != 0) {
                                            if ((*it)->nIndex - nstart != 1) {
                                                if ((*it)->nIndex - nstart != 0 ||
                                                    nstart != 0xFFFF) {
                                                    bOK = false;
                                                    break;
                                                }
                                            }
                                            nstart = (*it)->nIndex;
                                        }
                                        if (ix + (*it)->nLen - 20 < LEN_Buffer) {
                                            memcpy(jpgbuffer + ix, &((*it)->mBuffer[20]),
                                                   (*it)->nLen - 20);
                                            ix += ((*it)->nLen - 20);
                                        } else {
                                            bOK = false;
                                            break;
                                        }
                                    }
                                    if (bOK) {
                                        m_FFMpegPlayer.DecodeMjpeg(jpgbuffer, ix);
                                        F_ClearList(listA);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }


    if (jpgbuffer != nullptr) {
        delete[]jpgbuffer;
        jpgbuffer = nullptr;
    }


    F_SentRTPStop();
    //usleep(1000 * 10);

    jpg0.Release();
    jpg1.Release();
    jpg2.Release();


    F_ClearList(&list0);
    F_ClearList(&list1);
    F_ClearList(&list2);

    LOGE("xit Thread RecData_11111E");
    return nullptr;

}

//template<typename T>
//string toString(const T &t) {
//    ostringstream oss;  //创建一个格式化输出流
//    oss << t;             //把值传递如流中
//    return oss.str();
//}

void *doReceive_Udp20000(void *para) {

    int nInxA = -1;
    int nInxB = -1;
    int tmp;
    byte *readBuff = new byte[1500];
    byte *readBuffA = new byte[1500];
    ssize_t nbytes; /* the number of read **/
    int size;    /* the length of servaddr */

    struct sockaddr_in servaddr; /* the server's full addr */

    fd_set readset;
    NET_UTP_DATA *pHead;
    bNeedExit = false;
    struct timeval tv;

    tv.tv_sec = 0;
    tv.tv_usec = 1000 * 10;
    size = sizeof(servaddr);
    LOGE_B("Start Read 20000");
    while (bNeedRead20000) {
#if 1
        if (udp_socket20000 < 0) {
            break;
        }

        tv.tv_sec = 0;
        tv.tv_usec = 1000 * 5;

        FD_ZERO(&readset); // 在使用之前总是要清空
        // 开始使用select
        FD_SET(udp_socket20000, &readset); // 把socka放入要测试的描述符集中

        int nRet = select(udp_socket20000 + 1, &readset, nullptr, nullptr, &tv);// 检测是否有套接口是否可读
        if (nRet <= 0) {
            continue;
        }
        if (!FD_ISSET(udp_socket20000, &readset)) {
            continue;
        }
        memset(readBuff, 0, 1500);
        memset(readBuffA, 0, 1500);

        nbytes = recvfrom(udp_socket20000, readBuff, 1500, 0, (struct sockaddr *) &servaddr,
                          (socklen_t *) &size);
        if (nbytes <= 0) {
            continue;
        } else {
            if (nICType == IC_GKA) {
                if (nbytes > sizeof(NET_UTP_DATA)) {
                    pHead = (NET_UTP_DATA *) readBuff;
                    nbytes -= sizeof(NET_UTP_DATA);
                    memcpy(readBuffA, &pHead->seq, 4);
                    memcpy(readBuffA + 4, readBuff + sizeof(NET_UTP_DATA), (size_t) nbytes);
                    F_OnGetWifiData((byte *) readBuffA, (int) (nbytes + 4));
                }
                continue;
            } else {
                if (nbytes > 2048) {
                    nbytes = 2048;
                }

                if (bDebug) {
                    int nb = (int) nbytes;
                    memset(cmd_buffer, 0, 2048);
                    memcpy(cmd_buffer, readBuff, (size_t) (nb)); //wifi透传数据
                    int32_t datA = 0xFFFF0000;
                    datA |= nb;
                    F_SentGp_Status2Jave(datA);
                }

                uint32_t dat = 0;
                int16_t nCmdType;
                int nLen = 0;

                if (nbytes >= 8) {
                    if (readBuff[0] == 'J' && readBuff[1] == 'H' && readBuff[2] == 'C' &&
                        readBuff[3] == 'M' && readBuff[4] == 'D') {
                        nCmdType = (int16_t) (readBuff[5] * 0x100 + readBuff[6]);
                        switch (nCmdType) {
                            case 0x5444:
                                if (cmd_buffer != nullptr) {
                                    nLen = nbytes - 7;
                                    if (nLen > 1024)
                                        nLen = 1024;
                                    memset(cmd_buffer, 0, 2048);
                                    memcpy(cmd_buffer, readBuff + 7, nLen); //BK加密信息
                                    dat = 0x54440000;
                                    dat |= ((nLen) & 0xFF);
                                    F_SentGp_Status2Jave(dat);
                                }

                                break;
                            case 0x5443:    //'T'=0x54  'C'=43
                                if (cmd_buffer != nullptr) {
                                    nLen = nbytes - 7;
                                    if (nLen > 1024)
                                        nLen = 1024;
                                    memset(cmd_buffer, 0, 2048);
                                    memcpy(cmd_buffer, readBuff + 7, nLen); //wifi透传数据
                                    dat = 0x54430000;
                                    dat |= ((nLen) & 0xFF);
                                    F_SentGp_Status2Jave(dat);
                                }
                                break;
                            case 0x1020:                //返回LED灯亮度
                                dat = 0x10200000;
                                dat |= readBuff[7];
                                F_SentGp_Status2Jave(dat);
                                break;
                            case 0x1021:                //返回电量
                                dat = 0x10210000;
                                dat |= readBuff[7];
                                F_SentGp_Status2Jave(dat);
                                break;
                            case 0x2000:

                                if (nbytes >= 47) {
                                    nLen = nbytes - 7;
                                    if (nLen > 1024)
                                        nLen = 1024;
                                    memset(cmd_buffer, 0, 2048);
                                    memcpy(cmd_buffer, readBuff + 7, nLen);
//                                     int32_t dat = 0xAA55AA00;// GP RTPB  回传 模块本身信息数据
//                                     dat |=((nbytes - 7)&0xFF);
                                    dat = 0x20000000;
                                    dat |= ((nLen) & 0xFF);
                                    F_SentGp_Status2Jave(dat);
                                }
                                break;
                            case 0x0006: {
                                uint8_t nStyle = readBuff[7];
//                                 dat = 0x11223300;//             //Style type
//                                 dat |=nStyle;
                                dat = 0x00060000;
                                dat |= nStyle;
                                F_SentGp_Status2Jave(dat);
                            }
                                break;
                            case 0x3005: {
                                if (nbytes >= 9) {
                                    uint16_t nLen = (uint16_t) (readBuff[7] + readBuff[8] * 0x100);
                                    if (nbytes >= nLen + 9) {
                                        if (nLen > 1024)
                                            nLen = 1024;
                                        memset(cmd_buffer, 0, 2048);
                                        memcpy(cmd_buffer, readBuff + 9, nLen);
                                        dat = 0x30050000;//   返回 30 05 命令读取设备flash数据
                                        dat |= (nLen & 0xFFFF);
                                        F_SentGp_Status2Jave(dat);
                                    }
                                }
                            }
                                break;
                            case 0x3006: {
                                if (nbytes >= 7) {
                                    dat = 0x30060001;//   返回 30 06 命令写设备flash数据是否成功
                                    if (readBuff[7] == 0)
                                        dat = 0x30060000;//   返回 30 06 命令写设备flash数据
                                    F_SentGp_Status2Jave(dat);
                                }
                            }
                                break;

                            default:
                                if (cmd_buffer != nullptr) {
                                    nLen = nbytes - 5;
                                    if (nLen > 1024)
                                        nLen = 1024;
                                    memset(cmd_buffer, 0, 2048);
                                    memcpy(cmd_buffer, readBuff + 5, nLen); //wifi透传数据
                                    dat = 0xFBFB0000;
                                    dat |= ((nLen) & 0xFFFF);
                                    F_SentGp_Status2Jave(dat);
                                }
                                break;
                        }
                    }

                    if (readBuff[0] == 'F' && readBuff[1] == 'D' && readBuff[2] == 'W' &&
                        readBuff[3] == 'N') {
                        if (readBuff[5] == 0x00) {

                            memset(cmd_buffer, 0, 2048);
                            memcpy(cmd_buffer, readBuff, nbytes); //wifi透传数据
                            dat = 0xFBFB0000;
                            nLen = nbytes;
                            dat |= ((nLen) & 0xFFFF);
                            F_SentGp_Status2Jave(dat);
                        }
                    }

#if 0
                    //                    if (readBuff[0] == 'J' && readBuff[1] == 'H' && readBuff[2] == 'C' && readBuff[3] == 'M' && readBuff[4] == 'D' && readBuff[5] == 'T' && readBuff[6] == 'C')
                    //                    {
                    //                        if (cmd_buffer != nullptr)
                    //                        {
                    //                            memset(cmd_buffer, 0, 1024);
                    //                            memcpy(cmd_buffer, readBuff + 7, (size_t)(nbytes - 7)); //wifi透传数据
                    //                            int32_t datA = 0x55AA5500;
                    //                            datA |=((nbytes - 7)&0xFF);
                    //                            F_SentGp_Status2Jave(datA);
                    //                        }
                    //                    }
                    //                    else  if (readBuff[0] == 'J' && readBuff[1] == 'H' && readBuff[2] == 'C' && readBuff[3] == 'M' && readBuff[4] == 'D' && readBuff[5] == 0x20 && readBuff[6] == 0x00)
                    //                    {
                    //                        if(nbytes>=47)
                    //                        {
                    //                            memset(cmd_buffer, 0, 1024);
                    //                            memcpy(cmd_buffer, readBuff + 7, nbytes - 7);
                    //                            int32_t dat = 0xAA55AA00;// GP RTPB  回传 模块本身信息数据
                    //                            dat |=((nbytes - 7)&0xFF);
                    //                            F_SentGp_Status2Jave(dat);
                    //                        }
                    //                    }
                    //                    else if (readBuff[0] == 'J' && readBuff[1] == 'H' && readBuff[2] == 'C' && readBuff[3] == 'M' && readBuff[4] == 'D' && readBuff[5] == 0x00 && readBuff[6] == 0x06)
                    //                    {
                    //                            uint8_t  nStyle= readBuff[7];
                    //                            int32_t dat = 0x11223300;// //Style type
                    //                            dat |=nStyle;
                    //                            F_SentGp_Status2Jave(dat);
                    //                    }
                    //                    else if (readBuff[0] == 'J' && readBuff[1] == 'H' && readBuff[2] == 'C' && readBuff[3] == 'M' && readBuff[4] == 'D' && readBuff[5] == 0x30 && readBuff[6] == 0x05)
                    //                    {
                    //                          if(nbytes>=9)
                    //                          {
                    //                                 uint16_t nLen =(uint16_t)(readBuff[7]+readBuff[8]*0x100);
                    //                                 if(nbytes>=nLen+9)
                    //                                 {
                    //                                     memset(cmd_buffer, 0, 1024);
                    //                                     memcpy(cmd_buffer, readBuff + 9, nLen);
                    //                                     int32_t dat = 0x30050000;//   返回 30 05 命令读取设备flash数据
                    //                                     dat |=(nLen &0xFFFF);
                    //                                     F_SentGp_Status2Jave(dat);
                    //                                 }
                    //                          }
                    //                    }
                    //                    else if (readBuff[0] == 'J' && readBuff[1] == 'H' && readBuff[2] == 'C' && readBuff[3] == 'M' && readBuff[4] == 'D' && readBuff[5] == 0x30 && readBuff[6] == 0x06)
                    //                    {
                    //                        if(nbytes>=7)
                    //                        {
                    //                            //memset(cmd_buffer, 0, 1024);
                    //                            //memcpy(cmd_buffer, readBuff + 7, 1);'
                    //                            int32_t dat = 0x30060001;//   返回 30 06 命令写设备flash数据
                    //                            if(readBuff[7] ==0)
                    //                                int32_t dat = 0x30060000;//   返回 30 06 命令写设备flash数据
                    //                            F_SentGp_Status2Jave(dat);
                    //                        }
                    //                    }
#endif
                }

                if (nbytes == 7) {
                    if (readBuff[0] == 'J' && readBuff[1] == 'H' && readBuff[2] == 'C' &&
                        readBuff[3] == 'M' && readBuff[4] == 'D') {
                        if (readBuff[5] == 0x10) //状态
                        {

                            if ((readBuff[6] & 0x10) != 0) {
                                //int32_t dat = 0xAA555500;// 0x55AA5500;
                                dat = 0xFFFE0000;//
                                dat |= (readBuff[6] & 0x0F);
                                F_SentGp_Status2Jave(dat);
                            } else {

                                dat = 0xFFFD0000;

                                nSdStatus_GP &= 0xFFFF00FF;
                                readBuff[6] ^= 0x04;
                                if (readBuff[6] & 0x01)  //正在录像
                                {
                                    nSdStatus_GP |= 0x0100;
                                    dat |= 0x0100;
                                    nSDStatus |= SD_Recording;              //录像按键
                                } else {
                                    nSDStatus &= (SD_Recording ^ 0xFFFF);
                                }

                                if (readBuff[6] & 0x02)  //  拍照
                                {
                                    nSdStatus_GP |= 0x0200;                 //拍照按键
                                    dat |= 0x0200;
                                    nSDStatus |= SD_SNAP;
                                } else {
                                    nSDStatus &= ((SD_SNAP ^ 0xFFFF) & 0xFFFF);
                                }
                                if (readBuff[6] & 0x04)  //SD
                                {
                                    nSdStatus_GP |= 0x0400;
                                    dat |= 0x0400;
                                    nSDStatus |= SD_Ready;
                                    LOGE("SD_Ready");
                                } else {
                                    nSDStatus &= (SD_Ready ^ 0xFFFF);
                                }

                                if (readBuff[6] & 0x08)  //卡满
                                {
                                    dat |= 0x0800;
                                    nSdStatus_GP |= 0x0800;
                                    nSDStatus &= (SD_Ready ^ 0xFFFF);
                                }
                                if (readBuff[6] & 0x10)  //低电
                                {
                                    dat |= 0x1000;
                                    nSdStatus_GP |= 0x1000;
                                }
                                F_SendStatus2Jave();
                                //F_SentGp_Status2Jave(nSdStatus_GP);
                                F_SentGp_Status2Jave(dat);
                            }
                        }

                        if (readBuff[5] == 0x00) {         //按键命令
                            //nSdStatus_GP &= 0xFFFFFF00;

                            dat = 0xFFFC0000;
                            dat |= readBuff[6];
                            //nSdStatus_GP = 0x0;
                            //nSdStatus_GP |= readBuff[6];
                            F_SentGp_Status2Jave(dat);
                            F_SendKey2Jave(readBuff[6] & 0xFF);


                        }
                        if (readBuff[5] == 0x20)  //通道
                        {
                            // readBuff[6]  通道数
                            dat = 0xFFFB0000;
                            dat |= readBuff[6];
                            //nSdStatus_GP &= 0xFF00FFFF;
//                            tmp = readBuff[6];
//                            tmp <<= 16;
//                            tmp &= 0x00FF0000;
//                            nSdStatus_GP |= tmp;
                            F_SentGp_Status2Jave(dat);
                        }

                    } else if (readBuff[0] == 'U' && readBuff[1] == 'V' && readBuff[2] == 'C' &&
                               readBuff[3] == 'A') {
                        // 2020-04-26 添加，如果时读取摄像头硬件参数
                        int nRet = 0;
                        bool bFindCmd = false;
                        nCmdType = (int16_t) (readBuff[4] * 0x100 + readBuff[5]);

                        bool bF = true;
                        switch (nCmdType) {
                            case 0x0101:   //亮度
                                dat = 0xFFF01100;
                                dat |= readBuff[6];
                                break;
                            case 0x0102:    //对比度
                                dat = 0xFFF01200;
                                dat |= readBuff[6];
                                break;
                            case 0x0103:       //饱和度
                                dat = 0xFFF01300;
                                dat |= readBuff[6];
                                break;
                            case 0x0104:    //缩放
                                dat = 0xFFF01400;
                                dat |= readBuff[6];
                                break;
                            case 0x0105:    //全景
                                dat = 0xFFF01500;
                                dat |= readBuff[6];
                                break;
                            case 0x0106:    //倾斜
                                dat = 0xFFF01600;
                                dat |= readBuff[6];
                                break;
                            case 0x0107:    //滚动
                                dat = 0xFFF01700;
                                dat |= readBuff[6];
                                break;
                            case 0x0120:
                                dat = 0xFFF01A00;
                                break;

                            case 0x0201:   //亮度
                                dat = 0xFFF02100;
                                break;
                            case 0x0202:    //对比度
                                dat = 0xFFF02200;
                                break;
                            case 0x0203:       //饱和度
                                dat = 0xFFF02300;
                                break;
                            case 0x0204:    //缩放
                                dat = 0xFFF02400;
                                break;
                            case 0x0205:    //全景
                                dat = 0xFFF02500;
                                break;
                            case 0x0206:    //倾斜
                                dat = 0xFFF02600;
                                break;
                            case 0x0207:    //滚动
                                dat = 0xFFF02700;
                                break;
                            case 0x0220:
                                dat = 0xFFF02A00;
                                break;
                            default:
                                bF = false;
                                break;
                        }
                        if (bF) {
                            F_SentGp_Status2Jave(dat);
                        }
                    }
                }
            }
        }
#endif
    }
    delete[]readBuff;
    delete[]readBuffA;
    close(udp_socket20000);
   // pthread_detach(pthread_self());
    udp_socket20000 = -1;
    //rev_Udp_thread20000 = -1;
    LOGE("Exit ReadStatus Thread 20000!");
    return nullptr;
}


JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naDeleteSDFile(JNIEnv *env, jclass type,
                                                        jstring fileName_) {
    const char *sfile = env->GetStringUTFChars(fileName_, 0);

    T_NET_CMD_MSG Cmd;
    T_NET_CONFIG config;
    T_NET_SD_FILE_INFO fileinfo;
    T_REQ_MSG msg;

    memset(&fileinfo, 0, sizeof(T_NET_SD_FILE_INFO));
    memset(&config, 0, sizeof(T_NET_CONFIG));


    if (strlen(sfile) >= 40) {
        env->ReleaseStringUTFChars(fileName_, sfile);
        return -1;
    }

    MySocket_GKA socket;
    MySocketData data;
    if (socket.Connect(sServerIP.c_str(), 0x7102) < 0) {
        env->ReleaseStringUTFChars(fileName_, sfile);
        return -2;
    }

    int nLen = sizeof(T_NET_CMD_MSG);
    Cmd.session_id = session_id;
    Cmd.type = CMD_SEARCH_SOCK;
    data.nLen = 0;
    data.AppendData((uint8_t *) &Cmd, nLen);
    socket.Write(&data);
    int nRet = 0;
    Cmd.session_id = session_id;
    Cmd.type = CMD_SET_CONFIG;
    config.res = 0;
    config.type = CONFIG_SD_RM_FILE;
    memcpy(fileinfo.name, sfile, strlen(sfile));
    data.nLen = 0;
    data.AppendData(&Cmd, sizeof(T_NET_CMD_MSG));
    data.AppendData(&config, sizeof(T_NET_CONFIG));
    data.AppendData(&fileinfo, sizeof(T_NET_SD_FILE_INFO));
    socket.Write(&data);
    data.SetSize(sizeof(T_REQ_MSG));
    nRet = socket.Read(&data, 150);
    if (nRet == sizeof(T_REQ_MSG)) {
        memcpy(&msg, data.data, nLen);
        if (msg.ret == 0 && msg.session_id == session_id) {
            socket.DisConnect();
            env->ReleaseStringUTFChars(fileName_, sfile);
            return 0;
        }
    }
    socket.DisConnect();
    env->ReleaseStringUTFChars(fileName_, sfile);
    return -1;
}

JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naSetGPFps(JNIEnv *env, jclass type, jint nFps) {

    m_FFMpegPlayer.nfps = nFps;
    m_FFMpegPlayer.nRecFps = nFps;
    // TODO
    return 0;

}

JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naFillFlyCmdByC(JNIEnv *env, jclass type, jint nType) {


    jclass clazz = nullptr;
    FLY_CMD cmd_Data;
    jfieldID fid = nullptr;
    clazz = env->FindClass(fly_classpath);
    if (clazz == nullptr)
        return;
    jbyte *bytes = nullptr;
    jbyteArray jbd = nullptr;
    if (nType == 1) {
        fid = env->GetStaticFieldID(clazz, "Roll", "I");
        if (fid != nullptr) {
            cmd_Data.Roll = (uint32_t) (env->GetStaticIntField(clazz, fid));
        }
        fid = env->GetStaticFieldID(clazz, "Pitch", "I");
        if (fid != nullptr) {
            cmd_Data.Pitch = (uint32_t) (env->GetStaticIntField(clazz, fid));
        }
        fid = env->GetStaticFieldID(clazz, "Thro", "I");
        if (fid != nullptr) {
            cmd_Data.Thro = (uint32_t) (env->GetStaticIntField(clazz, fid));
        }

        fid = env->GetStaticFieldID(clazz, "Yaw", "I");
        if (fid != nullptr) {
            cmd_Data.Yaw = (uint32_t) (env->GetStaticIntField(clazz, fid));
        }

        fid = env->GetStaticFieldID(clazz, "TrimRoll", "I");
        if (fid != nullptr) {
            cmd_Data.TrimRoll = (uint32_t) (env->GetStaticIntField(clazz, fid));
        }
        fid = env->GetStaticFieldID(clazz, "TrimPitch", "I");
        if (fid != nullptr) {
            cmd_Data.TrimPitch = (uint32_t) (env->GetStaticIntField(clazz, fid));
        }
        fid = env->GetStaticFieldID(clazz, "TrimThro", "I");
        if (fid != nullptr) {
            cmd_Data.TrimThro = (uint32_t) (env->GetStaticIntField(clazz, fid));
        }

        fid = env->GetStaticFieldID(clazz, "TrimYaw", "I");
        if (fid != nullptr) {
            cmd_Data.TrimYaw = (uint32_t) (env->GetStaticIntField(clazz, fid));
        }

        fid = env->GetStaticFieldID(clazz, "FastMode", "I");
        if (fid != nullptr) {
            cmd_Data.FastMode = (uint32_t) (env->GetStaticIntField(clazz, fid));
        }
        fid = env->GetStaticFieldID(clazz, "CFMode", "I");
        if (fid != nullptr) {
            cmd_Data.CFMode = (uint32_t) (env->GetStaticIntField(clazz, fid));
        }
        fid = env->GetStaticFieldID(clazz, "FlipMode", "I");
        if (fid != nullptr) {
            cmd_Data.FlipMode = (uint32_t) (env->GetStaticIntField(clazz, fid));
        }

        fid = env->GetStaticFieldID(clazz, "GpsMode", "I");
        if (fid != nullptr) {
            cmd_Data.GpsMode = (uint32_t) (env->GetStaticIntField(clazz, fid));
        }

        fid = env->GetStaticFieldID(clazz, "Mode", "I");
        if (fid != nullptr) {
            cmd_Data.Mode = (uint32_t) (env->GetStaticIntField(clazz, fid));
        }

        fid = env->GetStaticFieldID(clazz, "LevelCor", "I");
        if (fid != nullptr) {
            cmd_Data.LevelCor = (uint32_t) (env->GetStaticIntField(clazz, fid));
        }


        fid = env->GetStaticFieldID(clazz, "MagCor", "I");
        if (fid != nullptr) {
            cmd_Data.MagCor = (uint32_t) (env->GetStaticIntField(clazz, fid));
        }

        fid = env->GetStaticFieldID(clazz, "AutoTakeoff", "I");
        if (fid != nullptr) {
            cmd_Data.AutoTakeoff = (uint32_t) (env->GetStaticIntField(clazz, fid));
        }

        fid = env->GetStaticFieldID(clazz, "AutoLand", "I");
        if (fid != nullptr) {
            cmd_Data.AutoLand = (uint32_t) (env->GetStaticIntField(clazz, fid));
        }

        fid = env->GetStaticFieldID(clazz, "GoHome", "I");
        if (fid != nullptr) {
            cmd_Data.GoHome = (uint32_t) (env->GetStaticIntField(clazz, fid));
        }

        fid = env->GetStaticFieldID(clazz, "Stop", "I");
        if (fid != nullptr) {
            cmd_Data.Stop = (uint32_t) (env->GetStaticIntField(clazz, fid));
        }

        fid = env->GetStaticFieldID(clazz, "FollowMe", "I");
        if (fid != nullptr) {
            cmd_Data.FollowMe = (uint32_t) (env->GetStaticIntField(clazz, fid));
        }

        fid = env->GetStaticFieldID(clazz, "CircleFly", "I");
        if (fid != nullptr) {
            cmd_Data.CircleFly = (uint32_t) (env->GetStaticIntField(clazz, fid));
        }

        fid = env->GetStaticFieldID(clazz, "PointFly", "I");
        if (fid != nullptr) {
            cmd_Data.PointFly = (uint32_t) (env->GetStaticIntField(clazz, fid));
        }

        fid = env->GetStaticFieldID(clazz, "FollowMe_A", "I");
        if (fid != nullptr) {
            cmd_Data.FollowMe_A = (uint32_t) (env->GetStaticIntField(clazz, fid));
        }

        fid = env->GetStaticFieldID(clazz, "Photo", "I");
        if (fid != nullptr) {
            cmd_Data.Photo = (uint32_t) (env->GetStaticIntField(clazz, fid));
        }

        fid = env->GetStaticFieldID(clazz, "Video", "I");
        if (fid != nullptr) {
            cmd_Data.Video = (uint32_t) (env->GetStaticIntField(clazz, fid));
        }

        fid = env->GetStaticFieldID(clazz, "CamMovStep", "I");
        if (fid != nullptr) {
            cmd_Data.CamMovStep = (uint32_t) (env->GetStaticIntField(clazz, fid));
        }

        fid = env->GetStaticFieldID(clazz, "CamMovDir", "I");
        if (fid != nullptr) {
            cmd_Data.CamMovDir = (uint32_t) (env->GetStaticIntField(clazz, fid));
        }

        fid = env->GetStaticFieldID(clazz, "cmd", "[B");
        if (fid != nullptr) {
            //获取Record对象data值
            jbd = (jbyteArray) env->GetStaticObjectField(clazz, fid);
            bytes = env->GetByteArrayElements(jbd, 0);
        }
    }
    jbyte *buffer = (jbyte *) (&cmd_Data);
    int nSize = sizeof(cmd_Data);
    if (bytes != nullptr) {
        int chars_len = env->GetArrayLength(jbd);

        int nSi = min(nSize, chars_len - 3);
        memset(bytes, 0, chars_len);
        if (nType & 0x80) {
            bytes[0] = (jbyte) 0xA6;
        } else {
            bytes[0] = (jbyte) 0xA5;
        }
        bytes[1] = (jbyte) nType;
        bytes[2] = 0;
        if (nType == 1) {
            bytes[2] = 11;
            memcpy(bytes + 3, buffer, nSi);
            jbyte checksum = 0;
            for (int ii = 0; ii < 11 + 3; ii++) {
                checksum ^= bytes[ii];
            }
            bytes[14] = checksum;
        }
    }
    if (bytes != nullptr)
        env->ReleaseByteArrayElements(jbd, bytes, 0);


}

//int naSave2FrameMp4(jbyte  *data, jint nLen,jint b,jboolean keyframe)

int _naSave2FrameMp4(uint8_t *data, int nLen, int b, bool keyframe) {
    if (b == 0)  //sps  pps
    {
#if 0
        uint8_t *sData = data;
        int nSize = nLen;

        int sps, pps;
        for (sps = 0; sps < nSize;)
            if (sData[sps++] == 0x00 && sData[sps++] == 0x00 && sData[sps++] == 0x00
                && sData[sps++] == 0x01)
                break;
        for (pps = sps; pps < nSize;)
            if (sData[pps++] == 0x00 && sData[pps++] == 0x00 && sData[pps++] == 0x00
                && sData[pps++] == 0x01)
                break;
        if (sps >= nSize || pps >= nSize) {
            return 0;
        }

        m_FFMpegPlayer.AddMp4Video(sData + sps, pps - sps - 4, sData + pps, nSize - pps);
#else

        uint8_t *sData = data;
        int nSize = nLen;
        int sps = -1;
        int pps = -1;
        for (int x = 0; x < nSize; x++) {
            if (sData[x] == 0x00 &&
                sData[x + 1] == 0x00 &&
                sData[x + 2] == 0x00 &&
                sData[x + 3] == 0x01 &&
                (sData[x + 4] & 0x1F) == 0x07) {
                sps = x + 4;
                break;
            }
        }

        for (int x = 0; x < nSize; x++) {
            if (sData[x] == 0x00 &&
                sData[x + 1] == 0x00 &&
                sData[x + 2] == 0x00 &&
                sData[x + 3] == 0x01 &&
                (sData[x + 4] & 0x1F) == 0x08) {
                pps = x + 4;
                break;
            }
        }
        int spsLen = 0;
        int ppsLen = 0;
        for (int x = sps; x < nSize; x++) {
            if (sData[x] == 0x00 &&
                sData[x + 1] == 0x00 &&
                sData[x + 2] == 0x00 &&
                sData[x + 3] == 0x01) {
                break;
            } else {
                spsLen++;
            }
        }

        for (int x = pps; x < nSize; x++) {
            if (sData[x] == 0x00 &&
                sData[x + 1] == 0x00 &&
                sData[x + 2] == 0x00 &&
                sData[x + 3] == 0x01) {
                break;
            } else {
                ppsLen++;
            }
        }

        if (spsLen == 0 || ppsLen == 0 || pps == -1 || sps == -1) {
            LOGE("SPS error!");
        } else {
            uint8_t *pSPS = sData + sps;
            uint8_t *pPPS = sData + pps;


            LOGE("SPS = %d, l= %d PPS = %d, l= %d size = %d", sps, spsLen, pps, ppsLen, nSize);
            for (int x = 0; x < spsLen; x++) {
                LOGE("%02X", *(pSPS + x));
            }
            pSPS[3] = 0x28;

            m_FFMpegPlayer.AddMp4Video(pSPS, spsLen, pPPS, ppsLen);


        }

#endif
    } else {

        nRecFrameCount++;
        m_FFMpegPlayer.WriteMp4Frame(data, nLen, keyframe);
    }

    return 0;
}

JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naSave2FrameMp4(JNIEnv *env, jclass type, jbyteArray data_,
                                                         jint nLen, jint b, jboolean keyframe) {
    jbyte *data = env->GetByteArrayElements(data_, nullptr);
    _naSave2FrameMp4((uint8_t *) data, nLen, b, keyframe);
    env->ReleaseByteArrayElements(data_, data, 0);
    return 0;
}


JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naSetRecordWH(JNIEnv *env, jclass type, jint ww, jint hh) {

    if (nSDStatus & bit1_LocalRecording) {
        return -1;
    } else {

        if (ww < 0 || hh < 0) {
            bRocordWHisSeted = false;
        } else {
            bRocordWHisSeted = true;
            m_FFMpegPlayer.F_ReSetRecordWH(ww, hh);
        }
        return 0;
    }
}


JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naGetFps(JNIEnv *env, jclass type)     //获取实际通过网络获得的fps
{
    return  0;//nFrameFps;
}


JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naGetwifiFps(JNIEnv *env, jclass type)  // 获取 wifi板子设定的fps
{
    return m_FFMpegPlayer.nfps;
}


JNIEXPORT jstring JNICALL
Java_com_joyhonest_wifination_wifination_naGetControlType(JNIEnv *env, jclass type) {
    F_GetServerIP();
    if (nICType == IC_GKA) { ;
    } else {
        sver = "";
        //F_StartReadUdp20000();
        uint8_t msg[20];
        msg[0] = 'J';
        msg[1] = 'H';
        msg[2] = 'C';
        msg[3] = 'M';
        msg[4] = 'D';
        msg[5] = 0x20;
        msg[6] = 0x00;
        msg[7] = 0x00;
        msg[8] = 0x00;
        msg[9] = 0x00;
        msg[10] = 0x00;
        send_cmd_udp(msg, 11, sServerIP.c_str(), 20000);
        usleep(1000 * 200);
        if (sver.length() == 0) {
            send_cmd_udp(msg, 11, sServerIP.c_str(), 20000);
            usleep(1000 * 100);
        }
    }
    return env->NewStringUTF(sver.c_str());
}

// TODO




extern AVFrame *gl_Frame;

//void frame_rotate_180(AVFrame *src, AVFrame *des);

void Adj23D(AVFrame *src, AVFrame *des);

bool F_SetBackGroud(jbyte *data, jint width, jint height) {
    AVFrame *myYUV = av_frame_alloc();
    myYUV->width = width;
    myYUV->height = height;
    myYUV->format = AV_PIX_FMT_YUV420P;
    int ret = av_image_alloc(
            myYUV->data, myYUV->linesize, width,
            height,
            AV_PIX_FMT_YUV420P, aligD);
    ret = libyuv::ABGRToI420((uint8_t *) data, width * 4,
                             myYUV->data[0], myYUV->linesize[0],
                             myYUV->data[1], myYUV->linesize[1],
                             myYUV->data[2], myYUV->linesize[2],
                             width, height);


    gl_Frame->width = myYUV->width;
    gl_Frame->height = myYUV->height;

    gl_Frame->linesize[0] = myYUV->linesize[0];
    gl_Frame->linesize[1] = myYUV->linesize[1];
    gl_Frame->linesize[2] = myYUV->linesize[2];

    if (m_FFMpegPlayer.bFlip) {
        AVFrame *frame_a = av_frame_alloc();
        frame_a->width = width;
        frame_a->height = height;
        frame_a->format = AV_PIX_FMT_YUV420P;
        int ret = av_image_alloc(
                frame_a->data, frame_a->linesize, width,
                height,
                AV_PIX_FMT_YUV420P, aligD);


        libyuv::I420Rotate(myYUV->data[0], myYUV->linesize[0],
                           myYUV->data[1], myYUV->linesize[1],
                           myYUV->data[2], myYUV->linesize[2],
                           frame_a->data[0], frame_a->linesize[0],
                           frame_a->data[1], frame_a->linesize[1],
                           frame_a->data[2], frame_a->linesize[2],
                           frame_a->width, frame_a->height,
                           libyuv::kRotate180);

        av_frame_copy(myYUV, frame_a);

        if (frame_a != nullptr) {
            av_freep(&frame_a->data[0]);
            av_frame_free(&frame_a);
        }
    }

    if (m_FFMpegPlayer.b3D) {
        AVFrame *frame_b = av_frame_alloc();
        frame_b->format = AV_PIX_FMT_YUV420P;
        frame_b->width = width / 2;
        frame_b->height = height / 2;
        av_image_alloc(frame_b->data, frame_b->linesize, frame_b->width,
                       frame_b->height,
                       AV_PIX_FMT_YUV420P, aligD);


        libyuv::I420Scale(myYUV->data[0], myYUV->linesize[0],
                          myYUV->data[1], myYUV->linesize[1],
                          myYUV->data[2], myYUV->linesize[2],
                          width, height,
                          frame_b->data[0], frame_b->linesize[0],
                          frame_b->data[1], frame_b->linesize[1],
                          frame_b->data[2], frame_b->linesize[2],
                          frame_b->width, frame_b->height,
                          libyuv::kFilterLinear);

        Adj23D(frame_b, myYUV);
        if (frame_b != nullptr) {
            av_freep(&frame_b->data[0]);
            av_frame_free(&frame_b);
        }
    }

    ret = libyuv::I420Copy(myYUV->data[0], myYUV->linesize[0],
                           myYUV->data[1], myYUV->linesize[1],
                           myYUV->data[2], myYUV->linesize[2],
                           gl_Frame->data[0], myYUV->linesize[0],
                           gl_Frame->data[1], myYUV->linesize[1],
                           gl_Frame->data[2], myYUV->linesize[2],
                           myYUV->width, myYUV->height);

    ret = 1;
    if (myYUV != nullptr) {
        av_freep(&myYUV->data[0]);
        av_frame_free(&myYUV);
    }
    return true;
}

JNIEXPORT jboolean JNICALL
Java_com_joyhonest_wifination_wifination_naSetBackground(JNIEnv *env, jclass type, jbyteArray data_,
                                                         jint width, jint height) {
    jboolean re = 0;
    if (gl_Frame == nullptr) {
        return re;
    }
    if (width > 1920 || height > 1080) {
        return re;
    }

    jbyte *data = env->GetByteArrayElements(data_, nullptr);
    bool rr = F_SetBackGroud(data, width, height);
    if (rr) {
        re = 1;
    } else {
        re = 0;
    }
    env->ReleaseByteArrayElements(data_, data, 0);
    return re;
}


JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetAdjFps(JNIEnv *env, jclass type, jboolean b) {
    bAdjFps = b;
}

#endif

bool bSentRevBMP = false;

extern "C"
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_JH_1GLSurfaceView_naDecordFrame(JNIEnv *env, jclass type,
                                                              jbyteArray data_, int size) {
    jbyte *data = env->GetByteArrayElements(data_, nullptr);
    int len_array = env->GetArrayLength(data_);
    MySocketData dat;
    dat.AppendData(data, len_array);
    //m_FFMpegPlayer.F_DispH264NoBuffer(&dat);
    env->ReleaseByteArrayElements(data_, data, 0);
    return 0;
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_JH_1GLSurfaceView_naDecordInit(JNIEnv *env, jclass type) {
    nICType = IC_GKA;
    m_FFMpegPlayer.InitMediaH264();
    return 0;

}extern "C"
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_JH_1GLSurfaceView_naDecordRelease(JNIEnv *env, jclass type) {

    m_FFMpegPlayer.Releaseffmpeg();
    LOGE("333");
    return 0;

}

JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetGKA_1SentCmdByUDP(JNIEnv *env, jclass type,
                                                                jboolean bUdp) {
    bGKACmd_UDP = bUdp;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetRevBmpA(JNIEnv *env, jclass type, jboolean b) {

    bSentRevBMP = b;
    // TODO

}


bool bWhitClolor = false;
extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetVrBackground(JNIEnv *env, jclass type, jboolean b) {

    // TODO
    bWhitClolor = b;
}

extern int nRotation;

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naRotation(JNIEnv *env, jclass type, jint n) {

    // TODO
    if (n == 0 || n == 90 || n == -90 || n == 180 || n == 270) {
        nRotation = n;
    }

}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_joyhonest_wifination_wifination_naSetWifiPassword(JNIEnv *env, jclass type,
                                                           jstring sPassword_) {
    const char *sPassword = env->GetStringUTFChars(sPassword_, 0);
    // TODO
    int nLen = strlen(sPassword);
    if (nLen == 0)
        return 0;
    if (nLen > 64)
        return 0;
    uint8_t msg[80];
    msg[0] = 'J';
    msg[1] = 'H';
    msg[2] = 'C';
    msg[3] = 'M';
    msg[4] = 'D';
    msg[5] = 0x30;
    msg[6] = 0x02;
    msg[7] = (uint8_t) nLen;
    for (int i = 0; i < nLen; i++) {
        msg[8 + i] = (uint8_t) (sPassword[i]);
    }
    send_cmd_udp(msg, 8 + nLen, sServerIP.c_str(), 20000);
    env->ReleaseStringUTFChars(sPassword_, sPassword);
    return 1;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetScal(JNIEnv *env, jclass type, jfloat fScal) {

    nScal = fScal;

}




extern "C"
JNIEXPORT jboolean JNICALL
Java_com_joyhonest_wifination_AudioEncoder_naSentVoiceData(JNIEnv *env, jclass type,
                                                           jbyteArray data_, jint nLen) {
    jbyte *data = env->GetByteArrayElements(data_, nullptr);
    int len_array = env->GetArrayLength(data_);
    if (len_array != 0) {

        m_FFMpegPlayer.F_WriteAudio(data, len_array);
    }
    env->ReleaseByteArrayElements(data_, data, 0);
    return true;
}

//bool bG_Audio = false;

//extern "C"
//JNIEXPORT void JNICALL
//Java_com_joyhonest_wifination_wifination_naSetRecordAudio(JNIEnv *env, jclass type, jboolean b) {
//    //bG_Audio = b;
//}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetDislplayData(JNIEnv *env, jclass type,
                                                           jbyteArray data_, jint width,
                                                           jint height) {
    jbyte *data = env->GetByteArrayElements(data_, nullptr);

    jint w2 = width / 2;
    jint nlen = width * height;
    jint nlen1 = (jint) (width * height / 4);

    jbyte *py = data;
    jbyte *pu = data + nlen;
    jbyte *pv = pu + nlen1;

    gl_Frame->width = width;
    gl_Frame->height = height;

    gl_Frame->linesize[0] = width;
    gl_Frame->linesize[1] = w2;
    gl_Frame->linesize[2] = w2;

    libyuv::I420Copy((const uint8_t *) py, width,
                     (const uint8_t *) pu, w2,
                     (const uint8_t *) pv, w2,
                     gl_Frame->data[0], width,
                     gl_Frame->data[1], w2,
                     gl_Frame->data[2], w2,
                     width, height);

    env->ReleaseByteArrayElements(data_, data, 0);
}

extern int nDispStyle;

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetDispStyle(JNIEnv *env, jclass type, jint nType) {
    nDispStyle = nType;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetGestureA(JNIEnv *env, jclass type, jboolean b) {

    bGesture = b;

}



extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetLedOnOff(JNIEnv *env, jclass type,
                                                       jboolean bOpenLed) {

    // TODO
    uint8_t msg[8];
    msg[0] = 'J';
    msg[1] = 'H';
    msg[2] = 'C';
    msg[3] = 'M';
    msg[4] = 'D';
    msg[5] = 0x50;
    msg[6] = 0x00;
    send_cmd_udp(msg, 7, sServerIP.c_str(), 20000);

}

extern bool bRotaHV;

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetbRotaHV(JNIEnv *env, jclass type, jboolean b) {

    // TODO
    bRotaHV = b;

}


extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetDebug(JNIEnv *env, jclass type, jboolean b) {

    bDebug = b;

}



extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naWriteport20000(JNIEnv *env, jclass type, jbyteArray cmd_,
                                                          jint nleng1) {
    jbyte *cmd = env->GetByteArrayElements(cmd_, nullptr);
    int len_array = env->GetArrayLength(cmd_);
    uint8_t msg[200];
    if (len_array > 200) {
        len_array = 200;
    }
    if (cmd != nullptr && len_array != 0) {
        for (int i = 0; i < len_array; i++) {
            msg[i] = (uint8_t) cmd[i];
        }
    }
    send_cmd_udp(msg, len_array, sServerIP.c_str(), 20000);
    env->ReleaseByteArrayElements(cmd_, cmd, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetNoTimeOut(JNIEnv *env, jclass type, jboolean b) {

    // TODO
    nCheckT_pre = av_gettime() / 1000;
    bNoTimeout = b;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetMirror(JNIEnv *env, jclass type, jboolean b) {

    m_FFMpegPlayer.bMirror = b;
}

//////////////--------------



uint32_t nFileRevCont = 0;

GP4225_FileStruct gp4225File;

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_joyhonest_wifination_wifination_naSentUdpData(JNIEnv *env, jclass type, jstring sIP_,
                                                       jint nPort, jbyteArray data_, jint nLen2) {
    bool bre = false;
    const char *sIP = env->GetStringUTFChars(sIP_, 0);
    char *data = (char *) env->GetByteArrayElements(data_, nullptr);
    int len_array = env->GetArrayLength(data_);

    if (data[0] == 'F' && data[1] == 'D' && data[2] == 'W' && data[3] == 'N' &&
        data[4] == 0x0A && data[5] == 0x00 &&
        data[6] == 0x01 && data[7] == 0x00 &&
        data[8] == 0x40 && data[9] == 0x00) {

        //nFileRevCont = 0;
        memset(gp4225File.sPaht, 0, 32);
        memset(gp4225File.sFileName, 0, 32);
        gp4225File.nFileLen = 0;
        memcpy(gp4225File.sPaht, data + 10, 32);
        memcpy(gp4225File.sFileName, data + 42, 32);
    }

    int re = send_cmd_udp((uint8_t *) data, (int) nLen2, sIP, (uint16_t) nPort);
    if (re >= 0) {
        bre = true;
    }
    env->ReleaseStringUTFChars(sIP_, sIP);
    env->ReleaseByteArrayElements(data_, (jbyte *) data, 0);
    return bre;

}

void F_RevData2Jave(byte *data, int nLen, int nPort);

void *doReceiveUdp20001_cmd(void *dat) {


    struct sockaddr_in servaddr; /* the server's full addr */
    struct timeval tv;
    fd_set readset;
    size_t nbytes;

    byte *readBuff = new byte[1400];

    socklen_t size = (socklen_t) sizeof(servaddr);

    //while (!bNeedExitReadUDP)
    while (bNeedRead20001) {
        tv.tv_sec = 0;
        tv.tv_usec = 1000 * 10;

        FD_ZERO(&readset); // 在使用之前总是要清空
        // 开始使用select
        FD_SET(udp_socket20001, &readset); // 把socka放入要测试的描述符集中

        int nRet = select(udp_socket20001 + 1, &readset, nullptr, nullptr, &tv);// 检测是否有套接口是否可读
        if (nRet <= 0) {
            usleep(1000 * 5);
            continue;
        }
        if (!FD_ISSET(udp_socket20001, &readset)) {
            usleep(1000 * 5);
            continue;
        }
        memset(readBuff, 0, 1400);
        nbytes = recvfrom(udp_socket20001, readBuff, 1400, 0, (struct sockaddr *) &servaddr,
                          (socklen_t *) &size);
        if (nbytes <= 0) {
            continue;
        } else {
            F_RevData2Jave(readBuff, nbytes, 20001);
        }

    }
    close(udp_socket20001);
    udp_socket20001 = -1;
    delete[]readBuff;
    LOGE_B("Exit Udp ReadThread 20001");
   // pthread_detach(pthread_self());
    return nullptr;
}


//void F_ReadGsensorStatus()
//{
//
//    uint8_t msg[10];
//    msg[0] = 'F';
//    msg[1] = 'D';
//    msg[2] = 'W';
//    msg[3] = 'N';
//
//    msg[4] = 0x20;
//    msg[5] = 0x00;
//
//    msg[6] = 0x12;
//    msg[7] = 0x00;
//
//    msg[8] = 0x01;
//    msg[9] = 0x00;
//    send_cmd_udp(msg, 10, sServerIP.c_str(), 20001);
//
//}

void F_StartReadUdp20001() {
    int nPort = 20001;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 1000 * 10;
    struct sockaddr_in myaddr;
    if (udp_socket20001 > 0) {
        return;
    }
    udp_socket20001 = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);


    if(nWifiInterface!=-1) {
        setsockopt(udp_socket20001, IPPROTO_IP, SO_BINDTODEVICE, &nWifiInterface,
                          sizeof(nWifiInterface));
    }

    bzero((char *) &myaddr, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(nPort);

    int value = 1;
    int status;
    //status = setsockopt(udp_socket20001, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));
    //status = setsockopt(udp_socket20001, SOL_SOCKET, SO_REUSEPORT, &value, sizeof(value));
    int ad = sizeof(myaddr);
    int ac = sizeof(struct sockaddr);
    int res = (int) (::bind(udp_socket20001, (struct sockaddr *) &myaddr,
                            (socklen_t) (sizeof(myaddr))));
    if (res < 0) {
        shutdown(udp_socket20001, 0);
        close(udp_socket20001);
        udp_socket20001 = -1;
        bNeedRead20001 = false;
        return;
    }

    int ret = -1;
    {
        //ret = pthread_create(&rev_Udp_thread, nullptr, doReceive_cmd, (void *) nullptr); // 成功返回0，错误返回错误编号
        ret = pthread_create(&rev_Udp_thread20001, nullptr, doReceiveUdp20001_cmd,(void *) nullptr); // 成功返回0，错误返回错误编号
        if (ret != 0) {
            {
                close(udp_socket20001);
                udp_socket20001 = -1;
                bNeedRead20001 = false;
            }
        } else {
            LOGE_B("Start Read Udp2001 ...");
        }
    }
}


void *doReceiveUdp_cmd(void *dat);

int nReadUdpPort = -1;

void F_StartRead_Udp(int nPort) {
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 1000 * 10;
    struct sockaddr_in myaddr;
    if (rev_udp_socket > 0)
    {
        return;
    }
    rev_udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);



    if (rev_udp_socket > 0) {
        setsockopt(rev_udp_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        if(nWifiInterface!=-1) {
            setsockopt(rev_udp_socket, IPPROTO_IP, SO_BINDTODEVICE, &nWifiInterface,
                       sizeof(nWifiInterface));
        }
    } else {
        return;
    }

    nReadUdpPort = nPort;

    bzero((char *) &myaddr, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(nPort);

    int value = 1;
    int status;
    //status = setsockopt(rev_udp_socket, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));
    //status = setsockopt(rev_udp_socket, SOL_SOCKET, SO_REUSEPORT, &value, sizeof(value));
    int ad = sizeof(myaddr);
    int ac = sizeof(struct sockaddr);
    int res = (int) (::bind(rev_udp_socket, (struct sockaddr *) &myaddr,
                            (socklen_t) (sizeof(myaddr))));
    if (res < 0) {
        DEBUG_PRINT("udp_socket20000 bind failed!");
        shutdown(rev_udp_socket, 0);
        close(rev_udp_socket);
        rev_udp_socket = -1;
        return;
    }

    int ret = -1;
    //if (rev_Udp_thread == -1)
    {
        //ret = pthread_create(&rev_Udp_thread, nullptr, doReceive_cmd, (void *) nullptr); // 成功返回0，错误返回错误编号
        ret = pthread_create(&rev_Udp_thread, nullptr, doReceiveUdp_cmd,
                             (void *) nullptr); // 成功返回0，错误返回错误编号
        if (ret != 0) {
            close(rev_udp_socket);
            rev_udp_socket = -1;
            rev_Udp_thread = -1;
        } else {
            LOGE_B("Start Read Udp ... prot: %d", nPort);
        }
    }

}


void F_SentPictureData2Java(uint8_t *data,int nLen,int nStatus)
{
    //onProgressGetPictureFromVideo_mid
    int needsDetach = 0;
//    if (data == nullptr)
//        return;
//    if (nLen == 0)
//        return;

    JNIEnv *evn = getJNIEnv(&needsDetach);
    if (evn == nullptr) {
        return;
    }
    int nCount = nLen;
    if (nCount > 0) {
        jbyteArray jbarray = evn->NewByteArray(nCount);
        evn->SetByteArrayRegion(jbarray, 0, nCount, (jbyte *)data);
        if (onProgressGetPictureFromVideo_mid != nullptr) {
            evn->CallStaticVoidMethod(objclass, onProgressGetPictureFromVideo_mid, jbarray, nStatus);
        }
        evn->DeleteLocalRef(jbarray);
    }
    else
    {
        jbyte  aa = 1;
        jbyteArray jbarray = evn->NewByteArray(1);
        evn->SetByteArrayRegion(jbarray, 0, 1, (jbyte *)&aa);
        if (onProgressGetPictureFromVideo_mid != nullptr) {
            evn->CallStaticVoidMethod(objclass, onProgressGetPictureFromVideo_mid, jbarray, nStatus);
        }
        evn->DeleteLocalRef(jbarray);
    }

    if (needsDetach)
        gJavaVM->DetachCurrentThread();
}

void F_RevData2Jave(byte *data, int nLen, int nPort) {
    int needsDetach = 0;
    if (data == nullptr)
        return;
    if (nLen == 0)
        return;

    JNIEnv *evn = getJNIEnv(&needsDetach);
    if (evn == nullptr) {
        return;
    }
    int nCount = nLen;
    if (nCount > 0) {
        jbyteArray jbarray = evn->NewByteArray(nCount);
        jbyte *jy = (jbyte *) data;
        evn->SetByteArrayRegion(jbarray, 0, nCount, jy);
        if (onUdpRevData_mid != nullptr) {
            evn->CallStaticVoidMethod(objclass, onUdpRevData_mid, jbarray, nPort);
        }
        evn->DeleteLocalRef(jbarray);
    }

    if (needsDetach)
        gJavaVM->DetachCurrentThread();
}


void *doReceiveUdp_cmd(void *dat) {

    struct sockaddr_in servaddr; /* the server's full addr */
    struct timeval tv;
    fd_set readset;
    size_t nbytes;

    byte *readBuff = new byte[1400];

    socklen_t size = (socklen_t) sizeof(servaddr);

    while (!bNeedExitReadUDP) {
        tv.tv_sec = 0;
        tv.tv_usec = 1000 * 10;

        FD_ZERO(&readset); // 在使用之前总是要清空
        // 开始使用select
        FD_SET(rev_udp_socket, &readset); // 把socka放入要测试的描述符集中

        int nRet = select(rev_udp_socket + 1, &readset, nullptr, nullptr, &tv);// 检测是否有套接口是否可读
        if (nRet <= 0) {
            usleep(1000 * 5);
            continue;
        }
        if (!FD_ISSET(rev_udp_socket, &readset)) {
            usleep(1000 * 5);
            continue;
        }
        memset(readBuff, 0, 1400);
        nbytes = recvfrom(rev_udp_socket, readBuff, 1400, 0, (struct sockaddr *) &servaddr,
                          (socklen_t *) &size);
        if (nbytes <= 0) {
            continue;
        } else {
            F_RevData2Jave(readBuff, nbytes, nReadUdpPort);
        }

    }
    close(rev_udp_socket);
    rev_udp_socket = -1;
    delete[]readBuff;
    LOGE_B("Exit Udp ReadThread");
    return nullptr;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_joyhonest_wifination_wifination_naStartReadUdp(JNIEnv *env, jclass type, jint nPort) {

    // TODO
    if (nPort == 20001 || nPort == 20000)
    {
        bNeedRead20000 = true;
        bNeedRead20001 = true;
        F_StartReadUdp20000();
        F_StartReadUdp20001();
        return 1;
    }

    bNeedExitReadUDP = false;
    F_StartRead_Udp(nPort);
    return 1;

}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_joyhonest_wifination_wifination_naStopReadUdp(JNIEnv *env, jclass type) {

    // TODO
    if (rev_Udp_thread > 0) {
        bNeedExitReadUDP = true;
        void *ret = nullptr;
        pthread_join(rev_Udp_thread, &ret);
        rev_Udp_thread = -1;
    }

    return 1;

}
extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetSnapPhoto(JNIEnv *env, jclass type, jint w, jint h,
                                                        jboolean b) {

    // TODO
    nPhotoW = w;
    nPhotoH = h;
    bPhotoSet = b;

}
extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naWriteData2Flash(JNIEnv *env, jclass type,
                                                           jbyteArray data_, jint nLen2) {
    jbyte *data = env->GetByteArrayElements(data_, nullptr);
    int len_array = env->GetArrayLength(data_);
    uint8_t *msg = new uint8_t[len_array + 9];

    msg[0] = 'J';
    msg[1] = 'H';
    msg[2] = 'C';
    msg[3] = 'M';
    msg[4] = 'D';
    msg[5] = 0x30;
    msg[6] = 0x06;
    msg[7] = (uint8_t) len_array;
    msg[8] = (uint8_t) (len_array >> 8);
    memcpy(msg + 9, data, len_array);
    send_cmd_udp(msg, len_array + 9, sServerIP.c_str(), 20000);
    env->ReleaseByteArrayElements(data_, data, 0);
    delete[]msg;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naReadDataFromFlash(JNIEnv *env, jclass type) {
    // TODO
    uint8_t msg[10];
    msg[0] = 'J';
    msg[1] = 'H';
    msg[2] = 'C';
    msg[3] = 'M';
    msg[4] = 'D';
    msg[5] = 0x30;
    msg[6] = 0x05;
    msg[7] = (uint8_t) 510;
    msg[8] = (uint8_t) (510 >> 8);
    send_cmd_udp(msg, 9, sServerIP.c_str(), 20000);
}


extern "C"
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naGetRtl_1Mode(JNIEnv *env, jclass type) {

    if (mRTL_DownLoad.Connect() == 0) {
        usleep(1000 * 20);
        mRTL_DownLoad.F_GetMode();
    }
    return 0;
}


extern "C"
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naGetRtl_1List(JNIEnv *env, jclass type, jint nType,
                                                        jint inx) {
    if (mRTL_DownLoad.Connect() == 0) {
        usleep(1000 * 20);
        mRTL_DownLoad.F_ReadList((int) nType, inx);
    }
    return 0;

}


extern "C"
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naDownLoadRtlFile(JNIEnv *env, jclass type,
                                                           jstring sFileName_) {
    const char *sFileName = env->GetStringUTFChars(sFileName_, 0);

    if (mRTL_DownLoad.Connect() == 0) {
        usleep(1000 * 20);
        mRTL_DownLoad.DownLoadFile(sFileName);
    }

    env->ReleaseStringUTFChars(sFileName_, sFileName);
    return 0;
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naCancelRTL(JNIEnv *env, jclass type) {

    mRTL_DownLoad.disConnected();
    return 0;

}


pthread_mutex_t mTcp_Lock = PTHREAD_MUTEX_INITIALIZER;

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetLedPWM(JNIEnv *env, jclass type, jbyte nPwm) {

    // TODO
    if(sServerIP.size()<6)
    {
        return;
    }

    uint8_t msg[30];

    pthread_mutex_lock(&m_Tcp_Cmd_lock);
    if(sServerIP == "192.168.34.1")
    {
        msg[0] = 'F';
        msg[1] = 'D';
        msg[2] = 'W';
        msg[3] = 'N';

        msg[4] = 0x20;
        msg[5] = 0x00;

        msg[6] = 0x0E;    //cmd
        msg[7] = 0x00;

        msg[8] = 0x01;    //len
        msg[9] = 0x00;

        msg[10] = (uint8_t)nPwm;           //Data
        send_cmd_udp(msg, 11, sServerIP.c_str(), 20001);
    }
    else if(sServerIP == "192.168.30.1")
    {
        MySocket_GKA  socketGka;

        if(socketGka.Connect(sServerIP,8081) == 0)
        {
            msg[0] = 'G';
            msg[1] = 'P';
            msg[2] = 'S';
            msg[3] = 'O';
            msg[4] = 'C';
            msg[5] = 'K';
            msg[6] = 'E';
            msg[7] = 'T';

            msg[8] = 0x01;
            msg[9] = 0x00;

            msg[10] = 0x04;
            msg[11] = 0x01;

            msg[12] = 0x00;
            msg[13] = 0x14;

            msg[14] = 0x00;
            msg[15] = 0x00;

            msg[16] = 0x01;
            msg[17] = (uint8_t)nPwm;
            MySocketData data;
            data.nLen = 0;
            data.AppendData(msg,18);
            socketGka.Write(&data);
            data.Clean();
            data.nLen =12;
            socketGka.Read(&data,100);
        }
        socketGka.DisConnect();

    }
    else {
        msg[0] = 'J';
        msg[1] = 'H';
        msg[2] = 'C';
        msg[3] = 'M';
        msg[4] = 'D';
        msg[5] = 0x20;
        msg[6] = 0x02;
        msg[7] = (uint8_t) nPwm;
        send_cmd_udp(msg, 8, sServerIP.c_str(), 20000);
    }
    pthread_mutex_unlock(&m_Tcp_Cmd_lock);

}


extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naGetLedPWM(JNIEnv *env, jclass type) {

    // TODO

    if(sServerIP.size()<6)
    {
        return;
    }

    uint8_t msg[32];

    pthread_mutex_lock(&m_Tcp_Cmd_lock);
    if(sServerIP == "192.168.34.1")
    {
        msg[0] = 'F';
        msg[1] = 'D';
        msg[2] = 'W';
        msg[3] = 'N';

        msg[4] = 0x20;
        msg[5] = 0x00;

        msg[6] = 0x0E;    //cmd
        msg[7] = 0x00;

        msg[8] = 0x00;    //len
        msg[9] = 0x00;
        send_cmd_udp(msg, 10, sServerIP.c_str(), 20001);

    }
    else if(sServerIP == "192.168.30.1")
    {
        MySocket_GKA socketGka;
        if(socketGka.Connect(sServerIP,8081)==0)
        {
            msg[0] = 'G';
            msg[1] = 'P';
            msg[2] = 'S';
            msg[3] = 'O';
            msg[4] = 'C';
            msg[5] = 'K';
            msg[6] = 'E';
            msg[7] = 'T';

            msg[8] = 0x01;
            msg[9] = 0x00;

            msg[10] = 0x04;
            msg[11] = 0x00;

            msg[12] = 0x00;
            msg[13] = 0x14;

            msg[14] = 0x00;
            msg[15] = 0x00;
            for (int i = 16; i<32; i++) {
                msg[i] = 0x00;
            }

            MySocketData data;
            data.nLen = 0;
            data.AppendData(msg,32);
            socketGka.Write(&data);
            data.Clean();
            data.nLen =15;
            socketGka.Read(&data,100);
            if(data.nLen >= 15)
            {
                 uint8_t *buffer =(uint8_t *)data.data;
                if(buffer[0] == 'G' &&
                   buffer[1] == 'P' &&
                   buffer[2] == 'S' &&
                   buffer[3] == 'O' &&
                   buffer[4] == 'C' &&
                   buffer[5] == 'K' &&
                   buffer[6] == 'E' &&
                   buffer[7] == 'T' )
                {
                    if (buffer[8] == 0x02) { //成功
                        if ((buffer[11] == 0x0) && (buffer[10] == 0x04)) {
                            int i = buffer[14];
                            onGetLedPwm(i);
                        }
                    }
                }
            }
        }
        socketGka.DisConnect();
    }
    else {
        msg[0] = 'J';
        msg[1] = 'H';
        msg[2] = 'C';
        msg[3] = 'M';
        msg[4] = 'D';
        msg[5] = 0x30;
        msg[6] = 0x07;
        send_cmd_udp(msg, 7, sServerIP.c_str(), 20000);
    }


    pthread_mutex_unlock(&m_Tcp_Cmd_lock);

}

bool bTransferSize = false;
int nTransferWidth = 640;
int nTransferHeight = 360;
AVFrame *pTranFrame = nullptr;


int F_SetTrabsferSize(jint nWidth, jint nHeight) {
    if ((nWidth & 0x07) != 0)    //宽度必须是8的倍数
    {
        return -1;
    }
    if (pTranFrame != nullptr) {
        av_freep(&pTranFrame->data[0]);
        av_frame_free(&pTranFrame);
        pTranFrame = nullptr;
    }

    pTranFrame = av_frame_alloc();

    nTransferWidth = nWidth;
    nTransferHeight = nHeight;

    pTranFrame->width = nTransferWidth;
    pTranFrame->height = nTransferHeight;
    av_image_alloc(pTranFrame->data, pTranFrame->linesize, nWidth,
                   nHeight,
                   AV_PIX_FMT_YUV420P, aligD);
    bTransferSize = true;

    return 0;
}


extern "C"
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naSetTransferSize(JNIEnv *env, jclass type, jint nWidth,
                                                           jint nHeight) {

    return F_SetTrabsferSize(nWidth, nHeight);

}

extern "C"
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naGetDispWH(JNIEnv *env, jclass type) {

    // TODO
    jint wh = 0;
    if (m_FFMpegPlayer.pFrameYUV != nullptr) {
        int w = m_FFMpegPlayer.pFrameYUV->width;
        int h = m_FFMpegPlayer.pFrameYUV->height;
        wh = w * 0x10000 + (h & 0xFFFF);
    }
    return wh;

}



extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetCameraDataRota(JNIEnv *env, jclass type, jint n) {

    // TODO
    if (n == 0 || n == 90 || n == 180 || n == 270) {
        nCameraDataRota = n;
    } else {
        nCameraDataRota = 0;
    }

}


bool bRead_tcp_Socket = false;


void F_SentError(int nError, const char *info) {
    if (nError == 0xFF) {
        if (info != nullptr) {
            memcpy(gp4225File.sFileName, info, 31);
            gp4225File.sFileName[31] = 0;
            F_CallDownloadCallBack(0, gp4225File.sFileName, nError);
            return;
        }
    }
    if (gp4225File.sFileName[0] != 0) {
        gp4225File.sFileName[31] = 0;
        F_CallDownloadCallBack(0, gp4225File.sFileName, nError);
    }
}


byte buffer_[0x48 * 2];
//int  nbufferInx=0;

uint8_t *data_Buffer = nullptr;


MyRevBuffer revBuffer;

FILE *saveFILE = nullptr;
string sfilename_tmp = "";


void F_CancelDownGP4225() {
    _tcp_Socket.DisConnect();
    usleep(1000 * 20);
    if (saveFILE != nullptr) {
        fclose(saveFILE);
        saveFILE = nullptr;
        if (sfilename_tmp.length() > 0) {
            if (access(sfilename_tmp.c_str(), F_OK) == 0) {
                remove(sfilename_tmp.c_str());
            }
        }
    }
    revBuffer.Clear();
}

void Receive_Tcp(char *dataA, int nLen) {

    byte *data = (byte *) dataA;
    uint32_t tmp = nFileRevCont + nLen;
    if (gp4225File.nFileLen <= 0)
        return;
    if (nFileRevCont == 0) {
        if (saveFILE != nullptr) {
            fclose(saveFILE);
            saveFILE = nullptr;
        }
        if (!gp4225File.bPlay) {
            const char *fileName = (const char *) gp4225File.sSaveFileName;
            sfilename_tmp = fileName;
            size_t len = strlen(sfilename_tmp.c_str());
            if (len != 0) {
                string stmp = ".tmp";
                sfilename_tmp = sfilename_tmp + stmp;
                saveFILE = fopen(sfilename_tmp.c_str(), "wb");
            }
        }

    }
    if (!gp4225File.bPlay && saveFILE != nullptr) {
        fwrite(dataA, sizeof(char), nLen, saveFILE);
    }


    if (gp4225File.bPlay)
        revBuffer.SaveBuffer((uint8_t *) dataA, nLen);


    if (nFileRevCont == 0) {
        if (gp4225File.bPlay)
            RevPlay.Play();
    }

    nFileRevCont = tmp;


    //if(!gp4225File.bPlay)
    {

        int nPercent = (int) ((nFileRevCont * ((u_int64_t) 1000)) / gp4225File.nFileLen);

        if (nFileRevCont >= gp4225File.nFileLen) {
            _tcp_Socket.DisConnect();
            if (saveFILE != nullptr) {
                fclose(saveFILE);
                saveFILE = nullptr;
                const char *fileName = (const char *) gp4225File.sSaveFileName;
                int r = rename(sfilename_tmp.c_str(), fileName);
                sfilename_tmp = "";
            }
            F_CallDownloadCallBack(1000, gp4225File.sFileName, 0);
            revBuffer.Clear();

        } else {
            F_CallDownloadCallBack(nPercent, gp4225File.sFileName, 0);
        }
    }

}


pthread_t connectdid = -1;
bool bconnecting = false;


void *connected_tcp(void *dat) {
    pthread_detach(pthread_self());
    _tcp_Socket.bNormalTCP = true;

    GP4225_FileStruct *server_s = (GP4225_FileStruct *) dat;

    if (_tcp_Socket.bConnected) {
        _tcp_Socket.DisConnect();
    }

    if (_tcp_Socket.Connect(server_s->sServerIP, server_s->nPort) == 0) {
        _tcp_Socket.fuc_getData_mjpeg = Receive_Tcp;
        _tcp_Socket.buffLen = 1024 * 20;
        _tcp_Socket.StartReadThread();
        usleep(1000 * 200);
        byte cmd[80];

        cmd[0] = 'F';
        cmd[1] = 'D';

        cmd[2] = 'W';
        cmd[3] = 'N';


        cmd[4] = 0x0A;
        cmd[5] = 0x00;


        cmd[6] = 0x01;
        cmd[7] = 0x00;

        cmd[8] = 0x40;
        cmd[9] = 0x00;

        for (int i = 0; i < 64; i++) {
            cmd[10 + i] = 0;
        }

        memcpy(cmd + 10, server_s->sPaht, 32);
        memcpy(cmd + 42, server_s->sFileName, 32);
        nFileRevCont = 0;

        int nLen = 74;
        uint16_t nPorta = 20001;
        int re = send_cmd_udp((uint8_t *) cmd, nLen, server_s->sServerIP, nPorta);
        LOGE("TCP_connected port %d", server_s->nPort);
    } else {
        F_SentError(0xFF, "Tcp Can not connected!!");
    }
    connectdid = -1;
    return nullptr;
}


void naDisConnectedTCP() {
    _tcp_Socket.DisConnect();
    RevPlay.Stop();
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_joyhonest_wifination_wifination_naDisConnectedTCP(JNIEnv *env, jclass type) {
    naDisConnectedTCP();
    return (jboolean) true;
}


int F_DownOrPlay(const char *sPath, const char *sFileName, int nLen, bool bPlay,
                 const char *sSaveName) {

    if (sPath == nullptr || sFileName == nullptr)
        return 0;
    if (!bPlay && sSaveName == nullptr)
        return 0;

    size_t nLen1 = strlen(sPath);
    size_t nLen2 = strlen(sFileName);


    size_t nLen4 = 0;
    if (!bPlay) {
        if (sSaveName == nullptr)
            return 0;
        else {
            nLen4 = strlen(sSaveName);
        }
    }


    if (nLen1 > 31)
        return 0;
    if (nLen2 > 31)
        return 0;

    if (nLen4 > 255)
        return 0;

    if (connectdid > 0)
        return 0;


    revBuffer.Init();

    memset(&gp4225File, 0, sizeof(GP4225_FileStruct));
    nFileRevCont = 0;

    gp4225File.nPort = 30000;
    memcpy(gp4225File.sPaht, sPath, nLen1);
    memcpy(gp4225File.sFileName, sFileName, nLen2);
    memcpy(gp4225File.sServerIP, sServerIP.c_str(), sServerIP.length());

    gp4225File.nFileLen = nLen;
    gp4225File.bPlay = bPlay;
    if (bPlay) {

    } else {
        memcpy(gp4225File.sSaveFileName, sSaveName, nLen4);
    }
    pthread_create(&connectdid, nullptr, connected_tcp, (void *) (&gp4225File));
    return 1;

}


void F_StopPlay() {
    _tcp_Socket.DisConnect();
    RevPlay.Stop();
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_joyhonest_wifination_wifination_na4225StartPlay(JNIEnv *env, jclass type, jstring sPath_,
                                                         jstring sFileName_, jint nLen) {

    const char *sPath = env->GetStringUTFChars(sPath_, 0);
    const char *sFileName = env->GetStringUTFChars(sFileName_, 0);

    _tcp_Socket.DisConnect();
    F_CancelDownGP4225();

    SwrContext *swrContext = nullptr;

    F_StopPlay();
    revBuffer.Init();
    F_GetServerIP();
    int re = F_DownOrPlay(sPath, sFileName, (int) nLen, true, nullptr);

    env->ReleaseStringUTFChars(sPath_, sPath);
    env->ReleaseStringUTFChars(sFileName_, sFileName);
    return 1;
}




extern "C"
JNIEXPORT jboolean JNICALL
Java_com_joyhonest_wifination_wifination_na4225StartDonwLoad(JNIEnv *env, jclass type,
                                                             jstring sPath_, jstring sFileName_,
                                                             jint nLen, jstring sSaveName_) {

    const char *sPath = env->GetStringUTFChars(sPath_, 0);
    const char *sFileName = env->GetStringUTFChars(sFileName_, 0);
    const char *sSaveName = env->GetStringUTFChars(sSaveName_, 0);
    _tcp_Socket.DisConnect();
    F_CancelDownGP4225();
    F_StopPlay();
    revBuffer.Init();
    F_GetServerIP();
    int re = F_DownOrPlay(sPath, sFileName, (int) nLen, false, sSaveName);
    env->ReleaseStringUTFChars(sPath_, sPath);
    env->ReleaseStringUTFChars(sFileName_, sFileName);
    env->ReleaseStringUTFChars(sSaveName_, sSaveName);
    return 1;
}



extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_na4225_1SetMode(JNIEnv *env, jclass type, jbyte nMode) {

    // TODO
    F_GetServerIP();
    byte cmd[20];

    cmd[0] = 'F';
    cmd[1] = 'D';
    cmd[2] = 'W';
    cmd[3] = 'N';

    cmd[4] = 0x01;
    cmd[5] = 0x00;

    cmd[6] = 0x01;
    cmd[7] = 0x00;

    cmd[8] = 0x01;
    cmd[9] = 0x00;

    cmd[10] = nMode;
    int nLen = 11;
    if (nMode != 0)  //文件模式
    {

        if (bInitA) {
            pthread_mutex_lock(&mOpenCamera_Lock);
            naStop_();
            bOpened = false;
            pthread_mutex_unlock(&mOpenCamera_Lock);
        }
        send_cmd_udp((uint8_t *) cmd, 11, sServerIP.c_str(), 20001);

    } else            //图传模式
    {
        send_cmd_udp((uint8_t *) cmd, 11, sServerIP.c_str(), 20001);
        if (bInitA) {
            naDisConnectedTCP();
            if (!bOpened) {
                const char *pFileName = "";
                bNeedStop = false;
                pthread_mutex_lock(&mOpenCamera_Lock);
                bOpened = true;
                int i32Ret = _naInit_(pFileName);
                pthread_mutex_unlock(&mOpenCamera_Lock);
            }
        }
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_na4225_1GetFileList(JNIEnv *env, jclass type, jint nType,
                                                             jint nStrtinx, jint nEndinx) {

    F_GetServerIP();

    byte cmd[20];
//
    cmd[0] = 'F';
    cmd[1] = 'D';
    cmd[2] = 'W';
    cmd[3] = 'N';


    cmd[4] = 0x02;
    cmd[5] = 0x00;


    cmd[6] = (byte) nType;
    cmd[7] = 0x00;

    cmd[8] = 0x04;
    cmd[9] = 0x00;

    cmd[10] = (byte) (nStrtinx & 0xFF);
    cmd[11] = (byte) ((nStrtinx >> 8) & 0xFF);

    cmd[12] = (byte) (nEndinx & 0xFF);
    cmd[13] = (byte) ((nEndinx >> 8) & 0xFF);

    send_cmd_udp((uint8_t *) cmd, 14, sServerIP.c_str(), 20001);


}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_na4225_1DeleteFile(JNIEnv *env, jclass type,
                                                            jstring sPath_, jstring sFileName_) {
    const char *sPath = env->GetStringUTFChars(sPath_, 0);
    const char *sFileName = env->GetStringUTFChars(sFileName_, 0);

    if (strlen(sFileName) > 32)
        return;

    F_GetServerIP();

    byte cmd[80];

    memset(cmd, 0, 80);

    cmd[0] = 'F';
    cmd[1] = 'D';
    cmd[2] = 'W';
    cmd[3] = 'N';


    cmd[4] = 0x09;
    cmd[5] = 0x00;


    cmd[6] = 0x01;
    cmd[7] = 0x00;

    cmd[8] = 0x40;
    cmd[9] = 0x00;

    memcpy(cmd + 10 + 32, sFileName, strlen(sFileName));

    send_cmd_udp((uint8_t *) cmd, 74, sServerIP.c_str(), 20001);

    env->ReleaseStringUTFChars(sPath_, sPath);
    env->ReleaseStringUTFChars(sFileName_, sFileName);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_na4225_1DeleteAll(JNIEnv *env, jclass type, jint nType) {

    if (nType != 2 && nType != 3 && nType != 4)
        return;

    F_GetServerIP();


    byte cmd[80];

    memset(cmd, 0, 80);

    cmd[0] = 'F';
    cmd[1] = 'D';
    cmd[2] = 'W';
    cmd[3] = 'N';


    cmd[4] = 0x09;
    cmd[5] = 0x00;


    cmd[6] = (byte) nType;
    cmd[7] = 0x00;

    cmd[8] = 0x00;
    cmd[9] = 0x00;

    send_cmd_udp((uint8_t *) cmd, 10, sServerIP.c_str(), 20001);

}


void naStop_() {
    m_FFMpegPlayer.StopSaveVideo();
    bNeedStop = true;

    bNeedRead20001 = false;
    bNeedRead20000 = false;
    if(rev_Udp_thread20000!=-1)
    {
        pthread_join(rev_Udp_thread20000,0);
        rev_Udp_thread20000=-1;
    }
    if(rev_Udp_thread20001!=-1)
    {
        pthread_join(rev_Udp_thread20001,0);
        rev_Udp_thread20001=-1;
    }

    nSDStatus &= (bit1_LocalRecording ^ 0xFF);

    F_SetRecordAudio(0);
    myMediaCoder.F_CloseEncoder();
    F_SetRecordAudio(0);
    F_CloseEncoder();
    F_SendStatus2Jave();
    naStop();
    F_StopPlay();
    nICType = IC_NO;
    F_CancelDownGP4225();
    m_FFMpegPlayer.F_ReleaseMediaCode1();
    bOpened = false;

}


extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_na4225_1SyncTime(JNIEnv *env, jclass clazz,
                                                          jbyteArray data_, jint n_len) {
    // TODO: implement na4225_SyncTime()
    jbyte *data = env->GetByteArrayElements(data_, nullptr);
    int nLen = n_len + 10;
    uint8_t *msg = new uint8_t[nLen];

    msg[0] = 'F';
    msg[1] = 'D';
    msg[2] = 'W';
    msg[3] = 'N';

    msg[4] = 0x20;
    msg[5] = 0x00;

    msg[6] = 0x01;
    msg[7] = 0x00;

    msg[8] = 0x07;
    msg[9] = 0x00;


    memcpy(msg + 10, data, n_len);
    env->ReleaseByteArrayElements(data_, data, 0);
    send_cmd_udp(msg, nLen, sServerIP.c_str(), 20001);
    delete[]msg;

}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_na4225_1ReadTime(JNIEnv *env, jclass clazz) {
    uint8_t msg[10];

    msg[0] = 'F';
    msg[1] = 'D';
    msg[2] = 'W';
    msg[3] = 'N';

    msg[4] = 0x20;
    msg[5] = 0x00;

    msg[6] = 0x01;
    msg[7] = 0x00;

    msg[8] = 0x00;
    msg[9] = 0x00;

    send_cmd_udp(msg, 10, sServerIP.c_str(), 20001);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_na4225_1SetOsd(JNIEnv *env, jclass clazz, jboolean b) {
    uint8_t msg[11];

    msg[0] = 'F';
    msg[1] = 'D';
    msg[2] = 'W';
    msg[3] = 'N';

    msg[4] = 0x20;
    msg[5] = 0x00;

    msg[6] = 0x02;
    msg[7] = 0x00;

    msg[8] = 0x01;
    msg[9] = 0x00;
    if (b)
        msg[10] = 1;
    else
        msg[10] = 0;

    send_cmd_udp(msg, 11, sServerIP.c_str(), 20001);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_na4225_1ReadOsd(JNIEnv *env, jclass clazz) {
    uint8_t msg[10];

    msg[0] = 'F';
    msg[1] = 'D';
    msg[2] = 'W';
    msg[3] = 'N';

    msg[4] = 0x20;
    msg[5] = 0x00;

    msg[6] = 0x02;
    msg[7] = 0x00;

    msg[8] = 0x00;
    msg[9] = 0x00;

    send_cmd_udp(msg, 10, sServerIP.c_str(), 20001);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_na4225_1SetReversal(JNIEnv *env, jclass clazz,
                                                             jboolean b) {
    // TODO: implement na4225_SetReversal()
    uint8_t msg[11];

    msg[0] = 'F';
    msg[1] = 'D';
    msg[2] = 'W';
    msg[3] = 'N';

    msg[4] = 0x20;
    msg[5] = 0x00;

    msg[6] = 0x03;
    msg[7] = 0x00;

    msg[8] = 0x01;
    msg[9] = 0x00;
    if (b)
        msg[10] = 1;
    else
        msg[10] = 0;

    send_cmd_udp(msg, 11, sServerIP.c_str(), 20001);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_na4225_1ReadReversal(JNIEnv *env, jclass clazz) {
    uint8_t msg[10];

    msg[0] = 'F';
    msg[1] = 'D';
    msg[2] = 'W';
    msg[3] = 'N';

    msg[4] = 0x20;
    msg[5] = 0x00;

    msg[6] = 0x03;
    msg[7] = 0x00;

    msg[8] = 0x00;
    msg[9] = 0x00;

    send_cmd_udp(msg, 10, sServerIP.c_str(), 20001);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_na4225_1SetRecordTime(JNIEnv *env, jclass clazz, jint n) {
    uint8_t msg[11];

    msg[0] = 'F';
    msg[1] = 'D';
    msg[2] = 'W';
    msg[3] = 'N';

    msg[4] = 0x20;
    msg[5] = 0x00;

    msg[6] = 0x04;
    msg[7] = 0x00;

    msg[8] = 0x01;
    msg[9] = 0x00;

    msg[10] = (uint8_t) n;

    send_cmd_udp(msg, 11, sServerIP.c_str(), 20001);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_na4225_1ReadRecordTime(JNIEnv *env, jclass clazz) {
    uint8_t msg[10];

    msg[0] = 'F';
    msg[1] = 'D';
    msg[2] = 'W';
    msg[3] = 'N';

    msg[4] = 0x20;
    msg[5] = 0x00;

    msg[6] = 0x04;
    msg[7] = 0x00;

    msg[8] = 0x00;
    msg[9] = 0x00;

    send_cmd_udp(msg, 10, sServerIP.c_str(), 20001);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_na4225_1FormatSD(JNIEnv *env, jclass clazz) {
    // TODO: implement na4225_FormatSD()
    uint8_t msg[11];

    msg[0] = 'F';
    msg[1] = 'D';
    msg[2] = 'W';
    msg[3] = 'N';

    msg[4] = 0x20;
    msg[5] = 0x00;

    msg[6] = 0x08;
    msg[7] = 0x00;

    msg[8] = 0x01;
    msg[9] = 0x00;

    msg[10] = 1;

    send_cmd_udp(msg, 11, sServerIP.c_str(), 20001);


}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_na4225_1ReadFireWareVer(JNIEnv *env, jclass clazz) {
    // TODO: implement na4225_ReadFireWareVer()
    uint8_t msg[11];

    msg[0] = 'F';
    msg[1] = 'D';
    msg[2] = 'W';
    msg[3] = 'N';

    msg[4] = 0x20;
    msg[5] = 0x00;

    msg[6] = 0x09;
    msg[7] = 0x00;

    msg[8] = 0x00;
    msg[9] = 0x00;


    send_cmd_udp(msg, 10, sServerIP.c_str(), 20001);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_na4225_1ResetDevice(JNIEnv *env, jclass clazz) {
    // TODO: implement na4225_ResetDevice()
    uint8_t msg[11];

    msg[0] = 'F';
    msg[1] = 'D';
    msg[2] = 'W';
    msg[3] = 'N';

    msg[4] = 0x20;
    msg[5] = 0x00;

    msg[6] = 0x0A;
    msg[7] = 0x00;

    msg[8] = 0x01;
    msg[9] = 0x00;

    msg[10] = 0x01;

    send_cmd_udp(msg, 11, sServerIP.c_str(), 20001);
}


int naGetVideoThumbnail(JNIEnv *env, const char *sfilename, jobject bitmao);

extern "C"
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naGetVideoThumbnailB(JNIEnv *env, jclass clazz,
                                                              jstring filename, jobject bitmap) {
    // TODO: implement naGetVideoThumbnail()
    const char *sFileName = env->GetStringUTFChars(filename, 0);
    int re = naGetVideoThumbnail(env, sFileName, bitmap);
    env->ReleaseStringUTFChars(filename, sFileName);
    return re;
}



// 自动调焦






void onAdjFocus(int n) {
    if (onAdjFocus_mid != nullptr) {
        int needsDetach = 0;
        JNIEnv *evn = getJNIEnv(&needsDetach);
        if (evn == nullptr) {
            return;
        }
        evn->CallStaticVoidMethod(objclass, onAdjFocus_mid, n);
        if (needsDetach)
            gJavaVM->DetachCurrentThread();
        return;
    }
}


int nScores[100];

int nVcm = 100;

#define   nVcmMin 90
#define   nVcmMax 550
#define     nTp   10
#define   nSteps   ((nVcmMax-nVcmMin)/nTp)


int nStartFocus_inx = -1;


void F_SetVcm() {

    byte cmd[32];
    mVCM_func.b.Vcm_Current = nVcm;
    memset(cmd, 0x00, sizeof(32));
    if (nICType == IC_GPH264_34) {

        cmd[0] = 'F';
        cmd[1] = 'D';
        cmd[2] = 'W';
        cmd[3] = 'N';

        cmd[4] = 0x20;
        cmd[5] = 0x00;

        cmd[6] = 0x0C;    //cmd
        cmd[7] = 0x00;

        cmd[8] = 0x02;    //len
        cmd[9] = 0x00;

        cmd[10] = (byte) mVCM_func.VcmAdjVal;           //Data
        cmd[11] = (byte) (mVCM_func.VcmAdjVal >> 8);;    //Data
        send_cmd_udp(cmd, 12, sServerIP.c_str(), 20001);
        return;
    }

    cmd[0] = 'G';
    cmd[1] = 'P';
    cmd[2] = 'S';
    cmd[3] = 'O';
    cmd[4] = 'C';
    cmd[5] = 'K';
    cmd[6] = 'E';
    cmd[7] = 'T';
    cmd[8] = 0x01;
    cmd[9] = 0x00;
    cmd[10] = 0x04;
    cmd[11] = 0x01;
    cmd[12] = 0x00;
    cmd[13] = 0x15;
    cmd[14] = 0x00;
    cmd[15] = 0x00;
    cmd[16] = 0x02;

    cmd[17] = (byte) (mVCM_func.VcmAdjVal >> 8);        //注意 高低位 与上面罗工的有不同
    cmd[18] = (byte) mVCM_func.VcmAdjVal;    //Data

    send_cmd_tcp((uint8_t *) cmd, 19, (char *) sServerIP.c_str(), 8081);

}

int F_GetVcm() {

    if (nICType == IC_GPH264_34) {

        cmd[0] = 'F';
        cmd[1] = 'D';
        cmd[2] = 'W';
        cmd[3] = 'N';

        cmd[4] = 0x20;
        cmd[5] = 0x00;

        cmd[6] = 0x0B;    //cmd
        cmd[7] = 0x00;

        cmd[8] = 0x00;    //len
        cmd[9] = 0x00;
        send_cmd_udp(cmd, 10, sServerIP.c_str(), 20001);
        return -100;
    }

    if (!mysocket.bConnected) {
        mysocket.Connect((char *) sServerIP.c_str(), 8081);
    }
    if (mysocket.bConnected) {
        byte cmd[32];

        memset(cmd, 0x00, sizeof(32));

        cmd[0] = 'G';
        cmd[1] = 'P';
        cmd[2] = 'S';
        cmd[3] = 'O';
        cmd[4] = 'C';
        cmd[5] = 'K';
        cmd[6] = 'E';
        cmd[7] = 'T';

        cmd[8] = 0x01;
        cmd[9] = 0x00;

        cmd[10] = 0x04;
        cmd[11] = 0x00;

        cmd[12] = 0x00;
        cmd[13] = 0x15;

        cmd[14] = 0x00;
        cmd[15] = 0x00;

        cmd[16] = 0x02;

        cmd[17] = 0x01;
        cmd[18] = 0x00;


        MySocketData data;
        data.AppendData(cmd, 19);
        mysocket.Write(&data);
        MySocketData data1;
        data1.nLen = 16;
        mysocket.Read(&data1, 100);
        mysocket.DisConnect();
        if (data1.nLen >= 16) {
            uint8_t *da = data1.data;
            uint16_t u16 = (*da * 0x100) + *(da + 1);
            VCM_FUNC vcm;
            vcm.VcmAdjVal = u16;
            nVcm = vcm.b.Vcm_Current;
            return nVcm;
        }
    }
    return 0;
}

bool bEnableAdjFocus = true;

void F_SetbAdjFocus_delay(int ms);

bool bTest1 = false;

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naStartAdjFocus(JNIEnv *env, jclass clazz, jint x,
                                                         jint y) {

    F_GetServerIP();
    m_FFMpegPlayer.bAdjFocus = false;
    if (nICType == IC_NO) {
        return;
    }

    if (x < 0 && y < 0) {
        bEnableAdjFocus = false;
        m_FFMpegPlayer.bAdjFocus = false;
        return;
    }

    bEnableAdjFocus = true;

    nStartFocus_inx = -1;
    m_FFMpegPlayer.nAdjX = x;
    m_FFMpegPlayer.nAdjY = y;

    memset(nScores, 0, sizeof(int) * 100);
    memset(&mVCM_func, 0, sizeof(VCM_FUNC));
    bTest1 = !bTest1;

    nVcm = nVcmMin;

    mVCM_func.b.Vcm_Srtc = 3;
    mVCM_func.b.Vcm_Step = 0;
    mVCM_func.b.Vcm_PD = 0;
    mVCM_func.b.Vcm_Flag = 0;
    mVCM_func.b.Vcm_Current = nVcm;
    F_SetVcm();

    F_SetbAdjFocus_delay(nAdjDelay);

}


void *SetAdj_thread(void *dat) {
    pthread_detach(pthread_self());
    int *ms = (int *) dat;
    int nMs = 1000 * (*ms);
    usleep(nMs);
    m_FFMpegPlayer.bAdjFocus = true;
    return nullptr;
}

void F_SetbAdjFocus_delay(int ms) {
    pthread_t id;
    int i, ret;
    if (ms > 0) {
        ret = pthread_create(&id, nullptr, SetAdj_thread, (void *) &ms);
    } else {
        m_FFMpegPlayer.bAdjFocus = true;
    }
}

void F_GetEvaluation(int nEvaluation) {
    if (!bEnableAdjFocus)
        return;

    if (nStartFocus_inx < 0) {
        nVcm = nVcmMin;
        F_SetVcm();
        nStartFocus_inx = 0;
        F_SetbAdjFocus_delay(nAdjDelay);
        return;
    }

    if (nStartFocus_inx < 100) {
        nScores[nStartFocus_inx] = nEvaluation;
    }

    nStartFocus_inx++;

    if (nStartFocus_inx > nSteps) {
        int max = nScores[0];
        int xx = 1;
        int inx = 0;
        for (xx = 1; xx < nSteps; xx++) {
            if (nScores[xx] > max) {
                max = nScores[xx];
                inx = xx;
            }
        }
        m_FFMpegPlayer.bAdjFocus = false;
        nVcm = nVcmMin + inx * nTp;
        F_SetVcm();
        LOGE_B("nVcm = %d va = %d", nVcm, mVCM_func.b.Vcm_Current);
        onAdjFocus(nVcm);
        return;
    } else {
        //m_FFMpegPlayer.bAdjFocus=false;
        nVcm = nVcmMin + nStartFocus_inx * nTp;
        F_SetVcm();
        F_SetbAdjFocus_delay(nAdjDelay);

    }
}


extern "C"
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naGetVcm(JNIEnv *env, jclass clazz) {
    F_GetServerIP();
    if (nICType == IC_NO) {
        return -1;
    }
    return F_GetVcm();
}


extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetSaveTypeByName(JNIEnv *env, jclass clazz,
                                                             jboolean b) {
    // TODO: implement naSetSaveType()
    bSaveByName = b;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetnAdjDelay(JNIEnv *env, jclass clazz, jint nms) {
    // TODO: implement naSetnAdjDelay()
    nAdjDelay = nms;
}




extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSet3DDenoiser(JNIEnv *env, jclass clazz,
                                                         jboolean b) {
// TODO: implement naSet3DNoiseReduction()
    if (b != b3D_noise_reduction) {
        bFilterChanged = true;
        b3D_noise_reduction = b;
        //b3D_noise_reduction_bak = b;
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetEnableRotate(JNIEnv *env, jclass clazz, jboolean b) {
    // TODO: implement naSetEnableRotate()
    if (bEnableRotateFilter != b) {
        bFilterChanged = true;
        bEnableRotateFilter = b;
    }
}


extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetUVCA_1Brightness(JNIEnv *env, jclass clazz,
                                                               jint n_brightness) {
    // TODO: implement naSetUVCA_Brightness()


    cmd[0] = 'U';
    cmd[1] = 'V';
    cmd[2] = 'C';
    cmd[3] = 'A';

    cmd[4] = 0x00;
    cmd[5] = 0x01;
    cmd[6] = (uint8_t) n_brightness;
    send_cmd_udp(cmd, 7, sServerIP.c_str(), 20000);


}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetUVCA_1Contrast(JNIEnv *env, jclass clazz,
                                                             jint n_contrast) {
    // TODO: implement naSetUVCA_Contrast()

    cmd[0] = 'U';
    cmd[1] = 'V';
    cmd[2] = 'C';
    cmd[3] = 'A';

    cmd[4] = 0x00;
    cmd[5] = 0x02;
    cmd[6] = (uint8_t) n_contrast;
    send_cmd_udp(cmd, 7, sServerIP.c_str(), 20000);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetUVCA_1Saturation(JNIEnv *env, jclass clazz,
                                                               jint naturation) {
    // TODO: implement naSetUVCA_Saturation()

    cmd[0] = 'U';
    cmd[1] = 'V';
    cmd[2] = 'C';
    cmd[3] = 'A';

    cmd[4] = 0x00;
    cmd[5] = 0x03;
    cmd[6] = (uint8_t) naturation;
    send_cmd_udp(cmd, 7, sServerIP.c_str(), 20000);

}
extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetUVCA_1Zoom(JNIEnv *env, jclass clazz, jint n_zoom) {
    // TODO: implement naSetUVCA_Zoom()

    cmd[0] = 'U';
    cmd[1] = 'V';
    cmd[2] = 'C';
    cmd[3] = 'A';

    cmd[4] = 0x00;
    cmd[5] = 0x04;
    cmd[6] = (uint8_t) n_zoom;
    send_cmd_udp(cmd, 7, sServerIP.c_str(), 20000);

}
extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetUVCA_1Panorama(JNIEnv *env, jclass clazz,
                                                             jint n_panorama) {
    // TODO: implement naSetUVCA_Panorama()

    cmd[0] = 'U';
    cmd[1] = 'V';
    cmd[2] = 'C';
    cmd[3] = 'A';

    cmd[4] = 0x00;
    cmd[5] = 0x05;
    cmd[6] = (uint8_t) n_panorama;
    send_cmd_udp(cmd, 7, sServerIP.c_str(), 20000);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetUVCA_1Inclination(JNIEnv *env, jclass clazz,
                                                                jint n_inclination) {
    // TODO: implement naSetUVCA_Inclination()

    cmd[0] = 'U';
    cmd[1] = 'V';
    cmd[2] = 'C';
    cmd[3] = 'A';

    cmd[4] = 0x00;
    cmd[5] = 0x06;
    cmd[6] = (uint8_t) n_inclination;
    send_cmd_udp(cmd, 7, sServerIP.c_str(), 20000);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetUVCA_1Roll(JNIEnv *env, jclass clazz, jint n_roll) {
    // TODO: implement naSetUVCA_Roll()

    cmd[0] = 'U';
    cmd[1] = 'V';
    cmd[2] = 'C';
    cmd[3] = 'A';

    cmd[4] = 0x00;
    cmd[5] = 0x07;
    cmd[6] = (uint8_t) n_roll;
    send_cmd_udp(cmd, 7, sServerIP.c_str(), 20000);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naGetUVCA_1Brightness(JNIEnv *env, jclass clazz) {
    // TODO: implement naGetUVCA_Brightness()

    cmd[0] = 'U';
    cmd[1] = 'V';
    cmd[2] = 'C';
    cmd[3] = 'A';

    cmd[4] = 0x01;
    cmd[5] = 0x01;

    cmd[6] = 0;
    send_cmd_udp(cmd, 7, sServerIP.c_str(), 20000);

}
extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naGetUVCA_1Contrast(JNIEnv *env, jclass clazz) {
    // TODO: implement naGetUVCA_Contrast()

    cmd[0] = 'U';
    cmd[1] = 'V';
    cmd[2] = 'C';
    cmd[3] = 'A';

    cmd[4] = 0x01;
    cmd[5] = 0x02;

    cmd[6] = 0;
    send_cmd_udp(cmd, 7, sServerIP.c_str(), 20000);

}
extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naGetUVCA_1Saturation(JNIEnv *env, jclass clazz) {
    // TODO: implement naGetUVCA_Saturation()

    cmd[0] = 'U';
    cmd[1] = 'V';
    cmd[2] = 'C';
    cmd[3] = 'A';

    cmd[4] = 0x01;
    cmd[5] = 0x03;

    cmd[6] = 0;
    send_cmd_udp(cmd, 7, sServerIP.c_str(), 20000);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naGetUVCA_1Zoom(JNIEnv *env, jclass clazz) {
    // TODO: implement naGetUVCA_Zoom()
    cmd[0] = 'U';
    cmd[1] = 'V';
    cmd[2] = 'C';
    cmd[3] = 'A';

    cmd[4] = 0x01;
    cmd[5] = 0x04;

    cmd[6] = 0;
    send_cmd_udp(cmd, 7, sServerIP.c_str(), 20000);

}
extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naGetUVCA_1Panorama(JNIEnv *env, jclass clazz) {
    // TODO: implement naGetUVCA_Panorama()

    cmd[0] = 'U';
    cmd[1] = 'V';
    cmd[2] = 'C';
    cmd[3] = 'A';

    cmd[4] = 0x01;
    cmd[5] = 0x05;

    cmd[6] = 0;
    send_cmd_udp(cmd, 7, sServerIP.c_str(), 20000);

}
extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naGetUVCA_1Inclination(JNIEnv *env, jclass clazz) {
    // TODO: implement naGetUVCA_Inclination()

    cmd[0] = 'U';
    cmd[1] = 'V';
    cmd[2] = 'C';
    cmd[3] = 'A';

    cmd[4] = 0x01;
    cmd[5] = 0x06;

    cmd[6] = 0;
    send_cmd_udp(cmd, 7, sServerIP.c_str(), 20000);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naGetUVCA_1Roll(JNIEnv *env, jclass clazz) {
    // TODO: implement naGetUVCA_Roll()

    cmd[0] = 'U';
    cmd[1] = 'V';
    cmd[2] = 'C';
    cmd[3] = 'A';
    cmd[4] = 0x01;
    cmd[5] = 0x07;
    cmd[6] = 0;
    send_cmd_udp(cmd, 7, sServerIP.c_str(), 20000);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSentPhotoKey(JNIEnv *env, jclass clazz) {
    // TODO: implement naSentPhotoKey()
    cmd[0] = 'U';
    cmd[1] = 'V';
    cmd[2] = 'C';
    cmd[3] = 'A';
    cmd[4] = 0x00;
    cmd[5] = 0x20;
    cmd[6] = 0;
    send_cmd_udp(cmd, 7, sServerIP.c_str(), 20000);
}


extern bool bNeedChangedRotate;
extern float nFilterRotate;
extern float nPreAngle;

extern "C"
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naSetRotate(JNIEnv *env, jclass clazz, jint n_rotate) {
    // TODO: implement naSetRotate()
    if (!bEnableRotateFilter)
        return 0;
    nFilterRotate = n_rotate;
    nPreAngle = n_rotate;
    bNeedChangedRotate = true;
    return 1;
}


bool bEnableGsensor = false;
bool bEnableCircul = false;

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetSensorA(JNIEnv *env, jclass clazz, jboolean b) {
    // TODO: implement naSetSensorA()
    byte on = 0;
//    if(b)
    on = 1;
    F_GetServerIP();
    byte cmd[20];
    cmd[0] = 'F';
    cmd[1] = 'D';
    cmd[2] = 'W';
    cmd[3] = 'N';
    cmd[4] = 0x20;
    cmd[5] = 0x00;

    cmd[6] = 0x12;
    cmd[7] = 0x00;

    cmd[8] = 0x01;
    cmd[9] = 0x00;

    cmd[10] = on;

    send_cmd_udp((uint8_t *) cmd, 11, sServerIP.c_str(), 20001);
//    if(on==0)
//    {
//        nFilterRotate=0;
//        nPreAngle = 0;
//        bNeedChangedRotate = true;
//    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetSensor(JNIEnv *env, jclass clazz, jboolean b) {
    // TODO: implement naSetSensor()

    //F_InitGSensorData();

    bEnableGsensor = b;
    byte on = 0;
//    if(b)
    on = 1;
    F_GetServerIP();
    byte cmd[20];
    cmd[0] = 'F';
    cmd[1] = 'D';
    cmd[2] = 'W';
    cmd[3] = 'N';
    cmd[4] = 0x20;
    cmd[5] = 0x00;

    cmd[6] = 0x12;
    cmd[7] = 0x00;

    cmd[8] = 0x01;
    cmd[9] = 0x00;

    cmd[10] = on;

    send_cmd_udp((uint8_t *) cmd, 11, sServerIP.c_str(), 20001);
    if (on == 0) {
        nFilterRotate = 0;
        nPreAngle = 0;
        bNeedChangedRotate = true;
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetAcdetection(JNIEnv *env, jclass clazz, jboolean b) {
    // TODO: implement naSetAcdetection()
    byte on = 0;
    if (b)
        on = 1;
    F_GetServerIP();
    byte cmd[20];
    cmd[0] = 'F';
    cmd[1] = 'D';
    cmd[2] = 'W';
    cmd[3] = 'N';
    cmd[4] = 0x20;
    cmd[5] = 0x00;

    cmd[6] = 0x13;
    cmd[7] = 0x00;

    cmd[8] = 0x01;
    cmd[9] = 0x00;

    cmd[10] = on;

    send_cmd_udp((uint8_t *) cmd, 11, sServerIP.c_str(), 20001);
}


#define PP  (180.0 / (double)3.1415926)
#define PI    ((double)3.1415926)

int nGsenLen = 8;
float nAdjAngle = 2.0f;

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetGsensorPara(JNIEnv *env, jclass clazz, jint n,
                                                          jfloat nAngle) {
    // TODO: implement naSetGsensorPara()
    if (nAngle > 0.0f && nAngle < 20.0f) {
        nAdjAngle = nAngle;
    }

    if (n > 0 && n <= 50) {
        nGsenLen = n;
    }
}


void F_SortArray(int32_t *nums, int nLen) {
    //int nums[10] = {4, 5, 2, 10, 7, 1, 8, 3, 6, 9};
    int i, j, temp, isSorted;

    //优化算法：最多进行 n-1 轮比较
    for (i = 0; i < nLen - 1; i++) {
        isSorted = 1;  //假设剩下的元素已经排序好了
        for (j = 0; j < nLen - 1 - i; j++) {
            if (nums[j] > nums[j + 1]) {
                temp = nums[j];
                nums[j] = nums[j + 1];
                nums[j + 1] = temp;
                isSorted = 0;  //一旦需要交换数组元素，就说明剩下的元素没有排序好
            }
        }
        if (isSorted)
            break; //如果没有发生交换，说明剩下的元素已经排序好了
    }
//    for(i=0; i<10; i++){
//        printf("%d ", nums[i]);
//    }
//    printf("\n");
}




extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetGsensor2SDK(JNIEnv *env, jclass clazz, jint xx1,
                                                          jint yy1, jint zz1) {

    jint xx, yy, zz;
    xx = xx1;
    yy = yy1;
    zz = zz1;

    if (nGSInx < nGsenLen) {
        GX[nGSInx] = xx;
        GY[nGSInx] = yy;
        GZ[nGSInx] = zz;
        nGSInx++;
    } else {
        memmove(GX, GX + 1, (nGsenLen - 1) * sizeof(int32_t));
        memmove(GY, GY + 1, (nGsenLen - 1) * sizeof(int32_t));
        memmove(GZ, GZ + 1, (nGsenLen - 1) * sizeof(int32_t));

        GX[nGsenLen - 1] = xx;
        GY[nGsenLen - 1] = yy;
        GZ[nGsenLen - 1] = zz;


        int32_t nCx = 0;
        int32_t nCy = 0;
        int32_t nCz = 0;


        for (int x = 0; x < nGsenLen; x++)
        {
            nCx += GX[x];
            nCy += GY[x];
            nCz += GZ[x];
        }

        xx = nCx / (nGsenLen);
        yy = nCy / (nGsenLen);
        zz = nCz / (nGsenLen);

        double yz_plane = (double) (sqrt((yy) * (yy) + (zz) * (zz)));
        double xz_plane = (double) (sqrt((xx) * (xx) + (zz) * (zz)));
        double xy_plane = (double) (sqrt((xx) * (xx) + (yy) * (yy)));

        float θx, θy, θz;

        if (yz_plane == 0) {
            θx = 90.0;
        } else {
            θx = (float) (atan(xx / yz_plane) * PP);
        }
        if (xz_plane == 0) {
            θy = 90.0;
        } else {
            θy = (float) (atan(yy / xz_plane) * PP);
        }
        if (xy_plane == 0) {
            θz = 90.0;
        } else {
            θz = (float) (atan(zz / xy_plane) * PP);
        }

        //LOGE("angle2 = %f  %f  %f", θx,θy,θz);

        //int x1 = θx;
//        double PS = sin(θx*PI/180);
//        PS = abs(PS);
//        float da = abs(θy);
//        da /=2;
        if (θx > 0)
        {
//            if(abs((int)θx)<4)
//                θx +=da;
            if (θy > 0) {
                if (θx > 4)  //6
                    θx += θy/2;
            } else {
                if (θx > 4)   //6
                    θx -= θy/2;
            }
        } else {
//            if(abs((int)θx)<4)
//                θx -=da;
            if (θy > 0) {
                if (θx < -4)
                    θx -= θy/2;
            } else {
                if (θx < -4)
                    θx += θy/2;
            }
        }

        //LOGE("angle6 = %f  %f  %f", θx,θy,θz);

        if (zz < 0) {
            θx = 180 - θx;
        }

//        θx = -θx;

        float dd = θx - nPreAngle;
        dd = abs(dd);

//        if(((int)(abs(θx)*10) <=5))
//        {
//            θx = 0.0f;
//            dd = 1.6f;
//        }

        //if(nPreAngle !=θx)
        if (dd >= 1.0f ) {
            nFilterRotate = θx;
            nPreAngle = θx;
            bNeedChangedRotate = true;
            F_OnGetAngle(θx);
            //LOGE("angle3 = %f", θx);
        }
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetCircul(JNIEnv *env, jclass clazz, jboolean b) {
    // TODO: implement naSetCircul()
    bEnableCircul = b;
}

int nleftTopDx = 0;
int nleftTopDy = 0;

int nrightbottomDx = 0;
int nrightbottomDy = 0;

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetDarkcornerWidth(JNIEnv *env, jclass clazz,
                                                              jint nleftTopDx_, jint nleftTopDy_,
                                                              jint nrightbottomDx_,
                                                              jint nrightbottomDy_) {
    // TODO: implement naSetAdjWidth()
    nleftTopDx = nleftTopDx_;
    nleftTopDy = nleftTopDy_;
    nrightbottomDx = nrightbottomDx_;
    nrightbottomDy = nrightbottomDy_;


}


extern MyMp4 myMp4;

int F_Convert(const char *path, const char *out_filename);

extern "C"
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_HiSi_1Convert(JNIEnv *env, jclass clazz, jstring sPath_,
                                                       jstring sOutPath_) {
    // TODO: implement HiSi_Convert()
    const char *sPath = env->GetStringUTFChars(sPath_, 0);
    const char *sOutPath = env->GetStringUTFChars(sOutPath_, 0);
    myMp4.F_SetConvert(true);
    int ret = F_Convert(sPath, sOutPath);
    env->ReleaseStringUTFChars(sPath_, sPath);
    env->ReleaseStringUTFChars(sOutPath_, sOutPath);
    return ret;
}



extern "C"
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naSetIR(JNIEnv *env, jclass clazz, jint b_enable) {
    // TODO: implement naSetIR()
    F_GetServerIP();
    byte cmd[11];
    cmd[0] = 'F';
    cmd[1] = 'D';
    cmd[2] = 'W';
    cmd[3] = 'N';
    cmd[4] = 0x20;
    cmd[5] = 0x00;
    cmd[6] = 0x15;
    cmd[7] = 0x00;
    cmd[8] = 0x01;
    cmd[9] = 0x00;
    cmd[10] = (byte) b_enable;
    send_cmd_udp((uint8_t *) cmd, 11, sServerIP.c_str(), 20001);
    return 0;

}

extern "C"
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naReadIR(JNIEnv *env, jclass clazz) {
    // TODO: implement naReadIR()
    F_GetServerIP();
    byte cmd[10];
    cmd[0] = 'F';
    cmd[1] = 'D';
    cmd[2] = 'W';
    cmd[3] = 'N';
    cmd[4] = 0x20;
    cmd[5] = 0x00;
    cmd[6] = 0x15;
    cmd[7] = 0x00;
    cmd[8] = 0x00;
    cmd[9] = 0x00;
    send_cmd_udp((uint8_t *) cmd, 10, sServerIP.c_str(), 20001);
    return 0;
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naSetPIR(JNIEnv *env, jclass clazz, jboolean b_enable) {
    // TODO: implement naSetPIR()
    F_GetServerIP();
    byte cmd[11];
    cmd[0] = 'F';
    cmd[1] = 'D';
    cmd[2] = 'W';
    cmd[3] = 'N';
    cmd[4] = 0x20;
    cmd[5] = 0x00;
    cmd[6] = 0x16;
    cmd[7] = 0x00;
    cmd[8] = 0x01;
    cmd[9] = 0x00;
    cmd[10] = (b_enable ? 1 : 0);

    send_cmd_udp((uint8_t *) cmd, 11, sServerIP.c_str(), 20001);
    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naReadPIR(JNIEnv *env, jclass clazz) {
    // TODO: implement naReadPIR()
    F_GetServerIP();
    byte cmd[10];
    cmd[0] = 'F';
    cmd[1] = 'D';
    cmd[2] = 'W';
    cmd[3] = 'N';
    cmd[4] = 0x20;
    cmd[5] = 0x00;
    cmd[6] = 0x16;
    cmd[7] = 0x00;
    cmd[8] = 0x00;
    cmd[9] = 0x00;
    send_cmd_udp((uint8_t *) cmd, 10, sServerIP.c_str(), 20001);
    return 0;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetStatusLedDisp(JNIEnv *env, jclass clazz, jboolean b) {
    // TODO: implement naSetStatusLedDisp()
    F_GetServerIP();
    byte cmd[11];
    cmd[0] = 'F';
    cmd[1] = 'D';
    cmd[2] = 'W';
    cmd[3] = 'N';
    cmd[4] = 0x20;
    cmd[5] = 0x00;
    cmd[6] = 0x17;
    cmd[7] = 0x00;
    cmd[8] = 0x01;
    cmd[9] = 0x00;
    cmd[10] = b ? 1 : 0;
    send_cmd_udp((uint8_t *) cmd, 11, sServerIP.c_str(), 20001);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naReadStatusLedDisp(JNIEnv *env, jclass clazz) {
    // TODO: implement naReadStatusLedDisp()
    F_GetServerIP();
    byte cmd[10];
    cmd[0] = 'F';
    cmd[1] = 'D';
    cmd[2] = 'W';
    cmd[3] = 'N';
    cmd[4] = 0x20;
    cmd[5] = 0x00;
    cmd[6] = 0x17;
    cmd[7] = 0x00;
    cmd[8] = 0x00;
    cmd[9] = 0x00;
    send_cmd_udp((uint8_t *) cmd, 10, sServerIP.c_str(), 20001);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_na4225_1ReadDeviceInfo(JNIEnv *env, jclass clazz) {
    // TODO: implement na4225_ReadDeviceInfo()

    {
        F_GetServerIP();
        byte cmd[10];
        cmd[0] = 'F';
        cmd[1] = 'D';
        cmd[2] = 'W';
        cmd[3] = 'N';
        cmd[4] = 0x20;
        cmd[5] = 0x00;
        cmd[6] = 0x50;
        cmd[7] = 0x00;
        cmd[8] = 0x00;
        cmd[9] = 0x00;
        send_cmd_udp((uint8_t *) cmd, 10, sServerIP.c_str(), 20001);
        return;
    }

    return;
}


extern "C"
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naGetUvcCameraCount(JNIEnv *env, jclass clazz) {
    // TODO: implement naGetUvcCameraCount()
    int fd = open("/dev/bus/usb/001/002", O_RDWR, 0);
    fd = 0;
    return 0;
}


extern "C"
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naOpenUsbCamera(JNIEnv *env, jclass clazz,
                                                         jstring s_usb_camera_name) {
    const char *sPath = env->GetStringUTFChars(s_usb_camera_name, 0);

    //int fd = open(sPath,O_RDONLY,0);
    int fd = open(sPath, O_RDONLY, 0);
    env->ReleaseStringUTFChars(s_usb_camera_name, sPath);
    return 0;

}


MyPlayer *myPlayer = nullptr;


extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSeek(JNIEnv *env, jclass clazz, jfloat n_sec) {
    // TODO: implement naSeek()
    if (myPlayer == nullptr) {
        return;
    }
//    if(!myPlayer->bisPlaying)
//    {
//        return ;
//    }
    if (strlen(myPlayer->sPlayName) == 0)
        return;
    myPlayer->Seek(n_sec);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naStopPlay(JNIEnv *env, jclass clazz, jfloat n_sec) {
    // TODO: implement naStopPlay()
    if (myPlayer == nullptr) {
        return;
    }
    myPlayer->nSeekTime = -1;
    myPlayer->Stop();
    myPlayer->ReleaseObject();


}
extern "C"
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naPlayFlieB(JNIEnv *env, jclass clazz, jstring s_file_name,
                                                     jobject back_called) {


    if (myPlayer == nullptr) {
        myPlayer = new MyPlayer();
    }
    int ret = -1;
    if (myPlayer->bisPlaying)
        return ret;

    myPlayer->F_SetCallBack(back_called);

    const char *sPath = env->GetStringUTFChars(s_file_name, 0);
    ret = myPlayer->Play((char *) sPath);
    env->ReleaseStringUTFChars(s_file_name, sPath);
    return 0;

}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naPause(JNIEnv *env, jclass clazz) {
    // TODO: implement naPause()
    if (myPlayer == nullptr) {
        return;
    }
    if (!myPlayer->bisPlaying) {
        return;
    }

    myPlayer->Pause();
}


extern "C"
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naTransferData(JNIEnv *env, jclass clazz,
                                                        jbyteArray data_) {
    // TODO: implement naTransferData()
    jbyte *data = env->GetByteArrayElements(data_, nullptr);
    int nLen = env->GetArrayLength(data_);
    uint8_t *msg = new uint8_t[nLen + 10];

    msg[0] = 'F';
    msg[1] = 'D';
    msg[2] = 'W';
    msg[3] = 'N';

    msg[4] = 0x21;
    msg[5] = 0x00;

    msg[6] = 0x01;
    msg[7] = 0x00;

    msg[8] = (uint8_t) nLen;
    msg[9] = (uint8_t) (nLen >> 8);

    memcpy(msg + 10, data, nLen);
    env->ReleaseByteArrayElements(data_, data, 0);
    int re = send_cmd_udp(msg, nLen, sServerIP.c_str(), 20001);
    delete[]msg;
    if (re == 0) {
        return nLen;
    } else {
        return -1;
    }


}


#ifdef __cplusplus
extern "C" {
#endif
int http_post_upload_pic(char *data, int datLen, char *filename);
#ifdef __cplusplus
}
#endif


extern "C"
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_RTL_1Post(JNIEnv *env, jclass clazz, jbyteArray data_,
                                                   jint data_len) {
    // TODO: implement RTL_Post()

    jbyte *data = env->GetByteArrayElements(data_, nullptr);

    // int ret = http_post_upload_pic((char *)data,55,(char *)"abcef.png");

    env->ReleaseByteArrayElements(data_, data, 0);

    return 0;


}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetAngle(JNIEnv *env, jclass clazz, jfloat angle) {
    // TODO: implement naSetAngle()
    if (bEnableRotateFilter) {
        nFilterRotate = angle;
        nPreAngle = angle;
        bNeedChangedRotate = true;
    }
}

bool bSquare = false;

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetsquare(JNIEnv *env, jclass clazz, jboolean b) {
    // TODO: implement naSetsquare()
    bSquare = b;
}





//PicturesFromVideo  mPicturesFromVideo;


//MyMp4    mMp4;

JH_GetPics  mJH_GetPics;

extern "C"
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naGetPictListFromVideo_1(JNIEnv *env, jclass clazz,
                                                                  jstring s_path) {
    // TODO: implement naGetPictListFromVideo_()
    int nRet = 0;

    const char *sPath = env->GetStringUTFChars(s_path, 0);
   nRet = mJH_GetPics.StartGetPicture((char *)sPath);
    env->ReleaseStringUTFChars(s_path, sPath);
    return nRet;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naCancelGetPicList(JNIEnv *env, jclass clazz) {
    // TODO: implement naCancelGetPicList()
    mJH_GetPics.bProgress = false;

}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naStartRead20000_120001(JNIEnv *env, jclass clazz) {
    // TODO: implement naStartRead20000_20001()
    bNeedRead20000 = true;
    bNeedRead20001 = true;
    F_StartReadUdp20000();
    F_StartReadUdp20001();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_na4225_1ReadStatus(JNIEnv *env, jclass type) {


    F_GetServerIP();
    byte cmd[10];
    cmd[0] = 'F';
    cmd[1] = 'D';
    cmd[2] = 'W';
    cmd[3] = 'N';
    cmd[4] = 0x00;
    cmd[5] = 0x00;
    cmd[6] = 0x01;
    cmd[7] = 0x00;
    cmd[8] = 0x00;
    cmd[9] = 0x00;

    send_cmd_udp((uint8_t *) cmd, 10, sServerIP.c_str(), 20001);

}


int onGetBattery(int i)
{
    int needsDetach = 0;
    JNIEnv *evn = getJNIEnv(&needsDetach);
    if (evn == nullptr) {
        return -1;
    }
    //onGetLedPwm_mid

    if (onGetBattery_mid != nullptr) {
        evn->CallStaticVoidMethod(objclass, onGetBattery_mid, i);
    }
    if (needsDetach)
        gJavaVM->DetachCurrentThread();
    return 0;

}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naGetBattery(JNIEnv *env, jclass clazz) {
    // TODO: implement naGetBattery()

    F_GetServerIP();
    if(sServerIP.size()<6)
    {
        return;
    }
    uint8_t cmd[80];
    pthread_mutex_lock(&m_Tcp_Cmd_lock);
    if(sServerIP == "192.168.34.1")
    {
        byte cmd[10];
        cmd[0] = 'F';
        cmd[1] = 'D';
        cmd[2] = 'W';
        cmd[3] = 'N';
        cmd[4] = 0x00;
        cmd[5] = 0x00;
        cmd[6] = 0x01;
        cmd[7] = 0x00;
        cmd[8] = 0x00;
        cmd[9] = 0x00;
        send_cmd_udp((uint8_t *) cmd, 10, sServerIP.c_str(), 20001);

    }
    else if(sServerIP == "192.168.30.1")
    {
        MySocket_GKA socketGka;
        if(socketGka.Connect(sServerIP,8081)==0)
        {
            cmd[0] = 'G';
            cmd[1] = 'P';
            cmd[2] = 'S';
            cmd[3] = 'O';
            cmd[4] = 'C';
            cmd[5] = 'K';
            cmd[6] = 'E';
            cmd[7] = 'T';

            cmd[8] = 0x01;
            cmd[9] = 0x00;

            cmd[10] = 0x00;
            cmd[11] = 0x01;

            cmd[12] = 0x00;
            cmd[13] = 0x80;

            cmd[14] = 0x1A;
            cmd[15] = 0x32;

            for (int i = 16; i<32; i++) {
                cmd[i] = 0x00;
            }

            MySocketData data;
            data.nLen = 0;
            data.AppendData(cmd,32);
            socketGka.Write(&data);
            data.Clean();
            data.nLen =17;
            socketGka.Read(&data,100);
            if(data.nLen >= 17)
            {
                uint8_t *buffer =(uint8_t *)data.data;
                if(buffer[0] == 'G' &&
                   buffer[1] == 'P' &&
                   buffer[2] == 'S' &&
                   buffer[3] == 'O' &&
                   buffer[4] == 'C' &&
                   buffer[5] == 'K' &&
                   buffer[6] == 'E' &&
                   buffer[7] == 'T' )
                {
                    if (buffer[8] == 0x02) { //成功
                        if ((buffer[11] == 0x01) && (buffer[10] == 0x00)) {
                            int i = buffer[16]&0x7F;
                            onGetBattery(i);
                        }
                    }
                }
            }
        }
        socketGka.DisConnect();
    }
    else {
//        cmd[0] = 'J';
//        cmd[1] = 'H';
//        cmd[2] = 'C';
//        cmd[3] = 'M';
//        cmd[4] = 'D';
//        cmd[5] = 0x30;
//        cmd[6] = 0x07;
//        send_cmd_udp(cmd, 7, sServerIP.c_str(), 20000);
    }


    pthread_mutex_unlock(&m_Tcp_Cmd_lock);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naGetWifiSSID(JNIEnv *env, jclass clazz) {
    // TODO: implement naGetWifiSSID()
    F_GetServerIP();
    if(sServerIP.size()<6)
    {
        return;
    }
    pthread_mutex_lock(&m_Tcp_Cmd_lock);
    if(sServerIP == "192.168.34.1")
    {
        uint8_t msg[80];
        msg[0] = 'F';
        msg[1] = 'D';
        msg[2] = 'W';
        msg[3] = 'N';

        msg[4] = 0x20;
        msg[5] = 0x00;

        msg[6] = 0x05;              //cmd
        msg[7] = 0x00;

        msg[8] = 0;              //len
        msg[9] = 0;
        send_cmd_udp((uint8_t *) msg, 10, sServerIP.c_str(), 20001);
    }
    pthread_mutex_unlock(&m_Tcp_Cmd_lock);

}


extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetWifiSSID(JNIEnv *env, jclass clazz, jstring s_ssid_) {
    // TODO: implement naSetWifiSSID()
    F_GetServerIP();
    if(sServerIP.size()<6)
    {
        return;
    }

    const char *sSSid = env->GetStringUTFChars(s_ssid_, 0);
    pthread_mutex_lock(&m_Tcp_Cmd_lock);
    int nLen = strlen(sSSid);
    uint8_t msg[80];
    msg[0] = 'F';
    msg[1] = 'D';
    msg[2] = 'W';
    msg[3] = 'N';

    msg[4] = 0x20;
    msg[5] = 0x00;

    msg[6] = 0x05;              //cmd
    msg[7] = 0x00;

    msg[8] = (Byte)nLen;              //len
    msg[9] = (Byte)(nLen>>8);

    for(int i=0;i<nLen;i++)
    {
        msg[10+i]=sSSid[i];
    }
    send_cmd_udp((uint8_t *) msg, 10+nLen, sServerIP.c_str(), 20001);

    pthread_mutex_unlock(&m_Tcp_Cmd_lock);

    env->ReleaseStringUTFChars(s_ssid_, sSSid);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetLedMode(JNIEnv *env, jclass clazz, jint n_mode) {
    // TODO: implement naSetLedMode()
    if(sServerIP.size()<6)
    {
        F_GetServerIP();
    }
    if(sServerIP.size()<6)
    {
        return;
    }

    pthread_mutex_lock(&m_Tcp_Cmd_lock);
    uint8_t msg[32];
    msg[0] = 'F';
    msg[1] = 'D';
    msg[2] = 'W';
    msg[3] = 'N';

    msg[4] = 0x20;
    msg[5] = 0x00;

    msg[6] = 0x20;    //cmd
    msg[7] = 0x00;

    msg[8] = 0x04;    //len
    msg[9] = 0x00;

    msg[10] = 0x02;
    msg[11] = n_mode;
    msg[12] = 0;
    msg[13] = 0;
    send_cmd_udp((uint8_t *) msg, 14, sServerIP.c_str(), 20001);

    pthread_mutex_unlock(&m_Tcp_Cmd_lock);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naGetLedMode(JNIEnv *env, jclass clazz) {
    // TODO: implement naGetLedMode()
    if(sServerIP.size()<6)
    {
        F_GetServerIP();
    }
    if(sServerIP.size()<6)
    {
        return;
    }

    pthread_mutex_lock(&m_Tcp_Cmd_lock);
    uint8_t msg[32];
    msg[0] = 'F';
    msg[1] = 'D';
    msg[2] = 'W';
    msg[3] = 'N';

    msg[4] = 0x20;
    msg[5] = 0x00;

    msg[6] = 0x20;    //cmd
    msg[7] = 0x00;

    msg[8] = 0x00;    //len
    msg[9] = 0x00;

//    msg[10] = 0x02;
//    msg[11] = n_mode;
//    msg[12] = 0;
//    msg[13] = 0;
    send_cmd_udp((uint8_t *) msg, 10, sServerIP.c_str(), 20001);

    pthread_mutex_unlock(&m_Tcp_Cmd_lock);
}





extern "C"
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naSetSecurity(JNIEnv *env, jclass clazz, jint n_seed) {
    // TODO: implement naSetSecurity_a()
    uint32_t   factor = n_seed;
    uint32_t  enc_para = (factor^0x95764263)+(factor-0xa95826b1) + (uint32_t)(factor>>16u) + (uint32_t)(factor<<16u);

    uint8_t msg[32];
    msg[0] = 'J';
    msg[1] = 'H';
    msg[2] = 'C';
    msg[3] = 'M';
    msg[4] = 'D';
    msg[5] = 0x54;
    msg[6] = 0x44;
    msg[7] = 0x1;

    msg[8] = enc_para;
    msg[9] = enc_para>>8u;
    msg[10] = enc_para>>16u;
    msg[11] = enc_para>>24u;

    unsigned int d = (unsigned int)time(0);
    srand(d);
    enc_para = random();
    msg[12] = enc_para;
    msg[13] = enc_para>>8u;
    msg[14] = enc_para>>16u;
    msg[15] = enc_para>>24u;
    send_cmd_udp(msg, 16, sServerIP.c_str(), 20000);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naCheckSecurityStatus(JNIEnv *env, jclass clazz,
                                                               jint n_password) {
    // TODO: implement naCheckSecurityStatus()

    uint8_t msg[32];
    msg[0] = 'J';
    msg[1] = 'H';
    msg[2] = 'C';
    msg[3] = 'M';
    msg[4] = 'D';
    msg[5] = 0x54;
    msg[6] = 0x44;
    msg[7] = 0x0;
    send_cmd_udp(msg, 8, sServerIP.c_str(), 20000);

}



extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naRelinkPlay(JNIEnv *env, jclass clazz) {
    uint8_t msg[50];
    F_GetServerIP();
    if (nICType == IC_GPH264A || nICType == IC_RTLH264 || nICType == IC_GPH264_34) {
        if (GP_tcp_VideoSocket.bConnected)
        {
            GP_tcp_VideoSocket.DisConnect();
        }
        m_FFMpegPlayer.Stop(false);
        msg[0] = 'J';
        msg[1] = 'H';
        msg[2] = 'C';
        msg[3] = 'M';
        msg[4] = 'D';
        msg[5] = 0x10;
        msg[6] = 0x00;
        send_cmd_udp(msg, 7, sServerIP.c_str(), 20000);
        m_FFMpegPlayer.InitMedia("");
        Connect_GPH264();
        LOGE_B("ReLinker1");
    }
    else
    {
        LOGE_B("ReLinker1= %d",nICType);
    }
}


int nHightQuqlity = 1;


extern "C"
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetScaleHighQuality(JNIEnv *env, jclass clazz,
                                                               jint n_q) {
    // TODO: implement naSetScaleHighQuality()
    if(n_q<0)
        n_q = 0;
    if(n_q>3)
        n_q = 3;
    nHightQuqlity = n_q;
}