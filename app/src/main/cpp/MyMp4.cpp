//
// Created by AivenLau on 2018/12/10.
//

#include "MyMp4.h"
#include "stdio.h"
#include <unistd.h>
#include <jni.h>


static const int16_t alawtos16[256] =
        {
                -5504, -5248, -6016, -5760, -4480, -4224, -4992, -4736,
                -7552, -7296, -8064, -7808, -6528, -6272, -7040, -6784,
                -2752, -2624, -3008, -2880, -2240, -2112, -2496, -2368,
                -3776, -3648, -4032, -3904, -3264, -3136, -3520, -3392,
                -22016, -20992, -24064, -23040, -17920, -16896, -19968, -18944,
                -30208, -29184, -32256, -31232, -26112, -25088, -28160, -27136,
                -11008, -10496, -12032, -11520, -8960, -8448, -9984, -9472,
                -15104, -14592, -16128, -15616, -13056, -12544, -14080, -13568,
                -344, -328, -376, -360, -280, -264, -312, -296,
                -472, -456, -504, -488, -408, -392, -440, -424,
                -88, -72, -120, -104, -24, -8, -56, -40,
                -216, -200, -248, -232, -152, -136, -184, -168,
                -1376, -1312, -1504, -1440, -1120, -1056, -1248, -1184,
                -1888, -1824, -2016, -1952, -1632, -1568, -1760, -1696,
                -688, -656, -752, -720, -560, -528, -624, -592,
                -944, -912, -1008, -976, -816, -784, -880, -848,
                5504, 5248, 6016, 5760, 4480, 4224, 4992, 4736,
                7552, 7296, 8064, 7808, 6528, 6272, 7040, 6784,
                2752, 2624, 3008, 2880, 2240, 2112, 2496, 2368,
                3776, 3648, 4032, 3904, 3264, 3136, 3520, 3392,
                22016, 20992, 24064, 23040, 17920, 16896, 19968, 18944,
                30208, 29184, 32256, 31232, 26112, 25088, 28160, 27136,
                11008, 10496, 12032, 11520, 8960, 8448, 9984, 9472,
                15104, 14592, 16128, 15616, 13056, 12544, 14080, 13568,
                344, 328, 376, 360, 280, 264, 312, 296,
                472, 456, 504, 488, 408, 392, 440, 424,
                88, 72, 120, 104, 24, 8, 56, 40,
                216, 200, 248, 232, 152, 136, 184, 168,
                1376, 1312, 1504, 1440, 1120, 1056, 1248, 1184,
                1888, 1824, 2016, 1952, 1632, 1568, 1760, 1696,
                688, 656, 752, 720, 560, 528, 624, 592,
                944, 912, 1008, 976, 816, 784, 880, 848
        };


extern int encord_colorformat;

//MyMediaCoder myMediaCoder;

MyMp4::MyMp4() : bG_Audio(false), bRecording(false), sSavePath(""), sSavePathTmp(""),
                 nRecordWidth(640), nRecordHeight(360), pbPCMBuffer(nullptr), pbAACBuffer(nullptr),
                 fileHandle(MP4_INVALID_FILE_HANDLE), videoTrackId(MP4_INVALID_TRACK_ID),
                 audioTrackId(MP4_INVALID_TRACK_ID), bConvert(false), nSampleRate(8000),
                 m_writeThread(0), nFps(20), pFrameRecord(nullptr), nChannels(1) {
    pthread_mutex_init(&m_Frame_Queuelock, nullptr);
    pthread_cond_init(&m_Frame_condition, nullptr);
    pthread_mutex_init(&m_adjRecTime_lock, nullptr);
    InitfaacEnc();
}

MyMp4::~MyMp4() {
    LOGE("MyMP4 dealloc!!!");
}

int MyMp4::StopSaveVideo() {
    bRecording = false;

//    usleep(1000 * 50);
//    void *ret = nullptr;

//    if (m_writeThread != 0) {
//        pthread_join(m_writeThread, &ret);
//        m_writeThread = 0;
//    }
//
//    if (pthread_mutex_trylock(&m_Frame_Queuelock) == 0)
//    {
//        pthread_cond_signal(&m_Frame_condition);
//        pthread_mutex_unlock(&m_Frame_Queuelock);
//    }

    {
        MP4Close(fileHandle, 0);
        fileHandle = MP4_INVALID_FILE_HANDLE;
        videoTrackId = MP4_INVALID_TRACK_ID;
        audioTrackId = MP4_INVALID_TRACK_ID;
    }
    if (access(sSavePathTmp.c_str(), F_OK) == 0) {
        rename(sSavePathTmp.c_str(), sSavePath.c_str());
    }
    nRecTime = 0;
    return 0;
};

