#include <jni.h>

#ifndef _Included_com_generalplus_ffmpegLib_ffmpegWrapper
#define _Included_com_generalplus_ffmpegLib_ffmpegWrapper


//#define D_DEBUG
#ifndef _Nonnull
#define _Nonnull
#endif

#define OMX_COLOR_FormatYUV420SemiPlanar  21
#define OMX_COLOR_FormatYUV420Planar    19

#ifdef __cplusplus
extern "C" {
#endif
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naDownloadFile(JNIEnv *env, jclass type, jstring sPath_,
                                                        jstring dPath_);
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naInit(JNIEnv *env, jclass type,
                                                jstring pFileName_);
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naPlay(JNIEnv *env, jclass type);
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naStop(JNIEnv *env, jclass type);
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naStatus(JNIEnv *env, jclass type);
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naSaveSnapshot(JNIEnv *env, jclass type,
                                                        jstring pFileName_);
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naSaveVideo(JNIEnv *env, jclass type, jstring pFileName_);
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naStopSaveVideo(JNIEnv *env, jclass type);
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naSentCmd(JNIEnv *env, jclass type, jbyteArray cmd_,
                                                   jint n);
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naStartCheckSDStatus(JNIEnv *env, jclass type,
                                                              jboolean bStart);
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetIcType(JNIEnv *env, jclass type, jint nICType);

JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naRemoteSnapshot(JNIEnv *env, jclass type);

JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naRemoteSaveVideo(JNIEnv *env, jclass type);

JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSN_1WriteFrame(JNIEnv *env, jclass type,
                                                          jbyteArray data_, jint nLen);

JNIEXPORT jboolean JNICALL
Java_com_joyhonest_wifination_wifination_naCheckDevice(JNIEnv *env, jclass type);


JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naSetGP_1mode(JNIEnv *env, jclass type, jint mode);

JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naGetGP_1Status(JNIEnv *env, jclass type);

JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetFlip(JNIEnv *env, jclass type, jboolean b);

JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naGetPhotoDir(JNIEnv *env, jclass type);

JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naGetVideoDir(JNIEnv *env, jclass type);
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetdispRect(JNIEnv *env, jclass type, jint w, jint h);


JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naGetFiles(JNIEnv *env, jclass type,
                                                    jint nType);

/*
JNIEXPORT jboolean JNICALL
Java_com_joyhonest_wifination_wifination_naSetVideoSurface(JNIEnv *env, jclass type,
                                                           jobject surface);
*/
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSet3D(JNIEnv *env, jclass type, jboolean b);


JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSet3DA(JNIEnv *env, jclass type, jboolean b);

JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naGetSettings(JNIEnv *env, jclass type);

JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naSnapPhoto(JNIEnv *env, jclass type, jstring pFileName_,
                                                     jint PhoneOrSD);
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naStartRecordA(JNIEnv *env, jclass type,
                                                        jstring pFileName_,
                                                        jint PhoneOrSD);
JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naStopRecord_1All(JNIEnv *env, jclass type);

JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naGetSessionId(JNIEnv *env, jclass type);

JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetCustomer(JNIEnv *env, jclass type,
                                                       jstring sCustomer_);

JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naStopRecord(JNIEnv *env, jclass type, jint nType);

JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetDirectBuffer(JNIEnv *env, jclass type,
                                                           jobject bufferA,
                                                           jint nLen);

JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetFollow(JNIEnv *env, jclass type, jboolean bFollow);


JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetContinue(JNIEnv *env, jclass type);

JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naGetThumb(JNIEnv *env, jclass type, jstring filename_);


JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naSetMenuFilelanguage(JNIEnv *env, jclass type,
                                                               jint nLanguage);

JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naCancelDownload(JNIEnv *env, jclass type);

JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naCancelGetThumb(JNIEnv *env, jclass type);


JNIEXPORT jboolean JNICALL
Java_com_joyhonest_wifination_wifination_isPhoneRecording(JNIEnv *env, jclass type);


JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naGetGP_1RTSP_1Status(JNIEnv *env, jclass type);


JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naDeleteSDFile(JNIEnv *env, jclass type,
                                                        jstring fileName_);//

JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naSetGPFps(JNIEnv *env, jclass type, jint nFps);


JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naFillFlyCmdByC(JNIEnv *env, jclass type, jint nType);

JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetDirectBufferYUV(JNIEnv *env, jclass type,
                                                              jobject buffer, jint nLen);

JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naSave2FrameMp4(JNIEnv *env, jclass type, jbyteArray data_,
                                                         jint nLen, jint b, jboolean keyframe);

JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naSetRecordWH(JNIEnv *env, jclass type, jint ww, jint hh);

JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naGetFps(JNIEnv *env, jclass type);

JNIEXPORT jint JNICALL
Java_com_joyhonest_wifination_wifination_naGetwifiFps(JNIEnv *env, jclass type);

JNIEXPORT jstring JNICALL
Java_com_joyhonest_wifination_wifination_naGetControlType(JNIEnv *env, jclass type);

JNIEXPORT jboolean JNICALL
Java_com_joyhonest_wifination_wifination_naSetBackground(JNIEnv *env, jclass type, jbyteArray data_,
                                                         jint width, jint height);
JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetAdjFps(JNIEnv *env, jclass type, jboolean b);

//JNIEXPORT jint JNICALL
//Java_com_joyhonest_wifination_wifination_naGetRecordTime(JNIEnv *env, jclass type);


JNIEXPORT void JNICALL
Java_com_joyhonest_wifination_wifination_naSetGKA_1SentCmdByUDP(JNIEnv *env, jclass type,
                                                                jboolean bUdp);

#ifdef __cplusplus
}
#endif


#endif


