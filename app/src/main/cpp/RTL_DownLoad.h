//
// Created by AivenLau on 2019-05-13.
//

#ifndef SYMADEMO_ANDROID_RTL_DOWNLOAD_H
#define SYMADEMO_ANDROID_RTL_DOWNLOAD_H

#include <string>


typedef void (*Rtl_GetDAta)(uint8_t *data,int nLen);

class RTL_DownLoad {
public:


    int socketfd;
    bool bConnected;
    pthread_t readid;


    RTL_DownLoad();
    ~RTL_DownLoad();
    int Connect();
    void disConnected();



    int F_GetMode(void);

    int  F_ReadList(int  nImage,int inx);
    std::string string_format(const char* format, ...);
    void StartReadThread(void);
    static void *ReadData(void *dat);

    void GetData(uint8_t *data,int nLen);
    Rtl_GetDAta fuc_getData;


    int DownLoadFile(const char *sFile);






};


#endif //SYMADEMO_ANDROID_RTL_DOWNLOAD_H