int MyMp4::F_StartRecordMp4(const char *sPath, int _nFps, int w, int h, bool bAudio) {
    if (bRecording)
        return -1;
    T1 = 0;
    T1_V = 0;
    sSavePath = sPath;
    sSavePathTmp = sSavePath + ".part";
    nRecordWidth = w;
    nRecordHeight = h;
    nFps = _nFps;

    int bitrate;
    if (nRecordWidth <= 640) {
        bitrate = 1000 * 1000;
    } else if (nRecordWidth <= 1280) {
        bitrate = (int) (4 * 1000 * 1000);
    } else {
        bitrate = (int) (6 * 1000 * 1000);
    }


    bG_Audio = bAudio;
//    if (!myMediaCoder.F_InitEncoder(nRecordWidth, nRecordHeight, bitrate, nFps,bAudio)) {
//        return -1;
//    }

    if (fileHandle != MP4_INVALID_FILE_HANDLE) {
        MP4Close(fileHandle, 0);
        fileHandle = MP4_INVALID_FILE_HANDLE;
    }
    //创建mp4文件
    fileHandle = MP4Create(sSavePathTmp.c_str(), 0);
    if (fileHandle == MP4_INVALID_FILE_HANDLE) {
        return -2;
    }

    if (pFrameRecord != nullptr) {
        delete pFrameRecord;
        pFrameRecord = 0;
    }

    nCount = 0;

    nRecTime = 0;
    MP4SetTimeScale(fileHandle, 90000);
    if (bG_Audio) {
        //if(bConvert)
        {
            if (hEncoder != nullptr) {
                faacEncClose(hEncoder);
                hEncoder = nullptr;
            }
            InitfaacEnc();
        }
    }


    return 0;
}

void MyMp4::StartWrite() {
    bRecording = true;
    pthread_create(&m_writeThread, nullptr, WriteThreadFunction, this);
}

uint64_t MyMp4::getMs() {

    struct timeval tv;
    gettimeofday(&tv, nullptr);
    uint64_t df = (uint64_t)(tv.tv_sec * 1000000);
    df += tv.tv_usec;
    df /= 1000;
    return df;
}

/*

timespec ts;
clock_gettime(CLOCK_REALTIME, &ts);
uint64_t t =(uint64_t)( ts.tv_sec*1000+ts.tv_nsec/1000);
return t;
 */


int MyMp4::writeVideo(void) {
#if 0
    uint64_t TS = getMs();
    uint64_t T2 = TS;
    int nDelay = 1000 / nFps;
    struct timespec timespec1;
    bool bNeedDelete;
    while (bRecording) {
        if(!bConvert) {
            MyFrame *pMyFrame = nullptr;
            bNeedDelete = false;
            timespec1.tv_sec = 0;
            timespec1.tv_nsec = 1000 * 1000 * 5;
            pthread_mutex_lock(&m_Frame_Queuelock);
            pthread_cond_timedwait(&m_Frame_condition, &m_Frame_Queuelock, &timespec1);
            if (!m_FrameQueue_H264.empty()) {
                pMyFrame = m_FrameQueue_H264.front();
                if (m_FrameQueue_H264.size() > 1) {
                    m_FrameQueue_H264.pop();
                    bNeedDelete = true;
                } else {
                    bNeedDelete = false;
                }
                pthread_mutex_unlock(&m_Frame_Queuelock);
                if (pMyFrame != nullptr) {
                    myMediaCoder.offerEncoder(pMyFrame->data, pMyFrame->nLen);
                    if (bNeedDelete)
                        delete pMyFrame;
                    T2 = getMs();
                    int daa = (int) (T2 - TS);
                    if (daa > 0) {
                        if (daa < nDelay) {
                            usleep((useconds_t) ((nDelay - daa) * 1000));
                        }
                    }
                    TS = getMs();
                } else {
                    usleep(5 * 1000); //delay 10ms  让出CPU给其他线程
                }
            } else {
                pthread_mutex_unlock(&m_Frame_Queuelock);
            }
        } else
        {
            usleep(1000);
        }
    }
    ClearQueue();

    LOGE("Exit Write Frame!");
#endif
    return 0;
};

