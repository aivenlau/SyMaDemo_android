package com.joyhonest.jh_ui;

import android.util.Log;

import org.simple.eventbus.EventBus;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;
import java.net.SocketException;
import java.net.UnknownHostException;

public class Command {



    public static int nLedMax=40;

    public static String  sIP="";
    public static int  DeviceType = 0;    //0 普通   1 变焦无SD卡   2 变焦+SD
    public static int  nBattery = 4;

    public static int nResolution = -1;
    public static int nWifiChannel = -1;




    public static boolean  isGP4225_H264()
    {
         return  sIP.equalsIgnoreCase("192.168.34.1");
    }


    public static boolean  F_Connected(Socket socket)
    {

        SocketAddress socAddress = new InetSocketAddress(sIP, 8081);
        boolean  bConnected = false;
        InetAddress addr;
        try
        {
            socket.connect(socAddress, 250);
            bConnected = true;
            socket.setSoTimeout(100);
        }
        catch (IOException e)
        {
            bConnected = false;
        }
        if(!bConnected)
        {
            try
            {
                socket.connect(socAddress, 250);
                bConnected = true;
                socket.setSoTimeout(100);
            }
            catch (IOException e)
            {
                bConnected = false;
            }
        }

        if(!bConnected)
        {
            try
            {
                socket.connect(socAddress, 250);
                bConnected = true;
                socket.setSoTimeout(100);
            }
            catch (IOException e)
            {
                bConnected = false;
            }
        }
        return bConnected;
    }



    private  static  void F_GetDeviceInfo_a()
    {
        if(isGP4225_H264())
        {
            DeviceType = 2;
            return;
        }
        Socket socket = new Socket();
        if(!F_Connected(socket))
        {
            return;
        }


        byte  cmd[] = new byte[32];

        cmd[0] = 'G';
        cmd[1] = 'P';
        cmd[2] = 'S';
        cmd[3] = 'O';
        cmd[4] = 'C';
        cmd[5] = 'K';
        cmd[6] = 'E';
        cmd[7] = 'T';

        cmd[8] = (byte)0x01;
        cmd[9] = (byte)0x00;

        cmd[10] = (byte)0x00;
        cmd[11] = (byte)0x01;

        cmd[12] = (byte)0x00;
        cmd[13] = (byte)0x80;

        cmd[14] = (byte)0x1A;
        cmd[15] = (byte)0x32;

        for (int i = 16; i<32; i++) {
            cmd[i] = 0x00;
        }

        try {
            OutputStream outputStream = socket.getOutputStream();
            outputStream.write(cmd,0,32);
            outputStream.flush();
            socket.shutdownOutput();
            F_Delay(50);
            InputStream inputStream = socket.getInputStream();
            byte []buffer = new byte[50];
            int len = inputStream.read(buffer);
            if(len>=17)
            {
                if(buffer[0] == 'G' &&
                        buffer[1] == 'P' &&
                        buffer[2] == 'S' &&
                        buffer[3] == 'O' &&
                        buffer[4] == 'C' &&
                        buffer[5] == 'K' &&
                        buffer[6] == 'E' &&
                        buffer[7] == 'T' )
                {
                    if (buffer[8] == 0x02) { //成功
                        if ((buffer[11] == 0x01) && (buffer[10] == 0x00)) { //获取设备状态信息
                            if(len>=30)
                            {
                                DeviceType = buffer[29];
                            }
                            nBattery =(int)(buffer[16] & 0x7F);

                            if(nBattery>4)
                                nBattery=4;
                            if(nBattery<0)
                                nBattery=0;
                            //MyApp.nBattery =nBattery;
                            EventBus.getDefault().post("","GetDeviceBattery_Type");
                        }
                    }
                }
            }

        }
        catch (IOException e)
        {

        }
        finally {
            try {
                socket.close();
            }
            catch (IOException e)
            {
                ;
            }
        }

    }


    public  static void F_Delay(int ms)
    {
        try
        {
            Thread.sleep(ms);
        }
        catch (InterruptedException e)
        {
            ;
        }
    }


