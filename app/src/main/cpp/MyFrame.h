//
// Created by AivenLau on 2016/12/12.
//

#ifndef SIMA_WIFI_MYFRAME_H
#define SIMA_WIFI_MYFRAME_H

#include <stdio.h>

class MyFrame {
public:
    MyFrame();

    ~MyFrame();

    bool bKeyFranme;
    uint8_t *data;
    int nLen;

    void Release();

};


#endif //SIMA_WIFI_MYFRAME_H