void *MyMp4::WriteThreadFunction(void *param) {
    MyMp4 *pPlayer = (MyMp4 *) param;
    pPlayer->writeVideo();
    return nullptr;
}

void AudioEncoder(uint8_t *data, int nLen);


void MyMp4::F_AddAudioFrame(unsigned char *databuffer, int nLen) {
    if (!bG_Audio) {
        return;
    }
    if (audioTrackId == MP4_INVALID_TRACK_ID) {
        return;
    }

#if 1
    Hisi_AddAudioFrame(databuffer, nLen);
#else


    int PCMSize = nLen * 2;
    int nStatus = 0;
    //int nPCMRead = g711a_decode(_pbPCMTmpBuffer, &PCMSize, databuffer, nLen);
    int nPCMRead = nLen;//g711a_decode(_pbPCMTmpBuffer, &PCMSize, databuffer, nLen);
    memcpy(_pbPCMTmpBuffer,databuffer,nLen);
    if (nPCMRead > 0) {
        if (nPCMRead + _nCount >= 1024) {
            nStatus = 1;
            memcpy(_pbPCMBuffer + _nCount, _pbPCMTmpBuffer, (1024 - _nCount));
            //AudioEncoder(_pbPCMBuffer, 1024);

            int nTmp = nPCMRead - (1024 - _nCount);
            memset(_pbPCMBuffer, 0, 1024);
            memcpy(_pbPCMBuffer, _pbPCMTmpBuffer + (1024 - _nCount), nTmp);
            _nCount = 0;
            _nCount += nTmp;

        }
        if (nStatus == 0) {
            memcpy(_pbPCMBuffer + _nCount, _pbPCMTmpBuffer, nPCMRead);
            _nCount += nPCMRead;
        }
    }
#endif

}


int MyMp4::AddMp4Video(uint8_t *sps, int len1, uint8_t *pps, int len2) {
    if (fileHandle != MP4_INVALID_FILE_HANDLE) {
        int ds = (uint32_t)(90000 / nFps);
        videoTrackId = MP4AddH264VideoTrack(fileHandle, 90000, ds, (uint16_t) nRecordWidth,
                                            (uint16_t) nRecordHeight, sps[1], sps[2], sps[3], 3);
        if (videoTrackId == MP4_INVALID_TRACK_ID) {
            MP4Close(fileHandle, 0);
            fileHandle = MP4_INVALID_FILE_HANDLE;
            return -1;
        } else {
            MP4AddH264SequenceParameterSet(fileHandle, videoTrackId, sps, (uint16_t) len1);
            MP4AddH264PictureParameterSet(fileHandle, videoTrackId, pps, (uint16_t) len2);
            MP4SetVideoProfileLevel(fileHandle, 0x7F);
            LOGE("SPS PPS");
        }

        if (bG_Audio) {
            uint32_t samplesPerSecond;
            uint8_t profile;
            uint8_t channelConfig;
            /*
            0: Null
            1: AAC Main
            2: AAC LC (Low Complexity)
            3: AAC SSR (Scalable Sample Rate)
            4: AAC LTP (Long Term Prediction)
            5: SBR (Spectral Band Replication)
            6: AAC Scalable
            */
            audioTrackId = MP4AddAudioTrack(fileHandle, nSampleRate, 1024, MP4_MPEG4_AUDIO_TYPE);
            MP4SetAudioProfileLevel(fileHandle, 0x02);
            samplesPerSecond = nSampleRate;
            profile = 2; // AAC LC
            channelConfig = 1;
            uint8_t *pConfig = nullptr;
            uint32_t configLength = 0;
            MP4AacGetConfiguration(&pConfig, &configLength, profile, samplesPerSecond,
                                   channelConfig);
            MP4SetTrackESConfiguration(fileHandle, audioTrackId, pConfig, configLength);
            free(pConfig);
        }

        return 0;
    }
    return -1;
}

