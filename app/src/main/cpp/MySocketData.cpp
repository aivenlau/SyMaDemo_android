//
// Created by AivenLau on 2016/12/1.
//

#include <string.h>
#include "MySocketData.h"

MySocketData::MySocketData() : nLen(0), nFrameNo(-1) {
    nSize = DATA_LEN;
    this->nLen = 0;
    this->data = new uint8_t[nSize];
    //disp_no = -1;
}

MySocketData::~MySocketData() {
    if (data != nullptr) {
        delete[]data;
        data = nullptr;
    }
    nLen = 0;
}

void MySocketData::MemSet(uint8_t dat) {
    if (nSize > 0 && data != nullptr) {
        memset(data, dat, nSize);
    }
}

void MySocketData::Clean(void) {
    if (data != nullptr) {
        delete[]data;
        nSize = DATA_LEN;
        this->data = new uint8_t[nSize];
    }
    //disp_no = -1;
    nLen = 0;
};

void MySocketData::SetSize(int nLen) {
    if (nLen <= 0) {
        return;
    }
    if (nLen > nSize) {
        if (data != nullptr) {
            delete[]data;
            data = nullptr;
        }
        nSize = nLen;
        this->nLen = nLen;
        this->data = new uint8_t[nSize];
    } else {
        this->nLen = nLen;
    }
}

int MySocketData::AppendData(void *dataB, uint32_t nLenA) {
    uint32_t ix = 0;
    uint8_t *dataA = (uint8_t *) dataB;
    if (nLen + nLenA > nSize) {
        nSize = nLen + nLenA + DATA_LEN;
        uint8_t *temp = new uint8_t[nSize];
        memcpy(temp, data, nLen);
        memcpy(temp + nLen, dataA, nLenA);
        delete[]data;
        data = temp;
        nLen += nLenA;
    } else {
        memcpy(data + nLen, dataA, nLenA);
        nLen += nLenA;
    }
    return nLen;
}

MySocketData *MySocketData::ReadData(uint32_t nLenA) {

    if (nLenA >= nLen) {
        nLenA = nLen;
    }
    MySocketData *tmp = new MySocketData();
    memcpy(tmp->data, data, nLenA);
    tmp->nSize = nLenA;
    tmp->nLen = nLenA;
    if (nLenA == nLen) {
        nLen = 0;
    } else {
        nLen -= nLenA;
        memcpy(data, data + nLenA, nLen);
    }
    return tmp;
}

