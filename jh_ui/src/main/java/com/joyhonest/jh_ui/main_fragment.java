package com.joyhonest.jh_ui;


//import android.app.Fragment;

import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.os.Handler;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.constraintlayout.widget.ConstraintSet;
import androidx.fragment.app.Fragment;

import android.os.SystemClock;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Surface;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import com.joyhonest.wifination.BuildConfig;
import com.joyhonest.wifination.PictureFromVideo_Interface;
import com.joyhonest.wifination.R;
import com.joyhonest.wifination.wifination;

import org.simple.eventbus.EventBus;
import org.simple.eventbus.Subscriber;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;
import java.util.TimeZone;


/**
 * A simple {@link Fragment} subclass.
 */
public class main_fragment extends Fragment implements View.OnClickListener {



    //private final  boolean  D_Debug = true;

    //private Button  But_Power;
    //private Button  But_Adj;
    //private Button  But_Speed;

    private Button But_Floder;

    //private Button  But_Gravity;
    //private Button  But_HeadLess;
    //private Button  But_Path;

    private Button But_Back;

    private Button But_Show_Hide;

    private Button But_BrowSD;
    private Button But_BrowPhone;


    private Button But_Snap;
    private Button But_Record;

    private TextView RecordTime_textView;


    private Button But_Power;
    public MyControl myControl;

    private Button But_KeyUp;
    private Button But_KeyDn;
    private Button But_KeyStop;
    private Button But_Speed;

    private Button But_Adj;

    private Button But_Sensor;
    private Button But_HeadLess;
    private Button But_Path;

    private ConstraintLayout constraintLayout2;


    private boolean bSensor = false;
    private boolean bAdj = false;
    private boolean bUp = false;
    private boolean bDn = false;
    private boolean bStop = false;
    private boolean bHiSpeed = false;
    private boolean bHeadLess = false;

    public ImageView imageViewRssi;

    // public  boolean    bRun=false;


    private final static int _360P = 0;
    private final static int _720P_oldCtron = 1;
    private final static int _720P_newCtron = 2;
    int da = 0x10;  //0x80-0x10.
    float daf = 0.1f;// (float)da;

    int da1 = 0x20; // 0x80-0x20
    float daf1 = 0.1f;// (float)da;

    //  private  Button button7;

    private TextView infp_TestView;

    protected boolean bRecordding = false;

    private TextView snapshot;

    private boolean bChangedHide = false;
    private ConstraintSet mConstraintSet1, mConstraintSet2, mConstraintReset;
    private ConstraintLayout constraintLayout;
    private int nIsOldFlyControl = _360P;

    public main_fragment() {
        // Required empty public constructor
    }


    private boolean bFlib = false;
    private boolean bVr = false;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View view = inflater.inflate(R.layout.fragment_main_fragment_jh, container, false);


        infp_TestView = (TextView) view.findViewById(R.id.infp_TestView);
        infp_TestView.setVisibility(View.VISIBLE);

        snapshot = (TextView) view.findViewById(R.id.snapshot);
        snapshot.setVisibility(View.INVISIBLE);
        // bRun = false;

        daf = 0x80 / ((float) (0x80 - da));
        daf1 = 0x80 / ((float) (0x80 - da1));


        imageViewRssi = (ImageView) view.findViewById(R.id.imageView);

        mConstraintSet1 = new ConstraintSet();
        mConstraintSet2 = new ConstraintSet();
        constraintLayout = (ConstraintLayout) view.findViewById(R.id.constraint_layout);


        But_HeadLess = (Button) view.findViewById(R.id.button6);


        RecordTime_textView = (TextView) view.findViewById(R.id.RecordTime_textView);


        constraintLayout2 = (ConstraintLayout) view.findViewById(R.id.constraintLayout2);

        But_Power = (Button) view.findViewById(R.id.button01);
        But_Adj = (Button) view.findViewById(R.id.button2);
        But_Speed = (Button) view.findViewById(R.id.button3);

        But_Sensor = (Button) view.findViewById(R.id.button5);


        But_KeyUp = (Button) view.findViewById(R.id.button19);
        But_KeyDn = (Button) view.findViewById(R.id.button20);
        But_KeyStop = (Button) view.findViewById(R.id.button18);

        myControl = (MyControl) view.findViewById(R.id.myControl);

        But_Snap = (Button) view.findViewById(R.id.buttonSNap);
        But_Record = (Button) view.findViewById(R.id.buttonRecord);

        But_Show_Hide = (Button) view.findViewById(R.id.button17);
        But_Back = (Button) view.findViewById(R.id.button8);
        But_Floder = (Button) view.findViewById(R.id.button4);

        But_BrowPhone = (Button) view.findViewById(R.id.button15);
        But_BrowSD = (Button) view.findViewById(R.id.button16);

        But_Path = (Button) view.findViewById(R.id.button7);

        But_BrowSD.setVisibility(View.INVISIBLE);


        But_Path.setOnClickListener(this);
        But_HeadLess.setOnClickListener(this);
        But_Sensor.setOnClickListener(this);