    public static void F_GetAll()
    {
        new Thread(){
            @Override
            public void run() {
                super.run();
                if(Command.isGP4225_H264())
                {
                    Command.F_GetFirmWareVer();
                    DeviceType = 2;
                    F_Delay(25);
                    F_GetLed_a();
                    F_Delay(25);
                    F_GetVcm();
                    F_Delay(25);
                    F_GetResolution_a();
                    F_Delay(25);
                    F_GetWifiChannel_a();
                    EventBus.getDefault().post("","GetAllOK");
                    return;
                }
                //MyApp.bUpdateOK = false;
                F_GetDeviceInfo_a();
                F_Delay(10);
                Command.F_GetFirmWareVer();
                if(Command.nUpgareType==0)
                {
                    F_Delay(20);
                    F_GetLed_a();
                    F_Delay(20);
                    F_GetVcm();
                    F_Delay(20);
                    F_GetResolution_a();
                    F_Delay(20);
                    F_GetWifiChannel_a();
                    EventBus.getDefault().post("","GetAllOK");
                }
            }

        }.start();
    }

    public static void  F_GetDeviceInfo() {
        new Thread(){
            @Override
            public void run() {
                super.run();
                F_GetDeviceInfo_a();

            }
        }.start();
    }

    static InetAddress mAddress;
    static DatagramSocket socket = null;

