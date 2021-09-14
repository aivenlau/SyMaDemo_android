//
// Created by AivenLau on 2016/12/1.
//

#ifndef SIMA_WIFI_MYSOCKETDATA_H
#define SIMA_WIFI_MYSOCKETDATA_H


#include <stdint.h>

#define DATA_LEN  (1024*100)

class MySocketData {


public:
    MySocketData();

    ~MySocketData();
    void MemSet(uint8_t dat);

    int AppendData(void *dataB, uint32_t nLenA);

    MySocketData *ReadData(uint32_t t);

    void Clean(void);

    void SetSize(int nLen);

    uint32_t nLen;
    uint64_t nSize;

    uint8_t *data;
    //int64_t  disp_no;
    int      nFrameNo;
};


#endif //SIMA_WIFI_MYSOCKETDATA_H
