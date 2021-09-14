//
// Created by AivenLau on 2016/12/12.
//

#include "MyFrame.h"

MyFrame::MyFrame() : data(nullptr), nLen(0), bKeyFranme(false) {

}

MyFrame::~MyFrame() {
    if (data != nullptr) {
        delete[]data;
    }
    nLen = 0;
}

void MyFrame::Release() {
    if (data != nullptr) {
        delete[]data;
        data = nullptr;
    }
    nLen = 0;
}