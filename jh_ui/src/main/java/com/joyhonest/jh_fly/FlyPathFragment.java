package com.joyhonest.jh_fly;


import android.os.Bundle;
import androidx.fragment.app.Fragment;
import android.os.Handler;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.joyhonest.jh_ui.JH_App;
import com.joyhonest.jh_ui.MyControl;
import com.joyhonest.wifination.R;
import com.joyhonest.wifination.wifination;

import org.simple.eventbus.EventBus;

/**
 * A simple {@link Fragment} subclass.
 */
public class FlyPathFragment extends Fragment implements View.OnClickListener {


    private MyControl myControl;
    private Button Open_Close_eye_Btn;

    private MySwitch myswitch;
    private Button Photo_Record_Start_Btn;
    // private Button Floder_Btn;
    private Button Return_Btn;


    private Button StopFly_Btn;


    private boolean bControlUI = true;
    private boolean bPhoto = true;


    // private TextView Location_TxtView;
    //private LinearLayout Layout_LeftMenu;

    //private LinearLayout Layout_Menu;


    private TextView snapshot;

    private boolean bOpenEye = true;


    //private  boolean  bStop = false;


    private RelativeLayout rootlayout;
    private TextView Record_Time_TextCtrl;


    public ImageView WifiSingle;
    private RelativeLayout photo_mask;

    public FlyPathFragment() {
        // Required empty public constructor
    }


    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View view = inflater.inflate(R.layout.fragment_fly_path, container, false);

        view.findViewById(R.id.rooglayout).setBackgroundColor(0x00010000);
        photo_mask = (RelativeLayout) view.findViewById(R.id.photo_mask);
        photo_mask.setVisibility(View.GONE);

        WifiSingle = (ImageView) view.findViewById(R.id.WifiSingle);

        rootlayout = (RelativeLayout) view.findViewById(R.id.rooglayout);
        rootlayout.setBackgroundColor(0x00000000);


        snapshot = (TextView) view.findViewById(R.id.snapshot);
        snapshot.setVisibility(View.INVISIBLE);


        //    Location_TxtView = (TextView) view.findViewById(R.id.Location_TxtView);
        //   Location_TxtView.setText("");
        myControl = (MyControl) view.findViewById(R.id.myControl);
        myControl.F_SetImage(R.mipmap.cir_back_fly_jh_b, R.mipmap.cir_fly_jh);
        myControl.F_SetDispText(false);

        Record_Time_TextCtrl = (TextView) view.findViewById(R.id.Record_textView);


        Open_Close_eye_Btn = (Button) view.findViewById(R.id.button01);
        myswitch = (MySwitch) view.findViewById(R.id.button02);
        Photo_Record_Start_Btn = (Button) view.findViewById(R.id.button03);

        Return_Btn = (Button) view.findViewById(R.id.return_btn);

        StopFly_Btn = (Button) view.findViewById(R.id.button10);

        Open_Close_eye_Btn.setOnClickListener(this);
        Photo_Record_Start_Btn.setOnClickListener(this);

        Return_Btn.setOnClickListener(this);

        StopFly_Btn.setOnClickListener(this);


        bPhoto = true;

        //  Location_TxtView.setTextColor(0xFFFF0000);


        myControl.F_DispPahtView(true);


        myControl.F_SetDispText(false);

