//
// Created by AivenLau on 2019-08-05.
//

#include <libyuv.h>
#include <unistd.h>
#include "OnLinePlayer.h"
#include "Defines.h"
#include "MyRevBuffer.h"
#include "FFMpegPlayerCore.h"


extern AVFrame *gl_Frame;

extern MyRevBuffer  revBuffer;

//int OnLinePlayer::nLength=0;
//int OnLinePlayer::nReadInx=0;

//FILE   *file=nullptr;
//FILE   *outf=nullptr;

OnLinePlayer::OnLinePlayer():bisPlaying(false)
{

    connectdid = -1;
    decordName = nullptr;
};


OnLinePlayer::~OnLinePlayer() {

    if(decordName!= nullptr)
    {
        delete []decordName;
    }

};

void OnLinePlayer::setDecordName(const char *s)
{
    if(s== nullptr)
    {
        if(decordName!= nullptr)
        {
            delete []decordName;
            decordName = nullptr;
        }
    } else
    {
        int nlen = strlen(s);
        if(decordName!= nullptr)
        {
            delete []decordName;
        }
        decordName = new char[nlen+1];
        memset(decordName,0,nlen+1);
        memcpy(decordName,s,nlen);

    }
}

//int64_t OnLinePlayer::seek_buffer(void *opaque, int64_t offset, int whence)
//{
//    if(file!= nullptr)
//    {
//        LOGE("seek %ld, %d",offset,whence);
//        return fseek(file,offset,whence);
//    }
//    return 0;
//
//}


int OnLinePlayer::read_buffer(uint8_t *buf, int buf_size)
{
    int ix=400;
    int ret = revBuffer.ReadBuffer(buf, buf_size);
    while(ret == 0 && ix>0 && bisPlaying) {
        usleep(1000*4);
        ret = revBuffer.ReadBuffer(buf, buf_size);
        ix--;
    }
    if(ret>0) {
        return ret;
    }
    else {
        //bReadDataOK = false;
        return -1;
    }
}


int OnLinePlayer::read_buffer(void *opaque, uint8_t *buf, int buf_size) {
    auto * self  = (OnLinePlayer *)opaque;
    if(self!= nullptr)
    {
        return self->read_buffer(buf,buf_size);
    } else
    {
        return 0;
    }


}



int OnLinePlayer::Stop()
{
    if(!bisPlaying)
        return 0;
    bisPlaying=false;
    if(connectdid!=-1)
    {
        LOGE_B("Exit stop1");
        pthread_join(connectdid, 0);
        connectdid=-1;

    }
    LOGE_B("Exit stop2");
    return 0;
}


void OnLinePlayer::Recheck(AVFrame *pFrameYUV,AVFrame *pFrame)
{
    if (pFrameYUV != nullptr) {
        if (pFrameYUV->width != pFrame->width || pFrameYUV->height != pFrame->height) {
            av_freep(&pFrameYUV->data[0]);
            av_frame_free(&pFrameYUV);
            pFrameYUV = nullptr;
        }
    }
    if (pFrameYUV == nullptr) {
        pFrameYUV = av_frame_alloc();
        pFrameYUV->format = AV_PIX_FMT_YUV420P;
        pFrameYUV->width = pFrame->width;
        pFrameYUV->height = pFrame->height;
        av_image_alloc(
                pFrameYUV->data, pFrameYUV->linesize, pFrame->width,
                pFrame->height,
                AV_PIX_FMT_YUV420P, aligD);
    }
}

void *OnLinePlayer::Progress(void *data)
{
    OnLinePlayer *self =(OnLinePlayer *)data;
    self->Play_B();
    return nullptr;
}


void OnPlayIsStarting_Callback(int n);


extern C_FFMpegPlayer m_FFMpegPlayer;
void F_SentRevBmp(int32_t a);