bool MyMp4::WriteAudio(jbyte *data, int nLen) {
    uint64_t T2;
    int64_t dat;
    if (nLen <= 0)
        return false;

    //bRecording &&
    if (fileHandle != MP4_INVALID_FILE_HANDLE && audioTrackId != MP4_INVALID_TRACK_ID) {
        //if(!bConvert)
        {
//            if (T1 != 0)
//            {
//                T2 = getMs();
//                dat = T2 - T1;
//                T1 = T2;
//                if (dat < 0)
//                {
//                    dat = 0;
//                }
//                dat = (dat * 8000) / 1000;
//                return MP4WriteSample(fileHandle, audioTrackId, (const uint8_t *) data, (uint32_t) nLen, MP4_INVALID_DURATION, 0, 1);
//            }
//            else {
//                T1 = getMs();
//                return MP4WriteSample(fileHandle, audioTrackId, (const uint8_t *) data, (uint32_t) nLen, MP4_INVALID_DURATION, 0, 1);
//            }
            return MP4WriteSample(fileHandle, audioTrackId, (const uint8_t *) data, (uint32_t) nLen,
                                  MP4_INVALID_DURATION, 0, 1);
        }
//        else
//        {
//          return  MP4WriteSample(fileHandle, audioTrackId,(const uint8_t*)data,(uint32_t)nLen, MP4_INVALID_DURATION, 0, 1);
//        }

    } else {
        return false;
    }
}

int MyMp4::WriteMp4Frame(uint8_t *data, int nLen, bool b) {

    if (nLen == 0)
        return -1;
    int ds;
    ds = 90000 / nFps;

    if (fileHandle != MP4_INVALID_FILE_HANDLE && videoTrackId != MP4_INVALID_TRACK_ID) {
        if (T1 == 0) {
            T1 = getMs();
        }
        int nL = nLen - 4;
        data[0] = (uint8_t)((unsigned int) nL >> 24u);
        data[1] = (uint8_t)((unsigned int) nL >> 16u);
        data[2] = (uint8_t)((unsigned int) nL >> 8u);
        data[3] = (uint8_t)((unsigned int) nL);
        {
            MP4WriteSample(fileHandle, videoTrackId, data, (uint32_t) nLen, (MP4Duration) ds, 0, b);
            nRecTime++;
        }
        return 0;
    }
    return -1;
}


int MyMp4::F_GetRecordTime() {
    int32_t ret = 0;
    float df = 1000.0f / nFps;
    pthread_mutex_lock(&m_adjRecTime_lock);
    ret = (int32_t) (nRecTime * df);
    pthread_mutex_unlock(&m_adjRecTime_lock);
    return ret;

}

#define NUM_ADTS_SAMPLING_RATES    16
uint32_t AdtsSamplingRates1[NUM_ADTS_SAMPLING_RATES] = {
        96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050,
        16000, 12000, 11025, 8000, 7350, 0, 0, 0
};

uint8_t MyMp4::MP4AdtsFindSamplingRateIndex(uint32_t samplingRate) {
    uint8_t i;
    for (i = 0; i < NUM_ADTS_SAMPLING_RATES; i++) {
        if (samplingRate == AdtsSamplingRates1[i]) {
            return i;
        }
    }
    return NUM_ADTS_SAMPLING_RATES - 1;
}

bool MyMp4::MP4AacGetConfiguration(uint8_t **ppConfig,
                                   uint32_t *pConfigLength,
                                   uint8_t profile,
                                   uint32_t samplingRate,
                                   uint8_t channels) {
    /* create the appropriate decoder config */

    uint8_t *pConfig = (uint8_t *) malloc(2);

    if (pConfig == nullptr) {
        return false;
    }

    uint8_t samplingRateIndex = MP4AdtsFindSamplingRateIndex(samplingRate);

    pConfig[0] = (uint8_t)(((profile) << 3) | ((samplingRateIndex & 0xe) >> 1));
    pConfig[1] = (uint8_t)(((samplingRateIndex & 0x1) << 7) | (channels << 3));


    *ppConfig = pConfig;
    *pConfigLength = 2;

    return true;
}


#if 0

