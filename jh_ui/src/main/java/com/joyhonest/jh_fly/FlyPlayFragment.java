package com.joyhonest.jh_fly;


import android.animation.ObjectAnimator;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import androidx.fragment.app.Fragment;
import android.os.Handler;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Surface;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.joyhonest.jh_ui.JH_App;
import com.joyhonest.jh_ui.MyControl;
import com.joyhonest.wifination.R;
import com.joyhonest.jh_ui.Storage;
import com.joyhonest.wifination.wifination;

import org.simple.eventbus.EventBus;

/**
 * A simple {@link Fragment} subclass.
 */
public class FlyPlayFragment extends Fragment implements View.OnClickListener {

    public static boolean bTestMode = false;

    public MyControl myControl;
    private Button Fly_Camera_Btn;
    private Button Photo_Record_Select_Btn;
    public MySwitch myswitch;
    private MySwitch button_leftRight;
    private Button Photo_Record_Start_Btn;
    private Button Floder_Btn;
    private Button Return_Btn;
    private Button Return_Btn1;
    private Button OneKeyReturn_Btn;

    private Button Speed_Btn;
    private Button Adj_Btn;
    private Button VR_Btn;

    private Button Gsensor_Btn;
    private Button Path_Btn;
    private Button HeadLess_Btn;


    private Button Finger_Btn;
    private Button StopFly_Btn;
    private Button UpDn_Btn;


    private Button button_more_b;
    //private Button More_Btn;


    //private boolean bMore;


    private TextView Record_Time_TextCtrl;


    private boolean bControlUI = true;
    private boolean bPhoto = true;


    //  private TextView Location_TxtView;
    //private LinearLayout Layout_LeftMenu;

    private RelativeLayout Menu_Layout;

    private LinearLayout rect_layout;


    private TextView snapshot;


    private LinearLayout tool_1_layout;
    private LinearLayout tool_2_layout;


    public ImageView WifiSingle;

    private LinearLayout tool_menu;


    private RelativeLayout photo_mask;


    //  private RelativeLayout  LayoutMask;
    //  private Button           return_btn_b;


    private boolean bTestFlip = false;