int OnLinePlayer::Play_B() {



    int nStartPlay = 0;

    int i;

    if(bisPlaying)
        return -100;

    usleep(1000*100);

    //avcodec_register_all();

    FFmpegVideo ffmpegVideo;
    FFmpegMusic ffmpegMusic;

    bisPlaying = true;

    AVDictionary *optionsDict = nullptr;

    AVFormatContext *pFormatCtx = nullptr;
    unsigned char *aviobuffer = nullptr;
    AVIOContext *avio = nullptr;
    AVCodecContext *pVideoCodecCtx = nullptr;
    AVCodecContext *pAudioCodecCtx = nullptr;
    AVCodec *pCodec = nullptr;
    AVFrame *pFrame = nullptr;
    AVFrame *pFrameYUV = nullptr;
    AVPacket packet;
    struct SwsContext *img_convert_ctx;
    int videoindex;
    int audioindex;
    AVInputFormat* pInputFormat = nullptr;
    nReadInx = 0;
    av_log_set_level(AV_LOG_QUIET);
    pFormatCtx = avformat_alloc_context();
    aviobuffer = (unsigned char *) av_malloc(32768);
    //avio = avio_alloc_context(aviobuffer, 32768, 0, this, read_buffer, nullptr, seek_buffer);
    avio = avio_alloc_context(aviobuffer, 32768, 0, this, read_buffer, nullptr, nullptr);





    /* Open an input stream and read the header. */

//#define DD
#ifdef DD
    const char *ss = "/storage/emulated/0/MOVI0010.AVI";
    if(file!= nullptr) {
        fclose(file);
        file = nullptr;
    }
    file = fopen(ss, "rb");
    bReadDataOK = true;
#endif
    pFormatCtx->pb = avio;


//    if(decordName!= nullptr)
//        pInputFormat = av_find_input_format(decordName);

#ifdef DD
    if (avformat_open_input(&pFormatCtx, ss, pInputFormat, nullptr) != 0)
#else
    if (avformat_open_input(&pFormatCtx, nullptr, pInputFormat, nullptr) != 0)
#endif
    {
        LOGE("Couldn't open input stream.\n");
        bisPlaying = false;
        avio_context_free(&avio);
        return -1;
    }



    if (avformat_find_stream_info(pFormatCtx, nullptr) < 0) {
        LOGE("Couldn't find stream information.\n");
        avformat_close_input(&pFormatCtx);
        avformat_free_context(pFormatCtx);
        pFormatCtx = nullptr;
        bisPlaying = false;
        avio_context_free(&avio);
        return -2;
    }


    videoindex = -1;
    audioindex = -1;
    for (i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoindex = i;
        }
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioindex = i;
        }

    }

    if (videoindex == -1) {
        LOGE("Didn't find a video stream.\n");
        avformat_close_input(&pFormatCtx);
        avformat_free_context(pFormatCtx);
        pFormatCtx = nullptr;
        bisPlaying = false;
        avio_context_free(&avio);
        return -3;
    }

    /* Find a registered decoder with a matching codec ID */


    AVStream *stream = pFormatCtx->streams[videoindex];

