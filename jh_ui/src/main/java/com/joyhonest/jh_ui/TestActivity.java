package com.joyhonest.jh_ui;

import androidx.appcompat.app.AppCompatActivity;

import android.graphics.Bitmap;
import android.os.Bundle;
import android.os.Handler;
import android.os.SystemClock;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import com.joyhonest.wifination.R;
import com.joyhonest.wifination.wifination;

import org.simple.eventbus.EventBus;
import org.simple.eventbus.Subscriber;
import org.tensorflow.Session;

public class TestActivity extends AppCompatActivity implements View.OnClickListener {

    private static String TAG = "TestActive";
    private ImageView DispImageView;
    private Button btn_Play;
    private Button btn_Seek;
    private Button btn_Pause;
    private Button btn_Stop;
    private Button btn_Snap;
    private TextView recTime;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_test);
        wifination.naStop();
        wifination.naSetRevBmp(true);
        JH_App.init(getApplicationContext(),"da","db","dc","dd");

        DispImageView = findViewById(R.id.DispImageView);
        recTime = findViewById(R.id.recTime);
        recTime.setText("00:00");
        btn_Play = findViewById(R.id.btn_Play);
        btn_Seek = findViewById(R.id.btn_Seek);
        btn_Pause = findViewById(R.id.btn_Pause);
        btn_Stop = findViewById(R.id.btn_Stop);
        btn_Snap = findViewById(R.id.btn_Snap);
        btn_Play.setOnClickListener(this);
        btn_Seek.setOnClickListener(this);
        btn_Pause.setOnClickListener(this);
        btn_Stop.setOnClickListener(this);
        btn_Snap.setOnClickListener(this);
        EventBus.getDefault().register(this);
        wifination.naStartRead20000_20001();
        bOpen = true;
        wifination.naStartRead20000_20001();
        JH_App.F_OpenStream();
        checkHandler.post(checkRunnable);

    }

    Handler  recHandler = new Handler();
    Runnable recRunnable = new Runnable() {
        @Override
        public void run() {
            int  r = wifination.naGetRecordTime()/1000;
            int m = r/60;
            int s = r%60;
            recTime.setText(""+m+":"+s);
            recHandler.postDelayed(this,250);
        }
    };


    Handler  checkHandler = new Handler();
    Runnable checkRunnable = new Runnable() {
        @Override
        public void run() {
            nIsReceive++;
            if(nIsReceive>12)
            {
                nIsReceive = 0;
                wifination.naRelinkPlay();
            }
            checkHandler.postDelayed(this,250);
        }
    };

    @Override
    protected void onDestroy() {
        super.onDestroy();
        checkHandler.removeCallbacksAndMessages(null);
        wifination.naStopPlay();
        wifination.naStop();
        wifination.naStopReadUdp();
        EventBus.getDefault().unregister(this);
    }


    @Override
    public void onBackPressed() {
        super.onBackPressed();
        wifination.naStop();
        finish();
    }

    boolean  bOpen = false;

    @Override
    protected void onResume() {
        super.onResume();
        JH_App.checkDeviceHasNavigationBar(this);
    }

    @Override
    public void onClick(View v) {
        int id = v.getId();
        if (id == R.id.btn_Play) {
            //JH_App.F_OpenStream();
            if(!bOpen) {
                bOpen = true;
                wifination.naStartRead20000_20001();
                JH_App.F_OpenStream();
            }
            else
            {
                bOpen = false;
                wifination.naStop();
            }

           // wifination.naPlayFlie("/storage/emulated/0/1/2020-10-21-093452178.mp4");
            //wifination.naPlayFlie("/storage/emulated/0/Sports-Camera/Sports_Camera/2020-10-21-081908092.mp4");
        } else if (id == R.id.btn_Seek) {
//            wifination.naSetLedPWM((byte)10);
//            wifination.naGetLedPWM();       //通过   onGetLed  返回
             //wifination.naGetBattery();    //通过 onGetBattery 返回
//                wifination.naGetWifiSSID(); //通过onGetWiFiSSID 返回
            //wifination.naGetFirmwareVersion();
            //wifination.na4225_ReadStatus();

            String str = JH_App.F_GetSaveName(false);
            //wifination.naSetRecordWH(1920,720);
            wifination.naStartRecord(str, wifination.TYPE_ONLY_PHONE);
            recHandler.removeCallbacksAndMessages(null);
            recHandler.post(recRunnable);

          //  wifination.naSeek(40.0f);
        } else if (id == R.id.btn_Pause) {
            //wifination.naSetRecordWH(640,480);
            wifination.naStopRecord_All();
            recHandler.removeCallbacksAndMessages(null);
          //  wifination.naPause();
        } else if (id == R.id.btn_Stop) {
           // wifination.naStopPlay();
            //onBackPressed();
            F_SetRota();
        }
        else if(id ==  R.id.btn_Snap)
        {
//            String str = JH_App.F_GetSaveName(true);
//            wifination.naSnapPhoto(str,wifination.TYPE_ONLY_PHONE);

//            if(nd == 0) {
//                wifination.naSetRecordWH(640, 640);
//                Log.e(TAG,"480");
//            }
//            else if(nd == 1) {
//                wifination.naSetRecordWH(720, 1280);
//                Log.e(TAG,"720");
//            }
//            else {
//                wifination.naSetRecordWH(1080, 1920);
//                Log.e(TAG,"1080");
//            }
//            nd++;
//            if(nd>2)
//            {
//                nd = 0;
//            }
        }

    }


    int  nd = 0;

    int nRota=0;
    int nRotaArray[]={0,90,180,270};

    private void F_SetRota()
    {
        nRota++;
        if(nRota>3)
            nRota = 0;
        int n = nRotaArray[nRota];
        wifination.naSetCameraDataRota(n);
    }



    int  nIsReceive = 0;

    @Subscriber(tag = "ReviceBMP")
    private void ReviceBMP(Bitmap bmp) {
        nIsReceive = 0;
        DispImageView.setImageBitmap(bmp);
    }

    @Subscriber(tag = "Playduration")
    private void Playduration(Integer i) {
        Log.e(TAG, "duration = " + i);
    }

    @Subscriber(tag = "PlayStatus")
    private void PlayStatus(Integer i) {
        Log.e(TAG, "Status = " + i);
    }

    @Subscriber(tag = "PlayTime")
    private void PlayTime(Integer i) {
        Log.e(TAG, "time = " + i);
    }

    @Subscriber(tag = "onGetLed")
    private  void onGetLed(Integer nLed)
    {
        Log.e(TAG, "LedPWM = " + nLed);
    }
    @Subscriber(tag = "onGetBattery")
    private  void onGetBattery(Integer battery)
    {
        Log.e(TAG, "battery = " + battery);
    }

    @Subscriber(tag = "onGetWiFiSSID")
    private  void onGetWiFiSSID(String sSSid)
    {
        Log.e(TAG, " = " + sSSid);
    }

    @Subscriber(tag = "onGetFirmwareVersion")
    private  void onGetFirmwareVersion(String sVersion)
    {
        Log.e(TAG, " sVersion= " + sVersion);
    }



    byte Toogle_pre = -1;


    @Subscriber(tag = "GP4225_GetKey")
    private void GP4225_GetKey(byte[] data) {
        byte Toggle = data[0];
        byte Key = data[1];
        byte Para1 = data[2];
        byte Para2 = data[3];
        switch (Key) {
            case 1: //按键一次拍照一张
                break;
            case 2: //开始录像/停止录像 (Toggle 方式)
                break;
            case 3://如果已开始录像，不理会，否则开始录像
                break;
            case 4://如果已停止录像，不理会，否则停止录像
                break;
            case 5: {
                if (Para1 == 1) //放大
                {

                } else if (Para1 == 2) //缩小
                {

                } else if (Para1 == 3)  //无极缩放
                {
                    //Para2 为缩放级数
                }
            }
            break;
            case 6: {
                if (Para1 == 1)//left
                {

                } else if (Para1 == 2)//right
                {

                } else if (Para1 == 3)//up
                {

                } else if (Para1 == 4)//down
                {

                }
            }
            break;
        }

    }


}