    private static void Sent_Udp(String sIP,final int nPort,final byte[] sendBuf)
    {
        //初始化socket

        try {
            socket = new DatagramSocket();
        } catch (SocketException e) {
            e.printStackTrace();
        }
        try {
            mAddress = InetAddress.getByName(sIP);
        } catch (UnknownHostException e) {
            e.printStackTrace();
        }
        //创建线程发送信息
        new Thread() {


            public void run() {

                DatagramPacket recvPacket1 = new DatagramPacket(sendBuf, sendBuf.length, mAddress, nPort);
                try {
                    socket.send(recvPacket1);
                    socket.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }.start();

    }




    public static void  F_SetVcm(int nVCM_ ){

        nVCM_<<=4;
        nVCM_ &=0x3FF0;
        nVCM_|=3;
        final  int nVCM =nVCM_;

        if(isGP4225_H264())
        {
            byte []cmd = new byte[12];

            cmd[0] = 'F';
            cmd[1] = 'D';
            cmd[2] = 'W';
            cmd[3] = 'N';

            cmd[4] = 0x20;
            cmd[5] = 0x00;

            cmd[6] = 0x0C;    //cmd
            cmd[7] = 0x00;

            cmd[8] = 0x02;    //len
            cmd[9] = 0x00;

            cmd[10] = (byte)nVCM;           //Data
            cmd[11] = (byte)(nVCM>>8);    //Data
            Sent_Udp(sIP,20001,cmd);
            return;
        }



        new Thread(){
            @Override
            public void run() {
                super.run();
                Socket socket = new Socket();
                if(!F_Connected(socket))
                {
                    return;
                }

                byte  cmd[] = new byte[19];
                cmd[0] = 'G';
                cmd[1] = 'P';
                cmd[2] = 'S';
                cmd[3] = 'O';
                cmd[4] = 'C';
                cmd[5] = 'K';
                cmd[6] = 'E';
                cmd[7] = 'T';

                cmd[8] = 0x01;
                cmd[9] = 0x00;

                cmd[10] = 0x04;
                cmd[11] = 0x01;

                cmd[12] = 0x00;
                cmd[13] = 0x15;     //Read Version

                cmd[14] = 0x00;
                cmd[15] = 0x00;

                cmd[16] = 0x02;


                cmd[17] = (byte)(nVCM>>8);
                cmd[18] = (byte)nVCM;

                try {
                    OutputStream outputStream = socket.getOutputStream();
                    outputStream.write(cmd);
                    outputStream.flush();
                    socket.shutdownOutput();
                    F_Delay(10);
                }
                catch (IOException e)
                {

                }
                finally {
                    try {
                        socket.close();
                    }
                    catch (IOException e)
                    {
                        ;
                    }
                }
            }
        }.start();


    }


    public static void  F_SetLed(final int nLedValue) {
        if(isGP4225_H264())
        {
            byte []cmd = new byte[11];


            cmd[0] = 'F';
            cmd[1] = 'D';
            cmd[2] = 'W';
            cmd[3] = 'N';

            cmd[4] = 0x20;
            cmd[5] = 0x00;

            cmd[6] = 0x0E;    //cmd
            cmd[7] = 0x00;

            cmd[8] = 0x01;    //len
            cmd[9] = 0x00;

            cmd[10] = (byte)nLedValue;           //Data
            Sent_Udp(sIP,20001,cmd);
            F_Delay(20);
            Sent_Udp(sIP,20001,cmd);
            F_Delay(10);
            Sent_Udp(sIP,20001,cmd);
            return;
        }
        new Thread(){
            @Override
            public void run() {
                super.run();
                Socket socket = new Socket();
                if(!F_Connected(socket))
                {
                    return;
                }

                byte  cmd[] = new byte[18];
                cmd[0] = 'G';
                cmd[1] = 'P';
                cmd[2] = 'S';
                cmd[3] = 'O';
                cmd[4] = 'C';
                cmd[5] = 'K';
                cmd[6] = 'E';
                cmd[7] = 'T';

                cmd[8] = 0x01;
                cmd[9] = 0x00;

                cmd[10] = 0x04;
                cmd[11] = 0x01;

                cmd[12] = 0x00;
                cmd[13] = 0x14;

                cmd[14] = 0x00;
                cmd[15] = 0x00;

                cmd[16] = 0x01;
                cmd[17] = (byte)(nLedValue & 0xFF);

                try {
                    OutputStream outputStream = socket.getOutputStream();
                    outputStream.write(cmd);
                    outputStream.flush();
                    socket.shutdownOutput();
                    F_Delay(10);
                }
                catch (IOException e)
                {

                }
                finally {
                    try {
                        socket.close();
                    }
                    catch (IOException e)
                    {
                        ;
                    }
                }
            }
        }.start();

    }


    private  static void F_GetVcm()
    {
        if(isGP4225_H264())
        {
            byte []cmd = new byte[12];

            cmd[0] = 'F';
            cmd[1] = 'D';
            cmd[2] = 'W';
            cmd[3] = 'N';

            cmd[4] = 0x20;
            cmd[5] = 0x00;

            cmd[6] = 0x0B;    //cmd
            cmd[7] = 0x00;

            cmd[8] = 0x00;    //len
            cmd[9] = 0x00;
            Sent_Udp(sIP,20001,cmd);
            return;
        }

        Socket socket = new Socket();
        if(!F_Connected(socket))
        {
            return;
        }

        byte  cmd[] = new byte[19];

        cmd[0] = 'G';
        cmd[1] = 'P';
        cmd[2] = 'S';
        cmd[3] = 'O';
        cmd[4] = 'C';
        cmd[5] = 'K';
        cmd[6] = 'E';
        cmd[7] = 'T';

        cmd[8] = 0x01;
        cmd[9] = 0x00;

        cmd[10] = 0x04;
        cmd[11] = 0x00;

        cmd[12] = 0x00;
        cmd[13] = 0x15;     //Read Version

        cmd[14] = 0x00;
        cmd[15] = 0x00;

        cmd[16] = 0x02;

        cmd[17] = 0x01;
        cmd[18] = 0x00;

        try {
            OutputStream outputStream = socket.getOutputStream();
            outputStream.write(cmd);
            outputStream.flush();
            socket.shutdownOutput();
            F_Delay(80);

            InputStream inputStream = socket.getInputStream();
            byte []buffer = new byte[50];
            int len = inputStream.read(buffer);
            if(len>=15)
            {
                if(buffer[0] == 'G' &&
                        buffer[1] == 'P' &&
                        buffer[2] == 'S' &&
                        buffer[3] == 'O' &&
                        buffer[4] == 'C' &&
                        buffer[5] == 'K' &&
                        buffer[6] == 'E' &&
                        buffer[7] == 'T' )
                {
//                    if (buffer[8] == 0x02) { //成功
//                        if ((buffer[11] == 0x0) && (buffer[10] == 0x04)) {
//                            int n = buffer[14]&0xFF;
//                            if(n>nLedMax)
//                                n=(byte)nLedMax;
//                            Integer aa = n;
//                            EventBus.getDefault().post(aa,"GetLedLevel");
//                        }
//                    }
                        int nVCM = buffer[14]*0x100+buffer[15];
                        Integer  da = (nVCM>>4)&0x3FF;
                    EventBus.getDefault().post(da,"GetVcm");



                }
            }

        }
        catch (IOException e)
        {
            ;

        }
        finally {
            try {
                socket.close();
            }
            catch (IOException e)
            {
                ;
            }
        }

    }


    private  static void F_SentCalibration_Cmd()
    {
        byte []cmd = new byte[11];

        cmd[0] = 'F';
        cmd[1] = 'D';
        cmd[2] = 'W';
        cmd[3] = 'N';

        cmd[4] = 0x20;
        cmd[5] = 0x00;

        cmd[6] = 0x12;    //cmd
        cmd[7] = 0x00;

        cmd[8] = 0x01;    //len
        cmd[9] = 0x00;

        cmd[10] = 0x03;  //bit 0  是否打开Gsensor   bit1 ， 是否需要校准
        Sent_Udp(sIP,20001,cmd);

//        F_Delay(20);
//        Sent_Udp(sIP,20001,cmd);
//        F_Delay(10);
//        Sent_Udp(sIP,20001,cmd);
    }

    private  static void F_GetLed_a()
    {
        if(isGP4225_H264()) {
            byte []cmd = new byte[10];

            cmd[0] = 'F';
            cmd[1] = 'D';
            cmd[2] = 'W';
            cmd[3] = 'N';

            cmd[4] = 0x20;
            cmd[5] = 0x00;

            cmd[6] = 0x0E;    //cmd
            cmd[7] = 0x00;

            cmd[8] = 0x00;    //len
            cmd[9] = 0x00;
            Sent_Udp(sIP,20001,cmd);
            F_Delay(20);
            Sent_Udp(sIP,20001,cmd);
            F_Delay(10);
            Sent_Udp(sIP,20001,cmd);
            return;
        }

        Socket socket = new Socket();
        if(!F_Connected(socket))
        {
            return;
        }

        byte  cmd[] = new byte[32];

        cmd[0] = 'G';
        cmd[1] = 'P';
        cmd[2] = 'S';
        cmd[3] = 'O';
        cmd[4] = 'C';
        cmd[5] = 'K';
        cmd[6] = 'E';
        cmd[7] = 'T';

        cmd[8] = 0x01;
        cmd[9] = 0x00;

        cmd[10] = 0x04;
        cmd[11] = 0x00;

        cmd[12] = 0x00;
        cmd[13] = 0x14;

        cmd[14] = 0x00;
        cmd[15] = 0x00;

        for (int i = 16; i<32; i++) {
            cmd[i] = 0x00;
        }

        try {
            OutputStream outputStream = socket.getOutputStream();
            outputStream.write(cmd);
            outputStream.flush();
            socket.shutdownOutput();
            F_Delay(50);

            InputStream inputStream = socket.getInputStream();
            byte []buffer = new byte[50];
            int len = inputStream.read(buffer);
            if(len>=15)
            {
                if(buffer[0] == 'G' &&
                        buffer[1] == 'P' &&
                        buffer[2] == 'S' &&
                        buffer[3] == 'O' &&
                        buffer[4] == 'C' &&
                        buffer[5] == 'K' &&
                        buffer[6] == 'E' &&
                        buffer[7] == 'T' )
                {
                    if (buffer[8] == 0x02) { //成功
                        if ((buffer[11] == 0x0) && (buffer[10] == 0x04)) {
                            int n = buffer[14]&0xFF;
                            if(n>nLedMax)
                                n=(byte)nLedMax;
                            Integer aa = n;
                            EventBus.getDefault().post(aa,"GetLedLevel");
                        }
                    }
                }
            }

        }
        catch (IOException e)
        {
            ;

        }
        finally {
            try {
                socket.close();
            }
            catch (IOException e)
            {
                ;
            }
        }
    }

    public static void  F_GetLed() {
        if(isGP4225_H264())
        {
                F_Delay(120);
                F_GetLed_a();
                return;
        }
        new Thread(){
            @Override
            public void run() {
                super.run();
                F_Delay(100);
                F_GetLed_a();

            }
        }.start();
    }





    public static void F_SetResolution(final int nResolution_)
    {
        if(isGP4225_H264())
        {
            byte []cmd = new byte[11];

            //Resolution为INT8U，Resolution = 0:VGA,  =1:720P, =2:1080P, =other,reserved

            cmd[0] = 'F';
            cmd[1] = 'D';
            cmd[2] = 'W';
            cmd[3] = 'N';

            cmd[4] = 0x20;
            cmd[5] = 0x00;

            cmd[6] = 0x10;    //cmd
            cmd[7] = 0x00;

            cmd[8] = 0x01;    //len
            cmd[9] = 0x00;

            cmd[10] = (byte)nResolution_;           //Data
            Sent_Udp(sIP,20001,cmd);
            return;
        }
        new Thread(){
            @Override
            public void run() {
                super.run();
                Socket socket = new Socket();
                if(!F_Connected(socket))
                {
                    return;
                }


                byte  cmd[] = new byte[18];

                cmd[0] = 'G';
                cmd[1] = 'P';
                cmd[2] = 'S';
                cmd[3] = 'O';
                cmd[4] = 'C';
                cmd[5] = 'K';
                cmd[6] = 'E';
                cmd[7] = 'T';

                cmd[8] = 0x01;
                cmd[9] = 0x00;

                cmd[10] = 0x04;
                cmd[11] = 0x01;

                cmd[12] = 0x00;
                cmd[13] = 0x11;

                cmd[14] = 0x00;
                cmd[15] = 0x00;

                cmd[16] = 0x01;
                cmd[17] = (byte)(nResolution_ & 0xFF);

                try {
                    OutputStream outputStream = socket.getOutputStream();
                    outputStream.write(cmd);
                    outputStream.flush();
                    socket.shutdownOutput();
                    F_Delay(10);
                    Log.e("Set","Set Resolution");

                }
                catch (IOException e)
                {
                    ;
                }
                finally {
                    try {
                        socket.close();
                    }
                    catch (IOException e)
                    {
                        ;
                    }
                }
            }
        }.start();

    }


    private  static void F_GetResolution_a()
    {
        if(isGP4225_H264())
        {
            byte []cmd = new byte[10];


            cmd[0] = 'F';
            cmd[1] = 'D';
            cmd[2] = 'W';
            cmd[3] = 'N';

            cmd[4] = 0x20;
            cmd[5] = 0x00;

            cmd[6] = 0x10;    //cmd
            cmd[7] = 0x00;

            cmd[8] = 0x00;    //len
            cmd[9] = 0x00;
            Sent_Udp(sIP,20001,cmd);
            return;
        }
        Socket socket = new Socket();
        if(!F_Connected(socket))
        {
            return;
        }


        byte  cmd[] = new byte[32];

        cmd[0] = 'G';
        cmd[1] = 'P';
        cmd[2] = 'S';
        cmd[3] = 'O';
        cmd[4] = 'C';
        cmd[5] = 'K';
        cmd[6] = 'E';
        cmd[7] = 'T';

        cmd[8] = 0x01;
        cmd[9] = 0x00;

        cmd[10] = 0x04;
        cmd[11] = 0x00;

        cmd[12] = 0x00;
        cmd[13] = 0x11;

        cmd[14] = 0x00;
        cmd[15] = 0x00;

        //cmd[16] = 0x01;
        //cmd[17] = 0x01;

        for (int i = 16; i<32; i++) {
            cmd[i] = 0x00;
        }

        try {
            OutputStream outputStream = socket.getOutputStream();
            outputStream.write(cmd);
            outputStream.flush();
            socket.shutdownOutput();
            F_Delay(50);

            InputStream inputStream = socket.getInputStream();
            byte []buffer = new byte[50];
            int len = inputStream.read(buffer);
            if(len>=15)
            {
                if(buffer[0] == 'G' &&
                        buffer[1] == 'P' &&
                        buffer[2] == 'S' &&
                        buffer[3] == 'O' &&
                        buffer[4] == 'C' &&
                        buffer[5] == 'K' &&
                        buffer[6] == 'E' &&
                        buffer[7] == 'T' )
                {
                    if (buffer[8] == 0x02) { //成功
                        if ((buffer[11] == 0x0) && (buffer[10] == 0x04)) {
                            int n = buffer[14]&0xFF;
                            Integer aa = n;
                            nResolution = n;
                            EventBus.getDefault().post(aa,"GetResolution");
                        }
                    }
                }
            }


        }
        catch (IOException e)
        {

        }
        finally {
            try {
                socket.close();
            }
            catch (IOException e)
            {
                ;
            }
        }
    }

    public static void F_GetResolution()
    {
        if(isGP4225_H264())
        {
            F_GetResolution_a();
            return;
        }
        new Thread(){
            @Override
            public void run() {
                super.run();
                F_GetResolution_a();

            }
        }.start();
    }


    public static void F_SetWifiChannel(final  int nChancel)
    {
        if(isGP4225_H264())
        {
            byte []cmd = new byte[11];
            int aa = nChancel;

            if(aa<=10)
                aa+=1;

            cmd[0] = 'F';
            cmd[1] = 'D';
            cmd[2] = 'W';
            cmd[3] = 'N';

            cmd[4] = 0x20;
            cmd[5] = 0x00;

            cmd[6] = 0x07;    //cmd
            cmd[7] = 0x00;

            cmd[8] = 0x01;    //len
            cmd[9] = 0x00;
            cmd[10] = (byte)aa;           //Data
            Sent_Udp(sIP,20001,cmd);
            return;
        }
        new Thread(){
            @Override
            public void run() {
                super.run();
                Socket socket = new Socket();
                if(!F_Connected(socket))
                {
                    return;
                }


                byte  cmd[] = new byte[18];

                cmd[0] = 'G';
                cmd[1] = 'P';
                cmd[2] = 'S';
                cmd[3] = 'O';
                cmd[4] = 'C';
                cmd[5] = 'K';
                cmd[6] = 'E';
                cmd[7] = 'T';

                cmd[8] = 0x01;
                cmd[9] = 0x00;

                cmd[10] = 0x04;
                cmd[11] = 0x01;

                cmd[12] = 0x03;
                cmd[13] = 0x10;

                cmd[14] = 0x00;
                cmd[15] = 0x00;

                cmd[16] = 0x01;
                cmd[17] = (byte)nChancel;

                try {
                    OutputStream outputStream = socket.getOutputStream();
                    outputStream.write(cmd);
                    outputStream.flush();
                    socket.shutdownOutput();
                    F_Delay(10);

                }
                catch (IOException e)
                {

                }
                finally {
                    try {
                        socket.close();
                    }
                    catch (IOException e)
                    {
                        ;
                    }
                }
            }
        }.start();
    }


    private  static void F_GetWifiChannel_a()
    {
        if(isGP4225_H264())
        {
            byte []cmd = new byte[10];

            cmd[0] = 'F';
            cmd[1] = 'D';
            cmd[2] = 'W';
            cmd[3] = 'N';

            cmd[4] = 0x20;
            cmd[5] = 0x00;

            cmd[6] = 0x07;    //cmd
            cmd[7] = 0x00;

            cmd[8] = 0x00;    //len
            cmd[9] = 0x00;
            Sent_Udp(sIP,20001,cmd);
            return;
        }
        Socket socket = new Socket();
        if(!F_Connected(socket))
        {
            return;
        }

        byte  cmd[] = new byte[32];
        nWifiChannel = -1;

        cmd[0] = 'G';
        cmd[1] = 'P';
        cmd[2] = 'S';
        cmd[3] = 'O';
        cmd[4] = 'C';
        cmd[5] = 'K';
        cmd[6] = 'E';
        cmd[7] = 'T';

        cmd[8] = 0x01;
        cmd[9] = 0x00;

        cmd[10] = 0x04;
        cmd[11] = 0x00;

        cmd[12] = 0x03;
        cmd[13] = 0x10;

        cmd[14] = 0x00;
        cmd[15] = 0x00;

        //cmd[16] = 0x01;
        //cmd[17] = 0x01;

        for (int i = 16; i<32; i++) {
            cmd[i] = 0x00;
        }

        try {
            OutputStream outputStream = socket.getOutputStream();
            outputStream.write(cmd);
            outputStream.flush();
            socket.shutdownOutput();
            F_Delay(50);
            InputStream inputStream = socket.getInputStream();
            byte []buffer = new byte[50];
            int len = inputStream.read(buffer);
            if(len>=15)
            {
                if(buffer[0] == 'G' &&
                        buffer[1] == 'P' &&
                        buffer[2] == 'S' &&
                        buffer[3] == 'O' &&
                        buffer[4] == 'C' &&
                        buffer[5] == 'K' &&
                        buffer[6] == 'E' &&
                        buffer[7] == 'T' )
                {
                    if (buffer[8] == 0x02) { //成功
                        if ((buffer[11] == 0x0) && (buffer[10] == 0x04)) {
                            int n = buffer[14]&0xFF;
                            Integer aa = n;
                            if(n==15) {
                                nWifiChannel = 11;
                            }
                            else
                            {
                                nWifiChannel = n;
                            }

                            EventBus.getDefault().post(aa,"GetWifiCannel");
                        }
                    }
                }
            }

        }
        catch (IOException e)
        {
            e.printStackTrace();
        }
        finally {
            try {
                socket.close();
            }
            catch (IOException e)
            {
                e.printStackTrace();
            }
        }
    }

    public static void F_GetWifiChannel()
    {
        if(isGP4225_H264())
        {
            F_GetWifiChannel_a();
            return;
        }
        new Thread(){
            @Override
            public void run() {
                super.run();
                F_GetWifiChannel_a();

            }
        }.start();

    }



    public static void F_SaveSetting()
    {
        new Thread(){
            @Override
            public void run() {
                super.run();
                Socket socket = new Socket();
                if(!F_Connected(socket))
                {
                    return;
                }


                byte  cmd[] = new byte[32];

                cmd[0] = 'G';
                cmd[1] = 'P';
                cmd[2] = 'S';
                cmd[3] = 'O';
                cmd[4] = 'C';
                cmd[5] = 'K';
                cmd[6] = 'E';
                cmd[7] = 'T';

                cmd[8] = 0x01;
                cmd[9] = 0x00;

                cmd[10] = 0x04;
                cmd[11] = 0x01;

                cmd[12] = 0x02;
                cmd[13] = 0x12;     //Save

                cmd[14] = 0x00;
                cmd[15] = 0x00;

                //cmd[16] = 0x01;
                //cmd[17] = 0x01;

                for (int i = 16; i<32; i++) {
                    cmd[i] = 0x00;
                }

                try {
                    OutputStream outputStream = socket.getOutputStream();
                    outputStream.write(cmd);
                    outputStream.flush();
                    F_Delay(10);
                    socket.shutdownOutput();

//                    InputStream inputStream = socket.getInputStream();
//                    byte []buffer = new byte[15];
//                    int len = inputStream.read(buffer);
//                    if(len>=12)
//                    {
//                        if(buffer[0] == 'G' &&
//                                buffer[1] == 'P' &&
//                                buffer[2] == 'S' &&
//                                buffer[3] == 'O' &&
//                                buffer[4] == 'C' &&
//                                buffer[5] == 'K' &&
//                                buffer[6] == 'E' &&
//                                buffer[7] == 'T' )
//                        {
//                            if (buffer[8] == 0x02) { //成功
//                                if ((buffer[11] == 0x1) && (buffer[10] == 0x04)) {
//
//                                    //Log.e("")
//
//                                }
//                            }
//                        }
//                    }

                    Log.e("Set","Saved!!");
                }
                catch (IOException e)
                {

                }
                finally {
                    try {
                        socket.close();
                    }
                    catch (IOException e)
                    {
                        ;
                    }
                }
            }
        }.start();
    }


    private void  GP4225_GetVersion()
    {

    }

    public static int nUpgareType=0; // 0 no   1  en  2 jp

    public  static void F_GetFirmWareVer()
    {
        Log.e("TAG","Get ver1");
        if(isGP4225_H264())
        {
//            if(isGP4225_H264())
//            {
                byte []cmd = new byte[10];

                cmd[0] = 'F';
                cmd[1] = 'D';
                cmd[2] = 'W';
                cmd[3] = 'N';

                cmd[4] = 0x20;
                cmd[5] = 0x00;

                cmd[6] = 0x09;    //cmd
                cmd[7] = 0x00;

                cmd[8] = 0x00;    //len
                cmd[9] = 0x00;
                Sent_Udp(sIP,20001,cmd);  //读取版本
//
//            }

            nUpgareType = 0;
           // MyApp.nUpgrate = nUpgareType;
            return;

        }
        Socket socket = new Socket();
        nUpgareType = 0;

        byte  cmd[] = new byte[32];

        cmd[0] = 'G';
        cmd[1] = 'P';
        cmd[2] = 'S';
        cmd[3] = 'O';
        cmd[4] = 'C';
        cmd[5] = 'K';
        cmd[6] = 'E';
        cmd[7] = 'T';

        cmd[8] = 0x01;
        cmd[9] = 0x00;

        cmd[10] = 0x04;
        cmd[11] = 0x00;

        cmd[12] = 0x01;
        cmd[13] = 0x12;     //Read Version

        cmd[14] = 0x00;
        cmd[15] = 0x00;

        //cmd[16] = 0x01;
        //cmd[17] = 0x01;

        for (int i = 16; i<32; i++) {
            cmd[i] = 0x00;
        }
        nUpgareType = 0;
        //MyApp.nUpgrate = nUpgareType;

        if(!F_Connected(socket))
        {
            return;
        }
        Log.e("TAG","Get ver2");



        try {
            OutputStream outputStream = socket.getOutputStream();
            outputStream.write(cmd);
            outputStream.flush();
            socket.shutdownOutput();
            F_Delay(100);
            InputStream inputStream = socket.getInputStream();
            byte []buffer = new byte[50];
            int len = inputStream.read(buffer);
            if(len>=15)
            {
                if(buffer[0] == 'G' &&
                        buffer[1] == 'P' &&
                        buffer[2] == 'S' &&
                        buffer[3] == 'O' &&
                        buffer[4] == 'C' &&
                        buffer[5] == 'K' &&
                        buffer[6] == 'E' &&
                        buffer[7] == 'T' )
                {

//                    if (buffer[8] == 0x02) { //成功
//                        if ((buffer[11] == 0x0) && (buffer[10] == 0x04)) {
//                            int aa = (int) (buffer[12]+buffer[13]*0x100);
//                            if(aa>0x15)
//                                aa=0x15;
//
//                            SharedPreferences sharedPreferences = MyApp.getInstance().getApplicationContext().getSharedPreferences("Ferret_setup", 0);
//                           int nUpgrade = sharedPreferences.getInt("Upgrade", -1);
//                            nUpgareType = 0;
//
//
//                               String ver = new String(buffer, 14, aa - 1);
//                               String nver = ver.substring(ver.length() - 3, ver.length());
//                               Log.e("TAG","Ver = "+nver);
//
//
//                               try {
//                                   int nVer = Integer.parseInt(nver);
//                                   nUpgareType = 0;
//                                   if (nVer == 9) {  // = 9 jp 旧版   11 jp新版 其他的为en版本
//                                       nUpgareType = 2;
//                                   } else {
//                                       if (nVer < 10)
//                                       {
//                                           nUpgareType = 1;
//                                       }
//                                   }
//                               } catch (NumberFormatException e) {
//                                   ;
//                               }
//                          //  MyApp.nUpgrate = nUpgareType;
//
//                            if(nUpgrade != 1)
//                            {
//
//                               if (nUpgareType != 0)
//                               {
//
//                                   SharedPreferences.Editor editor = sharedPreferences.edit();
//                                   editor.putInt("Upgrade", 1);
//                                   editor.apply();
//                                   Integer nv = nUpgareType;
//                                   EventBus.getDefault().post(nv, "NeedUpgrate");
//                               }
//                           }
//                           else
//                            {
//                                   nUpgareType = 0;
//                            }
//                        }
//                    }
                }
            }

        }
        catch (IOException e)
        {
            e.printStackTrace();
        }
        finally {
            try {
                socket.close();
            }
            catch (IOException e)
            {
                ;
            }
        }
    }






}