        But_Adj.setOnClickListener(this);
        But_Power.setOnClickListener(this);
        But_KeyUp.setOnClickListener(this);
        But_KeyDn.setOnClickListener(this);
        But_KeyStop.setOnClickListener(this);
        But_Speed.setOnClickListener(this);


        But_BrowPhone.setOnClickListener(this);
        But_BrowSD.setOnClickListener(this);

        But_Show_Hide.setOnClickListener(this);
        But_Back.setOnClickListener(this);
        But_Floder.setOnClickListener(this);

        But_Snap.setOnClickListener(this);
        But_Record.setOnClickListener(this);
        RecordTime_textView.setText("00:00");
        RecordTime_textView.setVisibility(View.INVISIBLE);


        constraintLayout2.setVisibility(View.INVISIBLE);
        myControl.setVisibility(View.INVISIBLE);


        mConstraintSet1.clone(constraintLayout);
        mConstraintSet2.clone(constraintLayout);


        But_KeyUp.setVisibility(View.INVISIBLE);
        But_KeyDn.setVisibility(View.INVISIBLE);

        JH_App.F_ReadSaveSetting(false);
        myControl.F_SetRotateAdj(JH_App.nAdjRota);
        myControl.F_SetForwardBackAdj(JH_App.nAdjForwardBack);
        myControl.F_SetLeftRightAdj(JH_App.nAdjLeftRight);


        if (JH_App.bInitDispCtrol) {
            new Handler().postDelayed(new Runnable() {
                @Override
                public void run() {
                    F_Power(true);
                    But_Show_Hide_Click();
                    F_DispSpeed();
                    System.gc();
                }
            }, 50);
        } else {
            new Handler().postDelayed(new Runnable() {
                @Override
                public void run() {
                    F_Power(false);
                    But_Show_Hide_Click();
                    F_DispSpeed();
                    System.gc();
                }
            }, 50);
        }

