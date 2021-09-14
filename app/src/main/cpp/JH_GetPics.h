//
// Created by 小米哥 on 1/20/21.
//

#ifndef SYMADEMO_ANDROID_JH_GETPICS_H
#define SYMADEMO_ANDROID_JH_GETPICS_H

#include <pthread.h>



class JH_GetPics {
public:
    JH_GetPics();
    ~JH_GetPics();
    bool   bProgress;

    pthread_t m_decodeThread;
    char *sPath;
    int getPicturesFromVideo(void);
    int StartGetPicture(char *sPaht_);
    static void *getPicturesFromVideo_(void *param);
};


#endif //SYMADEMO_ANDROID_JH_GETPICS_H
