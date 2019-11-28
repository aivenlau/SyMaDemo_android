//
// Created by AivenLau on 2019-08-05.
//

#include <libyuv.h>
#include <unistd.h>
#include "OnLinePlayer.h"
#include "Defines.h"
#include "MyRevBuffer.h"



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
        usleep(1000*10);
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
        pthread_join(connectdid, 0);
        connectdid=-1;
    }
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
                AV_PIX_FMT_YUV420P, 4);
    }
}

void *OnLinePlayer::Progress(void *data)
{
    OnLinePlayer *self =(OnLinePlayer *)data;
    self->Play_B();
    return nullptr;
}





int OnLinePlayer::Play_B() {


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
            AV_PIX_FMT_YUV420P, 4);

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

#if 0
        while(1)
        {
            ret = avcodec_receive_frame(pCodecCtx, pFrame);
            if (ret < 0)
            {
                if (ret == AVERROR_EOF)
                {
                    av_log(nullptr, AV_LOG_INFO, "video avcodec_receive_frame(): the decoder has been fully flushed\n");
                    avcodec_flush_buffers(pCodecCtx);
                    break;
                }
                else if (ret == AVERROR(EAGAIN))
                {
                    av_log(nullptr, AV_LOG_INFO, "video avcodec_receive_frame(): output is not available in this state - "
                                              "user must try to send new input\n");
                    break;
                }
                else
                {
                    av_log(nullptr, AV_LOG_ERROR, "video avcodec_receive_frame(): other errors\n");
                    break;
                }
            }
            else
            {
                pFrame->pts = pFrame->best_effort_timestamp;
                {
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
                    Recheck(pFrameYUV,pFrame);

                    if (pFrame->format == AV_PIX_FMT_YUVJ420P || pFrame->format == AV_PIX_FMT_YUV420P){
                        libyuv::I420Copy(pFrame->data[0], pFrame->linesize[0],
                                         pFrame->data[1], pFrame->linesize[1],
                                         pFrame->data[2], pFrame->linesize[2],
                                         pFrameYUV->data[0], pFrameYUV->linesize[0],
                                         pFrameYUV->data[1], pFrameYUV->linesize[1],
                                         pFrameYUV->data[2], pFrameYUV->linesize[2],
                                         pFrameYUV->width, pFrameYUV->height);

                    } else if (pFrame->format == AV_PIX_FMT_YUV422P || pFrame->format == AV_PIX_FMT_YUVJ422P){
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
                                  pCodecCtx->height,
                                  pFrameYUV->data, pFrameYUV->linesize);
                    }

                    if (gl_Frame != nullptr) {

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
                }


            }
        }

        // 1. 取出一个packet。使用pkt对应的serial赋值给d->pkt_serial
        if (av_read_frame(pFormatCtx, &packet) < 0)
        {
            break;
        }
        if (packet.data == NULL)
        {
            // 复位解码器内部状态/刷新内部缓冲区
            avcodec_flush_buffers(pCodecCtx);
        }
        else
        {
            // 2. 将packet发送给解码器
            //    发送packet的顺序是按dts递增的顺序，如IPBBPBB
            //    pkt.pos变量可以标识当前packet在视频文件中的地址偏移
            if (avcodec_send_packet(pCodecCtx, &packet) == AVERROR(EAGAIN))
            {
                av_log(NULL, AV_LOG_ERROR, "receive_frame and send_packet both returned EAGAIN, which is an API violation.\n");
            }

        }

#else
#if 0
        if (av_read_frame(pFormatCtx, &packet) < 0)
            break;
        if (packet.stream_index == videoindex)
        {

        }
#else

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

                            if (gl_Frame != nullptr) {//} && bReadDataOK) {


//                                if(gl_Frame->width != pFrameYUV->width ||
//                                   gl_Frame->height != pFrameYUV->height)
//                                {
//                                    av_freep(&gl_Frame->data[0]);
//                                    av_frame_free(&gl_Frame);
//
//                                    gl_Frame = av_frame_alloc();
//                                    gl_Frame->format = AV_PIX_FMT_YUV420P;
//                                    gl_Frame->width = pFrameYUV->width;
//                                    gl_Frame->height = pFrameYUV->height;
//                                    av_image_alloc(
//                                            gl_Frame->data, gl_Frame->linesize, pFrameYUV->width,
//                                            pFrameYUV->height,
//                                            AV_PIX_FMT_YUV420P, 4);
//
//                                }

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
#endif
#endif
        av_packet_unref(&packet);

    }

    if(audioindex!=-1)
    {
        while(bisPlaying && ffmpegMusic.queue.size()!=0)
        {
            usleep(1000*5);
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