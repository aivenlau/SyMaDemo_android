//
// Created by Administrator on 2017/11/20.
//




#include <unistd.h>


#include "FFmpegMusic.h"

//播放线程
void *MusicPlay(void *args) {
    auto *musicplay = (FFmpegMusic *) args;
    musicplay->CreatePlayer();
    pthread_exit(0);//退出线程
}

//得到pcm数据
int getPcm(FFmpegMusic *agrs) {
#if 0
    while (agrs->isPlay){

    }

    return 0;

#else
    auto *avPacket = (AVPacket *) av_mallocz(sizeof(AVPacket));
    AVFrame *avFrame = av_frame_alloc();
    int size = 0;
    int gotframe = 0;
    while (agrs->isPlay) {
        size = 0;
        agrs->get(avPacket);
#if 0
        if (avcodec_send_packet(agrs->codec, avPacket) == 0)
        {
            if (avcodec_receive_frame(agrs->codec, avFrame) >= 0)
            {
                swr_convert(agrs->swrContext, &agrs->out_buffer, 8000 * 2, (const uint8_t **) avFrame->data, avFrame->nb_samples);
//                缓冲区的大小
                size = av_samples_get_buffer_size(nullptr, agrs->out_channer_nb, avFrame->nb_samples,
                                                  AV_SAMPLE_FMT_S16, 1);
                break;
            }
        }
#endif
        gotframe = 0;
        if (avcodec_send_packet(agrs->codec, avPacket) == 0) {
            if (avcodec_receive_frame(agrs->codec, avFrame) == 0) {
                gotframe = 1;
            }
        }
        if (gotframe) {
            if (avFrame->pts != AV_NOPTS_VALUE) {
                agrs->clock = av_q2d(agrs->time_base) * avFrame->pts;
            }
            int nRet = swr_convert(agrs->swrContext, &agrs->out_buffer,
                                   (agrs->codec->sample_rate) * 2,
                                   (const uint8_t **) (avFrame->data), avFrame->nb_samples);
            size = av_samples_get_buffer_size(nullptr, agrs->out_channer_nb, avFrame->nb_samples,
                                              AV_SAMPLE_FMT_S16, 1);
            break;
        }
    }
    av_free(avPacket);
    av_frame_free(&avFrame);
    return size;
#endif

}

//回调函数
void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
    //得到pcm数据
    //LOGE("回调pcm数据");

    auto *musicplay = (FFmpegMusic *) context;
    if(!musicplay->isPlay)
    {
        return;
    }
    int datasize = getPcm(musicplay);
    if (datasize > 0) {
        double time = datasize / musicplay->nSize;
        musicplay->clock = time + musicplay->clock;
        (*bq)->Enqueue(bq, musicplay->out_buffer, (SLuint32) datasize);
    }
}


//初始化ffmpeg
int FFmpegMusic::createFFmpeg() {

    swrContext = swr_alloc();
    int in_rate = codec->sample_rate;
    int in_bytes = (codec->sample_fmt == AV_SAMPLE_FMT_S16 ? 2 : 1);
    int nLen = (int) (in_rate * in_bytes * 1);
    nSize = (double) nLen;
    uint64_t in_ch_layout = AV_CH_LAYOUT_MONO;//(codec->channel_layout == 0? AV_CH_LAYOUT_MONO:codec->channel_layout);
    if (codec->channel_layout != 0) {
        in_ch_layout = codec->channel_layout;
    }
    out_buffer = (uint8_t *) av_mallocz((size_t) nLen);
    //单通道
    uint64_t out_ch_layout = AV_CH_LAYOUT_MONO;
//  输出采样位数  16位
    enum AVSampleFormat out_formart = AV_SAMPLE_FMT_S16;
//输出的采样率必须与输入相同
    int out_sample_rate = codec->sample_rate;
    swr_alloc_set_opts(swrContext, out_ch_layout, out_formart, out_sample_rate,
                       in_ch_layout, codec->sample_fmt, in_rate, 0,
                       nullptr);

    swr_init(swrContext);
//    获取通道数  1
    out_channer_nb = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_MONO);
    LOGE("------>通道数%d  sample_rate = %d", out_channer_nb, out_sample_rate);
    return 0;
}


