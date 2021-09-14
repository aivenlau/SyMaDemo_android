//
// Created by 小米哥 on 1/20/21.
//

#include "Defines.h"
#include "PicturesFromVideo.h"
#include <pthread.h>

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

void F_SentPictureData2Java(uint8_t *data,int nLen,int nStatus);

PicturesFromVideo::PicturesFromVideo()
{
    bProgress = false;
}
