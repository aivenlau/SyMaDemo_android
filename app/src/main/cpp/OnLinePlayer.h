//
// Created by AivenLau on 2019-08-05.
//

#ifndef SYMADEMO_ANDROID_ONLINEPLAYER_H
#define SYMADEMO_ANDROID_ONLINEPLAYER_H

#include "FFmpegMusic.h"

#include "FFmpegVideo.h"

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


class OnLinePlayer {


    void Recheck(AVFrame *frame,AVFrame *pfram1);

    int Play_B();


    static void* Progress(void *data);

    char  *decordName;

    pthread_t connectdid;

public:
    bool   bReadDataOK;
    bool   bisPlaying;
    int    nReadInx;
    int    nLength;
    OnLinePlayer();
    ~OnLinePlayer();






    void setDecordName(const char *s);



    static int read_buffer(void *opaque, uint8_t *buf, int buf_size);
    int read_buffer(uint8_t *buf, int buf_size);
    static int64_t seek_buffer(void *opaque, int64_t offset, int whence);


    void Play();
    int Stop();

};
#endif