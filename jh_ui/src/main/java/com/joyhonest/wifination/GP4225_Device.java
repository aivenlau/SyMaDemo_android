package com.joyhonest.wifination;

import android.util.Log;

import org.simple.eventbus.EventBus;

import java.util.ArrayList;
import java.util.List;

public class GP4225_Device {


    public int nMode;
    public boolean bSD;
    public boolean bSDRecording;

    public int    nSDAllSize;      //1024*1024 unit
    public int    nSDAvaildSize;

    public int VideosCount;
    public int LockedCount;
    public int PhotoCount;
    public int nBattery;


    public String   sVer="";


//    public List<MyFile> PhotoFileList;
//    public List<MyFile> LockedFileList;
//    public List<MyFile> VideoFileList;

    public GP4225_Device()
    {
//        PhotoFileList = new ArrayList<>();
//        LockedFileList = new ArrayList<>();
//        VideoFileList = new ArrayList<>();
        nBattery = -1;
        VideosCount = 0;
        LockedCount = 0;
        PhotoCount = 0;
        bSD = false;
        nSDAllSize = -1;
        nSDAvaildSize = -1;

        bSDRecording = false;
        nMode = 0;
    }


//    private  void F_AddFile(int i,MyFile file)
//    {
//        if(i==1)
//        {
//            VideoFileList.add(file);
//        }
//        if(i==2)
//        {
//            LockedFileList.add(file);
//        }
//        if(i==3)
//        {
//            PhotoFileList.add(file);
//        }
//    }
//
//
//    public  void F_ClearAllList()
//    {
//        VideoFileList.clear();
//        LockedFileList.clear();
//        PhotoFileList.clear();
//    }


    byte bytes[] = new byte[32];