        view.findViewById(R.id.buttonA).setVisibility(View.INVISIBLE);
        view.findViewById(R.id.buttonA).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                bFlib = !bFlib;
                wifination.naSetFlip(bFlib);
                wifination.F_AdjBackGround(getActivity(), R.mipmap.loginbackground_jh);
            }
        });
        view.findViewById(R.id.buttonB).setVisibility(View.INVISIBLE);
        view.findViewById(R.id.buttonB).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                bVr = !bVr;
                wifination.naSet3D(bVr);
                wifination.F_AdjBackGround(getActivity(), R.mipmap.loginbackground_jh);
            }
        });
        F_DispInfo("on");

        //wifination.naSet3DDenoiser(false);

        return view;

    }


    public void F_GetSYMA_X5UW() {
        String str = wifination.naGetControlType();
        if (str.equalsIgnoreCase("SYMA_X5UW")) {
            nIsOldFlyControl = _720P_oldCtron;
            wifination.naGkASetRecordResolution(true);
        }
        else if (str.equalsIgnoreCase("X5UW2")) {
            nIsOldFlyControl = _720P_newCtron;
            wifination.naGkASetRecordResolution(true);
        } else {
            nIsOldFlyControl = _360P;
            wifination.naGkASetRecordResolution(false);
        }
        if (nIsOldFlyControl == _720P_oldCtron) {
            But_KeyUp.setVisibility(View.INVISIBLE);
            But_KeyDn.setVisibility(View.VISIBLE);
            But_KeyDn.setBackgroundResource(R.mipmap.takeoff_a_jh);
        } else {
            But_KeyUp.setVisibility(View.VISIBLE);
            But_KeyDn.setVisibility(View.VISIBLE);
            But_KeyDn.setBackgroundResource(R.mipmap.landing_nor_jh);

        }
    }

    private void F_DispSpeed() {
        if (bHiSpeed) {
            But_Speed.setBackgroundResource(R.mipmap.btnhighspeed_jh);

        } else {
            But_Speed.setBackgroundResource(R.mipmap.btnlowspeed_jh);
        }
    }

    private Handler sentHander = new Handler();
    private Runnable sentRunnable = new Runnable() {
        @Override
        public void run() {
            F_SentCmd();
            sentHander.postDelayed(this, 16);
        }
    };

    byte[] cmd = new byte[20];


    public void F_DispInfo(String str) {
        if (infp_TestView != null) {
            infp_TestView.setText(str);
        }
    }


    private void F_SentCmd()        //司马demo  协议
    {

        if (myControl.getVisibility() != View.VISIBLE)
            return;
        if (JH_App.bisPathMode)
            return;


        //if(!bRun)
        //    return;

        int i = 0;
        int X1, Y1, X2, Y2, X_ADJ2, Y_ADJ2, X_ADJ1;
        int X2_bak;
        int Y2_bak;

        X1 = myControl.F_GetRotate();
        Y1 = myControl.F_GetThrottle();
        X2 = myControl.F_GetLeftRight();
        Y2 = myControl.F_GetForwardBack();
        X_ADJ1 = myControl.F_GetRotateAdj();
        X_ADJ2 = myControl.F_GetLeftRightAdj();
        Y_ADJ2 = myControl.F_GetForwardBackAdj();


        F_SetSensor();


        //if (JH_App.nICType == wifination.IC_GKA)     // Sima
        if (JH_App.bIsSyMa) {

            if (X2 > 0x70 && X2 < 0x90) {
                X2 = 0x80;
            }

            if (Y2 > 0x70 && Y2 < 0x90) {
                Y2 = 0x80;
            }

            if (X1 > (0x80 - 0x25) && X1 < (0x80 + 0x25)) {
                X1 = 0x80;
            }
            i = 0;


            if (Y2 > 0x80) {
                Y2 -= 0x80;
            } else if (Y2 < 0x80) {
                Y2 = 0x80 - Y2;
                Y2 += 0x80;
                if (Y2 > 0xFF) {
                    Y2 = 0xFF;
                }
            }

            if (X1 > 0x80) {

            } else if (X1 < 0x80) {
                X1 = 0x80 - X1;
                if (X1 > 0x7F) {
                    X1 = 0x7F;
                }
            }

            if (X2 > 0x80) {
            } else if (X2 < 0x80) {
                X2 = 0x80 - X2;
                if (X2 > 0x7F) {
                    X2 = 0x7F;
                }
            }


            cmd[0] = (byte) Y1;   //油门
            cmd[1] = (byte) Y2;
            cmd[2] = (byte) X1;
            cmd[3] = (byte) X2;

            cmd[4] = 0x20;          //油门微调  这里没有。

            int da = Y_ADJ2 - 0x80;
            if (da < 0)               // 后调
            {
                da = 0 - da;
                da += 0x20;
                if (da > 0x3F) {
                    da = 0x3F;
                }
            } else if (da > 0) {
                if (da > 0x1F)
                    da = 0x1F;
            } else {
                da = 0x00;
            }


            cmd[5] = (byte) da;       //前后微调
            if (bHiSpeed)
                cmd[5] |= 0x80;          //高速模式

            cmd[5] |= 0x40;


            da = X_ADJ1 - 0x80;          //旋转微调
            if (da < 0) {
                da = 0 - da;
                if (da > 0x1F) {
                    da = 0x1F;
                }
            } else if (da > 0) {
                da += 0x20;
                if (da > 0x3F)
                    da = 0x3F;
            } else {
                da = 0x20;
            }


            cmd[6] = (byte) da;


            da = X_ADJ2 - 0x80;
            if (da < 0) {

                da = 0 - da;
                if (da > 0x1F) {
                    da = 0x1F;
                }
            } else if (da > 0) {
                da += 0x20;
                if (da > 0x3F)
                    da = 0x3F;
            } else {
                da = 0x20;
            }

            cmd[7] = (byte) da;        //平移

            if (bUp) {
                cmd[7] |= 0x40;
            }
            if (bHeadLess) {
                cmd[7] |= 0x80;
            }

            cmd[8] = 0;

            if (bStop) {
                cmd[8] |= 0x10;
            }
            if (bAdj) {
                cmd[8] |= 0x20;
            }
            if (bDn) {
                cmd[8] |= 0x08;
            }
            cmd[9] = (byte) (((cmd[0] ^ cmd[1] ^ cmd[2] ^ cmd[3] ^ cmd[4] ^ cmd[5] ^ cmd[6] ^ cmd[7] ^ cmd[8]) & 0xFF) + 0x55);
            wifination.naSentCmd(cmd, 10);
            //   Log.e("Cmd:  ","Sent NromalComd  X1=" +X1+" Y1="+Y1+" X2="+X2+" Y2="+Y2);

        } else {


            int flag = 0;
            int flag2 = 0;
            int flag3 = 0;


            if (bUp) {
                flag |= 0x01;
            }
            if (bDn) {
                flag |= 0x02;
            }

        /*
        if(bAutoReturn)
        {
            flag |= 0x04;
        }
        */


            if (bAdj)            //平衡校正
            {
                flag |= 0x08;
            }

            if (bHeadLess) {
                flag |= 0x10;
            }
        /*
        if (bRoll)
        {
            flag |= 0x20;          // 翻滚
        }
        */

            //if (bHoldHight)              // 定高
            {
                flag |= 0x40;
            }

            if (bStop) {          // 紧急停止
                flag |= 0x80;
            }


            if (bHiSpeed) {
                flag2 = 0x00;           // 100

            } else {
                flag2 = 0x02;            //30
            }

            {
                //    flag2 = 0x01;           //60
            }

        /*
        if(bRoll)
        {
            flag2 = 0x00;
        }
        */

        /*
        if(bAdjLR_add)    //副翼+
        {
            flag2 |=0x04;
            //Log.e("Adj","LR++");
        }
        if(bAdjLR_sub)
        {
            flag2 |=0x08;
            Log.e("Adj","LR--");
        }

        if(bAdjFB_add)      //前后 +
        {
            flag2 |=0x10;

            Log.e("Adj","FB++ %d ="+Y_ADJ2);
        }
        if(bAdjFB_sub)      //前后 -
        {
            flag2 |=0x20;
            Log.e("Adj","FB-- %d ="+Y_ADJ2);
        }
        if(bAdjRol_add)      //旋转 +
        {
            flag2 |=0x40;
            Log.e("Adj","Rol++");
        }
        if(bAdjRol_sub)      //旋转 -
        {
            flag2 |=0x80;
            Log.e("Adj","Rol--");
        }
*/

            if (X2 >= 0x80 - da && X2 <= 0x80 + da) {
                X2 = 0x80;
            } else {
                if (X2 < 0x80 - da) {
                    X2 = (int) (daf * X2);   //0x70 = 112
                }
                if (X2 > 0x80 + da) {
                    X2 -= (0x80 + da);
                    X2 = (int) (daf * X2);
                    X2 += 0x80;
                }
            }
            if (X2 > 0xFF)
                X2 = 0xFF;


            if (Y2 > 0x80 - da && Y2 < 0x80 + da) {
                Y2 = 0x80;
            } else {
                if (Y2 < 0x80 - da) {
                    Y2 = (int) (daf * Y2);   //0x70 = 112
                }
                if (Y2 > 0x80 + da) {
                    Y2 -= (0x80 + da);
                    Y2 = (int) (daf * Y2);
                    Y2 += 0x80;
                }
            }
            if (Y2 > 0xFF) {
                Y2 = 0xFF;
            }

            if (X1 > (0x80 - da1) && X1 < (0x80 + da1)) {
                X1 = 0x80;
            } else {
                if (X1 <= 0x80 - da1) {
                    X1 = (int) (daf1 * X1);   //0x70 = 112
                }
                if (X1 >= 0x80 + da1) {
                    X1 -= 0x90;
                    X1 = (int) (daf1 * X1);
                    X1 += 0x80;
                }
            }
            if (X1 > 0xFF) {
                X1 = 0xFF;
            }


            cmd[0] = 0x66;           //0
            cmd[1] = (byte) X2;
            cmd[2] = (byte) Y2;
            cmd[3] = (byte) Y1;
            cmd[4] = (byte) X1;
            cmd[5] = (byte) flag;     //5
            cmd[6] = (byte) flag2;
            cmd[7] = (byte) flag3;
            cmd[8] = (byte) X_ADJ2;
            cmd[9] = (byte) Y_ADJ2;
            cmd[10] = (byte) X_ADJ1;
            wifination.naSentCmd(cmd, 11);
        }
    }


    void F_SetSensor() {
        if (bSensor) {
            int org = getActivity().getWindowManager().getDefaultDisplay().getRotation();

            if (Surface.ROTATION_270 == org) {
                getActivity().setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_REVERSE_LANDSCAPE);

            } else {
                getActivity().setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
            }
        } else {
            if (myControl.F_GetThrottle() != 0x80 ||
                    myControl.F_GetRotate() != 0x80 ||
                    myControl.F_GetForwardBack() != 0x80 ||
                    myControl.F_GetLeftRight() != 0x80) {
                int org = getActivity().getWindowManager().getDefaultDisplay().getRotation();

                if (Surface.ROTATION_270 == org) {
                    getActivity().setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_REVERSE_LANDSCAPE);
                } else {
                    getActivity().setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
                }
            } else {
                getActivity().setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_SENSOR_LANDSCAPE);
            }
        }
    }


    byte NMODE_4225 = 0;

    void F_Power(boolean b) {
        if (b) {

            bSensor = false;
            bAdj = false;
            bHeadLess = false;
            But_HeadLess.setBackgroundResource(R.mipmap.nohead_nor_jh);
            But_Sensor.setBackgroundResource(R.mipmap.gravity_nor_jh);
            But_Adj.setBackgroundResource(R.mipmap.trest_nor_jh);
            myControl.F_SetMode(0);
            myControl.setVisibility(View.VISIBLE);
            constraintLayout2.setVisibility(View.VISIBLE);
            sentHander.removeCallbacksAndMessages(null);
            sentHander.post(sentRunnable);

        } else {
            bSensor = false;
            bAdj = false;
            bHeadLess = false;
            But_HeadLess.setBackgroundResource(R.mipmap.nohead_nor_jh);
            But_Sensor.setBackgroundResource(R.mipmap.gravity_nor_jh);
            But_Adj.setBackgroundResource(R.mipmap.trest_nor_jh);
            myControl.F_SetMode(0);
            myControl.setVisibility(View.INVISIBLE);
            constraintLayout2.setVisibility(View.INVISIBLE);
            sentHander.removeCallbacksAndMessages(null);

        }
        F_SetSensor();
        mConstraintSet1.clone(constraintLayout);
        mConstraintSet2.clone(constraintLayout);

    }

    boolean bChangeResolution=false;

    String sRecordFileName = "";
    int   nType = 0;
    boolean  bOsd=false;
    int nRecT = 0;
    boolean  bGesture = false;
    boolean  bSDRec = false;
    // 按键
    boolean  bTest = false;

    private int nRota = 0;
    private  int nRotaArray[]={0,90,180,270};

    int nInx = 0;
    @Override
    public void onClick(View view) {
        if (view == But_Path) {
            if(BuildConfig.D_Debug)
            {
//                nRota++;
//                if(nRota>3)
//                    nRota=0;
//                int n = nRotaArray[nRota];
//                wifination.naSetRotate(n);
                nInx = 0;




            }
            else {
                EventBus.getDefault().post("abc", "GotoPath");
            }

        }

        if (view == But_HeadLess) {
            if(BuildConfig.D_Debug)
            {
                byte []dd = new byte[7];
                // 1、取得本地时间：
                Calendar cal = Calendar.getInstance() ;
                // 2、取得时间偏移量：
                int zoneOffset = cal.get(java.util.Calendar.ZONE_OFFSET);
                // 3、取得夏令时差：
                int dstOffset = cal.get(java.util.Calendar.DST_OFFSET);
                // 4、从本地时间里扣除这些差量，即可以取得UTC时间：
                cal.add(java.util.Calendar.MILLISECOND, -(zoneOffset + dstOffset));
                int year = cal.get(Calendar.YEAR);
                int month = cal.get(Calendar.MONTH)+1;
                int day = cal.get(Calendar.DAY_OF_MONTH);
                int hour = cal.get(Calendar.HOUR_OF_DAY);
                int minute = cal.get(Calendar.MINUTE);
                int sec = cal.get(Calendar.SECOND);
                TimeZone tz = TimeZone.getDefault();
                int  _t =tz.getOffset(System.currentTimeMillis())/(3600*1000);
                dd[0] = (byte)(year-2000);
                dd[1] = (byte)month;
                dd[2] = (byte)day;
                dd[3] = (byte)hour;
                dd[4] = (byte)minute;
                dd[5] = (byte)sec;
                dd[6] = (byte)_t;
                wifination.na4225_SyncTime(dd,7);
               // wifination.na4225_ReadStatus();
                //wifination.na4225_FormatSD();
            }
            else {
                bHeadLess = !bHeadLess;
                if (bHeadLess) {
                    But_HeadLess.setBackgroundResource(R.mipmap.nohead_sel_jh);
                } else {
                    But_HeadLess.setBackgroundResource(R.mipmap.nohead_nor_jh);
                }
            }

        }
        if (view == But_Power) {

            //F_NeedReadCmd();
//            if(BuildConfig.D_Debug)
//            {
//                 wifination.na4225_ReadStatus();
//
//            }
//            else
                {
                    if (myControl.getVisibility() == View.VISIBLE) {
                        F_Power(false);
                    } else {
                        F_Power(true);
                    }
            }

        }

        if (view == But_Adj) {

//            if(nType==0)
//            {
//                nType = 1;
//                Command.F_SetResolution(0);
//
//                Log.e("RRR","360P");
//            }
//            else
//            {
//                nType = 0;
//                Command.F_SetResolution(1);
//                Log.e("RRR","720P");
//
//            }
            if(BuildConfig.D_Debug)
            {

                if(nType ==0) {
                    nType = 1;
                   // wifination.naReadStatusLedDisp();
                    wifination.naSetStatusLedDisp(true);
                }
                else {
                    //wifination.na4225_ReadDeviceInfo();
                    wifination.naSetStatusLedDisp(false);
                    nType = 0;
                }
                Log.e("AAAA","mode = "+nType);

                //wifination.na4225_SetMode((byte)nType);
//                if(nType==0)
//                    wifination.naInit("1");

            }
            else {

                myControl.F_SetRotateAdj(0x80);
                myControl.F_SetForwardBackAdj(0x80);
                myControl.F_SetLeftRightAdj(0x80);

                JH_App.nAdjRota = 0x80;
                JH_App.nAdjForwardBack = 0x80;
                JH_App.nAdjLeftRight = 0x80;
                JH_App.F_ReadSaveSetting(true);


                bAdj = true;
                But_Adj.setBackgroundResource(R.mipmap.trest_sel_jh);
                new Handler().postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        bAdj = false;
                        But_Adj.setBackgroundResource(R.mipmap.trest_nor_jh);
                    }
                }, 1000);
            }
        }

        if (view == But_KeyStop) {
            if(BuildConfig.D_Debug)
            {
                // wifination.na4225_GetFileList(wifination.GP4225_Type_Video,1,20);
                wifination.naPause();
            }
            else {
                bStop = true;
                bUp = false;
                bDn = false;
                But_KeyStop.setBackgroundResource(R.mipmap.stop_sel_jh);
                new Handler().postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        bStop = false;
                        But_KeyStop.setBackgroundResource(R.mipmap.stop_nor_jh);
                    }
                }, 500);
            }

        }

        if (view == But_KeyUp) {
            if(BuildConfig.D_Debug)
            {

                wifination.na4225_SetMode((byte)1);
                SystemClock.sleep(1000*500);

                wifination.na4225StartDonwLoad("","MOVI0013.mov",7381504,"/storage/emulated/0/MOVI_ABCmov");

                //wifination.naDisConnectedTCP();

               // wifination.F_Convert("/storage/emulated/0/1/MOVI0209.mov","/storage/emulated/0/1/MOVI0209_aaa.mp4");

//                wifination.naStop();
//                startActivity(new Intent(this.getActivity(), TestActivity.class));




                //wifination.naSeek(18.0f);


                //wifination.na4225_GetFileList(1,1,10);
                //"192.168.33.1",30000,
                //String str = Storage.getNormalSDCardPath();


//                //wifination.na4225StartDonwLoad("","MOVI0264.AVI",73302016,"/storage/emulated/0/MOVI_ABC.AVI");
//                Bitmap bitmap = wifination.naGetVideoThumbnail("/storage/emulated/0/1/MOVI0763.avi");
//                Drawable drawable = new BitmapDrawable(this.getResources(),bitmap);
//                But_KeyUp.setBackground(drawable);

//                byte[]data = new byte[10];
//                data[0]='J';
//                data[1]='H';
//                data[2]='C';
//                data[3]='M';
//                data[4]='D';
//                data[5]=0x20;
//                data[6]=0x01;
//                data[7]=0x00;
//
//                wifination.naSentUdpData("192.168.29.1",20000,data,8);

//                wifination.naSentUdpData("192.168.29.1",20000,data,7);
//                wifination.naSentUdpData("192.168.29.1",20000,data,7);
//                wifination.naSentUdpData("192.168.29.1",20000,data,7);
//                wifination.naSentUdpData("192.168.29.1",20000,data,7);

            }
            else {
                if (nIsOldFlyControl == _720P_oldCtron) {
                    bUp = !bUp;
                    bDn = false;
                    But_KeyUp.setBackgroundResource(R.mipmap.takeoff_sel_jh);
                    new Handler().postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            But_KeyUp.setBackgroundResource(R.mipmap.takeoff_nor_jh);
                        }
                    }, 500);
                } else {
                    bUp = true;
                    bDn = false;
                    But_KeyUp.setBackgroundResource(R.mipmap.takeoff_sel_jh);
                    new Handler().postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            bUp = false;
                            bDn = false;
                            But_KeyUp.setBackgroundResource(R.mipmap.takeoff_nor_jh);
                        }
                    }, 500);
                }
            }
        }
        if (view == But_KeyDn) {
            if(BuildConfig.D_Debug)
            {

                String ss = Storage.getNormalSDCardPath();
                //
                wifination.na4225_SetMode((byte) 1);
                SystemClock.sleep(500);
                wifination.naStartRead20000_20001();

                //MOVI0010-1633280.mov

                wifination.na4225StartPlay("","MOVI0010.mov",1633280);
                //wifination.naPlayFlie("/storage/emulated/0/1/MOVI0209_aaa.mp4");



                //wifination.na4225StartPlay("192.168.33.1",30000,"","MOVI0140.AVI",38043648);
                //wifination.na4225StartPlay("192.168.33.1",30000,"","MOVI0136.AVI",44695552);
                //wifination.na4225StartPlay("192.168.33.1",30000,"","MOVI0133.AVI",65667072);
                //wifination.na4225StartPlay("192.168.33.1",30000,"","MOVI0010.AVI",9535488);
                //wifination.na4225StartPlay("192.168.33.1",30000,"","MOVI0007.AVI",18808832);
                //wifination.na4225StartPlay("192.168.33.1",30000,"","MOVI0008.AVI",19136512);
            }
            else {
                if (nIsOldFlyControl == _720P_oldCtron) {
                    bUp = !bUp;
                    bDn = false;
                    But_KeyDn.setBackgroundResource(R.mipmap.takeoff_a_sel_jh);
                    new Handler().postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            //bDn = false;
                            //But_KeyDn.setBackgroundResource(R.mipmap.landing_nor_jh);
                            But_KeyDn.setBackgroundResource(R.mipmap.takeoff_a_jh);
                        }
                    }, 500);
                } else {
                    bUp = false;
                    bDn = true;
                    But_KeyDn.setBackgroundResource(R.mipmap.landing_sel_jh);
                    new Handler().postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            bDn = false;
                            But_KeyDn.setBackgroundResource(R.mipmap.landing_nor_jh);
                        }
                    }, 500);
                }
            }
        }

        if (view == But_Sensor) {
            if(BuildConfig.D_Debug)
            {
                //wifination.naStopPlay();
                wifination.naDisConnectedTCP();
                //wifination.naSnapPhoto("",wifination.TYPE_ONLY_SD);
            }else
            {
                int Sensor_img = R.mipmap.gravity_sel_jh;
                bSensor = !bSensor;
                if (!bSensor) {
                    Sensor_img = R.mipmap.gravity_nor_jh;
                    myControl.F_SetMode(0);
                } else {
                    myControl.F_SetMode(1);
                }
                But_Sensor.setBackgroundResource(Sensor_img);
                F_SetSensor();
            }
        }

        if (view == But_Speed) {
            if(BuildConfig.D_Debug)
            {
                bRe = !bRe;
                if(bRe)
                {
                    wifination.naStartRecord("",wifination.TYPE_ONLY_SD);
                }
                else
                {
                    wifination.naStopRecord(wifination.TYPE_ONLY_SD);
                }
                return;
            }

            bHiSpeed = !bHiSpeed;
            JH_App.bHiSpeed = bHiSpeed;
            F_DispSpeed();
        }

        if (view == But_Floder) {
            But_Floder_Click();
        }
        if (view == But_Show_Hide) {
            But_Show_Hide_Click();
        }
        if (view == But_Back) {
            But_Back_Click();
        }
        if (view == But_BrowSD) {
            JH_App.bBrowSD = true;
            F_GotoSelectVideo_Photo();
        }
        if (view == But_BrowPhone) {
            JH_App.bBrowSD = false;
            F_GotoSelectVideo_Photo();
        }
        if (view == But_Snap) {
//            bChangeResolution =!bChangeResolution;
//            Log.e("ABC_","b = "+bChangeResolution);
//            EventBus.getDefault().post("s", "StartGame");
//            if ((JH_App.nSdStatus & JH_App.SD_SNAP) != 0) {
//                return;
//            }
//            if (JH_App.bPhone_SNAP) {
//                return;
//            }
//            JH_App.bPhone_SNAP = true;
            final String str = JH_App.F_GetSaveName(true);
//
//            wifination.naSetSaveTypeByName(true);
//
            wifination.naSnapPhoto(str, wifination.TYPE_ONLY_PHONE);
//            Handler handler = new Handler();
//            Runnable runnable = new Runnable() {
//                @Override
//                public void run() {
//                    JH_App.bPhone_SNAP = false;
//                }
//            };
//            handler.postDelayed(runnable, 1000);
        }
        if (view == But_Record) {
            if ((JH_App.nSdStatus & JH_App.Status_Connected) == 0) {
                Log.e("Error:", "Not DispVideo!");
                return;
            }
            if ((JH_App.nSdStatus & JH_App.LocalRecording) != 0) {
                // long nD=System.currentTimeMillis() / 1000;
                int recrodT = wifination.naGetRecordTime();
                //if(JH_App.nRecTime<3)  //1.5sec
//                if (recrodT < 1200)  //1.5sec
//                {
//                    return;
//                }


                //wifination.naStopRecord_All(); //aivenlau
                wifination.naStopRecord(wifination.TYPE_ONLY_PHONE);

                if (sRecordFileName.length() > 10) {
                    sRecordFileName = "";
                }
                //JH_App.nRecTime=0;
            } else {
                sRecordFileName = JH_App.F_GetSaveName(false);
                 //wifination.naStartRecord(sRecordFileName, wifination.TYPE_BOTH_PHONE_SD); //aivenlau
                wifination.naSetRecordAudio(true);
                    wifination.naSetRecordWH(-1 ,-1);
                Log.e("ABC_","bb = "+bChangeResolution);
                wifination.naStartRecord(sRecordFileName, wifination.TYPE_ONLY_PHONE);
                RecordTime_textView.setText("00:00");
            }
        }
    }





    boolean  bRe=false;

    public void F_DispMessage(String str) {
        snapshot.setText(str);
        snapshot.setVisibility(View.VISIBLE);
        snapshot.bringToFront();
        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                snapshot.setVisibility(View.INVISIBLE);
            }
        }, 500);
    }


    private void F_GotoSelectVideo_Photo() {
        But_Floder.setBackgroundResource(R.mipmap.folder_nor_jh);
        But_BrowSD.setVisibility(View.INVISIBLE);
        But_BrowPhone.setVisibility(View.INVISIBLE);
        JH_App.F_ClearDataSDArray();
        EventBus.getDefault().post("abc", "GotoSelect_Video_Photo");
    }

    private void But_Floder_Click() {
        if (But_BrowPhone.getVisibility() == View.VISIBLE) {
            But_BrowSD.setVisibility(View.INVISIBLE);
            But_BrowPhone.setVisibility(View.INVISIBLE);
            But_Floder.setBackgroundResource(R.mipmap.folder_nor_jh);
        } else {
            if (JH_App.nICType == wifination.IC_GKA) {
                But_BrowSD.setVisibility(View.VISIBLE);
            } else {
                But_BrowSD.setVisibility(View.INVISIBLE);
            }
            But_BrowPhone.setVisibility(View.VISIBLE);
            But_Floder.setBackgroundResource(R.mipmap.folder_sel_jh);
        }

    }

    private void But_Show_Hide_Click() {
        bChangedHide = !bChangedHide;
        //AutoTransition autoTransition = new AutoTransition();
        //autoTransition.setDuration(200);
        int bVisable = RecordTime_textView.getVisibility();

        if (bChangedHide) {
            // TransitionManager.beginDelayedTransition(constraintLayout,autoTransition);
            mConstraintSet2.clear(R.id.Menu_View);
            mConstraintSet2.connect(R.id.Menu_View, ConstraintSet.LEFT, R.id.constraint_layout, ConstraintSet.LEFT, 0);
            mConstraintSet2.connect(R.id.Menu_View, ConstraintSet.RIGHT, R.id.constraint_layout, ConstraintSet.RIGHT, 0);
            mConstraintSet2.constrainHeight(R.id.Menu_View, 40);
            mConstraintSet2.connect(R.id.Menu_View, ConstraintSet.TOP, R.id.constraint_layout, ConstraintSet.BOTTOM, 0);
            mConstraintSet2.applyTo(constraintLayout);
            But_Show_Hide.setBackgroundResource(R.mipmap.menu_nor_jh);
            But_Floder.setBackgroundResource(R.mipmap.folder_nor_jh);
            But_BrowSD.setVisibility(View.INVISIBLE);
            But_BrowPhone.setVisibility(View.INVISIBLE);
        } else {
            //TransitionManager.beginDelayedTransition(constraintLayout,autoTransition);
            mConstraintSet1.applyTo(constraintLayout);
            But_Show_Hide.setBackgroundResource(R.mipmap.menu_sel_jh);
        }
        RecordTime_textView.setVisibility(bVisable);
    }

    private void But_Back_Click() {
        myControl.F_SetMode(0);
        F_Power(false);
        EventBus.getDefault().post("abc", "Exit2Spalsh");
    }

    public void F_DispRcordIcon(Boolean bRecordding) {
        if (bRecordding) {
            But_Record.setBackgroundResource(R.mipmap.video_sel_jh);
            RecordTime_textView.setVisibility(View.VISIBLE);
            //snapshot.setVisibility(View.VISIBLE);
        } else {
            But_Record.setBackgroundResource(R.mipmap.video_nor_jh);
            RecordTime_textView.setVisibility(View.INVISIBLE);
            //snapshot.setVisibility(View.INVISIBLE);
        }
    }


    Handler RecHander = new Handler();
    Runnable RecRunnable = new Runnable() {
        @Override
        public void run() {
            F_DispRecorTime();
            /*
            if ((JH_App.nSdStatus & JH_App.Status_Connected) != 0) {
                if (JH_App.bNeedStartsasyRecord) {
                    String str = JH_App.F_GetSaveName(false);
                    wifination.naStartRecord(str, wifination.TYPE_ONLY_PHONE);
                    //JH_App.nRecTime = System.currentTimeMillis() / 1000;
                    //JH_App.nRecTime = 0;
                    RecordTime_textView.setText("00:00");
                    JH_App.bNeedStartsasyRecord = false;
                }
            }
            */
            RecHander.postDelayed(this, 245);
        }
    };

    @Override
    public void onPause() {
        super.onPause();
        RecHander.removeCallbacksAndMessages(null);

    }

    @Override
    public void onResume() {
        super.onResume();
        F_StartAdjRecord(true);


    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        RecHander.removeCallbacksAndMessages(null);

    }

    public void F_StartAdjRecord(boolean b) {
        if (b) {
            RecHander.removeCallbacksAndMessages(null);
            RecHander.post(RecRunnable);
        } else {
            RecHander.removeCallbacksAndMessages(null);
        }
    }

    public void F_DispRecorTime() {
        int sec = wifination.naGetRecordTime() / 1000;
        //long sec = System.currentTimeMillis() / 1000;
        //sec = sec - JH_App.nRecTime;
        int nMin = (int) (sec / 60);
        if (nMin > 99)
            nMin = 0;
        int nSec = (int) (sec % 60);
        String str = String.format("%02d:%02d", nMin, nSec);

        /*
        long sec = System.currentTimeMillis() / 1000;
        */
        /*
        if((JH_App.nSdStatus & JH_App.Status_Connected) != 0) {
            JH_App.nRecTime++;
        }

        long sec = JH_App.nRecTime/2;
        //sec = sec - JH_App.nRecTime;
        int nMin = (int) (sec / 60);
        int nSec = (int) (sec % 60);
        */
        RecordTime_textView.setText(str);

    }

    private void F_NeedReadCmd()
    {
        wifination.naReadDataFromFlash();
    }

    private  void F_Write()
    {
        byte []cmd = new byte[512];
        cmd[0]=(byte)0xAA;
        cmd[1]=(byte)0x55;
        cmd[2]=(byte)0xBB;
        cmd[3]=(byte)0xCC;
        int x;
        for(int i=4;i<512;i++)
        {
            x=i;
            if(x>0xFF)
                x-=0xFF;
            cmd[i]=(byte)x;
        }
        cmd[510]=0x55;
        cmd[511]=(byte)0xAA;

        wifination.naWriteData2Flash(cmd,512);

    }

}
