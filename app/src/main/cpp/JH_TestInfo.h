//
// Created by AivenLau on 2018/3/5.
//

#ifndef SYMADEMO_ANDROID_JH_TESTINFO_H
#define SYMADEMO_ANDROID_JH_TESTINFO_H

#include <vector>
#include <string>

using namespace std;

typedef struct {
    string sStartTime;
    int nFrameNo;
    int nCount;
    int nNormalCount;
    int nRepeatCount;
    int nArray[2000];
} TestInfo_;

class JH_TestInfo {
public:
    vector<TestInfo_ *> TestInfo_vector;

    JH_TestInfo();

    ~JH_TestInfo();

    void F_InsertInof(int nInxFrame, int nCount, int nNo);

    void F_SaveTestInfo(void);

};


#endif //SYMADEMO_ANDROID_JH_TESTINFO_H
