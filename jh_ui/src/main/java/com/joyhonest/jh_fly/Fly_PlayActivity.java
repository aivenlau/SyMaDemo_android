package com.joyhonest.jh_fly;

import android.Manifest;
import android.animation.ObjectAnimator;
import android.content.DialogInterface;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.BitmapDrawable;
import android.location.Location;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentTransaction;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.joyhonest.jh_ui.DispPhoto_Fragment;
import com.joyhonest.jh_ui.DispVideo_Fragment;
import com.joyhonest.jh_ui.JH_App;
import com.joyhonest.jh_ui.MyControl;
import com.joyhonest.jh_ui.MyFilesItem;
import com.joyhonest.jh_ui.MyItemData;
import com.joyhonest.jh_ui.PermissionAsker;
import com.joyhonest.wifination.R;
import com.joyhonest.wifination.JH_GLSurfaceView;
import com.joyhonest.wifination.MyThumb;
import com.joyhonest.wifination.jh_dowload_callback;
import com.joyhonest.wifination.wifination;

import org.simple.eventbus.EventBus;
import org.simple.eventbus.Subscriber;
import org.simple.eventbus.ThreadMode;
import org.tensorflow.Session;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.lang.ref.WeakReference;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

//import android.support.v4.FragmentManager;
//import android.support.v4.FragmentTransaction;

public class Fly_PlayActivity extends AppCompatActivity implements View.OnClickListener {

    private  int nDispStyle = 0;

    public static boolean bTestMode = false;

    public int nGetFileType = 0;

    private ImageView imageView4;


    private boolean bCancelDownLoadVideo = false;
    private boolean bCancelDownLoad = false;
    private boolean bCancelDownLoad_B = false;

    private Bitmap bmpThmb = null;
    private String bmpThmb_fileName = null;

    FragmentManager mFragmentMan;
    private RelativeLayout Fragment_Layout;
    //private SurfaceHolder surfaceHolder;
    //private SurfaceView surfaceView = null;
    public HandlerThread thread1;
    private Handler openHandler;

    private Handler RssiHander;
    private Runnable RssiRunable;


    private FlyPlayFragment flyPlayFragment;
    private BrowSelectFragment browSelectFragment;
    private BrowFilesFragment browFilesFragment;
    private DispVideo_Fragment dispVideo_fragment;
    private DispPhoto_Fragment dispPhoto_Fragment;
    private FlyPathFragment flyPathFragment;

    //private Path_Fragment path_fragment;


    private Fragment mActiveFragment = null;

    private Runnable openRunnable = new Runnable() {
        @Override
        public void run() {
            //  if(surfaceHolder!=null)
            //  JH_App.F_OpenStream(surfaceHolder.getSurface());
            nDispStyle = 0;
            wifination.naSetDispStyle(nDispStyle);
            JH_App.F_OpenStream();
        }
    };
    private PermissionAsker mAsker;

    //public JH_GLSurfaceView glSurfaceView;
    public  ImageView  DispImageV;

    private TextView  text_number_View;


    //private Handler   myHandler;
    //private Runnable   myRunnable;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        wifination.naSetCmdResType(1);
        wifination.appContext = getApplicationContext();
        wifination.naSetRecordAudio(JH_App.bRecordVoice);
//        wifination.naSetGesture(true,this.getApplicationContext());
        //wifination.naSetGPFps(17);
        wifination.naSetVrBackground(true);
        JH_App.bFlyDisableAll = true;
        JH_App.nType = JH_App.nStyle_fly;
        JH_App.F_InitMusic();




        //String str = wifination.naGetControlType();
        mAsker = new PermissionAsker(10, new Runnable() {
            @Override
            public void run() {
                setContentView(R.layout.activity_fly_play_jh);
                F_Init();
            }
        }, new Runnable() {
            @Override
            public void run() {
                F_DispAlert();

            }
        }).askPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.RECORD_AUDIO);

    }

    private void F_DispAlert() {
        AlertDialog dialog = new AlertDialog.Builder(this)
                .setTitle("Waring")
                .setMessage("The necessary permission denied, the application exit")
                .setNegativeButton("OK", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        finish();
                    }
                }).create();
        dialog.show();
    }


    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        mAsker.onRequestPermissionsResult(grantResults);
    }


    @Subscriber(tag = "GetWifiSendData")
    private void GetWifiSendData(byte[] cmd) {
        Log.e("GET Wifi Data", "" + cmd.length);
    }


    private void F_Init() {
        JH_App.bFlying = false;

        text_number_View = (TextView)findViewById(R.id.text_number_View);
        text_number_View.setVisibility(View.INVISIBLE);

        //glSurfaceView = (JH_GLSurfaceView) findViewById(R.id.glSurfaceView);
        //glSurfaceView.setVisibility(View.INVISIBLE);
        //imageView4 = (ImageView)findViewById(R.id.imageView4);
        //wifination.naSetRevBmp(true);

        DispImageV = findViewById(R.id.DispImageV);

        MyControl.bFlyType = true;

      //  wifination.F_AdjBackGround(this, R.mipmap.loginbackground_fly_jh);//R.mipmap.loginbackground_jh)
        JH_App.checkDeviceHasNavigationBar(this);
        JH_App.F_Clear_not_videoFiles();

        mFragmentMan = getSupportFragmentManager();//getFragmentManager();// getSupportFragmentManager();
        Fragment_Layout = (RelativeLayout) findViewById(R.id.Fragment_Layout);
        thread1 = new HandlerThread("MyHandlerThread_fly");
        thread1.start(); //创建一个HandlerThread并启动它
        openHandler = new Handler(thread1.getLooper());
        RssiHander = new Handler();
        RssiRunable = new Runnable() {
            @Override
            public void run() {
                int nrssi = JH_App.F_GetWifiRssi();
                {
                    if (flyPlayFragment != null) {
                        F_DispRssi(flyPlayFragment.WifiSingle, nrssi);
                    }
                    if (flyPathFragment != null) {
                        F_DispRssi(flyPathFragment.WifiSingle, nrssi);
                    }
                }
                RssiHander.postDelayed(this, 1000);

            }
        };

        RssiHander.postDelayed(RssiRunable, 100);
        JH_App.F_CreateLocalFlyDefalutDir();
        F_InitFragment();
        EventBus.getDefault().register(this);
    }

    @Subscriber(tag = "HideSurfaceView")
    private void HideSurfaceView(boolean bHide) {
        if (bHide) {
            //glSurfaceView.setVisibility(View.INVISIBLE);
            DispImageV.setVisibility(View.INVISIBLE);
        } else {
            //glSurfaceView.setVisibility(View.VISIBLE);
            DispImageV.setVisibility(View.VISIBLE);
        }
    }


