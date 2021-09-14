//
// Created by 小米哥 on 1/20/21.
//

#include "JH_GetPics.h"
#include "Defines.h"

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

JH_GetPics::JH_GetPics():bProgress(false),m_decodeThread(-1)
{
    sPath = new char[256];
    memset(sPath,0,256);
}

JH_GetPics::~JH_GetPics()
{
    delete []sPath;
}


void *JH_GetPics::getPicturesFromVideo_(void *param)
{
    auto *self = (JH_GetPics *)param;
    self->getPicturesFromVideo();
    pthread_detach(pthread_self());
    return nullptr;

}



int JH_GetPics::StartGetPicture(char *sPaht_)
{
    if(bProgress)
        return -1;
    strcpy(sPath, sPaht_);
    pthread_create(&m_decodeThread, nullptr, getPicturesFromVideo_, (void *) this);
    return  0;

}

int JH_GetPics::getPicturesFromVideo()
{

    int videoindex = -1;

    struct SwsContext *img_convert_ctx;
    AVPacket packet;

    AVCodecContext *m_codecCtx = nullptr;
    AVFormatContext *m_formatCtx = nullptr;
    AVFrame *m_decodedFrame = nullptr;
    AVCodec *pCodec = nullptr;
    AVInputFormat* pInputFormat = nullptr;

    if (avformat_open_input(&m_formatCtx, sPath, pInputFormat, nullptr) != 0)
    {
        LOGE("Couldn't open input stream.\n");
        bProgress = false;
        F_SentPictureData2Java(nullptr,0,-2);
        return -2;
    }

    if (avformat_find_stream_info(m_formatCtx, nullptr) < 0) {
        LOGE("Couldn't find stream information.\n");
        avformat_close_input(&m_formatCtx);
        avformat_free_context(m_formatCtx);
        m_formatCtx = nullptr;
        bProgress = false;
        F_SentPictureData2Java(nullptr,0,-3);
        return -3;
    }

    int i = 0;
    for (i = 0; i < m_formatCtx->nb_streams; i++)
        if (m_formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoindex = i;
            break;
        }

    if (videoindex == -1) {
        avformat_close_input(&m_formatCtx);
        avformat_free_context(m_formatCtx);
        m_formatCtx = nullptr;
        F_SentPictureData2Java(nullptr,0,-4);
        return -4;
    }

    AVStream *stream = m_formatCtx->streams[videoindex];
    pCodec = avcodec_find_decoder(stream->codecpar->codec_id);
    if (pCodec == nullptr) {

        avformat_close_input(&m_formatCtx);
        avformat_free_context(m_formatCtx);
        m_formatCtx = nullptr;
        F_SentPictureData2Java(nullptr,0,-5);
        return -5;
    }
    m_codecCtx = avcodec_alloc_context3(pCodec);
    avcodec_parameters_to_context(m_codecCtx, stream->codecpar);
    if (avcodec_open2(m_codecCtx, pCodec, nullptr) < 0) {
        avformat_close_input(&m_formatCtx);
        avformat_free_context(m_formatCtx);
        m_formatCtx = nullptr;
        avcodec_free_context(&m_codecCtx);
        F_SentPictureData2Java(nullptr,0,-6);
        return -6;
    }

    if (m_codecCtx->width <= 0 || m_codecCtx->height <= 0) {
        avformat_close_input(&m_formatCtx);
        avformat_free_context(m_formatCtx);
        m_formatCtx = nullptr;
        avcodec_free_context(&m_codecCtx);
        F_SentPictureData2Java(nullptr,0,-7);
        return -7;
    }

    if (m_codecCtx->pix_fmt == AV_PIX_FMT_NONE) {
        avformat_close_input(&m_formatCtx);
        avformat_free_context(m_formatCtx);
        m_formatCtx = nullptr;
        avcodec_free_context(&m_codecCtx);
        F_SentPictureData2Java(nullptr,0,-8);
        return -8;
    }

    int frame_rate = 20;

    if (stream->r_frame_rate.den > 0)
    {
        frame_rate = stream->r_frame_rate.num / stream->r_frame_rate.den;
        if (frame_rate == 19) {
            frame_rate = 20;
        }
    }

    float times = stream->duration * av_q2d(stream->time_base);
    int  nTimes = (int)times;




    int ret = 0;
    int w = 60;
    int h = 60;

    AVFrame *pFrameRGBA_Real;

    pFrameRGBA_Real = av_frame_alloc();

    pFrameRGBA_Real->format = AV_PIX_FMT_RGBA;
    pFrameRGBA_Real->width = w;
    pFrameRGBA_Real->height = h;
    av_image_alloc(
            pFrameRGBA_Real->data, pFrameRGBA_Real->linesize, w,
            h,
            AV_PIX_FMT_RGBA, 4);
    img_convert_ctx = sws_getContext(m_codecCtx->width, m_codecCtx->height, m_codecCtx->pix_fmt,
                                     w, h, AV_PIX_FMT_RGBA,
                                     SWS_FAST_BILINEAR, nullptr, nullptr, nullptr); //
    bProgress=true;
    int re = 10;
    m_decodedFrame = av_frame_alloc();

    int32_t nCountFrame = (int32_t)(stream->nb_frames);
    uint8_t  data1[12];
    memcpy(data1,&nCountFrame,4);
    memcpy(data1+4,&nTimes,4);
    F_SentPictureData2Java((uint8_t  *)(&data1),8,1);

    int nf = nCountFrame/nTimes/2;

    int index=0;
    bool  bNextKey=false;
    int ii=0;
    while (bProgress)
    {
        if (av_read_frame(m_formatCtx, &packet) < 0)
        {
            re = -10;
            break;
        }
        bool  bNeedDecord = false;
        if (packet.stream_index == videoindex) {
            ii++;
//            if(packet.flags & AV_PKT_FLAG_KEY)
//            {
//                LOGE("ii = %d",ii);
//                bNeedDecord = true;
//                bNextKey = true;
//            }
//            else if(bNextKey){
//                bNextKey = false;
//                bNeedDecord = true;
//            }
            bNeedDecord = true;
            if(bNeedDecord) {
                if (avcodec_send_packet(m_codecCtx, &packet) == 0) {
                    if (avcodec_receive_frame(m_codecCtx, m_decodedFrame) != 0) {
                        ret = -1;
                    } else {
                        ret = 0;
                    }
                } else {
                    ret = -1;
                }
                if (ret == 0) {
                    //if((index % nf)==0)
                    {
                        sws_scale(img_convert_ctx,
                                  (const uint8_t *const *) m_decodedFrame->data,
                                  m_decodedFrame->linesize, 0,
                                  m_codecCtx->height,
                                  pFrameRGBA_Real->data, pFrameRGBA_Real->linesize);
                        F_SentPictureData2Java(pFrameRGBA_Real->data[0], w * h * 4, 2);
                    }
                }
            }
            index++;
        }
        av_packet_unref(&packet);
    }
    F_SentPictureData2Java(nullptr,0,3);
    av_freep(&pFrameRGBA_Real->data[0]);
    av_frame_free(&pFrameRGBA_Real);
    sws_freeContext(img_convert_ctx);
    avformat_close_input(&m_formatCtx);
    avformat_free_context(m_formatCtx);
    bProgress = false;
    return 0;
}