FFmpegMusic::FFmpegMusic() : bqPlayerPlay(nullptr), bqPlayerObject(nullptr),
                             outputMixObject(nullptr), engineObject(
                nullptr), swrContext(nullptr) {
    bStartPlay = false;
    clock = 0;
    playId = -1;
    isPlay = false;
    isPause = false;
//初始化锁
    pthread_mutex_init(&mutex, nullptr);
    pthread_cond_init(&cond, nullptr);
}

FFmpegMusic::~FFmpegMusic() {
    if (out_buffer) {
        free(out_buffer);
    }
    for (int i = 0; i < queue.size(); ++i) {
        AVPacket *pkt = queue.front();
        queue.erase(queue.begin());
        av_free(pkt);
    }
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
}

void FFmpegMusic::setAvCodecContext(AVCodecContext *avCodecContext) {
    codec = avCodecContext;
    createFFmpeg();
}
//将packet压入队列,生产者

int FFmpegMusic::put(AVPacket *avPacket) {
    //LOGE("插入队列");
    auto *avPacket1 = (AVPacket *) av_mallocz(sizeof(AVPacket));
    //克隆
    if (av_packet_ref(avPacket1, avPacket)) {
        return 0;
    }
    pthread_mutex_lock(&mutex);
    queue.push_back(avPacket1);//将packet压入队列
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    return 1;
}
//将packet弹出队列

int FFmpegMusic::get(AVPacket *avPacket) {
    //LOGE("取出队列");
    //pthread_mutex_lock(&mutex);
    while (isPlay) {
        if (!queue.empty() && !isPause) {
            //如果队列中有数据可以拿出来
            if (av_packet_ref(avPacket, queue.front()) != 0) {
                break;     //出错
            }
            //取成功了，弹出队列，销毁packet
            AVPacket *packet2 = queue.front();
            queue.erase(queue.begin());
            av_free(packet2);
            break;
        } else {
            LOGE("音频执行wait");
            usleep(1000 * 5);
            //pthread_cond_wait(&cond, &mutex);
        }
    }
    //LOGE("Exit GreatPacket");
    //pthread_mutex_unlock(&mutex);
    return 0;
}

void FFmpegMusic::play() {
    isPause = false;
    bStartPlay = false;
    isPlay = false;
    if (playId != -1) {
        pthread_join(playId, 0);
        playId = -1;
    }
    isPlay = true;
    pthread_create(&playId, nullptr, MusicPlay, this);//开启begin线程
    LOGE_B("playId = %d",(int)playId);


}

void FFmpegMusic::stop() {
    //LOGE("声音暂停");
//    if(playId<=0)
//        return;
    //因为可能卡在 deQueue
    if (isPlay) {
        //pthread_mutex_lock(&mutex);
        isPlay = false;
        //pthread_cond_signal(&cond);
        //pthread_mutex_unlock(&mutex);
    }

    if (playId != -1) {
        pthread_join(playId, 0);
        playId = -1;
    }

    SLresult result;
    if (bqPlayerPlay) {
        SLuint32  nStatus;
        (*bqPlayerPlay)->GetPlayState(bqPlayerPlay,&nStatus);
        (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_STOPPED);
        (*bqPlayerPlay)->GetPlayState(bqPlayerPlay,&nStatus);
        bqPlayerPlay = nullptr;
    }


    if (bqPlayerObject) {
        (*bqPlayerObject)->Destroy(bqPlayerObject);
        bqPlayerBufferQueue = nullptr;
        bqPlayerObject = nullptr;
        bqPlayerVolume = nullptr;
    }


    if (outputMixObject) {
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject = nullptr;
    }

    if (engineObject) {
        (*engineObject)->Destroy(engineObject);
        engineObject = nullptr;
        engineEngine = nullptr;
    }


    if (swrContext)
        swr_free(&swrContext);
    swrContext = nullptr;
    usleep(1000 * 150);

    for (int i = 0; i < queue.size(); ++i) {
        AVPacket *pkt = queue.front();
        queue.erase(queue.begin());
        LOGE("销毁音频帧%d", (int) queue.size());
        av_free(pkt);
    }
//    queue.clear();

//    if (this->codec) {
//        if (avcodec_is_open(this->codec))
//            avcodec_close(this->codec);
//        avcodec_free_context(&this->codec);
//        this->codec = 0;
//    }

    LOGE("AUDIO clear");
}