        /*
        else {
            RelativeLayout.LayoutParams params = (RelativeLayout.LayoutParams) return_btn.getLayoutParams();
            params.width = Storage.dip2px(getActivity(), 30);
            params.height = Storage.dip2px(getActivity(), 30);
            params.removeRule(RelativeLayout.ALIGN_PARENT_BOTTOM);
            params.addRule(RelativeLayout.ALIGN_PARENT_TOP);
            return_btn.setLayoutParams(params);
            return_btn.setBackgroundResource(R.mipmap.return_icon_black_fly_jh);
            myControl.F_SetDispText(false);
        }
        */
        F_DispPhoto_Record();
        return view;
    }


    public void F_DispOpenEye(boolean b) {
        bOpenEye = b;
        if (bOpenEye) {
            rootlayout.setBackgroundColor(0x00000000);
            Open_Close_eye_Btn.setBackgroundResource(R.mipmap.open_eye_fly);
        } else {
            rootlayout.setBackgroundResource(R.mipmap.loginbackground_fly_jh);
            Open_Close_eye_Btn.setBackgroundResource(R.mipmap.close_eye_fly);
        }
    }

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

    @Override
    public void onClick(View v) {
        if (v == Return_Btn) {
            F_StopPaht();
            EventBus.getDefault().post("abc", "GoTo_Main");
        }
        if (v == Open_Close_eye_Btn) {
            bOpenEye = !bOpenEye;
            F_DispOpenEye(bOpenEye);
        }
        if (v == StopFly_Btn) {
            JH_App.bStop = true;
            StopFly_Btn.setBackgroundResource(R.mipmap.stop_sel_fly_jh);
            new Handler().postDelayed(new Runnable() {
                @Override
                public void run() {
                    JH_App.bStop = false;
                    {
                        //  StopFly_Btn.setBackgroundResource(R.mipmap.stop_nor_fly_jh_b);
                        StopFly_Btn.setBackgroundResource(R.mipmap.stop_nor_fly_jh);
                    }

                }
            }, 500);
        }
        if (v == Photo_Record_Start_Btn) {
            if ((JH_App.nSdStatus & JH_App.Status_Connected) == 0)
                return;
            if (bPhoto) {

                if ((JH_App.nSdStatus & JH_App.SD_SNAP) != 0)     //SD 拍照还没有完成。就不进行此次拍照
                    return;

                if (JH_App.bPhone_SNAP)
                    return;
                JH_App.bPhone_SNAP = true;
                final String str = JH_App.F_GetSaveName(true);
                wifination.naSnapPhoto(str, wifination.TYPE_BOTH_PHONE_SD);
                Handler handler = new Handler();
                Runnable runnable = new Runnable() {
                    @Override
                    public void run() {
                        JH_App.bPhone_SNAP = false;
                    }
                };
                handler.postDelayed(runnable, 500);
                JH_App.F_PlayPhoto();
                photo_mask.setVisibility(View.VISIBLE);
                new Handler().postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        photo_mask.setVisibility(View.GONE);
                    }
                }, 120);

            } else {
                if ((JH_App.nSdStatus & JH_App.Status_Connected) == 0) {
                    Log.e("Error:", "Not DispVideo!");
                    return;
                }

                if ((JH_App.nSdStatus & JH_App.LocalRecording) != 0) {
                    wifination.naStopRecord_All();
                    Photo_Record_Start_Btn.setBackgroundResource(R.mipmap.photo_record_icon_fly_jh);
                } else {
                    String strRecordFilename = JH_App.F_GetSaveName(false);
                    wifination.naStartRecord(strRecordFilename, wifination.TYPE_BOTH_PHONE_SD);
                    Record_Time_TextCtrl.setText("00:00");
                    Photo_Record_Start_Btn.setBackgroundResource(R.mipmap.photo_recording_icon_fly_jh);
                }
            }
        }


    }

    @Override
    public void onResume() {
        super.onResume();
        F_StartAdjRecord(true);

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
        if ((JH_App.nSdStatus & JH_App.LocalRecording) != 0) {
            int sec = wifination.naGetRecordTime() / 1000;
            int nMin = (sec / 60);
            if (nMin > 99)
                nMin = 0;
            int nSec = (int) (sec % 60);
            String str = String.format("%02d:%02d", nMin, nSec);
            Record_Time_TextCtrl.setText(str);
            Record_Time_TextCtrl.setVisibility(View.VISIBLE);
        } else {
            Record_Time_TextCtrl.setVisibility(View.INVISIBLE);
        }
    }


    Handler RecHander = new Handler();
    Runnable RecRunnable = new Runnable() {
        @Override
        public void run() {
            F_DispRecorTime();
            RecHander.postDelayed(this, 500);
        }
    };


    public void F_SetPhoto(boolean b) {
        bPhoto = b;
        F_DispPhoto_Record();
    }

    public void F_DispPhoto_Record() {

        if (bPhoto) {
            Photo_Record_Start_Btn.setBackgroundResource(R.mipmap.photo_icon_fly_jh);
            if ((JH_App.nSdStatus & JH_App.LocalRecording) != 0) {
                Record_Time_TextCtrl.setVisibility(View.VISIBLE);
            } else {
                Record_Time_TextCtrl.setVisibility(View.INVISIBLE);
            }
        } else {
            Photo_Record_Start_Btn.setBackgroundResource(R.mipmap.photo_record_icon_fly_jh);
            //myControl.F_SetImage(R.mipmap.cir_back_fly_jh_b, R.mipmap.cir_fly_jh);
            myControl.F_SetImage(R.mipmap.cir_back_fly_jh, R.mipmap.cir_fly_jh);
            myControl.F_SetFlyRecord(false);
            F_DispOpenEye(bOpenEye);
            //StopFly_Btn.setBackgroundResource(R.mipmap.stop_nor_fly_jh_b);
            StopFly_Btn.setBackgroundResource(R.mipmap.stop_nor_fly_jh);
        }
    }

    public void F_StopPaht() {

        myControl.F_StopPath();
        sentHander.removeCallbacksAndMessages(null);
    }


    public void F_StartPath() {
        sentHander.removeCallbacksAndMessages(null);
        sentHander.postDelayed(sentRunnable, 10);
    }


    private Handler sentHander = new Handler();
    private Runnable sentRunnable = new Runnable() {
        @Override
        public void run() {
            F_SentCmd();
            sentHander.postDelayed(this, 15);
        }
    };


    byte[] cmd = new byte[20];

    private void F_SentCmd()        //司马demo  协议
    {

        if (!JH_App.bisPathMode)
            return;
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


        X2_bak = X2;
        Y2_bak = Y2;


        int flag = 0;
        int flag2 = 0;
        int flag3 = 0;
/*
Data0：油门数据（00-FF）
Data1：前进后退数据（00-FF）（前进：0-7F；后退：80-FF；没有动作：00或80）
Data2：左右转弯数据（00-FF）（左转：0-7F；右转：80-FF；没有动作：00或80）
Data3：左右平移数据（00-FF）（左移：0-7F；右移：80-FF；没有动作：00或80）

Data4（Bit0-Bit5）：油门微调（00-3F）（上调：0-1F；下调：20-3F；没有动作：00或20）
Data4（Bit6）：Bit6=1 翻滚功能	；Bit6=0 没有翻滚功能
Data4（Bit7）：Bit7=0 不是Flow Me功能

Data5（Bit0-Bit5）：前后微调（00-3F）（前调：0-1F；后调：20-3F；没有动作：00或20）
Data5（Bit6）：Bit6=1 Mode1标志；Bit6=0 Mode2标志
Data5（Bit7）：Bit7=1 快档标志	；Bit7=0 慢档标志；
Data5（Bit7）与Data8(Bit3)配合使用：
Data5（Bit7）：Data8(Bit3) = 00  ：慢档标志；
Data5（Bit7）：Data8(Bit3) = 01  ：中速档标志；
Data5（Bit7）：Data8(Bit3) = 10  ：快档标志；

Data6（Bit0-Bit5）：转弯微调（00-3F）（左调：0-1F；右调：20-3F；没有动作：00或20）
Data6（Bit6）：Bit6=1拍照；Bit6=0 拍照功能无效
Data6（Bit7）：Bit7=1录像；Bit7=0 录像功能无效

Data7（Bit0-Bit5）：平移微调（00-3F）（左调：0-1F；右调：20-3F；没有动作：00或20）
Data7（Bit6）：Bit6=1 一键起飞；Bit6=0 一键起飞无效
Data7（Bit7）：Bit7=1 无头模式；Bit7=0 有头模式

Data8：（Bit2- Bit0）保留未用
当Data4（Bit6）=1时：Data8(Bit6), Data8(Bit7) = (00：前翻，01：后翻，10：左翻，11：右翻)
当Data4（Bit6）=0时：Data8(Bit7)=1 镜头下调，Data8(Bit7)=0 镜头下调无效
当Data4（Bit6）=0时：Data8(Bit6)=1 镜头上调，Data8(Bit6)=0 镜头上调无效
Data8(Bit5)=1 水平校正，Data8(Bit5)=0无效
Data8(Bit4)=1 一键急停，Data8(Bit4)=0无效
Data8(Bit3)=1 中速档标志，Data8(Bit3)=0无效


Data9：Data0- Data8异或后，再加0X55

 */
        if (JH_App.bIsSyMa) {

            if (X1 > (0x80 - 0x25) && X1 < (0x80 + 0x25)) {
                X1 = 0x80;
            }


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
                ;
            } else if (X1 < 0x80) {
                X1 = 0x80 - X1;
                if (X1 > 0x7F) {
                    X1 = 0x7F;
                }
            }

            if (X2 > 0x80) {
                ;
            } else if (X2 < 0x80) {
                X2 = 0x80 - X2;
                if (X2 > 0x7F) {
                    X2 = 0x7F;
                }
            }
            i = 0;


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
                da = 0x20;
            }
            cmd[5] = (byte) da;       //前后微调
            if (JH_App.bHiSpeed)
                cmd[5] |= 0x80;          //高速模式

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


            da = X_ADJ2 - 0x80;    //左右微调
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
            cmd[8] = 0;

            if (JH_App.bStop) {
                cmd[8] |= 0x10;
            }
            cmd[9] = (byte) (((cmd[0] ^ cmd[1] ^ cmd[2] ^ cmd[3] ^ cmd[4] ^ cmd[5] ^ cmd[6] ^ cmd[7] ^ cmd[8]) & 0xFF) + 0x55);

            wifination.naSentCmd(cmd, 10);
            //   Log.e("Cmd:  ","Sent Path  X1=" +X1+" Y1="+Y1+" X2="+X2+" Y2="+Y2);
            /*
            if(Y2!=0x80 || X2!=0x80) {
                String str = String.format("X2=%02X  Y2=%02X", X2, Y2);
                Log.e("CMD", "SentCmd Path =" + str);
            }
            */
        }
    }

}