    public boolean  GP4225_PressData(byte[] data)
    {
        int nStartInx, nEndInx,inx,nLen;
        if(data==null)
            return false;
        if(data.length<=10)
            return false;

        if((data[0] & 0xFF) != 'F' ||
                (data[1] & 0xFF) != 'D' ||
                (data[2] & 0xFF) != 'W' ||
                (data[3] & 0xFF) != 'N')
        {
            return false;
        }


        String sFileName="";
        byte  nStatus=0;

        int m_cmd = data[4] & 0xFF + (data[5] & 0xFF) * 0x100;
        int s_cmd = data[6] & 0xFF + (data[7] & 0xFF) * 0x100;
        int n_len = data[8] & 0xFF + (data[9] & 0xFF) * 0x100;
        if(n_len==0)
            return false;
        if(n_len+10>data.length)
            return false;

        if (m_cmd == 0x0000 && s_cmd == 0x0001) {   //Device Status
            nMode = data[10] & 0xFF;
            bSD   = ((data[11] & 0x01)==0); // 0 have SD  1 NoSD
            bSDRecording =  ((data[11] & 0x02)!=0);

            VideosCount = ((data[12] & 0xFF) + (data[13] & 0xFF) * 0x100 + (data[14] & 0xFF) * 0x10000 + (data[15] & 0xFF) * 0x1000000);
            LockedCount = ((data[16] & 0xFF) + (data[17] & 0xFF) * 0x100 + (data[18] & 0xFF) * 0x10000 + (data[19] & 0xFF) * 0x1000000);
            PhotoCount  = ((data[20] & 0xFF) + (data[21] & 0xFF) * 0x100 + (data[22] & 0xFF) * 0x10000 + (data[23] & 0xFF) * 0x1000000);

            if(n_len>=0x1A) {
                nSDAllSize = ((data[24] & 0xFF) + (data[25] & 0xFF) * 0x100 + (data[26] & 0xFF) * 0x10000 + (data[27] & 0xFF) * 0x1000000);
                nSDAvaildSize = ((data[28] & 0xFF) + (data[29] & 0xFF) * 0x100 + (data[30] & 0xFF) * 0x10000 + (data[31] & 0xFF) * 0x1000000);
            }
            nBattery = data[32] & 0xFF;
            EventBus.getDefault().post("","GP4225_GetStatus");
            // Log.e("TAG","Mode = "+bSD+" bSD = "+bSD+" recording = "+bSDRecording +" Vides = "+VideosCount+" Locks = "+LockedCount+" Photos = "+PhotoCount);
            // Log.e("TAG","SDSize = "+nSDAllSize+" SDAvilidSize = "+nSDAvaildSize);
            return true;
        }
        if (m_cmd == 0x0002)  //GetFileList
        {
            if (s_cmd == 0x0001 || s_cmd == 0x0002 || s_cmd == 0x0003) {  //VideoList   LockFileList  //图片文件
                nStartInx = (data[10] & 0xFF) + (data[11] & 0xFF) * 0x100;
                nEndInx =   (data[12] & 0xFF) + (data[13] & 0xFF) * 0x100;

                GetFiles FF = new GetFiles();

                FF.files = new ArrayList<>();
                for (int ii = 0; ii <= nEndInx - nStartInx; ii++) {

                    inx = 14 + 32 + (ii * 68);
                    nLen = (data[inx]&0xFF)+(data[inx+1]&0xFF)*0x100+(data[inx+2]&0xFF)*0x10000+(data[inx+3]&0xFF)*0x1000000;
                    inx += 4;
                    int da = 0;
                    for (int xx = 0; xx < 32; xx++) {
                        if (data[inx + xx] != 0) {
                            da++;
                        }
                    }
                    sFileName = "";
                    if (da != 0) {
                        System.arraycopy(data, inx, bytes, 0, da);
                        sFileName = new String(bytes,0,da);
                    }
                    MyFile file = new MyFile("", sFileName, nLen);
                    file.nInx1 = nStartInx;
                    file.nInx2 = nEndInx;
                    FF.files.add(file);
                    //EventBus.getDefault().post(file,"GP4225_RevFile");
                }
                EventBus.getDefault().post(FF,"GP4225_RevFiles");
            }
            return true;
        }

        if(m_cmd == 0x0009)  //Delete File
        {
            nStatus = data[10];
            sFileName="";
            if(n_len>64)
            {
                nLen = 0;
                for (int xx = 0; xx < 32; xx++) {
                    if (data[xx + 11 + 32] == 0) {
                        break;
                    } else {
                        nLen++;
                    }
                }
                if (nLen != 0)
                    sFileName = new String(data, 11 + 32, nLen);
            }

            switch(s_cmd)
            {
                case  0x00001:          //delete a file
                    MyFile file = new MyFile("", sFileName, (int)nStatus);
                    EventBus.getDefault().post(file,"GP4225_DeleteFile");
                    break;
                case 0x0002:             //delete all file
                    Integer i = (int)nStatus;
                    EventBus.getDefault().post(i,"GP4225_DeleteAllFile");
                    break;
            }
            return true;
        }
        if(m_cmd == 0x0020)
        {
            boolean  bOK=true;
            switch(s_cmd)
            {
                case 0x0001: //时间
                {
                    byte[] bytes = new byte[n_len];
                    System.arraycopy(data, 10, bytes, 0, n_len);
                    Log.e("同步时间","OK");
                    EventBus.getDefault().post(bytes, "GP4225_GetDeviceDateTime");
                }

                break;
                case 0x0002: //水印开关
                {
                    byte a = data[10];
                    Integer aa = (int) a;
                    Log.e("水印",a+"");
                    EventBus.getDefault().post(aa, "GP4225_GetDeviceOsdStatus");
                }

                break;
                case 0x0003:  //图像翻转
                {
                    byte a = data[10];
                    Integer aa = (int)a;
                    Log.e("图像",a+"");
                    EventBus.getDefault().post(aa, "GP4225_GetDeviceReversaltatus");
                }

                break;
                case 0x0004: //录像分段时间
                {
                    byte a = data[10];
                    Integer aa = (int)a;
                    Log.e("录像时间",a+"");
                    EventBus.getDefault().post(aa, "GP4225_GetDeviceRecordTime");
                }
                break;
                case 0x0005:
                    break;
                case 0x0006:
                    break;
                case 0x0007:
                    break;
                case 0x0008:  //format SD卡
                {
                    byte a = data[10];
                    Integer aa = (int)a;
                    Log.e("Format",a+"");
                    EventBus.getDefault().post(aa, "GP4225_FormatSD_Status");
                }
                break;
                case 0x0009:  //Ver
                {
                    byte[] bytes = new byte[n_len];
                    System.arraycopy(data, 10, bytes, 0, n_len);
                    sVer = new String(bytes);
                    Log.e("ABC",sVer);
                    EventBus.getDefault().post(sVer, "GP4225_GetFireWareVersion");
                }
                break;
                case 0x000A:
                {
                    byte a = data[10];
                    Integer aa = (int)a;
                    EventBus.getDefault().post(aa, "GP4225_Reset_Status");
                }
                break;
                default:
                    bOK = false;
                    break;
            }
            return bOK;
        }

        return  false;

    }


    public class  GetFiles
    {
        public List<MyFile>  files;
    }

    public class MyFile {
        public String sPath;
        public String sFileName;
        public int nLength;
        public int nInx1;
        public int nInx2;

        public MyFile(String sPath_, String sFileName_, int nLength_)
        {
            sPath = sPath_;
            sFileName = sFileName_;
            nLength = nLength_;
            nInx1 = 0;
            nInx2 = 0;
        }

    }

}
