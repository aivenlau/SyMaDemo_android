//
// Created by AivenLau on 2017/8/25.
//

#include <string.h>
#include "myRTP.h"
#include "Defines.h"

myRTP::myRTP(): nIndex(0),nLen(0)
{

}
myRTP::myRTP(uint8_t *buffer,int nLena)
{
    //if(nLen>0)
    {
        memcpy(mBuffer,buffer,nLena);
        nIndex = (uint16_t )buffer[2]*0x100+buffer[3];
        nLen = nLena;
    }


}
myRTP::~myRTP()
{
    //LOGE("decall");
}
