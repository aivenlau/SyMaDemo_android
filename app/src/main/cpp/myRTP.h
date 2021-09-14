//
// Created by AivenLau on 2017/8/25.
//

#ifndef SYMADEMO_ANDROID_MYRTP_H
#define SYMADEMO_ANDROID_MYRTP_H

#include <list>
#include <algorithm>
#include <iostream>
#include <stdint.h>



using namespace std;
class myRTP {
public:
    myRTP();
    myRTP(uint8_t *buffer,int nLen);
    ~myRTP();
    uint8_t   mBuffer[2000];
    uint16_t  nIndex;
    uint32_t  nLen;
    uint32_t  nTimestamp;
    bool operator < (const myRTP &m)const {
        return this->nIndex > m.nIndex;
    }
};


template<>
struct std::greater<myRTP*>
{
    bool operator()(myRTP* AA,myRTP* BB) const
    {
        return (AA->nIndex < BB->nIndex);
    }
};





#endif //SYMADEMO_ANDROID_MYRTP_H