    public FlyPlayFragment() {
        // Required empty public constructor
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {


        View view = inflater.inflate(R.layout.fragment_fly_play_jh, container, false);

        photo_mask = (RelativeLayout) view.findViewById(R.id.photo_mask);
        photo_mask.setVisibility(View.GONE);

        view.findViewById(R.id.rooglayout).setBackgroundColor(0x00010000);


        button_more_b = (Button) view.findViewById(R.id.button_more_b);

        WifiSingle = (ImageView) view.findViewById(R.id.WifiSingle);


        //  LayoutMask = (RelativeLayout)view.findViewById(R.id.LayoutMask);
        //  return_btn_b =(Button)view.findViewById(R.id.return_btn_b);


        tool_1_layout = (LinearLayout) view.findViewById(R.id.tool_1_layout);
        tool_2_layout = (LinearLayout) view.findViewById(R.id.tool_2_layout);
        tool_1_layout.setVisibility(View.VISIBLE);
        tool_2_layout.setVisibility(View.GONE);
        tool_menu = (LinearLayout) view.findViewById(R.id.tool_menu);
        tool_menu.setVisibility(View.INVISIBLE);

        rect_layout = (LinearLayout) view.findViewById(R.id.rect_layout);

        snapshot = (TextView) view.findViewById(R.id.snapshot);
        snapshot.setVisibility(View.INVISIBLE);

        //  Layout_LeftMenu = (LinearLayout) view.findViewById(R.id.Layout_LeftMenu);
        Menu_Layout = (RelativeLayout) view.findViewById(R.id.Menu_Layout);

        //   Location_TxtView = (TextView) view.findViewById(R.id.Location_TxtView);
        //   Location_TxtView.setText("");
        myControl = (MyControl) view.findViewById(R.id.myControl);
        // myControl.F_SetImage(R.mipmap.cir_back_fly_jh_b, R.mipmap.cir_fly_jh);
        myControl.F_SetImage(R.mipmap.cir_back_fly_jh, R.mipmap.cir_fly_jh);
        myControl.F_SetDispText(false);

        Record_Time_TextCtrl = (TextView) view.findViewById(R.id.Record_textView);


        Fly_Camera_Btn = (Button) view.findViewById(R.id.button01);
        //Photo_Record_Select_Btn = (Button)view.findViewById(R.id.button02);
        myswitch = (MySwitch) view.findViewById(R.id.button02);
        button_leftRight = (MySwitch) view.findViewById(R.id.button_leftRight);
        Photo_Record_Start_Btn = (Button) view.findViewById(R.id.button03);
        Floder_Btn = (Button) view.findViewById(R.id.button04);
        Return_Btn = (Button) view.findViewById(R.id.return_btn);
        Return_Btn1 = (Button) view.findViewById(R.id.return_btn1);
        Return_Btn1.setVisibility(View.INVISIBLE);

        //OneKeyReturn_Btn = (Button)view.findViewById(R.id.button05);

        Speed_Btn = (Button) view.findViewById(R.id.button05);
        Adj_Btn = (Button) view.findViewById(R.id.button06);
        Gsensor_Btn = (Button) view.findViewById(R.id.button07);

        Path_Btn = (Button) view.findViewById(R.id.button08);
        VR_Btn = (Button) view.findViewById(R.id.button09);
        HeadLess_Btn = (Button) view.findViewById(R.id.button0A);

        //Finger_Btn = (Button)view.findViewById(R.id.button08);

        StopFly_Btn = (Button) view.findViewById(R.id.button10);
        UpDn_Btn = (Button) view.findViewById(R.id.button11);

        //  More_Btn = (Button) view.findViewById(R.id.button_more);
        button_leftRight.F_SetMenu(true);


        // More_Btn.setOnClickListener(this);
        Fly_Camera_Btn.setOnClickListener(this);
        //  Photo_Record_Select_Btn.setOnClickListener(this);
        Photo_Record_Start_Btn.setOnClickListener(this);
        Floder_Btn.setOnClickListener(this);
        Return_Btn.setOnClickListener(this);
        Return_Btn1.setOnClickListener(this);
        button_more_b.setOnClickListener(this);

        StopFly_Btn.setOnClickListener(this);

        UpDn_Btn.setOnClickListener(this);
        bPhoto = true;

        Speed_Btn.setOnClickListener(this);
        Adj_Btn.setOnClickListener(this);
        Gsensor_Btn.setOnClickListener(this);

        Path_Btn.setOnClickListener(this);
        VR_Btn.setOnClickListener(this);
        HeadLess_Btn.setOnClickListener(this);
        Menu_Layout.setOnClickListener(this);

        //  LayoutMask.setOnClickListener(this);
        //  return_btn_b.setOnClickListener(this);
        //   bMore = false;


        F_DispUI();

        //F_DispToolMenu(true,true);

        bDispMenu1 = true;
        bDispMenu2 = false;
        F_DispAllMenu();

        //   F_SetLocaiotn("");
        //   Location_TxtView.setTextColor(0xFFFF0000);
        //  F_DispAllMenu(false);

        F_DispDisableAll();

        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                F_Adj_rect();
            }
        }, 10);
        return view;
    }


    public void F_DispDisableAll() {
        float nAlpha = 0.2f;
        if (!JH_App.bFlyDisableAll) {
            nAlpha = 1.0f;
        }
        Fly_Camera_Btn.setAlpha(nAlpha);
        myswitch.setAlpha(nAlpha);
        Photo_Record_Start_Btn.setAlpha(nAlpha);
        Floder_Btn.setAlpha(nAlpha);
        Return_Btn.setAlpha(nAlpha);
        StopFly_Btn.setAlpha(nAlpha);
        button_more_b.setAlpha(nAlpha);
        UpDn_Btn.setAlpha(nAlpha);
        WifiSingle.setAlpha(nAlpha);
        myControl.F_Invalidate();

    }

    public void F_DispNoMoremenu() {
        bDispMenu1 = true;
        bDispMenu2 = false;
        F_DispAllMenu();
    }

    private void F_SetRectIcon(Button button, int rect) {
        LinearLayout.LayoutParams params = (LinearLayout.LayoutParams) button.getLayoutParams();
        params.width = rect;
        params.height = rect;
        button.setLayoutParams(params);
    }

    private void F_Adj_rect() {
        int rect = Math.min(rect_layout.getWidth(), rect_layout.getHeight());
        int pitch = Storage.dip2px(getActivity(), 6);
        int rr = Storage.dip2px(getActivity(), 46);
        if (rect > rr) {
            rect = rr;
            rect -= pitch;
        }

        F_SetRectIcon(Speed_Btn, rect);
        F_SetRectIcon(Adj_Btn, rect);
        F_SetRectIcon(Gsensor_Btn, rect);
        F_SetRectIcon(Path_Btn, rect);
        F_SetRectIcon(VR_Btn, rect);
        F_SetRectIcon(HeadLess_Btn, rect);


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

    /*
    private void F_DispMore() {
        if (bMore) {
            tool_1_layout.setVisibility(View.GONE);
            tool_2_layout.setVisibility(View.VISIBLE);
        } else {
            tool_1_layout.setVisibility(View.VISIBLE);
            tool_2_layout.setVisibility(View.GONE);
        }
    }
    */


    public void F_SetLocaiotn(String str) {
        //  Location_TxtView.setVisibility(View.INVISIBLE);
        // Location_TxtView.setText(str);
    }


    public void F_DispSpeedIcon() {

        if (Speed_Btn != null) {
            if (JH_App.bHiSpeed) {
                Speed_Btn.setBackgroundResource(R.mipmap.speed_high_fly_jh);
            } else {
                Speed_Btn.setBackgroundResource(R.mipmap.speed_low_fly_jh);
            }
        }

    }

    public void F_DispGSensorIcon() {

        if (Gsensor_Btn != null) {

            if (JH_App.bSensor) {
                myControl.F_SetMode(1);
                //   Gsensor_Btn.setBackgroundResource(R.mipmap.gsensor_icon_sel_fly_jh);
                int org = getActivity().getWindowManager().getDefaultDisplay().getRotation();

                if (Surface.ROTATION_270 == org) {
                    getActivity().setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_REVERSE_LANDSCAPE);
                } else {
                    getActivity().setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
                }
            } else {
                myControl.F_SetMode(0);
                //  Gsensor_Btn.setBackgroundResource(R.mipmap.gsensor_icon_fly_jh);
                getActivity().setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_SENSOR_LANDSCAPE);
            }

        }
    }

    private void F_DispHeadLessIcon() {

        if (HeadLess_Btn != null) {
            if (JH_App.bHeadLess) {
                HeadLess_Btn.setBackgroundResource(R.mipmap.headless_sel_fly_jh);
            } else {
                HeadLess_Btn.setBackgroundResource(R.mipmap.headless_fly_jh);
            }
        }

    }


    public void F_InitDisp() {
        F_Disp3DUI();
    }

    public void F_Disp3DUI() {
        if (JH_App.bVR) {
            boolean b = bControlUI;
            bControlUI = false;
            F_DispUI();
            bControlUI = b;

            sentHander.removeCallbacksAndMessages(null);
            if (bControlUI) {
                sentHander.post(sentRunnable);
            }
            boolean b1 = bDispMenu1;
            boolean b2 = bDispMenu2;
            bDispMenu1 = false;
            bDispMenu2 = false;
            F_DispAllMenu();
            bDispMenu2 = b2;
            bDispMenu1 = b1;

            WifiSingle.setVisibility(View.INVISIBLE);
            Fly_Camera_Btn.setVisibility(View.INVISIBLE);
            myswitch.setVisibility(View.INVISIBLE);
            Photo_Record_Start_Btn.setVisibility(View.INVISIBLE);
            Record_Time_TextCtrl.setVisibility(View.INVISIBLE);
            Floder_Btn.setVisibility(View.INVISIBLE);
            Return_Btn.setBackgroundResource(R.mipmap.return_icon_black_fly_jh);
            Fly_PlayActivity activity = (Fly_PlayActivity) getActivity();
         //   RelativeLayout.LayoutParams params = (RelativeLayout.LayoutParams) activity.glSurfaceView.getLayoutParams();
          //  int tp = Storage.dip2px(activity, 40);
          //  params.topMargin = tp;
          //  activity.glSurfaceView.setLayoutParams(params);
            Return_Btn1.setVisibility(View.VISIBLE);
            Return_Btn1.setBackgroundResource(R.mipmap.return_icon_black_fly_jh);


        } else {
            F_DispUI();
            F_DispAllMenu();
            Return_Btn1.setVisibility(View.INVISIBLE);

            WifiSingle.setVisibility(View.VISIBLE);
            Fly_Camera_Btn.setVisibility(View.VISIBLE);
            myswitch.setVisibility(View.VISIBLE);
            Photo_Record_Start_Btn.setVisibility(View.VISIBLE);
            Record_Time_TextCtrl.setVisibility(View.VISIBLE);
            Floder_Btn.setVisibility(View.VISIBLE);
            Return_Btn.setBackgroundResource(R.mipmap.return_icon_fly_jh);
            Fly_PlayActivity activity = (Fly_PlayActivity) getActivity();

//            RelativeLayout.LayoutParams params = (RelativeLayout.LayoutParams) activity.glSurfaceView.getLayoutParams();
//            params.topMargin = 0;
//            activity.glSurfaceView.setLayoutParams(params);
        }
        wifination.naSet3D(JH_App.bVR);
        //  wifination.F_AdjBackGround(getActivity(), R.mipmap.loginbackground_fly_jh);//R.mipmap.loginbackground_jh)


    }

