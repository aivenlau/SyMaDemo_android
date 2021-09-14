//
// Created by AivenLau on 2017/6/3.
//

#ifndef SYMADEMO_ANDROID_MYDOWNLOAD_GKA_H
#define SYMADEMO_ANDROID_MYDOWNLOAD_GKA_H

#include "MySocket_GKA.h"

class MyDownLoad_GKA {

public:
    MyDownLoad_GKA();

    ~MyDownLoad_GKA();

    MySocket_GKA socket;

    int DownLoadFile(const char *sPath, const char *dPath, int session_id);

    void CancelDownLod();

    bool bCancel;
    int8_t aa;

};


#endif //SYMADEMO_ANDROID_MYDOWNLOAD_GKA_H
