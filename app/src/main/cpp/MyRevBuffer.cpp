//
// Created by AivenLau on 2019-08-12.
//

#include <pthread.h>
#include <cstring>
#include "MyRevBuffer.h"
#include "Defines.h"


MyRevBuffer::MyRevBuffer() {
    nStartInx = 0;
    nLength = 0;
    nCount = 0;
    buffer = nullptr;
    pthread_mutex_init(&mutex, nullptr);
    pthread_cond_init(&cond, nullptr);

}

MyRevBuffer::~MyRevBuffer() {
    if (buffer != nullptr) {
        delete[]buffer;
        buffer = nullptr;
    }

    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);

}

uint32_t MyRevBuffer::getBufferLength() {
    return nLength;

}

int MyRevBuffer::ReadBuffer(uint8_t *buffer_, int size) {
    pthread_mutex_lock(&mutex);
    int nRead = size;
    if (nLength < size)
        nRead = nLength;
    if (nRead != 0) {
        memcpy(buffer_, buffer + nStartInx, nRead);
        nStartInx += nRead;
        nLength -= nRead;
    }

    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    return nRead;

}

int MyRevBuffer::SaveBuffer(uint8_t *buffer_, int size) {

    int ret = 0;
    pthread_mutex_lock(&mutex);
    if (buffer == nullptr) {
        Init();
    }
    uint32_t nAll = nLength + size;
    {
        uint32_t ix = nStartInx + nLength + size;
        if (ix <= nCount) {
            memcpy(buffer + nStartInx + nLength, buffer_, size);
            nLength += size;
        } else {
            uint8_t *buf = new uint8_t[nCount];
            memset(buf, 0, nCount);
            if (nLength != 0)
                memcpy(buf, buffer + nStartInx, nLength);
            if (nLength + size > nCount) {
                size = nCount - nLength;
            }
            memcpy(buf + nLength, buffer_, size);
            delete[]buffer;
            buffer = buf;
            nStartInx = 0;
            nLength += size;
        }
    }
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    return ret;

}

void MyRevBuffer::Init() {
    nStartInx = 0;
    nLength = 0;
    nCount = 1000 * 1000 * 20;
    if (buffer != nullptr) {
        delete[]buffer;
        buffer = nullptr;
    }
    buffer = new uint8_t[nCount];
}

void MyRevBuffer::Clear() {
    nLength = 0;
    nCount = 0;
    if (buffer != nullptr) {
        delete[]buffer;
        buffer = nullptr;
    }


}
