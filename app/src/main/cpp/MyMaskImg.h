//
// Created by 小米哥 on 2020/5/27.
//

#ifndef SYMADEMO_ANDROID_MYMASKIMG_H
#define SYMADEMO_ANDROID_MYMASKIMG_H
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
#include "libavfilter/avfilter.h"
#include "libavfilter/buffersrc.h"
#include "libavfilter/buffersink.h"

#ifdef __cplusplus
}
#endif

class MyMaskImg {
public:
    MyMaskImg();

    ~MyMaskImg();

    int nWidth;
    int nHeight;
    int *pLeftLen;
    int *pRiggtLen;

    void MakeMaskImg(int w, int h);
};


#endif //SYMADEMO_ANDROID_MYMASKIMG_H