void MyMp4::F_AddVideoFrameA(int w,int h, char * databuffer,int nlen)
{
    int len1 = w * h;
    int len2 = len1 / 4;

    const uint8_t *pY = (const uint8_t *) databuffer;
    const uint8_t *pU = pY + len1;
    const uint8_t *pV = pU + len2;

    int line1 = w;
    int line2 = w / 2;
    int line3 = w / 2;

    if (pFrameRecord == nullptr) {
        pFrameRecord = new MyFrame(nRecordWidth, nRecordHeight);
    }
    if (pFrameRecord->nWidth != nRecordWidth || pFrameRecord->nHeight != nRecordHeight) {
        delete pFrameRecord;
        pFrameRecord = new MyFrame(nRecordWidth, nRecordHeight);
    }




    //MyFrame *myFrame = new MyFrame(w, h);
    if (w != nRecordWidth || h != nRecordHeight) {

        libyuv::I420Scale(pY, line1,
                          pU, line2,
                          pV, line3,
                          w, h,
                          pFrameRecord->pY, pFrameRecord->nY,
                          pFrameRecord->pU, pFrameRecord->nU,
                          pFrameRecord->pV, pFrameRecord->nV,
                          nRecordWidth, nRecordHeight, libyuv::kFilterNone);

    } else {

        libyuv::I420Copy(pY, line1,
                         pU, line2,
                         pV, line3,
                         pFrameRecord->pY, pFrameRecord->nY,
                         pFrameRecord->pU, pFrameRecord->nU,
                         pFrameRecord->pV, pFrameRecord->nV,
                         nRecordWidth, nRecordHeight);

    }

#if 1

    int HH = nRecordHeight;
    int H2 = HH >> 1;

    int WW = nRecordWidth;
    int W2 = WW >> 1;

    MyFrame *frame_rec = pFrameRecord;

    uint8_t *srcY = frame_rec->pY;
    uint8_t *srcU = frame_rec->pU;
    uint8_t *srcV = frame_rec->pV;

    uint32_t nCount = 0;

    MyFrame *myfram = new MyFrame(nRecordHeight, nRecordWidth);


    //myfram->data = new uint8_t[nCount];
    //myfram->nLen = nCount;
    unsigned char *bufY = myfram->pY;
    unsigned char *bufU = myfram->pU;
    unsigned char *bufV = myfram->pV;


    if (encord_colorformat == OMX_COLOR_FormatYUV420SemiPlanar)   //YUV420 或者 NV2 主要是Android  硬编码，有些机型支持的 数据格式只有OMX_COLOR_FormatYUV420SemiPlanar
    {

        libyuv::I420ToNV12(srcY, frame_rec->nY,
                           srcU, frame_rec->nU,
                           srcV, frame_rec->nV,
                           bufY, WW,
                           bufU, WW,
                           WW, HH);
    } else {
        libyuv::I420Copy(srcY, frame_rec->nY,
                         srcU, frame_rec->nU,
                         srcV, frame_rec->nV,
                         bufY, WW,
                         bufU, W2,
                         bufV, W2,
                         WW, HH);
    }
    pthread_mutex_lock(&m_Frame_Queuelock);
    m_FrameQueue_H264.push(myfram);
    pthread_cond_signal(&m_Frame_condition);
    pthread_mutex_unlock(&m_Frame_Queuelock);
#endif
}
#endif

void MyMp4::F_AddVideoFrame(int w, int h, char *databuffer, int nlen) {


//    if (!bRecording) {
//        return;
//    }
//
//    //F_AddVideoFrameA(w,h,databuffer,nlen);


}

void MyMp4::ClearQueue() {
    pthread_mutex_lock(&m_Frame_Queuelock);
    while (m_FrameQueue_H264.size()) {
        MyFrame *fa = m_FrameQueue_H264.front();
        m_FrameQueue_H264.pop();
        delete fa;
    }
    pthread_mutex_unlock(&m_Frame_Queuelock);
}


int MyMp4::g711a_decode(void *pout_buf, int *pout_len, void *pin_buf, int in_len) {
    int16_t *dst = (int16_t *) pout_buf;
    uint8_t *src = (uint8_t *) pin_buf;
    uint32_t i = 0;
    int Ret = 0;
    if ((nullptr == pout_buf) || \
        (nullptr == pout_len) || \
        (nullptr == pin_buf) || \
        (0 == in_len)) {
        return -1;
    }

    if (*pout_len < 2 * in_len) {
        return -2;
    }

    for (i = 0; i < in_len; i++) {
        *(dst++) = alawtos16[*(src++)];
    }
    *pout_len = 2 * in_len;
    Ret = 2 * in_len;
    return Ret;
}

void MyMp4::F_SetConvert(bool b) {
    bConvert = b;
}