int FFmpegMusic::CreatePlayer() {
    LOGE("创建opnsl es播放器");
    //创建播放器
    SLresult result;
    // 创建引擎engineObject
    result = slCreateEngine(&engineObject, 0, nullptr, 0, nullptr, nullptr);
    if (SL_RESULT_SUCCESS != result) {
        return 0;
    }
    // 实现引擎engineObject
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) {
        return 0;
    }
    // 获取引擎接口engineEngine
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE,
                                           &engineEngine);
    if (SL_RESULT_SUCCESS != result) {
        return 0;
    }
    // 创建混音器outputMixObject
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 0,
                                              0, 0);
    if (SL_RESULT_SUCCESS != result) {
        return 0;
    }
    // 实现混音器outputMixObject
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) {
        return 0;
    }
    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
                                              &outputMixEnvironmentalReverb);
    const SLEnvironmentalReverbSettings settings = SL_I3DL2_ENVIRONMENT_PRESET_DEFAULT;
    if (SL_RESULT_SUCCESS == result) {
        (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
                outputMixEnvironmentalReverb, &settings);
    }


    SLDataLocator_AndroidSimpleBufferQueue android_queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                                                            2};
    SLuint32 aa = SL_SAMPLINGRATE_8;
    if (codec->sample_rate == 16000) {
        aa = SL_SAMPLINGRATE_16;
    }

    SLDataFormat_PCM pcm = {SL_DATAFORMAT_PCM, 1, aa, SL_PCMSAMPLEFORMAT_FIXED_16,
                            SL_PCMSAMPLEFORMAT_FIXED_16,
                            SL_SPEAKER_FRONT_CENTER,//SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
                            SL_BYTEORDER_LITTLEENDIAN};
//   新建一个数据源 将上述配置信息放到这个数据源中
    SLDataSource slDataSource = {&android_queue, &pcm};
//    设置混音器
    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};

    SLDataSink audioSnk = {&outputMix, nullptr};

    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND,
            /*SL_IID_MUTESOLO,*/ SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE,
            /*SL_BOOLEAN_TRUE,*/ SL_BOOLEAN_TRUE};
    //先讲这个
    (*engineEngine)->CreateAudioPlayer(engineEngine, &bqPlayerObject, &slDataSource,
                                       &audioSnk, 3,
                                       ids, req);
    //初始化播放器
    (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);

//    得到接口后调用  获取Player接口
    (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerPlay);

//    注册回调缓冲区 //获取缓冲队列接口
    (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE,
                                    &bqPlayerBufferQueue);
    //缓冲接口回调
    (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue, bqPlayerCallback, this);
//    获取音量接口
    (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_VOLUME, &bqPlayerVolume);

//    获取播放状态接口
    (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);

    bqPlayerCallback(bqPlayerBufferQueue, this);


    playId = -1;

    return 1;
}

int FFmpegMusic::getSize() {
    int nsize = 0;
    pthread_mutex_lock(&mutex);
    nsize = (int) queue.size();
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    return nsize;

}

void FFmpegMusic::F_ClearQueue() {

    //pcmBuffer.Clear();

    pthread_mutex_lock(&mutex);
    for (int i = 0; i < queue.size(); ++i) {
        AVPacket *pkt = queue[i];
        //queue.erase(queue.begin());
        av_free(pkt);
    }
    queue.clear();
    pthread_mutex_unlock(&mutex);


}


void FFmpegMusic::pause() {
    isPause = !isPause;
    pthread_cond_signal(&cond);
//    if(isPause){
//        isPause=false;
//    } else{
//        isPause=true;
//        pthread_cond_signal(&cond);
//    }
}