//    if(stream->codecpar->codec_id == AV_CODEC_ID_H264) {
//        pCodec = avcodec_find_decoder_by_name("h264_mediacodec");//mpeg硬解码
//    }
//    else if(stream->codecpar->codec_id == AV_CODEC_ID_HEVC) {
//        pCodec = avcodec_find_decoder_by_name("hevc_mediacodec");//mpeg硬解码
//    }
//    else if(stream->codecpar->codec_id == AV_CODEC_ID_MPEG4){
//            pCodec = avcodec_find_decoder_by_name("mpeg4_mediacodec");//mpeg硬解码
//    }
//    if(pCodec== nullptr) {
//        pCodec = avcodec_find_decoder(stream->codecpar->codec_id);
//    }

    pCodec = avcodec_find_decoder(stream->codecpar->codec_id);
    bool  bIsH264 = (stream->codecpar->codec_id == AV_CODEC_ID_H264);

    if (pCodec == nullptr) {
        printf("Codec not found.\n");
        avformat_close_input(&pFormatCtx);
        avformat_free_context(pFormatCtx);
        pFormatCtx = nullptr;
        bisPlaying = false;
        avio_context_free(&avio);
        return -4;
    }

    pVideoCodecCtx = avcodec_alloc_context3(pCodec);
    avcodec_parameters_to_context(pVideoCodecCtx, stream->codecpar);

    if (avcodec_open2(pVideoCodecCtx, pCodec, &optionsDict) < 0) {
        DEBUG_PRINT("avcodec_open2 failed!\n");
        avformat_close_input(&pFormatCtx);
        avformat_free_context(pFormatCtx);
        pFormatCtx = nullptr;
        avio_context_free(&avio);
        avcodec_free_context(&pVideoCodecCtx);
        return -5;
    }

    if (pVideoCodecCtx->width <= 0 || pVideoCodecCtx->height <= 0) {
        avformat_close_input(&pFormatCtx);
        avformat_free_context(pFormatCtx);
        pFormatCtx = nullptr;
        bisPlaying = false;
        avio_context_free(&avio);
        avcodec_free_context(&pVideoCodecCtx);
        return -6;
    }

    if(audioindex!=-1)
    {
        AVStream *stream = pFormatCtx->streams[audioindex];
        pCodec = avcodec_find_decoder(stream->codecpar->codec_id);

        if (pCodec == nullptr) {
            printf("Codec not found.\n");
            avformat_close_input(&pFormatCtx);
            avformat_free_context(pFormatCtx);
            pFormatCtx = nullptr;
            bisPlaying = false;
            avio_context_free(&avio);
            avcodec_free_context(&pVideoCodecCtx);
            return -4;
        }

        pAudioCodecCtx = avcodec_alloc_context3(pCodec);
        avcodec_parameters_to_context(pAudioCodecCtx, stream->codecpar);

        if (avcodec_open2(pAudioCodecCtx, pCodec, &optionsDict) < 0) {
            DEBUG_PRINT("avcodec_open2 failed!\n");
            avformat_close_input(&pFormatCtx);
            avformat_free_context(pFormatCtx);
            pFormatCtx = nullptr;
            bisPlaying = false;
            avio_context_free(&avio);
            avcodec_free_context(&pVideoCodecCtx);
            avcodec_free_context(&pAudioCodecCtx);

            return -5;
        }
    }


    pFrame = av_frame_alloc();
    pFrameYUV = av_frame_alloc();

    pFrameYUV->format = AV_PIX_FMT_YUV420P;
    pFrameYUV->width = pVideoCodecCtx->width;
    pFrameYUV->height = pVideoCodecCtx->height;
    av_image_alloc(
            pFrameYUV->data, pFrameYUV->linesize, pVideoCodecCtx->width,
            pVideoCodecCtx->height,
            AV_PIX_FMT_YUV420P, aligD);

    img_convert_ctx = sws_getContext(pVideoCodecCtx->width, pVideoCodecCtx->height, pVideoCodecCtx->pix_fmt,
                                     pVideoCodecCtx->width, pVideoCodecCtx->height, AV_PIX_FMT_YUV420P,
                                     SWS_BICUBIC, nullptr, nullptr, nullptr);
    int  frame_rate = 25;

    stream = pFormatCtx->streams[videoindex];
    if(stream->r_frame_rate.den > 0)
    {
        frame_rate = stream->r_frame_rate.num/stream->r_frame_rate.den;
    }
    else if(pVideoCodecCtx->framerate.den > 0)
    {
        frame_rate = pVideoCodecCtx->framerate.num/pVideoCodecCtx->framerate.den;
    }


    int nDelay=1000/frame_rate;


    int64_t pre = -1;   //ms


    //audioindex=-1;
    if(!bIsH264 && pVideoCodecCtx->width*pVideoCodecCtx->height>640*480) //720*1280
    {
        audioindex=-1;
    }

    if(audioindex!=-1 && videoindex!=-1)
    {
        ffmpegMusic.stop();
        ffmpegMusic.index = audioindex;
        ffmpegMusic.setAvCodecContext(pAudioCodecCtx);
        ffmpegMusic.time_base = pFormatCtx->streams[audioindex]->time_base;

        ffmpegVideo.stop();
        ffmpegVideo.index = videoindex;
        ffmpegVideo.setFFmepegMusic(&ffmpegMusic);
        ffmpegVideo.setAvCodecContext(pVideoCodecCtx);
        ffmpegVideo.time_base = pFormatCtx->streams[videoindex]->time_base;


        ffmpegMusic.play();
        ffmpegVideo.play();
    }




    while (bisPlaying)
    {

        int ret = av_read_frame(pFormatCtx, &packet);
        if(ret!=0)
        {
            break;
        }
        else {
            if (packet.stream_index == videoindex) {
                if (audioindex != -1) {
                    ffmpegVideo.put(&packet);
                } else {
                    if (avcodec_send_packet(pVideoCodecCtx, &packet) == 0) {

                        if (avcodec_receive_frame(pVideoCodecCtx, pFrame) == 0) {
                            int64_t current = av_gettime() / 1000;   //ms
                            if (pre > 0) {
                                int da = (int) (current - pre);
                                while (da < nDelay) {
                                    usleep(1000 * 1);
                                    current = av_gettime() / 1000;
                                    da = (int) (current - pre);
                                }
                            }
                            pre = av_gettime() / 1000;
                            Recheck(pFrameYUV, pFrame);

                            if (pFrame->format == AV_PIX_FMT_YUVJ420P || pFrame->format == AV_PIX_FMT_YUV420P) {
                                libyuv::I420Copy(pFrame->data[0], pFrame->linesize[0],
                                                 pFrame->data[1], pFrame->linesize[1],
                                                 pFrame->data[2], pFrame->linesize[2],
                                                 pFrameYUV->data[0], pFrameYUV->linesize[0],
                                                 pFrameYUV->data[1], pFrameYUV->linesize[1],
                                                 pFrameYUV->data[2], pFrameYUV->linesize[2],
                                                 pFrameYUV->width, pFrameYUV->height);

                            } else if (pFrame->format == AV_PIX_FMT_YUV422P || pFrame->format == AV_PIX_FMT_YUVJ422P) {
                                libyuv::I422ToI420(pFrame->data[0], pFrame->linesize[0],
                                                   pFrame->data[1], pFrame->linesize[1],
                                                   pFrame->data[2], pFrame->linesize[2],
                                                   pFrameYUV->data[0], pFrameYUV->linesize[0],
                                                   pFrameYUV->data[1], pFrameYUV->linesize[1],
                                                   pFrameYUV->data[2], pFrameYUV->linesize[2],
                                                   pFrame->width, pFrame->height);

                            } else {
                                sws_scale(img_convert_ctx,
                                          (const uint8_t *const *) pFrame->data,
                                          pFrame->linesize, 0,
                                          pVideoCodecCtx->height,
                                          pFrameYUV->data, pFrameYUV->linesize);
                            }

                            if(m_FFMpegPlayer.Rgbabuffer!= nullptr) {
                                libyuv::I420ToABGR(pFrameYUV->data[0], pFrameYUV->linesize[0],
                                                   pFrameYUV->data[1], pFrameYUV->linesize[1],
                                                   pFrameYUV->data[2], pFrameYUV->linesize[2],
                                                   m_FFMpegPlayer.Rgbabuffer, pFrameYUV->width * 4,
                                                   pFrameYUV->width, pFrameYUV->height);
                            }

                            int W1 = pFrameYUV->width;
                            int H1 = pFrameYUV->height;
                            F_SentRevBmp(W1 + H1 * 0x10000);

                            /*

                            if (gl_Frame != nullptr) {//} && bReadDataOK) {

                                if(nStartPlay==0)
                                {
                                    nStartPlay = 1;
                                    OnPlayIsStarting_Callback(nStartPlay);
                                }

                                gl_Frame->width = pFrameYUV->width;
                                gl_Frame->height = pFrameYUV->height;

                                gl_Frame->linesize[0] = pFrameYUV->linesize[0];
                                gl_Frame->linesize[1] = pFrameYUV->linesize[1];
                                gl_Frame->linesize[2] = pFrameYUV->linesize[2];
                                libyuv::I420Copy(pFrameYUV->data[0], pFrameYUV->linesize[0],
                                                 pFrameYUV->data[1], pFrameYUV->linesize[1],
                                                 pFrameYUV->data[2], pFrameYUV->linesize[2],
                                                 gl_Frame->data[0], pFrameYUV->linesize[0],
                                                 gl_Frame->data[1], pFrameYUV->linesize[1],
                                                 gl_Frame->data[2], pFrameYUV->linesize[2],
                                                 pFrameYUV->width, pFrameYUV->height);

                            }
                             */
                        }
                    }
                }
            }
        }

        if (packet.stream_index == audioindex)
        {
            if(!ffmpegMusic.bStartPlay)
            {
                ffmpegMusic.bStartPlay=true;
                ffmpegMusic.audio_start_time = av_gettime()/1000000.0;
            }
            ffmpegMusic.put(&packet);
        }
        av_packet_unref(&packet);
    }

    if(audioindex!=-1)
    {
        while(bisPlaying && ffmpegMusic.queue.size()!=0)
        {
            usleep(1000*2);
        }
        ffmpegMusic.stop();
        ffmpegVideo.stop();
    }



    bisPlaying = false;
    sws_freeContext(img_convert_ctx);



    if(pVideoCodecCtx!= nullptr) {
        if(avcodec_is_open(pVideoCodecCtx))
            avcodec_close(pVideoCodecCtx);

        avcodec_free_context(&pVideoCodecCtx);
        pVideoCodecCtx=nullptr;

    }

    if(pAudioCodecCtx!= nullptr) {
        if(avcodec_is_open(pAudioCodecCtx))
            avcodec_close(pAudioCodecCtx);

        avcodec_free_context(&pAudioCodecCtx);
        pAudioCodecCtx = nullptr;

    }


    avformat_close_input(&pFormatCtx);
    av_freep(&pFrameYUV->data[0]);
    av_frame_free(&pFrameYUV);

    avio_context_free(&avio);


    LOGE("video Play exit!");
    connectdid = -1;

    return 0;


}


void OnLinePlayer::Play()
{

    if(connectdid!=-1)
    {
        bisPlaying=false;

        pthread_join(connectdid, 0);
        connectdid=-1;
    }

    pthread_create(&connectdid, nullptr, Progress, (void *)this);

}