int MyMp4::F_InitMp4_A(Byte *sps, int n1, Byte *pps, int n2) {
    if (fileHandle == MP4_INVALID_FILE_HANDLE)
        return -1;

    int ds = (uint32_t)(90000 / nFps);

    //Byte spsData[]={0x67,0x64,0x00,0x1f,0xAD,0x84,0x01,0x0C,0x20,0x08,0x61,0x00,0x43,0x08,0x02,0x18,
    //  0x40,0x10,0xC2,0x00,0x84,0x3B,0x50,0x28,0x02,0xDD,0x37,0x01,0x01,0x01,0x02};
    //Byte ppsData[]={0x68,0xEE,0x3C,0xB0};

    Byte *spsData = (Byte *) sps;
    Byte *ppsData = (Byte *) pps;


    videoTrackId = MP4AddH264VideoTrack(fileHandle, 90000, ds, (uint16_t) nRecordWidth,
                                        (uint16_t) nRecordHeight, spsData[1], spsData[2],
                                        spsData[3], 3);
    if (videoTrackId == MP4_INVALID_TRACK_ID) {
        MP4Close(fileHandle, 0);
        fileHandle = MP4_INVALID_FILE_HANDLE;
        return -1;
    } else {
        MP4AddH264SequenceParameterSet(fileHandle, videoTrackId, spsData, (uint16_t) n1);
        MP4AddH264PictureParameterSet(fileHandle, videoTrackId, ppsData, (uint16_t) n2);
        MP4SetVideoProfileLevel(fileHandle, 0x7F);
        LOGE("SPS PPS");
    }

    if (bG_Audio) {
        uint32_t samplesPerSecond;
        uint8_t profile;
        uint8_t channelConfig;
        /*
        0: Null
        1: AAC Main
        2: AAC LC (Low Complexity)
        3: AAC SSR (Scalable Sample Rate)
        4: AAC LTP (Long Term Prediction)
        5: SBR (Spectral Band Replication)
        6: AAC Scalable
        */
        audioTrackId = MP4AddAudioTrack(fileHandle, nSampleRate, 1024, MP4_MPEG4_AUDIO_TYPE);
        MP4SetAudioProfileLevel(fileHandle, 0x02);
        samplesPerSecond = nSampleRate;
        profile = 2; // AAC LC
        channelConfig = nChannels;
        uint8_t *pConfig = nullptr;
        uint32_t configLength = 0;
        MP4AacGetConfiguration(&pConfig, &configLength, profile, samplesPerSecond, channelConfig);
        MP4SetTrackESConfiguration(fileHandle, audioTrackId, pConfig, configLength);
        free(pConfig);
    }
    return 0;
}


int MyMp4::F_InitMp4(void) {
    if (fileHandle == MP4_INVALID_FILE_HANDLE)
        return -1;

    int ds = (uint32_t)(90000 / nFps);

    Byte spsData[] = {0x67, 0x64, 0x00, 0x1f, 0xAD, 0x84, 0x01, 0x0C, 0x20, 0x08, 0x61, 0x00, 0x43,
                      0x08, 0x02, 0x18,
                      0x40, 0x10, 0xC2, 0x00, 0x84, 0x3B, 0x50, 0x28, 0x02, 0xDD, 0x37, 0x01, 0x01,
                      0x01, 0x02};
    Byte ppsData[] = {0x68, 0xEE, 0x3C, 0xB0};

    videoTrackId = MP4AddH264VideoTrack(fileHandle, 90000, ds, (uint16_t) nRecordWidth,
                                        (uint16_t) nRecordHeight, spsData[1], spsData[2],
                                        spsData[3], 3);
    if (videoTrackId == MP4_INVALID_TRACK_ID) {
        MP4Close(fileHandle, 0);
        fileHandle = MP4_INVALID_FILE_HANDLE;
        return -1;
    } else {
        MP4AddH264SequenceParameterSet(fileHandle, videoTrackId, spsData,
                                       (uint16_t)
        sizeof(spsData));
        MP4AddH264PictureParameterSet(fileHandle, videoTrackId, ppsData,
                                      (uint16_t)
        sizeof(ppsData));
        MP4SetVideoProfileLevel(fileHandle, 0x01);
        LOGE("SPS PPS");
    }

    if (bG_Audio) {
        uint32_t samplesPerSecond;
        uint8_t profile;
        uint8_t channelConfig;
        /*
        0: Null
        1: AAC Main
        2: AAC LC (Low Complexity)
        3: AAC SSR (Scalable Sample Rate)
        4: AAC LTP (Long Term Prediction)
        5: SBR (Spectral Band Replication)
        6: AAC Scalable
        */
        audioTrackId = MP4AddAudioTrack(fileHandle, 8000, 1024, MP4_MPEG4_AUDIO_TYPE);
        MP4SetAudioProfileLevel(fileHandle, 0x02);
        samplesPerSecond = 8000;
        profile = 2; // AAC LC
        channelConfig = nChannels;
        uint8_t *pConfig = nullptr;
        uint32_t configLength = 0;
        MP4AacGetConfiguration(&pConfig, &configLength, profile, samplesPerSecond, channelConfig);
        MP4SetTrackESConfiguration(fileHandle, audioTrackId, pConfig, configLength);
        free(pConfig);
    }
    return 0;

}

