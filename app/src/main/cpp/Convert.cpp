//
// Created by AivenLau on 2019/3/6.
//

#include "Defines.h"

#include "Convert.h"
#include "MyMp4.h"
#include "stdio.h"
#include <unistd.h>
#include <jni.h>
#include "faac.h"
#include "JH_GetPics.h"
#define  TAG "Convert"


typedef unsigned char Byte;
MyMp4 myMp4;




int F_FindNaul(char *pData, int *pnDataLen, int *ResultBuffer, int *ResultBuffer_Len,
               int *nResult_Len,bool bHeader = true) {

    int32_t *pD;

    memset(ResultBuffer, -1, (size_t)(*nResult_Len));
    int nDataLen = *pnDataLen;
    if (pData == nullptr)
        return 0;
    if (nDataLen < 5)
        return 0;

    int nCount = 0;
    for (int i = 0; i < nDataLen;i++) {
        pD = (int32_t *) (pData + i);
        if (*pD == 0x01000000) {
            ResultBuffer[nCount] = i;
            nCount++;
        }

    }


    char H1;
    char H2;
    char H3;
    char H4;

    uint32_t naluLength;
    int nOffset;
    int nNext;
    if (nCount > 0) {
        for (int i = 0; i < nCount; i++) {
            nOffset = ResultBuffer[i];
            if (i == (nCount - 1)) {
                naluLength = (uint32_t)(nDataLen - nOffset);
            } else {

                nNext = ResultBuffer[i + 1];
                naluLength = (uint32_t)(nNext - nOffset);
            }
            naluLength -= 4;
            ResultBuffer_Len[i] = naluLength;

            if(bHeader) {
                H1 = (char) (naluLength >> 24u);
                H2 = (char) (naluLength >> 16u);
                H3 = (char) (naluLength >> 8u);
                H4 = (char) (naluLength);
                pData[nOffset] = H1;
                pData[nOffset + 1] = H2;
                pData[nOffset + 2] = H3;
                pData[nOffset + 3] = H4;
            }
        }
    }
    return nCount;
}