/*
    void F_DispToolMenu(boolean bDisp,boolean bInit)
    {
        if(bDisp)
        {
             if(bInit)
             {
                 Menu_Layout.setVisibility(View.VISIBLE);
                 tool_menu.setVisibility(View.INVISIBLE);
             }
             else
             {
                 Menu_Layout.setVisibility(View.INVISIBLE);
                 tool_menu.setVisibility(View.VISIBLE);
             }
        }
        else
        {
            Menu_Layout.setVisibility(View.INVISIBLE);
            tool_menu.setVisibility(View.INVISIBLE);
        }

    }
    */

    boolean bDispMenu1 = true;
    boolean bDispMenu2 = false;


    private void F_DispAllMenu() {
        int nLeft = Storage.dip2px(getActivity(), 54);
        if (bDispMenu1) {
            if (bDispMenu2) {
                //Menu_Layout.setVisibility(View.INVISIBLE);
                Menu_Layout.setVisibility(View.VISIBLE);
                tool_menu.setVisibility(View.VISIBLE);
                ObjectAnimator.ofFloat(tool_menu, "translationX", nLeft, 0).setDuration(500).start();
            } else {
                Menu_Layout.setVisibility(View.VISIBLE);
                //tool_menu.setVisibility(View.INVISIBLE);
                tool_menu.setVisibility(View.VISIBLE);
                int xx = (int) tool_menu.getTranslationX();
                if (xx < nLeft)
                    ObjectAnimator.ofFloat(tool_menu, "translationX", 0, nLeft).setDuration(500).start();
            }
        } else {
            if (bDispMenu2) {
                Menu_Layout.setVisibility(View.INVISIBLE);
                tool_menu.setVisibility(View.VISIBLE);
                // ObjectAnimator.ofFloat(tool_menu, "X", wScreen - nLeft, (wScreen + 10)).setDuration(500).start();
                ObjectAnimator.ofFloat(tool_menu, "translationX", 0, nLeft).setDuration(500).start();
            } else {
                Menu_Layout.setVisibility(View.INVISIBLE);
                tool_menu.setVisibility(View.INVISIBLE);
            }
        }
    }

    int ixxxx = 0;
    int ch = 0x55;

    String strRecordFilename = "";


    public void F_SetNoGsensor() {
        JH_App.bSensor = false;
        F_DispGSensorIcon();
    }

    boolean bTest = true;
    int nRota = 0;

    @Override
    public void onClick(View v) {
        if (v != Return_Btn) {
            if (JH_App.bFlyDisableAll) {
                JH_App.bFlyDisableAll = false;
                F_DispDisableAll();
                return;

            }
        }
        if (Menu_Layout == v || v.getId() == R.id.button_more_b) {
            if (!bControlUI)
                return;

            if (JH_App.bVR)
                return;

            bDispMenu1 = true;
            bDispMenu2 = true;
            F_DispAllMenu();

        }
        /*
        if(v == LayoutMask)
        {
            bDispMenu1=true;
            bDispMenu2=false;
            F_DispAllMenu();
            return;
        }
        */

        if (v == Return_Btn || v == Return_Btn1)//|| v==return_btn_b) {
        {
            EventBus.getDefault().post("exit", "Exit");
        }

        /*
        if (v == More_Btn) {
            bMore = !bMore;
            F_DispMore();
        }
        */
        if (v == StopFly_Btn) {

            /*
            bTestFlip =!bTestFlip;
            wifination.StartAudio(bTestFlip);
            //wifination.naSetFlip(bTestFlip);
            wifination.naSet3D(bTestFlip);
            */

            JH_App.bStop = true;
            JH_App.bUp = false;
            JH_App.bDn = false;
            StopFly_Btn.setBackgroundResource(R.mipmap.stop_sel_fly_jh);
            new Handler().postDelayed(new Runnable() {
                @Override
                public void run() {
                    JH_App.bStop = false;
                    if ((JH_App.nSdStatus & JH_App.LocalRecording) != 0) {
                        StopFly_Btn.setBackgroundResource(R.mipmap.stop_nor_fly_jh);
                    } else {
                        // StopFly_Btn.setBackgroundResource(R.mipmap.stop_nor_fly_jh_b);
                        StopFly_Btn.setBackgroundResource(R.mipmap.stop_nor_fly_jh);
                    }

                }
            }, 500);
        }

        if (v == UpDn_Btn) {

            JH_App.bFlying = !JH_App.bFlying;
            if (JH_App.bFlying) {
                JH_App.bUp = true;
                JH_App.bDn = false;
            } else {
                JH_App.bUp = false;
                JH_App.bDn = true;
            }

            //UpDn_Btn.setBackgroundResource(R.mipmap.keyup_dn_sel_fly_jh);
            UpDn_Btn.setAlpha(0.4f);
            new Handler().postDelayed(new Runnable() {
                @Override
                public void run() {
                    JH_App.bUp = false;
                    JH_App.bDn = false;
                    UpDn_Btn.setAlpha(1.0f);
                    /*
                    if((JH_App.nSdStatus & JH_App.LocalRecording)!=0)
                    {
                        UpDn_Btn.setBackgroundResource(R.mipmap.keyup_dn_fly_jh);
                    }
                    else
                    {
                       // UpDn_Btn.setBackgroundResource(R.mipmap.keyup_dn_fly_jh_b);
                        UpDn_Btn.setBackgroundResource(R.mipmap.keyup_dn_fly_jh);
                    }
                    */

                }
            }, 500);
        }
        if (v == Path_Btn) {
            F_SetNoGsensor();
            EventBus.getDefault().post("abc", "GotoPath");
        }
        if (v == VR_Btn) {
            F_SetNoGsensor();

            JH_App.bVR = !JH_App.bVR;
            F_Disp3DUI();

        }

        if (v == Speed_Btn) {
            JH_App.bHiSpeed = !JH_App.bHiSpeed;
            F_DispSpeedIcon();
            JH_App.F_PlaySpeed(JH_App.bHiSpeed);
        }
        if (v == Gsensor_Btn) {
            JH_App.bSensor = !JH_App.bSensor;
            F_DispGSensorIcon();

        }
        if (v == HeadLess_Btn) {
            JH_App.bHeadLess = !JH_App.bHeadLess;
            F_DispHeadLessIcon();

        }

        if (v == Adj_Btn) {
            //myControl.F_SetLeftRightAdj(0x80);
            //myControl.F_SetForwardBackAdj(0x80);
            //myControl.F_SetRotateAdj(0x80);
            JH_App.bAdj = true;
            JH_App.F_PlayCenter();
            Adj_Btn.setBackgroundResource(R.mipmap.adj_sel_fly_jh);
            new Handler().postDelayed(new Runnable() {
                @Override
                public void run() {
                    JH_App.bAdj = false;

                    Adj_Btn.setBackgroundResource(R.mipmap.adj_fly_jh);

                }
            }, 500);
        }

        if (v == Fly_Camera_Btn) {
            //wifination.naSetLedOnOff(true);

            bControlUI = !bControlUI;
            F_DispUI();

            if (bControlUI) {
                bDispMenu1 = true;
                bDispMenu2 = false;
                F_DispAllMenu();
            } else {
                bDispMenu1 = false;
                F_DispAllMenu();
            }

        }


        if (v == Floder_Btn) {


            F_SetNoGsensor();
            Integer nFragment = JH_Fly_Setting.Brow_Select_Fragment;
            EventBus.getDefault().post(nFragment, "gotoFragment");


        }
        if (v == Photo_Record_Start_Btn) {

            if ((JH_App.nSdStatus & JH_App.Status_Connected) == 0)
                return;
            if (bPhoto) {
                F_Photo();
                /*
                if ((JH_App.nSdStatus & JH_App.SD_SNAP) != 0)     //SD 拍照还没有完成。就不进行此次拍照
                    return;

                if (JH_App.bPhone_SNAP)
                    return;
                JH_App.bPhone_SNAP = true;

                final String str = JH_App.F_GetSaveName(true);
                wifination.naSnapPhoto(str, wifination.TYPE_BOTH_PHONE_SD);

                JH_App.F_PlayPhoto();
                photo_mask.setVisibility(View.VISIBLE);
                new Handler().postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        photo_mask.setVisibility(View.GONE);
                    }
                }, 120);


                Handler handler = new Handler();
                Runnable runnable = new Runnable() {
                    @Override
                    public void run() {
                        JH_App.bPhone_SNAP = false;
                    }
                };
                handler.postDelayed(runnable, 800);
                */

            } else {
                if ((JH_App.nSdStatus & JH_App.Status_Connected) == 0) {
                    Log.e("Error:", "Not DispVideo!");
                    return;
                }

                if ((JH_App.nSdStatus & JH_App.LocalRecording) != 0) {
                    JH_App.F_PlayStopRecord();
                    wifination.naStopRecord_All();
                    Photo_Record_Start_Btn.setBackgroundResource(R.mipmap.photo_record_icon_fly_jh);
                    // JH_App.F_Save2ToGallery(getActivity(), strRecordFilename,false);
                } else {
                    strRecordFilename = JH_App.F_GetSaveName(false);
                    JH_App.F_PlayStartRecord();
                    new Handler().postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            wifination.naStartRecord(strRecordFilename, wifination.TYPE_BOTH_PHONE_SD);
                        }
                    }, 800);
                    Record_Time_TextCtrl.setText("00:00");
                    Photo_Record_Start_Btn.setBackgroundResource(R.mipmap.photo_recording_icon_fly_jh);
                }
            }
        }

    }




    public  void F_Photo()
    {
        if ((JH_App.nSdStatus & JH_App.SD_SNAP) != 0)     //SD 拍照还没有完成。就不进行此次拍照
            return;

        if (JH_App.bPhone_SNAP)
            return;
        JH_App.bPhone_SNAP = true;

        final String str = JH_App.F_GetSaveName(true);
        wifination.naSnapPhoto(str, wifination.TYPE_BOTH_PHONE_SD);

        JH_App.F_PlayPhoto();
        photo_mask.setVisibility(View.VISIBLE);
        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                photo_mask.setVisibility(View.GONE);
            }
        }, 120);


        Handler handler = new Handler();
        Runnable runnable = new Runnable() {
            @Override
            public void run() {
                JH_App.bPhone_SNAP = false;
            }
        };
        handler.postDelayed(runnable, 800);
    }

    public void F_StartAdjRecord(boolean b) {

        if (b) {
            RecHander.removeCallbacksAndMessages(null);
            RecHander.post(RecRunnable);
        } else {
            RecHander.removeCallbacksAndMessages(null);
        }
    }

    @Override
    public void onResume() {
        super.onResume();
        F_StartAdjRecord(true);
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
            if ((JH_App.nSdStatus & JH_App.Status_Connected) != 0) {
                if (JH_App.bNeedStartsasyRecord) {
                    String str = JH_App.F_GetSaveName(false);
                    wifination.naSetRecordWH(1280,720);
                    wifination.naStartRecord(str, wifination.TYPE_ONLY_PHONE);
                    // JH_App.nRecTime = System.currentTimeMillis() / 1000;
                    Record_Time_TextCtrl.setText("00:00");
                    JH_App.bNeedStartsasyRecord = false;
                }
            }
            RecHander.postDelayed(this, 500);
        }
    };

    public void F_DispRcordIcon(Boolean bRecordding) {
        if (bRecordding) {
            Record_Time_TextCtrl.setVisibility(View.VISIBLE);

        } else {
            Record_Time_TextCtrl.setVisibility(View.INVISIBLE);
        }
    }

    public void F_SetPhoto(boolean b) {
        bPhoto = b;
        F_DispPhoto_Record();
    }

    public void F_SetMenuLeftRight(boolean b) {
        if (b) {
            tool_1_layout.setVisibility(View.VISIBLE);
            tool_2_layout.setVisibility(View.GONE);
        } else {
            tool_1_layout.setVisibility(View.GONE);
            tool_2_layout.setVisibility(View.VISIBLE);
        }

    }


    public void F_DispPhoto_Record() {

        if (bPhoto) {

            Photo_Record_Start_Btn.setBackgroundResource(R.mipmap.photo_icon_fly_jh);
            if ((JH_App.nSdStatus & JH_App.LocalRecording) != 0) {
                wifination.naStopRecord_All();
                Record_Time_TextCtrl.setVisibility(View.INVISIBLE);
            }

            /*
            if ((JH_App.nSdStatus & JH_App.LocalRecording) != 0)
            {
                Record_Time_TextCtrl.setVisibility(View.VISIBLE);
            }
            else
            {
                Record_Time_TextCtrl.setVisibility(View.INVISIBLE);
            }
            */


        } else {

            if ((JH_App.nSdStatus & JH_App.LocalRecording) != 0) {
                Record_Time_TextCtrl.setVisibility(View.VISIBLE);
                Photo_Record_Start_Btn.setBackgroundResource(R.mipmap.photo_recording_icon_fly_jh);
                myControl.F_SetFlyRecord(true);
                if (bControlUI) {
                    Fly_Camera_Btn.setBackgroundResource(R.mipmap.remote_control_fly_jh);
                } else {
                    Fly_Camera_Btn.setBackgroundResource(R.mipmap.no_remote_fly_jh);
                }


                Floder_Btn.setBackgroundResource(R.mipmap.folder_fly_jh);
                StopFly_Btn.setBackgroundResource(R.mipmap.stop_nor_fly_jh);
                UpDn_Btn.setBackgroundResource(R.mipmap.keyup_dn_fly_jh);
            } else {
                Photo_Record_Start_Btn.setBackgroundResource(R.mipmap.photo_record_icon_fly_jh);

                myControl.F_SetFlyRecord(false);

                if (bControlUI) {

                    Fly_Camera_Btn.setBackgroundResource(R.mipmap.remote_control_fly_jh);
                } else {

                    Fly_Camera_Btn.setBackgroundResource(R.mipmap.no_remote_fly_jh);
                }
                Floder_Btn.setBackgroundResource(R.mipmap.folder_fly_jh);
                StopFly_Btn.setBackgroundResource(R.mipmap.stop_nor_fly_jh);
                UpDn_Btn.setBackgroundResource(R.mipmap.keyup_dn_fly_jh);

            }
        }
    }

    public void F_StartStopSentCmd(boolean bStart) {
        if (bStart) {

            sentHander.removeCallbacksAndMessages(null);
            sentHander.post(sentRunnable);
        } else {
            sentHander.removeCallbacksAndMessages(null);
            //bCanSentCmd = false;
        }
    }

    public void F_StopSentCmd() {
        F_StartStopSentCmd(false);
    }

    public void F_DispUI() {
        F_DispHeadLessIcon();
        F_DispSpeedIcon();
        F_DispGSensorIcon();
        if (bControlUI) {
            //Fly_Camera_Btn.setBackgroundResource(R.mipmap.remote_control_fly_jh_b);
            Fly_Camera_Btn.setBackgroundResource(R.mipmap.remote_control_fly_jh);

            UpDn_Btn.setVisibility(View.VISIBLE);
            myControl.setVisibility(View.VISIBLE);
            StopFly_Btn.setVisibility(View.VISIBLE);

            F_StartStopSentCmd(true);

        } else {

            Fly_Camera_Btn.setBackgroundResource(R.mipmap.no_remote_fly_jh);

            UpDn_Btn.setVisibility(View.INVISIBLE);
            myControl.setVisibility(View.INVISIBLE);
            StopFly_Btn.setVisibility(View.INVISIBLE);


            F_StartStopSentCmd(false);
        }
        F_DispPhoto_Record();

    }

