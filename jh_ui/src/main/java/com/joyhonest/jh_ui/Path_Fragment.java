package com.joyhonest.jh_ui;


//import android.app.Fragment;

import android.os.Bundle;
import android.os.Handler;
//import android.support.v4.app.Fragment;
import androidx.fragment.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.RelativeLayout;

import com.joyhonest.wifination.R;
import com.joyhonest.wifination.fly_cmd;
import com.joyhonest.wifination.wifination;

import org.simple.eventbus.EventBus;


/**
 * A simple {@link Fragment} subclass.
 */
public class Path_Fragment extends Fragment {

    MyControl myControl;
    Button But_KeyStop;
    boolean bStop = false;
    public ImageView imageViewRssi;
    Button return_btn;

    public Path_Fragment() {
        // Required empty public constructor
    }


    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View view = inflater.inflate(R.layout.fragment_path, container, false);

        imageViewRssi = (ImageView) view.findViewById(R.id.imageView4_Rssi);

        myControl = (MyControl) view.findViewById(R.id.myControl);
        return_btn = (Button) view.findViewById(R.id.button_back);

        return_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                F_StopPaht();
                EventBus.getDefault().post("abc", "GoTo_Main");
            }
        });

        But_KeyStop = (Button) view.findViewById(R.id.button_stop);


        But_KeyStop.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                bStop = true;
                But_KeyStop.setBackgroundResource(R.mipmap.stop_sel_jh);
                new Handler().postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        bStop = false;
                        But_KeyStop.setBackgroundResource(R.mipmap.stop_nor_jh);
                    }
                }, 500);
            }
        });

        sentHander.postDelayed(sentRunnable, 10);
        myControl.F_DispPahtView(true);
        if (JH_App.nType == JH_App.nStyle_ui) {
            myControl.F_SetDispText(true);
        } else {
            RelativeLayout.LayoutParams params = (RelativeLayout.LayoutParams) return_btn.getLayoutParams();
            params.width = Storage.dip2px(getActivity(), 30);
            params.height = Storage.dip2px(getActivity(), 30);
            params.removeRule(RelativeLayout.ALIGN_PARENT_BOTTOM);
            params.addRule(RelativeLayout.ALIGN_PARENT_TOP);
            return_btn.setLayoutParams(params);
            return_btn.setBackgroundResource(R.mipmap.return_icon_black_fly_jh);
            myControl.F_SetDispText(false);
        }


        return view;
    }


    @Override
    public void onDestroy() {
        super.onDestroy();
        sentHander.removeCallbacksAndMessages(null);

    }

    public void F_StopPaht() {

        myControl.F_StopPath();
        sentHander.removeCallbacksAndMessages(null);
    }

    public void F_SttartPath() {
        sentHander.removeCallbacksAndMessages(null);
        sentHander.postDelayed(sentRunnable, 200);
    }


    private Handler sentHander = new Handler();
    private Runnable sentRunnable = new Runnable() {
        @Override
        public void run() {
            F_SentCmd();
            sentHander.postDelayed(this, 20);
        }
    };


    private void F_SetCmd_Fly() {
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


        int da = X_ADJ1 - 0x80;          //旋转微调
        da += 32;
        if (da < 0)
            da = 0;
        if (da > 63) {
            da = 63;
        }
        X_ADJ1 = da;

        da = X_ADJ2 - 0x80;          //
        da += 32;
        if (da < 0)
            da = 0;
        if (da > 63) {
            da = 63;
        }
        X_ADJ2 = da;

        da = Y_ADJ2 - 0x80;          //
        da += 32;
        if (da < 0)
            da = 0;
        if (da > 63) {
            da = 63;
        }
        Y_ADJ2 = da;


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


        fly_cmd.Thro = Y1;
        fly_cmd.Yaw = X1;

        fly_cmd.Pitch = Y2;
        fly_cmd.Roll = X2;

        fly_cmd.TrimYaw = X_ADJ1;

        fly_cmd.TrimPitch = Y_ADJ2;
        fly_cmd.TrimRoll = X_ADJ2;


        fly_cmd.CFMode = 0;
        fly_cmd.FastMode = 0;
        fly_cmd.GpsMode = 0;
        fly_cmd.LevelCor = 0;

        fly_cmd.AutoTakeoff = 0;
        fly_cmd.AutoLand = 0;
        fly_cmd.Stop = 0;

        if (JH_App.bHiSpeed) {
            fly_cmd.FastMode = 02;
        }

        if (JH_App.bGPSMode) {
            fly_cmd.GpsMode = 1;
        }
        if (JH_App.bAdj) {
            fly_cmd.LevelCor = 1;
        }
        if (JH_App.bDn) {
            fly_cmd.AutoLand = 1;
        }

        if (JH_App.bUp) {
            fly_cmd.AutoTakeoff = 1;
        }
        if (fly_cmd.AutoLand != 0) {
            fly_cmd.AutoTakeoff = 0;
        }
        if (JH_App.bStop) {
            fly_cmd.Stop = 1;
        }
        wifination.naFillFlyCmdByC(1);
        wifination.naSentCmd(fly_cmd.cmd, 15);
        Log.e("SentCmd Path", "Sent OK!");
    }

    byte[] cmd = new byte[20];
    boolean bas = false;

    private void F_SentCmd()        //司马demo  协议
    {
        //if(myControl.getVisibility()!=View.VISIBLE)
        //    return;
        if (!JH_App.bisPathMode)
            return;
/*
        if (JH_App.nType == JH_App.nStyle_fly) {
            F_SetCmd_Fly();
            return;
        }
*/
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

            if (bStop) {
                cmd[8] |= 0x10;
            }
            cmd[9] = (byte) (((cmd[0] ^ cmd[1] ^ cmd[2] ^ cmd[3] ^ cmd[4] ^ cmd[5] ^ cmd[6] ^ cmd[7] ^ cmd[8]) & 0xFF) + 0x55);

            wifination.naSentCmd(cmd, 10);
            //Log.e("Cmd:  ","Sent NromalComd  X1=" +X1+" Y1="+Y1+" X2="+X2+" Y2="+Y2);

            /*
            if(Y2!=0x80 || X2!=0x80) {
                String str = String.format("X2=%02X  Y2=%02X", X2, Y2);
                Log.e("CMD", "SentCmd Path =" + str);
            }
            */
        } else {


            //if (bHoldHight)              // 定高
            {
                flag |= 0x40;
            }

            if (bStop) {          // 紧急停止
                flag |= 0x80;
            }


            if (JH_App.bHiSpeed) {
                flag2 = 0x00;           // 100
            } else {
                flag2 = 0x02;            //30
            }

            {
                //    flag2 = 0x01;           //60
            }


            i = 0;
            cmd[i++] = 0x66;           //0
            cmd[i++] = (byte) X2;
            cmd[i++] = (byte) Y2;
            cmd[i++] = (byte) Y1;
            cmd[i++] = (byte) X1;
            cmd[i++] = (byte) flag;     //5
            cmd[i++] = (byte) flag2;
            cmd[i++] = (byte) flag3;
            cmd[i++] = (byte) X_ADJ2;
            cmd[i++] = (byte) Y_ADJ2;
            cmd[i++] = (byte) X_ADJ1;

            wifination.naSentCmd(cmd, i);
            // String sInfo = String.format("X1 = %02X  X1 = %02X   X2 = %02X  Y2 = %02X  Flag = %02X   Flag1= %02X",X1,Y1,X2,Y2,flag,flag2);
            // Logger.e("Cmd: ",sInfo);
        /*
        if(bRoll)
        {
            if(Y2_bak>=0xE0 || Y2_bak <=0x10 || X2_bak >=0xE0 || X2_bak <=0x10)
            {
                //bCanRoll = false;
                bRoll = false;
            }
        }
        */
            // String  str = String.format("X1=%02X  Y1=%02X  X2=%02X  Y2=%02X",X1,Y1,X2,Y2);
            // Log.e("CMD","SentCmd ="+str);


        }

    }

}