//    @Subscriber(tag ="ReceiveBMP")
//    private void ReviceBMP(Bitmap bmp)
//    {
//        imageView4.setImageBitmap(bmp);
//    }

    @Subscriber(tag = "NeedEnable_123")
    private void NeedEnable_123(String str) {
        if (mActiveFragment == flyPlayFragment) {
            JH_App.bFlyDisableAll = false;
            flyPlayFragment.F_DispDisableAll();
        }
    }

    @Subscriber(tag = "SavePhotoOK")
    private void SavePhotoOK(String Sn) {
/*
        if (Sn.length() < 5) {
            return;
        }
        String sType = Sn.substring(0, 2);
        String sName = Sn.substring(2, Sn.length());
        int nPhoto = Integer.parseInt(sType);
        if (nPhoto == 0) {
           // JH_App.F_Save2ToGallery(sName, true);
            if(mActiveFragment == flyPathFragment)
            {
                flyPathFragment.F_DispMessage("snapshot");
            }
            if(mActiveFragment == flyPlayFragment) {
                flyPlayFragment.F_DispMessage("snapshot");
            }
        } else {
            ;
            //JH_App.F_Save2ToGallery(sName, false);
        }
*/
    }


    @Subscriber(tag = "GPS_LocationChanged")
    private void GPS_LocationChanged(Location location) {
        // TextView tv1;
        // tv1 = (TextView) this.findViewById(R.id.gps);

        if (location != null) {
            double latitude = location.getLatitude();
            double longitude = location.getLongitude();
            String str = "";
            if (latitude < -300 || longitude < -300) {

            } else {
                JH_App.nCheckt++;
                str = String.format(" 纬度：%f  经度 %f 次数 %d", latitude, longitude, JH_App.nCheckt);
            }

            if (flyPlayFragment != null)
                flyPlayFragment.F_SetLocaiotn(str);
        } else {
            //tv1.setText("无法获取地理信息");
        }
    }


    // 显示wifi信号强度
    private void F_DispRssi(ImageView imageView, int nRssi) {
        if (imageView == null)
            return;
        if (nRssi >= 3) {
            imageView.setBackgroundResource(R.mipmap.ic_wifi04);
        } else if (nRssi == 2) {
            imageView.setBackgroundResource(R.mipmap.ic_wifi03);
        } else if (nRssi == 1) {
            imageView.setBackgroundResource(R.mipmap.ic_wifi02);
        } else {
            imageView.setBackgroundResource(R.mipmap.ic_wifi01);
        }

    }

    private void F_OpenCamera(boolean b) {
        if (b) {

            new Handler().postDelayed(new Runnable() {
                @Override
                public void run() {
                    openHandler.removeCallbacksAndMessages(null);
                    openHandler.postDelayed(openRunnable, 25);
                }
            }, 30);

        } else {
            wifination.naStopRecord(wifination.TYPE_ONLY_PHONE);
            //wifination.naSetVideoSurface(null);
            wifination.naCancelDownload();
            wifination.naCancelGetThumb();
            wifination.naStop();
        }
    }


    @Subscriber(tag = "Exit2Spalsh")
    private void Exit2Spalsh(String str) {
       // F_OpenCamera(false);
        if (flyPlayFragment != null) {
            flyPlayFragment.F_StopSentCmd();
        }
        if (flyPathFragment != null)
            flyPathFragment.F_StopPaht();
        F_CancelDownLoad();
        JH_App.bHeadLess = false;
        JH_App.bVR = false;
        JH_App.bSensor = false;
        JH_App.bHiSpeed = false;
        finish();
    }


    @Override
    protected void onPause() {
        super.onPause();
        //sendCmdHandle.removeCallbacksAndMessages(null);
        //Exit2Spalsh("");
        if (flyPlayFragment != null) {
            flyPlayFragment.myControl.F_ReasetAll();
        }

    }

    @Override
    public void onBackPressed() {
        //  super.onBackPressed();
        EventBus.getDefault().post("Exoit", "Exit");

    }


    @Override
    public void onClick(View v) {

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        if (openHandler != null)
        {
            wifination.naStop();
            EventBus.getDefault().unregister(this);
            openHandler.removeCallbacksAndMessages(null);
            RssiHander.removeCallbacksAndMessages(null);
            flyPlayFragment.F_StopSentCmd();
            thread1.quit();
        }
    }

    private void hideFragments(FragmentTransaction ft) {
        Resources resources = this.getResources();
        DisplayMetrics dm = resources.getDisplayMetrics();
        int width = dm.widthPixels + 100;
        if (flyPlayFragment != null) {
            ObjectAnimator.ofFloat(flyPlayFragment.getView(), "Y", 0, 0).setDuration(1).start();
            ObjectAnimator.ofFloat(flyPlayFragment.getView(), "X", 0, 0).setDuration(1).start();
            ft.hide(flyPlayFragment);
        }
        if (browSelectFragment != null) {
            ObjectAnimator.ofFloat(browSelectFragment.getView(), "Y", 0, 0).setDuration(1).start();
            ObjectAnimator.ofFloat(browSelectFragment.getView(), "X", 0, 0).setDuration(1).start();
            ft.hide(browSelectFragment);
        }
        if (browFilesFragment != null) {
            ObjectAnimator.ofFloat(browFilesFragment.getView(), "Y", 0, 0).setDuration(1).start();
            ObjectAnimator.ofFloat(browFilesFragment.getView(), "X", 0, 0).setDuration(1).start();
            ft.hide(browFilesFragment);
        }
        if (dispVideo_fragment != null) {
            ObjectAnimator.ofFloat(dispVideo_fragment.getView(), "Y", 0, 0).setDuration(1).start();
            ObjectAnimator.ofFloat(dispVideo_fragment.getView(), "X", 0, 0).setDuration(1).start();
            ft.hide(dispVideo_fragment);
        }
        if (dispPhoto_Fragment != null) {
            ObjectAnimator.ofFloat(dispPhoto_Fragment.getView(), "Y", 0, 0).setDuration(1).start();
            ObjectAnimator.ofFloat(dispPhoto_Fragment.getView(), "X", 0, 0).setDuration(1).start();
            ft.hide(dispPhoto_Fragment);
        }

        if (flyPathFragment != null) {
            ObjectAnimator.ofFloat(flyPathFragment.getView(), "Y", 0, 0).setDuration(1).start();
            ObjectAnimator.ofFloat(flyPathFragment.getView(), "X", 0, 0).setDuration(1).start();
            ft.hide(flyPathFragment);
        }


    }

    @Subscriber(tag = "GoTo_Main")
    private void GoTo_Main(String str) {
        F_SetView(flyPlayFragment);
    }

    private void F_DispFramgent(Fragment fragment) {
        final int normal = 0;
        final int dn2up = 1;
        final int up2dn = 2;
        final int left2right = 3;
        final int right2left = 4;

        final int nDelay = 300;

        int type = normal;
        if (mActiveFragment == null) {
            type = normal;
        } else {
            type = right2left;
        }
        if (mActiveFragment == flyPlayFragment) {
            if (fragment == browSelectFragment)
                type = dn2up;
            //if(fragment == path_fragment)
            //    type = right2left;
        }
        if (mActiveFragment == browSelectFragment) {
            if (fragment == flyPlayFragment) {
                type = up2dn;
            } else {
                type = dn2up;
            }
        }
        if (mActiveFragment == browFilesFragment) {
            if (fragment == browSelectFragment) {
                type = up2dn;
            } else {
                type = dn2up;
            }
        }

        if (mActiveFragment == dispVideo_fragment) {
            if (fragment == browFilesFragment) {
                type = up2dn;
            } else {
                type = dn2up;
            }
        }

        if (mActiveFragment == dispPhoto_Fragment) {
            if (fragment == browFilesFragment) {
                type = up2dn;
            } else {
                type = dn2up;
            }
        }
        //    if(mActiveFragment == path_fragment)
        //    {
        //        type = left2right;
        //    }


        try {
            Thread.sleep(10);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }


        Resources resources = this.getResources();
        DisplayMetrics dm = resources.getDisplayMetrics();

        int width = dm.widthPixels + 200;
        int height = dm.heightPixels + 200;


        if (type == dn2up) {
            if (mActiveFragment != null) {
                ObjectAnimator.ofFloat(mActiveFragment.getView(), "Y", 0, 0 - height).setDuration(nDelay).start();
                ObjectAnimator.ofFloat(mActiveFragment.getView(), "X", 0, 0).setDuration(1).start();
            }
            ObjectAnimator.ofFloat(fragment.getView(), "Y", height, 0).setDuration(nDelay).start();
            ObjectAnimator.ofFloat(fragment.getView(), "X", 0, 0).setDuration(1).start();
        } else if (type == up2dn) {
            if (mActiveFragment != null) {
                ObjectAnimator.ofFloat(mActiveFragment.getView(), "Y", 0, height + 100).setDuration(nDelay).start();
                ObjectAnimator.ofFloat(mActiveFragment.getView(), "X", 0, 0).setDuration(1).start();
            }
            ObjectAnimator.ofFloat(fragment.getView(), "Y", 0 - height, 0).setDuration(nDelay).start();
            ObjectAnimator.ofFloat(fragment.getView(), "X", 0, 0).setDuration(1).start();
        } else if (type == left2right) {
            if (mActiveFragment != null) {
                ObjectAnimator.ofFloat(mActiveFragment.getView(), "X", 0, width).setDuration(nDelay).start();
                ObjectAnimator.ofFloat(mActiveFragment.getView(), "Y", 0, 0).setDuration(1).start();
            }
            ObjectAnimator.ofFloat(fragment.getView(), "X", 0 - width - 100, 0).setDuration(nDelay).start();
            ObjectAnimator.ofFloat(fragment.getView(), "Y", 0, 0).setDuration(1).start();
        } else if (type == right2left) {
            if (mActiveFragment != null) {
                ObjectAnimator.ofFloat(mActiveFragment.getView(), "X", 0, 0 - width).setDuration(nDelay).start();
                ObjectAnimator.ofFloat(mActiveFragment.getView(), "Y", 0, 0).setDuration(1).start();
            }
            ObjectAnimator.ofFloat(fragment.getView(), "X", width, 0).setDuration(nDelay).start();
            ObjectAnimator.ofFloat(fragment.getView(), "Y", 0, 0).setDuration(1).start();
        } else {
            if (mActiveFragment != null) {
                ObjectAnimator.ofFloat(mActiveFragment.getView(), "X", 0, 0).setDuration(1).start();
                ObjectAnimator.ofFloat(mActiveFragment.getView(), "Y", 0, 0).setDuration(1).start();
            }
            ObjectAnimator.ofFloat(fragment.getView(), "X", 0, 0).setDuration(1).start();
            ObjectAnimator.ofFloat(fragment.getView(), "Y", 0, 0).setDuration(1).start();

        }

        mActiveFragment = fragment;


        JH_App.bisPathMode = (mActiveFragment == flyPathFragment);


    }

    private void F_SetView(final Fragment fragment) {

        JH_App.checkDeviceHasNavigationBar(this);

        if (fragment == null) {
            mActiveFragment = null;
            Fragment_Layout.setVisibility(View.INVISIBLE);
            FragmentTransaction transaction = mFragmentMan.beginTransaction();
            hideFragments(transaction);
            transaction.commit();
            return;
        }
        if (mActiveFragment == fragment)
            return;

        if (fragment == flyPlayFragment) {
            flyPlayFragment.F_InitDisp();
        }

        Fragment_Layout.setVisibility(View.VISIBLE);

        if (mActiveFragment != flyPlayFragment && fragment == flyPlayFragment) {
            //surfaceView.setVisibility(View.VISIBLE);
            //wifination.naSetVideoSurface(surfaceHolder.getSurface());
        }


        if (fragment == dispVideo_fragment) {
            //surfaceView.setVisibility(View.INVISIBLE);
        }

        if (browFilesFragment == fragment) {
            browFilesFragment.F_DispInit();
        }

        if (fragment == flyPlayFragment) {
            flyPlayFragment.F_StartAdjRecord(true);
        } else {
            flyPlayFragment.F_StartAdjRecord(false);
        }


        if (fragment == browFilesFragment) {
            browFilesFragment.F_DispSelectSDorPhone();
        }
        if (fragment == browSelectFragment) {
            browSelectFragment.F_DispSelectSDorPhone();
        }


        FragmentTransaction transaction = mFragmentMan.beginTransaction();
        transaction.show(fragment);
        transaction.commit();
        mFragmentMan.executePendingTransactions();
        F_DispFramgent(fragment);
    }


    boolean bFirst = false;

    private void F_InitFragment() {
        flyPlayFragment = new FlyPlayFragment();
        browSelectFragment = new BrowSelectFragment();
        browFilesFragment = new BrowFilesFragment();
        dispVideo_fragment = new DispVideo_Fragment();
        dispPhoto_Fragment = new DispPhoto_Fragment();
        flyPathFragment = new FlyPathFragment();

        FragmentTransaction transaction = mFragmentMan.beginTransaction();
        transaction.add(R.id.Fragment_Layout, flyPlayFragment);
        transaction.add(R.id.Fragment_Layout, browSelectFragment);
        transaction.add(R.id.Fragment_Layout, browFilesFragment);
        transaction.add(R.id.Fragment_Layout, dispVideo_fragment);
        transaction.add(R.id.Fragment_Layout, dispPhoto_Fragment);
        transaction.add(R.id.Fragment_Layout, flyPathFragment);

        transaction.commit();

        mFragmentMan.executePendingTransactions();
        F_OpenCamera(true);
        bFirst = true;
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (bFirst) {
            bFirst = false;
            FragmentTransaction transactionA = mFragmentMan.beginTransaction();
            hideFragments(transactionA);
            transactionA.commit();
            dispVideo_fragment.F_SetBackImg(R.mipmap.return_icon_black_fly_jh);
            F_SetView(flyPlayFragment);
        }
    }

    @Subscriber(tag = "SDStatus_Changed")
    private void _SDStatus_Changed(Integer nStatus) {
        int backStatus = JH_App.nSdStatus;
        if ((nStatus & 0x01) != 0) {
            JH_App.nSdStatus |= JH_App.Status_Connected;
        } else {
            JH_App.nSdStatus &= ((JH_App.Status_Connected ^ 0xFFFF) & 0xFFFF);
        }
        if ((nStatus & JH_App.SD_Recording) != 0) {
            //main_fragment.F_DispRcordIcon(true);
            JH_App.nSdStatus |= JH_App.SD_Ready;
            JH_App.nSdStatus |= JH_App.SD_Recording;
        } else {
            //main_fragment.F_DispRcordIcon(false);
            JH_App.nSdStatus &= ((JH_App.SD_Recording ^ 0xFFFF) & 0xFFFF);
        }

        if ((nStatus & JH_App.SD_SNAP) != 0) {
            JH_App.nSdStatus |= JH_App.SD_Ready;

            if ((JH_App.nSdStatus & JH_App.SD_SNAP) == 0) {
                if (!JH_App.bPhone_SNAP) {
                    //  [self menu_Photo_Click:nil];
                }
            }
            JH_App.nSdStatus |= JH_App.SD_SNAP;

        } else {
            JH_App.nSdStatus &= ((JH_App.SD_SNAP ^ 0xFFFF) & 0xFFFF);
        }

        if ((nStatus & JH_App.SD_Ready) != 0) {
            JH_App.nSdStatus |= JH_App.SD_Ready;
        } else {
            JH_App.nSdStatus &= ((JH_App.SD_Ready ^ 0xFFFF) & 0xFFFF);
            JH_App.nSdStatus &= ((JH_App.SD_SNAP ^ 0xFFFF) & 0xFFFF);
            JH_App.nSdStatus &= ((JH_App.SD_Recording ^ 0xFFFF) & 0xFFFF);
        }

        if ((JH_App.nSdStatus & JH_App.SD_Ready) == 0) {
            JH_App.nSdStatus &= ((JH_App.SD_SNAP ^ 0xFFFF) & 0xFFFF);
            JH_App.nSdStatus &= ((JH_App.SD_Recording ^ 0xFFFF) & 0xFFFF);
        }


        if ((nStatus & JH_App.LocalRecording) != 0) {
            if ((JH_App.nSdStatus & JH_App.LocalRecording) == 0) {
                JH_App.nSdStatus |= JH_App.LocalRecording;
                if (flyPlayFragment != null) {
                    flyPlayFragment.F_DispPhoto_Record();
                }
            }
            JH_App.nSdStatus |= JH_App.LocalRecording;
        } else {
            if ((JH_App.nSdStatus & JH_App.LocalRecording) != 0) {
                JH_App.nSdStatus &= ((JH_App.LocalRecording ^ 0xFFFF) & 0xFFFF);
                if (flyPlayFragment != null) {
                    flyPlayFragment.F_DispPhoto_Record();
                }
            }
            JH_App.nSdStatus &= ((JH_App.LocalRecording ^ 0xFFFF) & 0xFFFF);
        }

/*
        if((JH_App.nSdStatus & JH_App.SD_Recording) !=0  && (backStatus & JH_App.SD_Recording) ==0) //SD已经开始录像
        {
           // NSLog(@"SD Record");
            if((JH_App.nSdStatus & JH_App.LocalRecording) !=0)    //本地已经开始录像
            {
                JH_App.bPhone_Video = false;
                return;
            }
            else
            {
                if(!JH_App.bPhone_Video)                    // 如果不是因为iphone 发出的命令来同步录像，就启动本地录像
                {
                    JH_App.bNeedStartsasyRecord = true;

                }
                JH_App.bPhone_Video = false;
            }
        }

        if((JH_App.nSdStatus & JH_App.SD_Recording) == 0  && (backStatus & JH_App.SD_Recording)!=0)    //SD 卡录像停止， 那么，也需要停止本地录像。
        {
            wifination.naStopRecord_All();
            main_fragment.F_DispRcordIcon(false);
        }
*/
        if ((JH_App.nSdStatus & JH_App.LocalRecording) != 0) {
            //[_Button_Video setBackgroundImage:[self imageNamed:@"Video_sel"] forState:UIControlStateNormal];
            flyPlayFragment.F_DispRcordIcon(true);
        } else {
            //[_Button_Video setBackgroundImage:[self imageNamed:@"Video_nor"] forState:UIControlStateNormal];
            flyPlayFragment.F_DispRcordIcon(false);
        }
        if ((JH_App.nSdStatus & JH_App.SD_Recording) != 0) {
            //[_Button_Video_SD setImage:self.Img_Recing forState:UIControlStateNormal];
        } else {
            //[_Button_Video_SD setImage:self.Img_No_Recing forState:UIControlStateNormal];
        }
        if ((JH_App.nSdStatus & JH_App.SD_Ready) != 0) {
            // [_Button_SD setImage:[UIImage imageNamed:@"sd"] forState:UIControlStateNormal];
        } else {
            //  [_Button_SD setImage:[UIImage imageNamed:@"no_sd"] forState:UIControlStateNormal];
        }

    }

    @Subscriber(tag = "MyControlTouched")
    private void MyControlTouched(String str) {
        if (mActiveFragment == flyPlayFragment) {
            flyPlayFragment.F_DispNoMoremenu();
        }
    }

    @Subscriber(tag = "SwitchChanged")
    private void SwitchChanged(SwitchMesage b) {
        if (mActiveFragment == flyPlayFragment) {
            if (b.mySwitch == flyPlayFragment.myswitch)
                flyPlayFragment.F_SetPhoto(b.bLeft);
            else {
                if (b.bLeft) {
                    flyPlayFragment.F_SetNoGsensor();
                }
                flyPlayFragment.F_SetMenuLeftRight(b.bLeft);
            }
        }

        if (mActiveFragment == flyPathFragment) {
            flyPathFragment.F_SetPhoto(b.bLeft);
        }
    }

    @Subscriber(tag = "PlayBtnMusic")
    private void _OnPlayBtnMusic(Integer nn) {
        int n = nn & 0x0F;
        if (n != 0) {
            JH_App.F_PlayCenter();
        } else {
            JH_App.F_PlayAdj();

        }
        /*
        if(main_fragment!=null) {
            JH_App.nAdjRota = main_fragment.myControl.F_GetRotateAdj();
            JH_App.nAdjForwardBack=main_fragment.myControl.F_GetForwardBackAdj();
            JH_App.nAdjLeftRight = main_fragment.myControl.F_GetLeftRightAdj();
            JH_App.F_ReadSaveSetting(true);
        }
        */

        n = nn & 0xFF00;
        if (n == 0) {                   //副翼等调整
            n = nn & 0xF0;
            if (n == 0x70)   //y--       //前后-
            {
            }
            if (n == 0x80)   //y++       //前后+
            {
            }
            if (n == 0x90)   //x--       //左右-
            {
            }
            if (n == 0xA0)   //x++       //左右+
            {
            }
        } else if (n == 0x1000) {
            n = nn & 0xF0;
            if (n == 0x90)   //x--           //旋转-
            {
            }
            if (n == 0xA0)   //x++           //旋转+
            {
            }
        }
//        AdjHandler.postDelayed(AdjRunable,200);   //连续发送 200ms
    }


    @Subscriber(tag = "DownloadFile")
    private void DownloadFile(jh_dowload_callback dowload) {
        if (JH_App.bBrowSD && !JH_App.bBrowPhoto) {
            for (MyItemData data : JH_App.mGridList) {
                if (data.sSDPath.compareToIgnoreCase(dowload.sFileName) == 0) {
                    data.fPrecent = dowload.nPercentage;
                    browFilesFragment.F_UpdateLisetViewData();
                }
            }
        }
    }

    @Subscriber(tag = "GetThumb")
    private void GetThumb(MyThumb thmb) {
        if (thmb.thumb != null) {
            bmpThmb = thmb.thumb;
            bmpThmb_fileName = thmb.sFilename;
        }
    }

    @Subscriber(tag = "GetFiles")
    private void GetFiles(String sfileName) {
        Log.e("GetFiles--", "GetFiles == " + sfileName);
        String[] temp = null;
        temp = sfileName.split("--");
        if (sfileName.startsWith("---")) {
            if (sfileName.startsWith("---End")) {
                if (nGetFileType == 0) {
                    nGetFileType = 1;
                } else {
                    if (nGetFileType == 1) {
                        F_FillFilesData();
                        // Select_Video_Photo_Fragment.F_Update_number(JH_App.mSD_PhotosList.size(), JH_App.mSD_VideosList.size());
                    }
                }
            }
            return;
        }
        if (temp.length == 2) {
            MyFilesItem sd_PhotoFile = new MyFilesItem();
            sd_PhotoFile.sSDPath = temp[0];
            Integer it = new Integer(temp[1]);
            sd_PhotoFile.nSize = it.intValue();
            if (nGetFileType == 0) {

                JH_App.mSD_PhotosList.add(sd_PhotoFile);
            } else {
                JH_App.mSD_VideosList.add(sd_PhotoFile);
            }
        }

    }

    /////////// 获取文件列表

    private class SortComparator implements Comparator<String> {
        @Override
        public int compare(String lhs, String rhs) {
            String str1 = lhs;
            String str2 = rhs;
            str1 = JH_App.getFileName(str1);
            str2 = JH_App.getFileName(str2);
            return str2.compareTo(str1);
        }
    }

    public void F_GetFilesNumber() {
        //wifination.naGetFiles()
        JH_App.mSD_PhotosList.clear();
        JH_App.mSD_VideosList.clear();

        JH_App.mLocal_PhotosList.clear();
        JH_App.mLocal_VideosList.clear();


        if (JH_App.bBrowSD) {
            MyThread_GetFileNumber thread_getFileNumber = new MyThread_GetFileNumber(this);
            thread_getFileNumber.start();
        } else {
            File f = new File(JH_App.sLocalPhoto);
            File[] files = f.listFiles();// 列出所有文件
            String fileName;
            for (File file : files) {
                if (file.exists() && !file.isDirectory()) {
                    fileName = file.getAbsolutePath();
                    fileName = fileName.toLowerCase();
                    if (fileName.endsWith(".jpg") || fileName.endsWith(".png")) {
                        JH_App.mLocal_PhotosList.add(fileName);
                    }
                }
            }


            Collections.sort(JH_App.mLocal_PhotosList, new SortComparator());

            f = new File(JH_App.sLocalVideo);
            files = f.listFiles();// 列出所有文件
            for (File file : files) {
                if (file.exists() && !file.isDirectory()) {
                    fileName = file.getAbsolutePath();
                    fileName = fileName.toLowerCase();
                    if (fileName.endsWith(".mp4")) {
                        JH_App.mLocal_VideosList.add(fileName);
                    } else {
                        file.delete();
                    }
                }
            }
            Collections.sort(JH_App.mLocal_VideosList, new SortComparator());
            //Select_Video_Photo_Fragment.F_Update_number(JH_App.mLocal_PhotosList.size(), JH_App.mLocal_VideosList.size());
        }
    }

    private static class MyThread_GetFileNumber extends Thread {

        WeakReference<Fly_PlayActivity> mThreadActivityRef;

        public MyThread_GetFileNumber(Fly_PlayActivity activity) {
            mThreadActivityRef = new WeakReference<Fly_PlayActivity>(
                    activity);
        }

        @Override
        public void run() {
            super.run();
            if (mThreadActivityRef == null)
                return;
            if (mThreadActivityRef.get() != null) {
                mThreadActivityRef.get().nGetFileType = 0;
                wifination.naGetFiles(0);
                try {
                    Thread.currentThread();
                    Thread.sleep(50);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                wifination.naGetFiles(1);
            }
        }
    }


    private static class MyThread_GetAllLocalFile extends Thread {
        WeakReference<Fly_PlayActivity> mThreadActivityRef;

        public MyThread_GetAllLocalFile(Fly_PlayActivity activity) {
            mThreadActivityRef = new WeakReference<Fly_PlayActivity>(
                    activity);
        }

        @Override
        public void run() {
            super.run();

            BitmapFactory.Options options = new BitmapFactory.Options();
            if (JH_App.bBrowPhoto) {
                for (String filename : JH_App.mLocal_PhotosList) {
                    MyItemData data = new MyItemData();
                    Bitmap bitmap = null;
                    FileInputStream fis = null;
                    try {
                        fis = new FileInputStream(filename);
                        options.inSampleSize = 5;
                        bitmap = BitmapFactory.decodeStream(fis, null, options);
                    } catch (FileNotFoundException e) {
                        e.printStackTrace();
                    }
                    if (bitmap != null)
                        bitmap = JH_App.F_AdjBitmp(bitmap);
                    data.image = bitmap;
                    data.sPhonePath = filename;
                    JH_App.mGridList.add(data);
                    mThreadActivityRef.get().runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            mThreadActivityRef.get().browFilesFragment.F_UpdateLisetView();
                        }
                    });

                    if (mThreadActivityRef.get().bCancelDownLoad_B)
                        break;
                }
            } else {
                for (String filename : JH_App.mLocal_VideosList) {
                    MyItemData data = new MyItemData();
                    Bitmap bitmap = null;
                    bitmap = JH_App.getVideoThumbnail(filename);
                    if (bitmap != null)
                        bitmap = JH_App.F_AdjBitmp(bitmap);
                    if (bitmap != null)
                        data.image = bitmap;
                    data.sPhonePath = filename;
                    JH_App.mGridList.add(data);
                    mThreadActivityRef.get().runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            mThreadActivityRef.get().browFilesFragment.F_UpdateLisetView();
                        }
                    });
                    if (mThreadActivityRef.get().bCancelDownLoad_B)
                        break;
                }
            }
        }
    }

    private void F_ReadAllSdVideos() {

        bCancelDownLoad = false;
        //MyThread_DownLoadThmb downLoad
        downLoad = new MyThread_DownLoadThmb(this);
        downLoad.start();
    }


    private static class MyThread_DownLoadThmb extends Thread {

        WeakReference<Fly_PlayActivity> mThreadActivityRef;

        public MyThread_DownLoadThmb(Fly_PlayActivity activity) {
            mThreadActivityRef = new WeakReference<Fly_PlayActivity>(
                    activity);
        }

        @Override
        public void run() {
            super.run();
            String sFileName = "";
            String sPhonePath = "";
            BitmapFactory.Options options = new BitmapFactory.Options();
            options.inSampleSize = 5;
            Bitmap bitmap = null;
            JH_App.bDownLoading = true;
            for (MyFilesItem filesItem : JH_App.mSD_VideosList) {
                if (mThreadActivityRef.get().bCancelDownLoad)
                    break;

                Log.v("message", "Thmb ---" + filesItem.sSDPath);
                bitmap = null;
                MyItemData data = new MyItemData();
                data.sSDPath = filesItem.sSDPath;
                sFileName = JH_App.getFileName(filesItem.sSDPath);
                sPhonePath = JH_App.sRemoteVideo + "/" + sFileName;
                if (JH_App.F_CheckFileIsExist(sPhonePath, filesItem.nSize)) {
                    bitmap = JH_App.getVideoThumbnail(sPhonePath);
                    {
                        data.nDownloadStatus = JH_App.DownLoaded;
                        data.fPrecent = 100;
                        data.sPhonePath = sPhonePath;
                        data.nDuration = JH_App.F_GetVideoCountTime(sPhonePath);
                        if (bitmap != null)
                            data.image = bitmap;
                    }
                } else {
                    mThreadActivityRef.get().bmpThmb = null;
                    mThreadActivityRef.get().bmpThmb_fileName = null;
                    wifination.naGetThumb(sFileName);
                    if (mThreadActivityRef.get().bCancelDownLoad)
                        break;
                    try {
                        Thread.currentThread();
                        Thread.sleep(80);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    if (mThreadActivityRef.get().bCancelDownLoad)
                        break;
                    if (mThreadActivityRef.get().bmpThmb == null) {
                        try {
                            Thread.currentThread();
                            Thread.sleep(50);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                        if (mThreadActivityRef.get().bmpThmb != null)
                            data.image = mThreadActivityRef.get().bmpThmb;
                    } else {
                        data.image = mThreadActivityRef.get().bmpThmb;
                    }
                    if (mThreadActivityRef.get().bmpThmb == null) {
                        mThreadActivityRef.get().bmpThmb_fileName = null;
                        wifination.naGetThumb(sFileName);
                        if (mThreadActivityRef.get().bCancelDownLoad)
                            break;
                        try {
                            Thread.currentThread();
                            Thread.sleep(80);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                        if (mThreadActivityRef.get().bCancelDownLoad)
                            break;
                        if (mThreadActivityRef.get().bmpThmb == null) {
                            try {
                                Thread.currentThread();
                                Thread.sleep(30);
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                            if (mThreadActivityRef.get().bCancelDownLoad)
                                break;
                            if (mThreadActivityRef.get().bmpThmb != null)
                                data.image = mThreadActivityRef.get().bmpThmb;
                        } else {
                            data.image = mThreadActivityRef.get().bmpThmb;
                        }
                    }
                }
                if (mThreadActivityRef.get().bCancelDownLoad)
                    break;
                JH_App.mGridList.add(data);
                mThreadActivityRef.get().runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        mThreadActivityRef.get().browFilesFragment.F_UpdateLisetView();
                    }
                });
                if (mThreadActivityRef.get().bCancelDownLoad)
                    break;
            }
            JH_App.bDownLoading = false;
            Log.e("Exit read thmb", "------Exit read thmb");
        }
    }

    private void F_DownLoadVideoThread(boolean bStart) {
        if (bStart) {
            bCancelDownLoadVideo = true;
            wifination.naCancelDownload();
            try {
                Thread.currentThread();
                Thread.sleep(60);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            JH_App.mDownloadList.clear();
            bCancelDownLoadVideo = false;
            MyThread_DownLoadVideo downLoadVideo = new MyThread_DownLoadVideo(this);
            downLoadVideo.start();
        } else {
            bCancelDownLoadVideo = true;
            wifination.naCancelDownload();
        }

    }

    private static class MyThread_DownLoadVideo extends Thread {
        WeakReference<Fly_PlayActivity> mThreadActivityRef;

        int inx;

        public MyThread_DownLoadVideo(Fly_PlayActivity activity) {
            mThreadActivityRef = new WeakReference<Fly_PlayActivity>(
                    activity);

        }

        @Override
        public void run() {
            super.run();
            while (!mThreadActivityRef.get().bCancelDownLoadVideo) {
                if (JH_App.mDownloadList.size() > 0) {
                    Integer ix = JH_App.mDownloadList.get(0);
                    inx = ix.intValue();

                    MyItemData data = null;
                    if (inx >= JH_App.mGridList.size()) {
                        JH_App.mDownloadList.remove(0);
                        continue;
                    }

                    data = JH_App.mGridList.get(inx);
                    data.nDownloadStatus = JH_App.DownLoading;
                    Log.v("message", "Download ---" + data.sSDPath);
                    String sFileName = JH_App.getFileName(data.sSDPath);
                    String sPhonePath = JH_App.sRemoteVideo + "/" + sFileName;


                    MyFilesItem filesItem = null;
                    if (inx >= JH_App.mSD_VideosList.size()) {
                        JH_App.mDownloadList.remove(0);
                        continue;
                    }
                    filesItem = JH_App.mSD_VideosList.get(inx);

                    Bitmap bitmap = null;
                    if (JH_App.F_CheckFileIsExist(sPhonePath, filesItem.nSize)) {
                        bitmap = JH_App.getVideoThumbnail(sPhonePath);
                        data.nDownloadStatus = JH_App.DownLoaded;
                        data.fPrecent = 100;
                        data.sPhonePath = sPhonePath;
                        int du = JH_App.F_GetVideoCountTime(sPhonePath);
                        data.nDuration = du;
                        if (bitmap != null)
                            data.image = bitmap;
                        continue;
                    }

                    wifination.naDownloadFile(data.sSDPath, sPhonePath);

                    bitmap = JH_App.getVideoThumbnail(sPhonePath);
                    //if (bitmap != null)
                    {
                        data.nDownloadStatus = JH_App.DownLoaded;
                        data.fPrecent = 100;

                        data.sPhonePath = sPhonePath;
                        int du = JH_App.F_GetVideoCountTime(sPhonePath);
                        data.nDuration = du;
                        if (bitmap != null)
                            data.image = bitmap;
                    }


                    mThreadActivityRef.get().runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            mThreadActivityRef.get().browFilesFragment.F_UpdateLisetView();
                        }
                    });
                    JH_App.mDownloadList.remove(0);
                }
                try {
                    Thread.currentThread();
                    Thread.sleep(30);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }

    }


    private void F_ReadAllSdPhotos() {

        bCancelDownLoad = false;
        MyThread_DownLoad downLoad = new MyThread_DownLoad(this);
        downLoad.start();
    }

    private static class MyThread_DownLoad extends Thread {

        WeakReference<Fly_PlayActivity> mThreadActivityRef;

        public MyThread_DownLoad(Fly_PlayActivity activity) {
            mThreadActivityRef = new WeakReference<Fly_PlayActivity>(
                    activity);
        }

        @Override
        public void run() {
            super.run();
            if (mThreadActivityRef == null)
                return;
            if (mThreadActivityRef.get() != null) {

                String sFileName = "";
                String sPhonePath = "";
                BitmapFactory.Options options = new BitmapFactory.Options();
                List<MyFilesItem> list = JH_App.mSD_PhotosList;
                String sLocalDir = JH_App.sRemotePhoto;
                if (!JH_App.bBrowPhoto) {
                    list = JH_App.mSD_VideosList;
                    sLocalDir = JH_App.sRemoteVideo;
                }

                for (MyFilesItem filesItem : list) {
                    if (mThreadActivityRef.get().bCancelDownLoad)
                        break;
                    Log.v("message", "Download ---" + filesItem.sSDPath);
                    MyItemData data = new MyItemData();
                    data.sSDPath = filesItem.sSDPath;
                    sFileName = JH_App.getFileName(filesItem.sSDPath);
                    sPhonePath = sLocalDir + "/" + sFileName;
                    if (JH_App.F_CheckFileIsExist(sPhonePath, filesItem.nSize)) {
                        Bitmap bitmap = null;
                        try {
                            if (JH_App.bBrowPhoto) {
                                FileInputStream fis = new FileInputStream(sPhonePath);
                                options.inSampleSize = 5;
                                bitmap = BitmapFactory.decodeStream(fis, null, options);
                                if (bitmap != null)
                                    bitmap = JH_App.F_AdjBitmp(bitmap);
                            } else {
                                bitmap = JH_App.getVideoThumbnail(sPhonePath);
                            }
                        } catch (FileNotFoundException e) {
                            e.printStackTrace();
                        }
                        if (bitmap != null) {
                            data.nDownloadStatus = JH_App.DownLoaded;
                            data.fPrecent = 100;
                            data.image = bitmap;
                            data.sPhonePath = sPhonePath;
                        }
                    } else {
                        wifination.naDownloadFile(filesItem.sSDPath, sPhonePath);
                        if (JH_App.F_CheckFileIsExist(sPhonePath, filesItem.nSize)) {
                            Bitmap bitmap = null;
                            try {
                                if (JH_App.bBrowPhoto) {
                                    FileInputStream fis = new FileInputStream(sPhonePath);
                                    options.inSampleSize = 5;
                                    bitmap = BitmapFactory.decodeStream(fis, null, options);
                                    fis = null;
                                } else {
                                    bitmap = JH_App.getVideoThumbnail(sPhonePath);
                                }
                            } catch (FileNotFoundException e) {
                                e.printStackTrace();
                            }
                            if (bitmap != null) {
                                data.nDownloadStatus = JH_App.DownLoaded;
                                data.fPrecent = 100;
                                data.image = bitmap;
                                data.sPhonePath = sPhonePath;
                            }
                        } else {
                            File file = new File(sPhonePath);
                            if (file.exists() && !file.isDirectory()) {
                                file.delete();
                            }
                        }
                    }
                    JH_App.mGridList.add(data);
                    mThreadActivityRef.get().runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            mThreadActivityRef.get().browFilesFragment.F_UpdateLisetView();
                        }
                    });
                    if (mThreadActivityRef.get().bCancelDownLoad)
                        break;
                }

            }
        }
    }

    public void F_FillFilesData() {


        for (MyItemData itemData : JH_App.mGridList) {
            itemData.image = null;
            itemData.sSDPath = null;
            itemData.sPhonePath = null;
            itemData = null;
        }
        JH_App.mGridList.clear();
        browFilesFragment.F_UpdateLisetView();

        if (JH_App.bBrowSD) {
            if (JH_App.bBrowPhoto) {
                F_ReadAllSdPhotos();
            } else   //SD Video
            {
                F_DownLoadVideoThread(true);
                F_ReadAllSdVideos();
            }
        } else {
            bCancelDownLoad_B = false;
            locall = new MyThread_GetAllLocalFile(this);
            locall.start();
        }
    }

    MyThread_GetAllLocalFile locall = null;
    MyThread_DownLoadThmb downLoad = null;

    public void F_CancelDownLoad() {
        bCancelDownLoad = true;
        bCancelDownLoad_B = true;
        if (downLoad != null) {
            try {
                downLoad.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            downLoad = null;

        }
        if (locall != null) {
            try {
                locall.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            locall = null;
        }
        F_DownLoadVideoThread(false);
        wifination.naCancelDownload();
        wifination.naCancelGetThumb();
    }


    @Subscriber(tag = "gotoFragment")
    private void gotoFragment(Integer nFragment) {
        if (nFragment == JH_Fly_Setting.Brow_Select_Fragment) {
            if (mActiveFragment == flyPlayFragment) {
                JH_App.bBrowSD = false;
            }
            F_SetView(browSelectFragment);
            //F_GetFilesNumber();
            //F_FillFilesData();
            return;
        }
        if (nFragment == JH_Fly_Setting.Brow_Files_Fragment) {
            F_SetView(browFilesFragment);
            //JH_App.bBrowSD=false;
            // F_FillFilesData();
            return;
        }
        if (nFragment == JH_Fly_Setting.Play_Fragment) {
            F_SetView(flyPlayFragment);
            //JH_App.bBrowSD=false;
            //F_GetFilesNumber();
            return;
        }
    }

    @Subscriber(tag = "Return_Back")
    private void Return_Back(Integer i) {
        F_Exit("");
    }

    @Subscriber(tag = "GotoPath")
    private void GotoPath(String str) {
        F_SetView(flyPathFragment);
        flyPathFragment.F_DispOpenEye(true);
        flyPathFragment.F_StartPath();

    }

    @Subscriber(tag = "Exit")
    private void F_Exit(String str) {
        if (mActiveFragment == null) {
            Exit2Spalsh(str);
            return;
        }
        if (mActiveFragment == flyPlayFragment) {
            if (JH_App.bVR) {
                JH_App.bVR = false;
                flyPlayFragment.F_Disp3DUI();
                flyPlayFragment.F_DispUI();
                flyPlayFragment.F_DispSpeedIcon();
                flyPlayFragment.F_DispGSensorIcon();

                return;
            }
            Exit2Spalsh(str);
            return;
        }
        if (mActiveFragment == browSelectFragment) {
            F_CancelDownLoad();
            gotoFragment(JH_Fly_Setting.Play_Fragment);
            return;
        }
        if (mActiveFragment == browFilesFragment) {
            if (browFilesFragment.bCannSelected) {

                browFilesFragment.F_DispInit();
                return;

            }
            F_CancelDownLoad();
            gotoFragment(JH_Fly_Setting.Brow_Select_Fragment);
            return;
        }
        if (mActiveFragment == dispVideo_fragment) {
            dispVideo_fragment.F_Stop();
            //glSurfaceView.setVisibility(View.VISIBLE);
            DispImageV.setVisibility(View.VISIBLE);
            gotoFragment(JH_Fly_Setting.Brow_Files_Fragment);

        }

        if (mActiveFragment == dispPhoto_Fragment) {
            gotoFragment(JH_Fly_Setting.Brow_Files_Fragment);
        }
    }


   // @Subscriber(tag = "ReceiveBMP",mode = ThreadMode.ASYNC)
//   private void ReceiveBMP(final Bitmap bmp)
//   {
//       runOnUiThread(new Runnable() {
//           @Override
//           public void run() {
//               DispImageV.setImageBitmap(bmp);
//           }
//       });
//
//   }
   @Subscriber(tag = "ReceiveBMP")
    private void ReceiveBMP(final Bitmap bmp)
    {

        //DispImageV.setImageBitmap(bmp);
        DispImageV.setBackground(new BitmapDrawable(this.getResources(), bmp));
       // DispImageV.setImageDrawable(new BitmapDrawable(this.getResources(), bmp));

    }

    @Subscriber(tag = "Grid_Delete")
    private void Grid_Delete(String str) {

        MyThread_Delete delete = new MyThread_Delete(this);
        delete.start();


    }


    public boolean bDeleteing = false;


    private static class MyThread_Delete extends Thread {


        WeakReference<Fly_PlayActivity> mThreadActivityRef;

        public MyThread_Delete(Fly_PlayActivity activity) {
            mThreadActivityRef = new WeakReference<Fly_PlayActivity>(
                    activity);
        }

        @Override
        public void run() {
            super.run();
            int inx = 0;
            if (mThreadActivityRef.get().bDeleteing) {
                return;
            }
            mThreadActivityRef.get().bDeleteing = true;

            for (MyItemData data : JH_App.mGridList) {
                if (data.bSelected) {
                    if (JH_App.bBrowSD) {
                        wifination.naDeleteSDFile(data.sSDPath);
                        File file = new File(data.sPhonePath);
                        if (file.isFile() && file.exists()) {
                            file.delete();
                        }

                        try {
                            Thread.sleep(100);
                        } catch (InterruptedException e) {

                        }


                    } else {
                        JH_App.DeleteImage(data.sPhonePath);
                    }
                    data.bNeedDelete = true;
                }
                inx++;
            }

            String sdFileName;
            String sLocalFileName;
            String sFileName;

            while (true) {
                inx = 0;
                for (MyItemData data : JH_App.mGridList) {
                    if (data.bNeedDelete) {
                        JH_App.mGridList.remove(inx);
                        if (JH_App.bBrowSD) {
                            if (JH_App.bBrowPhoto) {

                                if (inx < JH_App.mSD_PhotosList.size()) {
                                    JH_App.mSD_PhotosList.remove(inx);
                                }

                            } else {


                                if (inx < JH_App.mSD_VideosList.size()) {
                                    JH_App.mSD_VideosList.remove(inx);
                                }
                            }
                        } else {
                            if (JH_App.bBrowPhoto) {

                                if (inx < JH_App.mLocal_PhotosList.size()) {
                                    JH_App.mLocal_PhotosList.remove(inx);
                                }

                            } else {


                                if (inx < JH_App.mLocal_VideosList.size()) {
                                    JH_App.mLocal_VideosList.remove(inx);
                                }
                            }
                        }
                        mThreadActivityRef.get().runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                mThreadActivityRef.get().browFilesFragment.F_UpdateLisetViewData();
                            }
                        });
                        break;
                    }
                    inx++;
                }
                try {
                    Thread.currentThread();
                    Thread.sleep(30);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                if (inx >= JH_App.mGridList.size())
                    break;
            }
            mThreadActivityRef.get().bDeleteing = false;
            mThreadActivityRef.get().runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    mThreadActivityRef.get().browFilesFragment.F_DispInit();


                }
            });
        }
    }


    @Subscriber(tag = "GridItem_Click")
    private void GridItem_Click(Integer ixx) {
        if (mActiveFragment != browFilesFragment) {
            return;
        }
        final int ix = ixx;
        int inx = 0;
        int inxA = 0;
        int nSelect = 0;
        if (JH_App.bBrowSD) {
            if (JH_App.bBrowPhoto) {
                JH_App.mDispList.clear();
                inx = 0;
                inxA = 0;

                for (MyItemData data : JH_App.mGridList) {
                    if (data.nDownloadStatus == JH_App.DownLoaded) {
                        JH_App.mDispList.add(data.sPhonePath);
                        if (ix == inx) {
                            nSelect = inxA;
                        }
                        inxA++;
                    }
                    inx++;
                }
                dispPhoto_Fragment.UpdateData(nSelect);
                F_SetView(dispPhoto_Fragment);
            } else {
                MyItemData data = JH_App.mGridList.get(ix);
                if (data.nDownloadStatus == JH_App.DownLoaded) {
                    dispVideo_fragment.F_Play(data.sPhonePath);
                    F_SetView(dispVideo_fragment);

                } else if (data.nDownloadStatus == JH_App.DownLoading) {
                } else if (data.nDownloadStatus == JH_App.DownLoaded_NO ||
                        data.nDownloadStatus == JH_App.DownLoadNormal)

                {

                    Integer x = ix;
                    data.nDownloadStatus = JH_App.DownLoadNeed;
                    JH_App.mDownloadList.add(x);
                    browFilesFragment.F_UpdateLisetViewData();

                }

            }
        } else {
            if (JH_App.bBrowPhoto) {
                JH_App.mDispList.clear();
                inx = 0;

                inxA = 0;
                nSelect = 0;

                for (MyItemData data : JH_App.mGridList) {
                    {
                        JH_App.mDispList.add(data.sPhonePath);
                        if (ix == inx) {
                            nSelect = inxA;
                        }
                        inxA++;
                    }
                    inx++;
                }
                dispPhoto_Fragment.UpdateData(nSelect);
                F_SetView(dispPhoto_Fragment);
            } else {


                MyItemData data = JH_App.mGridList.get(ix);
                dispVideo_fragment.F_Play(data.sPhonePath);
                F_SetView(dispVideo_fragment);
                //glSurfaceView.setVisibility(View.INVISIBLE);
                DispImageV.setVisibility(View.INVISIBLE);

            }
        }
    }

    @Subscriber(tag="OnGetSetStyle")
    private void OnGetSetStyle(Integer nStyle)
    {
           int nT = nStyle.intValue();
           if(nT>0)
           {
               nT--;
               nDispStyle = nT;
               wifination.naSetDispStyle(nDispStyle);
           }
    }

    @Subscriber(tag = "key_Press")
    private  void KeyPress(Integer nkeya)
    {
        Log.e("Key Press","key="+nkeya.toString());
        int nKey=nkeya.intValue();
        if(nKey ==1)  //photo
        {
            if (mActiveFragment == flyPlayFragment) {
                flyPlayFragment.F_Photo();
            }
        }
        /*
        if(nKey == 6)
        {
            nDispStyle++;
            if(nDispStyle>6)
                nDispStyle = 0;
            wifination.naSetDispStyle(nDispStyle);
        }
        */
    }

    int nKeyN=0;
    @Subscriber(tag="GetGueset")
    private void GetGueset(String strID)
    {
       // if(strID.length()!=0)
       //     Log.e("GetGueset","GetGueset = "+strID);
        if(strID.equalsIgnoreCase("D2"))
        {
            nKeyN++;
            if(nKeyN>5)
            {
                nKeyN=5;
            }
            if(nKeyN==2)
            {
                //F_Photo();
            }
        }
        else
        {
            nKeyN=0;
        }
    }


    private  int nInxStep = 1;
    private  Handler phtot_handler = new Handler();
    private Runnable phtot_Run = new Runnable() {
        @Override
        public void run() {

            text_number_View.setText(""+nInxStep);
            nInxStep++;
            if(nInxStep<5)
            {
                phtot_handler.postDelayed(this,1000);
            }
            else
            {
                text_number_View.setVisibility(View.INVISIBLE);
                if (mActiveFragment == flyPlayFragment)
                {
                    flyPlayFragment.F_Photo();
                }

            }
        }
    };

    private  void F_Photo()
    {
        if (mActiveFragment == flyPlayFragment) {
            nInxStep = 1;
            text_number_View.setVisibility(View.VISIBLE);
            phtot_handler.removeCallbacksAndMessages(null);
            phtot_handler.post(phtot_Run);
        }

    }


}