/*
    public  boolean  bCanSentCmd = true;
    private static class MyThread_SentCmd extends Thread {


        WeakReference<FlyPlayFragment> mThreadActivityRef;

        public MyThread_SentCmd(FlyPlayFragment activity) {
            mThreadActivityRef = new WeakReference<FlyPlayFragment>(
                    activity);
        }
        @Override
        public void run() {
            mThreadActivityRef.get().bCanSentCmd = true;
            while(mThreadActivityRef.get().bCanSentCmd)
            {
                mThreadActivityRef.get().F_SentCmd();
                try
                {
                    sleep(20);
                }
                catch (InterruptedException e)
                {
                    ;
                }
            }
            Log.e("Exit","Exit Sent Cmd");

        }
    }
*/


    private int nTestTemp;
    private Handler sentHander = new Handler();
    private Runnable sentRunnable = new Runnable() {
        @Override
        public void run() {
            nTestTemp++;
            F_SentCmd();
            sentHander.postDelayed(this, 20);
        }
    };

    byte[] cmd = new byte[20];

    private  void F_TestGpsCmd()
    {

    }

    private void F_SentCmd() {
        if (!bControlUI) {
            return;
        }

        //68 01 09 80 80 80 80 20 08 00 00 00 20

/*
        cmd[0] = (byte) 0x68;   //油门
        cmd[1] = (byte) 0x01;
        cmd[2] = (byte) 0x09;
        cmd[3] = (byte) 0x80;

        cmd[4] = (byte) 0x80;
        cmd[5] = (byte) 0x80;
        cmd[6] = (byte) 0x80;
        cmd[7] = (byte) 0x20;

        cmd[8] = (byte) 0x08;
        cmd[9] = (byte) 0x00;
        cmd[10] = (byte) 0x00;
        cmd[11] = (byte) 0x00;
        cmd[12] = (byte) 0x20;
        wifination.naSentCmd(cmd, 13);
        return;

*/


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

        //if(myControl.getVisibility()!=View.VISIBLE)
        //    return;


        if (JH_App.bisPathMode)
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
            da = 0x20;
        }


        cmd[5] = (byte) da;       //前后微调
        if (JH_App.bHiSpeed)
            cmd[5] |= 0x80;          //高速模式

        //cmd[5] |= 0x40;


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

        if (JH_App.bUp) {
            cmd[7] |= 0x40;
        }
        if (JH_App.bHeadLess) {
            cmd[7] |= 0x80;
        }

        cmd[8] = 0;

        if (JH_App.bStop) {
            cmd[8] |= 0x10;
        }
        if (JH_App.bAdj) {
            cmd[8] |= 0x20;
        }

        if (JH_App.bDn) {
            cmd[8] |= 0x08;
        }
        cmd[9] = (byte) (((cmd[0] ^ cmd[1] ^ cmd[2] ^ cmd[3] ^ cmd[4] ^ cmd[5] ^ cmd[6] ^ cmd[7] ^ cmd[8]) & 0xFF) + 0x55);
        wifination.naSentCmd(cmd, 10);
        //  Log.e("Cmd:  ","Sent NromalComd  X1=" +X1+" Y1="+Y1+" X2="+X2+" Y2="+Y2+"  temp="+nTestTemp);


        /*

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
        // wifination.naSentCmd(fly_cmd.cmd, 15);
        //  Log.e("SentCmd","Sent OK!");
        */
    }
}
