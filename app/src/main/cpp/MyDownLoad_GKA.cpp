//
// Created by AivenLau on 2017/6/3.
//

#include "MyDownLoad_GKA.h"
#include "Defines.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdlib.h>
#include <fcntl.h>

#include <time.h>
#include <pthread.h>
#include <jni.h>

#include <android/native_window.h>
#include <android/native_window_jni.h>


#include<unistd.h>
#include<arpa/inet.h>


#include <sys/ioctl.h>
//#include "MySocket.h"
#include "MySocket_GKA.h"
#include <algorithm>
#include <cctype>

#include <vector>
//#include <sys/socket.h>
#include <sstream>


void F_CallDownloadCallBack(int nPercentage, const char *sFileName, int nError);

std::string GetExtensionName(std::string in);

MyDownLoad_GKA::MyDownLoad_GKA() : bCancel(false) {

};

MyDownLoad_GKA::~MyDownLoad_GKA() {
    socket.DisConnect();
}

void MyDownLoad_GKA::CancelDownLod() {
    bCancel = true;
    socket.DisConnect();
}

int MyDownLoad_GKA::DownLoadFile(const char *sPath, const char *dPath, int session_id) {
    E_NET_DOWNLOAD_TYPE nType = DL_SNAP_FILE;

    std::string ext = GetExtensionName(sPath);

    transform(ext.begin(), ext.end(), ext.begin(), ::toupper);
    if (ext == "MOV" || ext == "AVI") {
        nType = DL_REC_FILE;
    }

    if (socket.Connect("175.16.10.2", 0x7102) != 0) {
        return -1;
    }


    int rrrr = -1;

    MySocketData data;
    int ret;
    int nLen;

    T_REQ_MSG msg;
    T_NET_CMD_MSG Cmd;

    T_NET_DOWNLOAD_CONTROL downCtrol;
    Cmd.session_id = session_id;
    Cmd.type = CMD_DOWNLOAD_SOCK;

    downCtrol.dl_type = nType;
    downCtrol.one_packet_size = 1024 * 512;

    memset(downCtrol.name, 0, 256);
    memcpy(downCtrol.name, (const void *) sPath, strlen(sPath));

    data.nLen = 0;
    data.AppendData(&Cmd, sizeof(T_NET_CMD_MSG));
    data.AppendData(&downCtrol, sizeof(T_NET_DOWNLOAD_CONTROL));
    socket.Write(&data);

    data.nLen = sizeof(T_REQ_MSG);
    ret = socket.Read(&data, 1000);

    T_NET_DL_PACKET_HEADER head;
    bool bReadHead = true;
    int nNeedReadLen = 0;
    uint64_t nFileSize = -1;
    uint64_t nCount = 0;

    int nFile = -1;


    if (ret == sizeof(T_REQ_MSG)) {
        memcpy(&msg, data.data, sizeof(T_REQ_MSG));
        if (msg.ret == 0 && msg.session_id == session_id) {
            while (true) {
                nLen = sizeof(T_NET_DL_PACKET_HEADER);
                data.SetSize(nLen);
                ret = socket.Read(&data, 0);
                if (ret == nLen) {
                    memcpy(&head, data.data, nLen);
                    if (nFileSize == -1) {
                        nFileSize = head.file_all_size;
                        LOGE("File Size:%ld", (long) nFileSize);
                        if (nFile == -1) {
                            nFile = open(dPath, O_RDWR | O_CREAT | O_TRUNC, 0666);
                            if (nFile == -1) {
                                break;
                            }
                        }
                    }
                    if (nFileSize <= 0) {
                        if (!bCancel)
                            F_CallDownloadCallBack(0, sPath, 1);
                        break;
                    }

                    nNeedReadLen = head.size;
                    data.SetSize(nNeedReadLen);
                    ret = socket.Read(&data, 0);
                    if (ret != nNeedReadLen) {
                        if (!bCancel)
                            F_CallDownloadCallBack(0, sPath, 1);
                        break;                    //读取出错
                    } else {
                        nCount += nNeedReadLen;
                        write(nFile, data.data, nNeedReadLen);
                        if (nCount >= nFileSize)   //文件已经读取完毕！
                        {
                            rrrr = 0;
                            if (!bCancel)
                                F_CallDownloadCallBack(100, sPath, 0);
                            break;
                        } else {
                            float df = (((float) nCount) * 100) / nFileSize;
                            int da = (int) df;
                            if (da == 100) {
                                da = 99;
                            }
                            if (!bCancel)
                                F_CallDownloadCallBack(da, sPath, 0);
                        }
                    }
                } else {
                    if (!bCancel)
                        F_CallDownloadCallBack(0, sPath, 1);
                    break;     //读取出错
                }
            }

        }
    }
    socket.DisConnect();
    if (rrrr == 0) {
        close(nFile);
    } else {
        if (nFile != -1) {
            close(nFile);
            remove(dPath);
        }
    }
    if (bCancel) {
        return -2;
    }
    return rrrr;
}
