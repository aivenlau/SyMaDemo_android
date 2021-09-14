//
// Created by AivenLau on 2019-08-05.
//

#ifndef SYMADEMO_ANDROID_GP4225_FILESTRUCT_H
#define SYMADEMO_ANDROID_GP4225_FILESTRUCT_H


#pragma pack(push) //保存对齐状态
#pragma pack(1)//设定为4字节对齐


typedef struct {
    char sPaht[32];
    char sFileName[32];
    char sServerIP[32];
    char sSaveFileName[256];
    int32_t nFileLen;
    int nPort;
    bool bPlay;

} GP4225_FileStruct;
#pragma pack(pop) //保存对齐状态

#endif //SYMADEMO_ANDROID_GP4225_FILESTRUCT_H