void MyMp4::InitfaacEnc() {

    nAudioFrame = 0;
    //nChannels = 1;
    nPCMBitSize = 16;
    nInputSamples = 0;


    //nSampleRate = 8000;

    nMaxOutputBytes = 0;

    hEncoder = faacEncOpen(nSampleRate, nChannels, &nInputSamples, &nMaxOutputBytes);

    nPCMBufferSize = (int) ((nInputSamples * (nPCMBitSize / 8)));

    if (pbPCMBuffer != nullptr) {
        free(pbPCMBuffer);
        pbPCMBuffer = 0;
    }
    pbPCMBuffer = (Byte *) malloc(nPCMBufferSize);
    memset(pbPCMBuffer, 0, nPCMBufferSize);
    if (pbAACBuffer != nullptr) {
        free(pbAACBuffer);
        pbAACBuffer = nullptr;
    }
    pbAACBuffer = (unsigned char *) malloc(nMaxOutputBytes);
    memset(pbAACBuffer, 0, nMaxOutputBytes);

    pConfiguration = faacEncGetCurrentConfiguration(hEncoder);
    pConfiguration->aacObjectType = LOW;//LC编码
    pConfiguration->mpegVersion = MPEG4;//
    pConfiguration->useTns = 1;//时域噪音控制,大概就是消爆音
    pConfiguration->allowMidside = 0;//
    pConfiguration->bandWidth = 0; //频宽
    pConfiguration->outputFormat = 0; //输出是否包含ADTS头
    pConfiguration->inputFormat = FAAC_INPUT_16BIT;
    pConfiguration->quantqual = 80;
    faacEncSetConfiguration(hEncoder, pConfiguration);
    nCount = 0;

}


int MyMp4::Hisi_AddAudioFrame(uint8_t *databuffer, int nLen) {
    int nStatus = 0;
    if ((bG_Audio && bRecording && pConfiguration != nullptr && hEncoder != nullptr) ||
        (bConvert && hEncoder != nullptr && bG_Audio && pConfiguration != nullptr)) {
        int nRet;
        int PCMSize = nLen;
        if (PCMSize <= PCM_BUFFER_KEN) {
            Byte *pbPCMTmpBuffer = databuffer;
            int nPCMRead = nLen;
            if (nPCMRead + nCount >= nPCMBufferSize)  //已经有1024 sample
            {
                nStatus = 1;
                memset(pbAACBuffer, 0, nMaxOutputBytes);
                memcpy(pbPCMBuffer + nCount, pbPCMTmpBuffer, (nPCMBufferSize - nCount));
                nInputSamples = (nPCMBufferSize / (nPCMBitSize / 8));
                nRet = faacEncEncode(hEncoder, (int *) pbPCMBuffer, (unsigned int) nInputSamples,
                                     pbAACBuffer, (unsigned int) nMaxOutputBytes);
                if (nRet > 0) {
                    WriteAudio((jbyte *) pbAACBuffer, nRet);
                }
                int nTmp = nPCMRead - (nPCMBufferSize - nCount);
                memset(pbPCMBuffer, 0, nPCMBufferSize);
                memcpy(pbPCMBuffer, pbPCMTmpBuffer + (nPCMBufferSize - nCount), nTmp);
                nCount = 0;
                nCount += nTmp;

            }
            if (nStatus == 0) {
                memcpy(pbPCMBuffer + nCount, pbPCMTmpBuffer, nPCMRead);
                nCount += nPCMRead;
            }
        } else {

        }
        return 0;
    } else {
        return -1;
    }

}

