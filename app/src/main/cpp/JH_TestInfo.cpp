//
// Created by AivenLau on 2018/3/5.
//

#include <cstring>

#include <stdio.h>
#include "libavutil/time.h"

#include "JH_TestInfo.h"

JH_TestInfo::JH_TestInfo() {
    TestInfo_vector.clear();
}

JH_TestInfo::~JH_TestInfo() {
    vector<TestInfo_ *>::iterator it;
    for (it = TestInfo_vector.begin(); it != TestInfo_vector.end(); it++) {
        delete *it;
    }
    vector<TestInfo_ *>().swap(TestInfo_vector);
}

void JH_TestInfo::F_InsertInof(int nInxFrame, int nCount, int nNo) {
    vector<TestInfo_ *>::iterator it;
    bool bFind = false;
    for (it = TestInfo_vector.begin(); it != TestInfo_vector.end(); it++) {
        if ((*it)->nFrameNo == nInxFrame) {
            bFind = true;
            (*it)->nCount = nCount;
            if (nNo < 2000) {
                if ((*it)->nArray[nNo] == -1) {
                    (*it)->nNormalCount++;
                } else {
                    (*it)->nRepeatCount++;
                }
            }
            break;
        }
    }
    if (!bFind) {
        //TestInfo_ *info = new TestInfo_();
    }
}

void JH_TestInfo::F_SaveTestInfo(void) {


}