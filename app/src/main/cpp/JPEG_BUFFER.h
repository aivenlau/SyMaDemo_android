//
// Created by AivenLau on 2017/9/7.
//

#ifndef SYMADEMO_ANDROID_JPEG_BUFFER_H
#define SYMADEMO_ANDROID_JPEG_BUFFER_H

#include <stdlib.h>

class JPEG_BUFFER {

public:
    JPEG_BUFFER(void);

    ~JPEG_BUFFER(void);

    void Clear(void);

    void Release(void);

    uint8_t *buffer;
    uint32_t nCount;
    uint32_t nJpegInx;
    uint16_t mInx[800];
    int64_t nSize;
    int32_t bOK;

    bool AppendData(uint8_t *data, int nLen);

    bool bKeyFrame;


    bool operator<(const JPEG_BUFFER *m) const {
        return nJpegInx < m->nJpegInx;
    }

/*
    bool operator()(const JPEG_BUFFER* t1,const JPEG_BUFFER* t2){
        return t1->nJpegInx < t2->nJpegInx;
    }
*/

    bool operator()(const JPEG_BUFFER *t1) {
        return nJpegInx == t1->nJpegInx;
    }

    bool operator==(
            const JPEG_BUFFER &obj) const //重载 “==” 操作符，函数最后的 const 别忘了，否则会报错。（详见：http://www.cnblogs.com/SZxiaochun/p/7731900.html）
    {
        return nJpegInx == obj.nJpegInx; //具体匹配条件，可以自己设定
    }

};


#endif //SYMADEMO_ANDROID_JPEG_BUFFER_H
