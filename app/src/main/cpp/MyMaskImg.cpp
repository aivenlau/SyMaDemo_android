//
// Created by 小米哥 on 2020/5/27.
//
#include "MyMaskImg.h"

MyMaskImg::MyMaskImg() : nWidth(-1), nHeight(-1), pLeftLen(nullptr), pRiggtLen(nullptr) {

}

void MyMaskImg::MakeMaskImg(int w, int h) {
    if (pLeftLen != nullptr) {
        delete[]pLeftLen;
        pLeftLen = nullptr;
    }
    if (pRiggtLen != nullptr) {
        delete[]pRiggtLen;
        pRiggtLen = nullptr;
    }


    nWidth = w;
    nHeight = h;
    if (w < 0 || h < 0)
        return;

    pLeftLen = new int[h];
    pRiggtLen = new int[h];

    int x2 = w / 2;
    int y2 = h / 2;
    int r = x2 <= y2 ? x2 : y2;
    int RR0 = r - 4;

    int32_t xa, ya;
    int32_t RR = RR0 * RR0;

    int nLen = 0;

    int dx, dy;

    for (int y = 0; y < h; y++) {
        nLen = 0;
        for (int x = 0; x < w; x++) {
            dx = x - x2;
            dy = y - y2;
            xa = dx * dx;
            ya = dy * dy;
            if (xa + ya > RR) {
                nLen++;
            }
        }
        nLen /= 2;

        int WW = w - (nLen << 1);
        if (WW < 0)
            WW = 0;
        WW += nLen;
        pLeftLen[y] = nLen;
        pRiggtLen[y] = WW;
    }
}

MyMaskImg::~MyMaskImg() {
    if (pLeftLen != nullptr) {
        delete[]pLeftLen;
        pLeftLen = nullptr;
    }

    if (pRiggtLen != nullptr) {
        delete[]pRiggtLen;
        pRiggtLen = nullptr;
    }
    nWidth = 0;
    nHeight = 0;
}