int F_Convert(const char *path, const char *out_filename) {

    //av_register_all();

    AVCodec *pVedioCodec = nullptr;
    AVCodec *pAudioCodec = nullptr;

    AVCodecContext *m_codecCtx_Video = nullptr;
    AVCodecContext *m_codecCtx_Audio = nullptr;
    AVFormatContext *m_formatCtx;
    int videoindex = -1;
    int audioindex = -1;
    char bufff[1025];
    int err_code;
    m_formatCtx = avformat_alloc_context();
    err_code = avformat_open_input(&m_formatCtx, path, nullptr, nullptr);
    if (err_code != 0) {
        av_strerror(err_code, bufff, 1024);
        LOGE("Open input path errorcode = %d info = %s ", err_code, bufff);
        avformat_free_context(m_formatCtx);
        m_formatCtx = nullptr;
        return -2;
    }
    if (avformat_find_stream_info(m_formatCtx, nullptr) < 0) {
        LOGE("avformat_find_stream_info failed!");
        if (m_formatCtx != nullptr) {
            avformat_close_input(&m_formatCtx);
            avformat_free_context(m_formatCtx);
            m_formatCtx = nullptr;
        }
        return -3;
    }


    //


    int nWidth = 0;
    int nHeight = 0;
    int frame_rate = 0;
    int ret = 0;
    av_dump_format(m_formatCtx, 0, path, 0);
    int32_t videoCount = 0;
    int32_t audioCount = 0;


    for (int i = 0; i < m_formatCtx->nb_streams; i++) {
        if (m_formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoindex = i;
            videoCount++;


        }
        if (m_formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioindex = i;
            audioCount++;
        }
    }

    if (audioCount > 1 || videoCount > 1) {
        avformat_close_input(&m_formatCtx);
        avformat_free_context(m_formatCtx);
        m_formatCtx = nullptr;
        return -4;
    }

    if (videoCount == 0) {
        avformat_close_input(&m_formatCtx);
        avformat_free_context(m_formatCtx);
        m_formatCtx = nullptr;
        return -5;
    }

    AVStream *stream_video = m_formatCtx->streams[videoindex];

    bool bIsH264 = (AV_CODEC_ID_H264 == stream_video->codecpar->codec_id);

    pVedioCodec = avcodec_find_decoder(stream_video->codecpar->codec_id);

    if (pVedioCodec == nullptr) {
        avformat_close_input(&m_formatCtx);
        avformat_free_context(m_formatCtx);
        m_formatCtx = nullptr;
        return -6;
    }

    m_codecCtx_Video = avcodec_alloc_context3(pVedioCodec);
    avcodec_parameters_to_context(m_codecCtx_Video, stream_video->codecpar);

    if (m_codecCtx_Video->pix_fmt == AV_PIX_FMT_NONE) {
        if (m_formatCtx) {
            avcodec_free_context(&m_codecCtx_Video);
            avformat_close_input(&m_formatCtx);
            avformat_free_context(m_formatCtx);
            m_formatCtx = nullptr;
            m_codecCtx_Video = nullptr;
        }
        return -7;
    }

    err_code = avcodec_open2(m_codecCtx_Video, pVedioCodec, nullptr);
    if (err_code < 0) {
        LOGE("avcodec_open2 failed! error");
        if (m_formatCtx) {
            avcodec_free_context(&m_codecCtx_Video);
            avformat_close_input(&m_formatCtx);
            avformat_free_context(m_formatCtx);
            m_formatCtx = nullptr;
            m_codecCtx_Video = nullptr;
        }
        return -8;
    }

    if (audioCount == 1) {
        AVStream *stream_audio = m_formatCtx->streams[audioindex];
        pAudioCodec = avcodec_find_decoder(stream_audio->codecpar->codec_id);
        m_codecCtx_Audio = avcodec_alloc_context3(pAudioCodec);
        avcodec_parameters_to_context(m_codecCtx_Audio, stream_audio->codecpar);
        err_code = avcodec_open2(m_codecCtx_Audio, pAudioCodec, nullptr);
        if (err_code < 0) {
            if (m_formatCtx) {
                avcodec_close(m_codecCtx_Video);
                avcodec_free_context(&m_codecCtx_Video);
                avcodec_free_context(&m_codecCtx_Audio);
                avformat_close_input(&m_formatCtx);
                avformat_free_context(m_formatCtx);
                m_formatCtx = nullptr;
                m_codecCtx_Video = nullptr;
            }
            return -8;
        }
        myMp4.nSampleRate = m_codecCtx_Audio->sample_rate;
        myMp4.nChannels = m_codecCtx_Audio->channels;
    }


    nWidth = m_codecCtx_Video->width;
    nHeight = m_codecCtx_Video->height;
    AVStream *stream = m_formatCtx->streams[videoindex];

    if (stream->r_frame_rate.den > 0) {
        frame_rate = stream->r_frame_rate.num / stream->r_frame_rate.den;
        if (frame_rate == 19) {
            frame_rate = 20;
        }
    }

    err_code = avcodec_open2(m_codecCtx_Audio, pAudioCodec, nullptr);

    if (err_code < 0) {
        LOGE("avcodec_open2 failed! error");
        if (m_formatCtx) {
            avcodec_close(m_codecCtx_Video);
            avcodec_free_context(&m_codecCtx_Video);
            if (m_codecCtx_Audio != nullptr)
                avcodec_free_context(&m_codecCtx_Audio);
            avformat_close_input(&m_formatCtx);
            avformat_free_context(m_formatCtx);
            m_formatCtx = nullptr;
            m_codecCtx_Video = nullptr;
        }
        return -9;
    }

    AVPacket pkt = {0};
    av_init_packet(&pkt);
    pkt.data = nullptr;
    pkt.size = 0;

//    AVPacket pkt_Audio = {0};
//    av_init_packet(&pkt_Audio);
//    pkt_Audio.data = nullptr;
//    pkt_Audio.size = 0;

#if 0
    AVFormatContext *oc;
    AVDictionary *opt=nullptr;
    AVOutputFormat *fmt = nullptr;

    av_dict_set_int(&opt,"video_track_timescale",30,0);
    avformat_alloc_output_context2(&oc, nullptr, nullptr, out_filename);
    fmt = oc->oformat;
    for (int i = 0; i < m_formatCtx->nb_streams; i++)
    {
        AVStream *in_stream = m_formatCtx->streams[i];
        AVStream *out_stream = avformat_new_stream(oc, in_stream->codec->codec);
        avcodec_parameters_copy(out_stream->codecpar, in_stream->codecpar);

    }



#else

    bool bGetspspps = false;
    int64_t nCount_B = 0;
    int64_t nCount_C = 0;
    int ResultArray[100];
    int ResultArray_Len[100];
    int nResult = 100;
    int dataLen;
    bool bSps = false;
    bool bPPS = false;

    Byte *sps = nullptr;
    int spsLen = 0;
    Byte *pps = nullptr;
    int ppsLen = 0;
    Byte *data_;
    int dataLen_;


    uint8_t *extradata = stream_video->codecpar->extradata;
    int nSzie = stream_video->codecpar->extradata_size;
    uint8_t *SPS1 = nullptr;
    int SPS1_Len = 0;
    uint8_t *PPS1 = nullptr;
    int PPS1_Len = 0;
//    for (int i=0;i<nSzie;i++)
//    {
//        LOGE("%x ",extradata[i]);
//    }
    if (nSzie > 11 && extradata != nullptr) {
        int n = extradata[6] * 0x100 + extradata[7];
        SPS1_Len = n;
        SPS1 = new uint8_t[n];
        memcpy(SPS1, extradata + 8, n);

        int offset = 6 + 2 + 1 + n;
        n = extradata[offset] * 0x100 + extradata[offset + 1];
        PPS1_Len = n;
        PPS1 = new uint8_t[n];
        memcpy(PPS1, extradata + offset + 2, n);
    }


    AVFrame *m_decodedFrame_Audio = av_frame_alloc();


    // myMp4.F_StartRecordMp4(out_filename, frame_rate, nWidth, nHeight, audioCount != 0);
    myMp4.F_StartRecordMp4(out_filename, frame_rate, nWidth, nHeight, audioCount != 0);


    if (SPS1 != nullptr && PPS1 != nullptr) {

        bGetspspps = true;
        myMp4.F_InitMp4_A(SPS1, SPS1_Len, PPS1, PPS1_Len);
    }

    bool re;
    while (av_read_frame(m_formatCtx, &pkt) >= 0) {

        int stream_index = pkt.stream_index;
        if (stream_index == videoindex) {
            nCount_C++;
            nResult = 100;
            dataLen = pkt.size;

#if 1
            if (SPS1 != nullptr) {
                data_ = (uint8_t *) pkt.data;
                dataLen_ = dataLen;
                int nType = data_[4] & 0x1F;
                if (nType == 5 || nType == 1) {
                    if (MP4_INVALID_FILE_HANDLE != myMp4.fileHandle && bGetspspps) {
                        nCount_B++;
                        re = MP4WriteSample(myMp4.fileHandle, myMp4.videoTrackId,
                                            (const uint8_t *) (data_), (uint32_t)(dataLen_),
                                            MP4_INVALID_DURATION, 0, pkt.flags = (nType == 5));
                        ret = false;
                    }
                } else {
                    LOGE("Type %d ", nType);
                }
            } else   //文件头没有SPS ，就从流中读取。
            {
                int aa = F_FindNaul((char *) pkt.data, &dataLen, ResultArray, ResultArray_Len,
                                    &nResult);
                int offset;
                char nType;
                char *pData = (char *) pkt.data;

                for (int i = 0; i < aa; i++) {
                    offset = ResultArray[i] + 4;
                    nType = (char) (pData[offset] & 0x1F);

                    data_ = (Byte *) pData + ResultArray[i];
                    dataLen_ = ResultArray_Len[i];

                    if (nType == 7 && !bSps) {
                        bSps = true;
                        spsLen = dataLen_;//ResultArray_Len[i];
                        sps = new Byte[spsLen];
                        memcpy(sps, data_ + 4, dataLen_);

                    }
                    if (nType == 8 && !bPPS) {
                        bPPS = true;
                        ppsLen = dataLen_;
                        pps = new Byte[ppsLen];
                        memcpy(pps, data_ + 4, dataLen_);

                    }
                    if (bSps && bPPS && !bGetspspps) {
                        bGetspspps = true;
                        myMp4.F_InitMp4_A(sps, spsLen, pps, ppsLen);
                    }

                    if (nType == 5 || nType == 1) {
                        if (MP4_INVALID_FILE_HANDLE != myMp4.fileHandle && bGetspspps) {
                            nCount_B++;
                            re = MP4WriteSample(myMp4.fileHandle, myMp4.videoTrackId,
                                                (const uint8_t *) (data_),
                                                (uint32_t)(dataLen_ + 4), MP4_INVALID_DURATION, 0,
                                                pkt.flags == AV_PKT_FLAG_KEY);
                            ret = false;
                        }
                    }
                }
            }
#else
            int aa = F_FindNaul((char *)pkt.data,&dataLen,ResultArray,ResultArray_Len,&nResult);
            int offset;
            char nType;
            char *pData =(char *)pkt.data;

            for(int i=0;i<aa;i++)
            {
                offset =ResultArray[i]+4;
                nType  =(char) (pData[offset] & 0x1F);

                data_ =(Byte *)pData+ResultArray[i];
                dataLen_ = ResultArray_Len[i];

                if(nType == 7 && !bSps)
                {
                    bSps = true;
                    spsLen = dataLen_;//ResultArray_Len[i];
                    sps = new Byte[spsLen];
                    memcpy(sps,data_+4,dataLen_);

                }
                if(nType == 8 && !bPPS)
                {
                    bPPS = true;
                    ppsLen = dataLen_;
                    pps = new Byte[ppsLen];
                    memcpy(pps,data_+4,dataLen_);

                }
                if(bSps && bPPS && !bGetspspps)
                {
                    bGetspspps = true;
                    myMp4.F_InitMp4_A(sps,spsLen,pps,ppsLen);
                }

                if(nType==5 || nType ==1)
                {
                    if(MP4_INVALID_FILE_HANDLE !=myMp4.fileHandle && bGetspspps)
                    //if(MP4_INVALID_FILE_HANDLE !=myMp4.fileHandle)
                    {
                        nCount_B++;
                        re =  MP4WriteSample(myMp4.fileHandle,  myMp4.videoTrackId,(const uint8_t*)(data_),(uint32_t)(dataLen_+4), MP4_INVALID_DURATION, 0, pkt.flags==AV_PKT_FLAG_KEY);
                       ret = false;
                    }
                }
            }
#endif
        }
#if 1
        //if(myMp4.bG_Audio)
        {
            if (stream_index == audioindex) {
                int frameFinished1 = 0;
                //ret = avcodec_decode_audio4(m_codecCtx_Audio, m_decodedFrame_Audio, &frameFinished1, &pkt);

                if (avcodec_send_packet(m_codecCtx_Audio, &pkt) == 0) {
                    if (avcodec_receive_frame(m_codecCtx_Audio, m_decodedFrame_Audio) == 0) {
                        frameFinished1 = 1;
                    }
                }
                if (frameFinished1) {
                    if (MP4_INVALID_FILE_HANDLE != myMp4.fileHandle && bGetspspps) {
                        myMp4.Hisi_AddAudioFrame(m_decodedFrame_Audio->extended_data[0],
                                                 m_decodedFrame_Audio->nb_samples * 2);
                    }
                }
            }
        }
#endif
        av_packet_unref(&pkt);
        pkt.data = nullptr;
        pkt.size = 0;
    }

    if (PPS1 != nullptr) {
        delete[]PPS1;
    }
    if (SPS1 != nullptr) {
        delete[]SPS1;
    }

    if (sps != nullptr) {
        delete[]sps;
    }
    if (pps != nullptr) {
        delete[]pps;
    }
#endif
    //if(m_codecCtx_Video!=nullptr)
    {
        avcodec_close(m_codecCtx_Video);
        avcodec_free_context(&m_codecCtx_Video);
    }

    if (m_codecCtx_Audio != nullptr) {
        avcodec_close(m_codecCtx_Audio);
        avcodec_free_context(&m_codecCtx_Audio);
    }


    if (m_decodedFrame_Audio != nullptr) {
        av_frame_free(&m_decodedFrame_Audio);
        m_decodedFrame_Audio = nullptr;
    }
    myMp4.StopSaveVideo();
    usleep(1000 * 1);
    avformat_close_input(&m_formatCtx);
    avformat_free_context(m_formatCtx);
    m_formatCtx = nullptr;
    // LOGE("Conver OK!!! frame count =%ld  cont1 = %ld",nCount_B,nCount_C);
    return 0;

}