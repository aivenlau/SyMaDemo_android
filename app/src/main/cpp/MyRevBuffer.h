//
// Created by AivenLau on 2019-08-12.
//

#ifndef SYMADEMO_ANDROID_MYREVBUFFER_H
#define SYMADEMO_ANDROID_MYREVBUFFER_H


#include <cstdint>
#include <sys/types.h>

class MyRevBuffer {



    pthread_mutex_t mutex;

    pthread_cond_t cond;

public:
    MyRevBuffer();
    ~MyRevBuffer();
    uint32_t nStartInx;
    uint32_t  nLength;
    uint32_t  nCount;
    uint8_t  *buffer;
    //bool      bNeedRead;


    int ReadBuffer(uint8_t *buffer_,int size);
    int SaveBuffer(uint8_t *buffer_,int size);
    uint32_t getBufferLength();

    void Clear();
    void Init();


};


#endif //SYMADEMO_ANDROID_MYREVBUFFER_H
