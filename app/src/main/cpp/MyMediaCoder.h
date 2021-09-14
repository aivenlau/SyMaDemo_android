//
// Created by AivenLau on 2017/12/21.
//

#ifndef SYMADEMO_ANDROID_MYMEDIACODE_H
#define SYMADEMO_ANDROID_MYMEDIACODE_H

#include "media/NdkMediaCodec.h"
//#include "NativeCodec.h"


#ifdef __cplusplus
extern "C" {
#endif

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/pixfmt.h"
#include "libavutil/imgutils.h"
#include "libavutil/time.h"
#include "libavutil/error.h"
#include "libavutil/frame.h"
#include "libavutil/opt.h"

#ifdef __cplusplus
}
#endif


int _naSave2FrameMp4(uint8_t *data, int nLen, int b, bool keyframe);

class MyMediaCoder {

private:

    int m_nDecWidth;
    int m_nDecHeight;

    int m_nRecWidth;
    int m_nRecHeight;
    int m_nBitrate;
    int m_nFps;
    AMediaCodec*  encoder;
    //AMediaCodec*  decoder;
    int64_t pts;
    uint64_t decpts;
public:
    int encord_colorformat;
    MyMediaCoder();

    ~MyMediaCoder();

    bool F_InitEncoder(int w, int h, int bitrate, int fps);

    bool offerEncoder(uint8_t *data, int32_t nLen);

    bool offerDecoder(uint8_t *data, int32_t nLen, int flag);

    void F_CloseEncoder(void);

    void F_CloseDecoder(void);


    bool F_InitDecoder(int w, int h, int8_t *sps, int spsLen, int8_t *pps, int ppsLen, int nfps);


};


#endif //SYMADEMO_ANDROID_MYMEDIACODE_H
