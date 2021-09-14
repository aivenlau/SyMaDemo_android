package com.joyhonest.jh_ui;

import android.animation.Animator;
import android.animation.AnimatorSet;
import android.animation.ObjectAnimator;
import android.animation.ValueAnimator;
import android.app.Activity;
import android.content.Context;
import android.content.pm.ActivityInfo;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.PathMeasure;
import android.graphics.Point;
import android.graphics.RectF;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Build;
import android.os.Handler;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.LinearInterpolator;
import android.view.animation.PathInterpolator;
import android.widget.ImageView;
import android.widget.RelativeLayout;

import com.joyhonest.wifination.GP4225_Device;
import com.joyhonest.wifination.JH_GLSurfaceView;
import com.joyhonest.wifination.R;

import org.simple.eventbus.EventBus;
import org.simple.eventbus.Subscriber;

import java.util.ArrayList;

/**
 * Created by aivenlau on 2017/2/23.
 */


public class MyControl extends RelativeLayout implements SensorEventListener {

    public static boolean bFlyType = true;
    public boolean bDisText = true;
    public MyRockeViewA RockeLeft;         //左操作盘
    private MyRockeViewA RockeRight;       //右操作盘
    private MyRockeViewA RockeLeftA;
    private MyRockeViewA RockeRightA;
    public PathViewA pathView;

    private  MyPrivView           leftView;
    private int nCentWidth;
    private int nRockWidth;

    public boolean bGravity = false;
    private int nMode = 0;
    public boolean bHoldHeight = true;


    private SensorManager mSensorManager;
    private Sensor mSensor;
    private float[] gravity;


    LayoutParams params1;
    LayoutParams params2;
    LayoutParams params3;

    LayoutParams params_leftview;


    public  void  F_Invalidate()
    {
        RockeLeft.invalidate();
        RockeRight.invalidate();
    }


    public void F_ReasetAll()
    {
        RockeLeftA.F_ReasetDir();
        RockeRightA.F_ReasetDir();

    }

    public void F_SetFlyRecord(boolean b)
    {
        if(RockeLeft!=null)
        {
            RockeLeft.F_SetFlyRecord(b);
        }
        if(RockeRight!=null)
        {
            RockeRight.F_SetFlyRecord(b);
        }
    }


    public void F_SetDispText(boolean b) {
        if (b) {
            if (RockeRight != null) {
                RockeLeft.Str1 = "THRO";
                RockeLeft.Str2 = "RUDO";
            }

            if (RockeRight != null) {
                RockeRight.Str1 = "ELEV";
                RockeRight.Str2 = "AILE";
            }

        } else {
            if (RockeLeftA != null) {
                RockeLeftA.Str1 = "";
                RockeLeftA.Str2 = "";

            }
            if (RockeRightA != null) {
                RockeRightA.Str1 = "";
                RockeRightA.Str2 = "";
            }
        }

        RockeLeftA.invalidate();
        RockeRightA.invalidate();
    }


    public MyControl(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public MyControl(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        init();
    }

    public MyControl(Context context) {
        super(context);
        init();
    }

    public void F_SetSize(MyRockeViewA ctl) {

        RockeLeft.CirRadius = ctl.CirRadius;
        RockeLeft.Radius = ctl.Radius;
        RockeLeft.Radius_V1 = ctl.Radius_V1;

        RockeLeft.centx = ctl.centx;

        RockeLeft.centy = ctl.centy;

        RockeLeft.cirPoint.x = ctl.cirPoint.x;

        RockeLeft.cirPoint.y = ctl.cirPoint.y;

        RockeLeft.F_ReasetEngine();


    }


    public void F_SetNoDispVBar(boolean b) {
        RockeLeft.bDispVbar = !b;
        RockeRight.bDispVbar = !b;
        RockeLeft.invalidate();
        RockeRight.invalidate();


    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        if (event.sensor == null) {
            return;
        }
        int dat = 60;
        if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
            // Log.v("abc   ","x="+event.values[0]+"   y="+event.values[1]+"   z="+event.values[2]);
            final float alpha = 0.95f;
            // 用低通滤波器分离出重力加速度
            gravity[0] = alpha * gravity[0] + (1 - alpha) * event.values[0];
            gravity[1] = alpha * gravity[1] + (1 - alpha) * event.values[1];
            gravity[2] = alpha * gravity[2] + (1 - alpha) * event.values[2];


            // 用高通滤波器剔除重力干扰
            //gravity[0] = event.values[0] - gravity[0];
            //gravity[1] = event.values[1] - gravity[1];
            //gravity[2] = event.values[2] - gravity[2];

            int x = (int) (gravity[0] * dat);
            int y = (int) (gravity[1] * dat);
            int z = (int) (gravity[2] * dat);
            if (bGravity) {
                RockeLeft.F_Calculator_Acceleration(x, y, z);
                RockeRight.F_Calculator_Acceleration(x, y, z);
            }
        } else if (event.sensor.getType() == Sensor.TYPE_GRAVITY) {
            int x = (int) (event.values[0] * dat);
            int y = (int) (event.values[1] * dat);
            int z = (int) (event.values[2] * dat);
            if (bGravity) {
                RockeLeft.F_Calculator_Acceleration(x, y, z);
                RockeRight.F_Calculator_Acceleration(x, y, z);
            }
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {

    }


    Bitmap background;
    Bitmap cirBmp;




    public void F_SetImage(int nBackgroud, int nCir)
    {
        background = BitmapFactory.decodeResource(this.getContext().getResources(), nBackgroud);
        cirBmp = BitmapFactory.decodeResource(this.getContext().getResources(), nCir);
        RockeLeftA.F_SetImage(background, cirBmp);
        RockeRightA.F_SetImage(background, cirBmp);
        invalidate();
    }

    int topPx;
    int botPx;

    private  Bitmap bbitbmp; //= BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.cir_back_fly_jh_fig);

    private void init() {
        setWillNotDraw(false);
        Context context = this.getContext();
        bbitbmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.cir_back_fly_jh_fig);

        if (bFlyType) {
            topPx = Storage.dip2px(context, 40);
            botPx = Storage.dip2px(context, 40);
        } else {
            topPx = Storage.dip2px(context, 30);
            botPx = Storage.dip2px(context, 30);
        }

        background = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.cir_back_jh);
        cirBmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.cir_jh);

        bHoldHeight = true;
        bGravity = false;
        gravity = new float[3];
        gravity[0] = 0;
        gravity[1] = 0;
        gravity[2] = 0;

        nCentWidth = dip2px(context, 10);
        nRockWidth = dip2px(context, 60);

        RockeLeftA = new MyRockeViewA(context, background, cirBmp, bFlyType);
        RockeRightA = new MyRockeViewA(context, background, cirBmp, bFlyType);

        RockeLeftA.bLayoutLeft = false;
        RockeRightA.bLayoutLeft = true;


        RockeLeft = RockeLeftA;
        RockeRight = RockeRightA;


        RockeLeft.Str1 = "THRO";
        RockeLeft.Str2 = "RUDO";

        RockeRight.Str1 = "ELEV";
        RockeRight.Str2 = "AILE";

        pathView = new PathViewA(context);
        leftView = new MyPrivView(context);

        params1 = new LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT);

        params2 = new LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT);
        params3 = new LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT);



        this.addView(pathView, params1);
        pathView.setVisibility(INVISIBLE);
        pathView.setBackgroundColor(0x01000000);

        params_leftview = new LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT);
        this.addView(leftView, params_leftview);
        leftView.setVisibility(VISIBLE);
        leftView.setBackgroundColor(0x01000000);


        params2.addRule(RelativeLayout.ALIGN_PARENT_LEFT);
       // params2.setMargins(-1, topPx, -1, botPx);
        params2.setMargins(-1, -1, -1, -1);
        this.addView(RockeLeft, params2);
       // RockeLeft.setBackgroundColor(0x50FF0000);


        params3.addRule(RelativeLayout.ALIGN_PARENT_RIGHT);
       // params3.setMargins(-1, topPx, -1, botPx);
        params3.setMargins(-1, -1, -1, -1);
        this.addView(RockeRight, params3);
        //RockeRight.setBackgroundColor(0xFF00FF00);

        RockeLeft.bLeft = true;
        RockeRight.bLeft = false;
        F_SetGravity(true);
        F_SetMode(0);
        setBackgroundColor(0x01000000);




    }
    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        int width = MeasureSpec.getSize(widthMeasureSpec);
        int WW = width;
        int height = MeasureSpec.getSize(heightMeasureSpec);
        setMeasuredDimension(width, height);
   //     int sdd = width;

     //   int swww = width / 2;
        if (width > nCentWidth) {
            width -= nCentWidth;
          //  swww -= (nCentWidth / 2);
        }
        width /= 2;

        nRockWidth = width;

        params2.width = WW/2;
        RockeLeftA.setLayoutParams(params2);

        params3.width = WW/2;
        RockeRightA.setLayoutParams(params3);

        params_leftview.width=nRockWidth;
        leftView.setLayoutParams(params_leftview);

    }



    public void F_SetAdjType1(boolean bType1) {
        RockeLeft.bAdjMode1 = bType1;
        RockeRight.bAdjMode1 = bType1;
    }


    public void F_SetHoldHeight(boolean b) {
        int nType;
        bHoldHeight = b;
        if (!bHoldHeight) {
            nType = RockeLeft.nType;
            nType &= ((MyRockeViewA.TYPE_RestCentY) ^ 0xFF);
            RockeLeft.nType = nType;
        } else {
            nType = RockeLeft.nType;
            nType |= (MyRockeViewA.TYPE_RestCentY);
            RockeLeft.nType = nType;
        }
        nType = RockeRight.nType;
        nType |= (MyRockeViewA.TYPE_RestCentY);
        RockeRight.nType = nType;
        RockeLeft.F_ReasetEngine();
        RockeRight.F_ReasetEngine();

    }

    private void F_initSensor() {
        Context context = this.getContext();
        mSensorManager = (SensorManager) (context.getSystemService(Activity.SENSOR_SERVICE));
        if (null == mSensorManager) {
            Log.d("PlayerActivity", "deveice not support SensorManager");
        } else {
            mSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);//TYPE_ACCELEROMETER);// TYPE_GRAVITY
            if (mSensor == null) {
                mSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_GRAVITY);   //TYPE_ACCELEROMETER);// TYPE_GRAVITY
            }
        }

    }

    public void F_SetGravity(boolean b) {
        if (isInEditMode()) {
            return;
        }
        if (mSensorManager == null)
            F_initSensor();
        if (b) {
            if (!bGravity) {
                mSensorManager.registerListener(this, mSensor, SensorManager.SENSOR_DELAY_FASTEST);  // SENSOR_DELAY_GAME
                bGravity = true;
            }
        } else {
            mSensorManager.unregisterListener(this);
            bGravity = false;
        }
    }

    public int F_GetRotate() {
        int x = -1;
        switch (nMode) {
            case 0:
            case 1:
            case 2:
            case 3:
                x = RockeLeft.F_GetX();
                break;
            case 4:
            case 5:
                x = RockeRight.F_GetX();
                break;
        }
        if (x < 2)
            x = 0;
        return x;
    }

    public int F_GetThrottle() {
        int y = -1;
        switch (nMode) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
                y = RockeLeft.F_GetY();
                break;
        }
        if (y < 2)
            y = 0;
        return y;
    }


    public int F_GetForwardBack() {
        int data = -1;
        if (pathView.getVisibility() == VISIBLE)// && pathView.bFlight)
        {
            if (pathView.bFlight)
                return pathView.nY2;
            else
                return 0x80;
        } else {
            switch (nMode) {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                    data = RockeRight.F_GetY();
                    break;
            }
        }
        if (data > 0x70 && data < 0x90) {
            data = 0x80;
        }
        if (data < 2)
            data = 0;
        return data;
    }

    public int F_GetLeftRight() {
        int data = -1;
        if (pathView.getVisibility() == VISIBLE) {
            if (pathView.bFlight)
                return pathView.nX2;
            else
                return 0x80;
        } else {
            switch (nMode) {
                case 0:
                case 1:
                case 2:
                case 3:
                    data = RockeRight.F_GetX();
                    break;
                case 4:
                case 5:
                    data = RockeLeft.F_GetX();
                    break;
            }
        }
        if (data > 0x70 && data < 0x90) {
            data = 0x80;
        }
        if (data < 2)
            data = 0;
        return data;
    }


    public void F_SetRotateAdj(int n) {
        switch (nMode) {
            case 0:
            case 1:
            case 2:
            case 3:
                RockeLeft.F_SetAdjX(n);
                break;
            case 4:
            case 5:
                RockeRight.F_SetAdjX(n);
                break;
        }
    }

    public int F_GetRotateAdj() {
        int dat = -1;
        switch (nMode) {
            case 0:
            case 1:
            case 2:
            case 3:
                dat = RockeLeft.F_GetAdjX();
                break;
            case 4:
            case 5:
                dat = RockeRight.F_GetAdjX();
        }
        return dat;
    }

    public void F_SetForwardBackAdj(int n) {
        RockeRight.F_SetAdjY(n);
    }

    public int F_GetForwardBackAdj() {
        int dat = -1;
        switch (nMode) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
                dat = RockeRight.F_GetAdjY();
                break;
        }
        return dat;
    }

    public void F_SetLeftRightAdj(int n) {
        switch (nMode) {
            case 0:
            case 1:
            case 2:
            case 3:
                RockeRight.F_SetAdjX(n);
                break;
            case 4:
            case 5:
                RockeLeft.F_SetAdjX(n);
                break;
        }
    }

    public int F_GetLeftRightAdj() {
        int dat = -1;
        switch (nMode) {
            case 0:
            case 1:
            case 2:
            case 3:
                dat = RockeRight.F_GetAdjX();
                break;
            case 4:
            case 5:
                dat = RockeLeft.F_GetAdjX();
                break;
        }
        return dat;
    }



    public void F_SetMode(int nMode) {
        if (nMode < 0 || nMode > 5)
            return;
        this.nMode = nMode;

        if (nMode == 0)      //正常左手模式
        {
            F_SetGravity(false);
            RockeLeft = RockeLeftA;
            RockeRight = RockeRightA;
            int nType = 0;
            nType |= MyRockeViewA.TYPE_No_ADJY;
            if (bHoldHeight)
                nType |= MyRockeViewA.TYPE_RestCentY;
            nType |= MyRockeViewA.TYPE_RestCentX;
            RockeLeft.F_SetType(nType);
            nType = 0;

            nType |= MyRockeViewA.TYPE_RestCentY;
            nType |= MyRockeViewA.TYPE_RestCentX;

            RockeRight.F_SetType(nType);
            RockeRight.F_SetVLR(true);
            if(bFlyType)
            {
                RockeRight.F_SetImage(background);
            }
        }
        else if (nMode == 1)      //正常左手模式+重力
        {


            RockeLeft = RockeLeftA;
            RockeRight = RockeRightA;

            /*
            if(bFlyType)
            {
             //   Bitmap bmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.cir_back_fly_jh_fig);
             //   RockeRight.F_SetImage(bmp);

            }
            */

            int nType = 0;
            nType |= MyRockeViewA.TYPE_No_ADJY;
            if (bHoldHeight)
                nType |= MyRockeViewA.TYPE_RestCentY;
            nType |= MyRockeViewA.TYPE_RestCentX;
            RockeLeft.F_SetType(nType);
            nType = 0;
            nType |= MyRockeViewA.TYPE_X_Acceleration;
            nType |= MyRockeViewA.TYPE_Y_Acceleration;
            RockeRight.F_SetType(nType);
            RockeRight.F_SetVLR(true);
            F_SetGravity(true);
        } else if (nMode == 2)        //普通右手模式
        {
            F_SetGravity(false);
            RockeLeft = RockeRightA;
            RockeRight = RockeLeftA;
            int nType = 0;
            nType |= MyRockeViewA.TYPE_No_ADJY;
            if (bHoldHeight)
                nType |= MyRockeViewA.TYPE_RestCentY;
            nType |= MyRockeViewA.TYPE_RestCentX;
            RockeLeft.F_SetType(nType);
            nType = 0;
            nType |= MyRockeViewA.TYPE_RestCentY;
            nType |= MyRockeViewA.TYPE_RestCentX;
            RockeRight.F_SetType(nType);
            RockeRight.F_SetVLR(false);
        } else if (nMode == 3)      //普通右手模式+重力
        {
            RockeLeft = RockeRightA;
            RockeRight = RockeLeftA;
            int nType = 0;
            nType |= MyRockeViewA.TYPE_No_ADJY;
            if (bHoldHeight)
                nType |= MyRockeViewA.TYPE_RestCentY;
            nType |= MyRockeViewA.TYPE_RestCentX;
            RockeLeft.F_SetType(nType);
            nType = 0;
            nType |= MyRockeViewA.TYPE_X_Acceleration;
            nType |= MyRockeViewA.TYPE_Y_Acceleration;
            RockeRight.F_SetType(nType);
            RockeRight.F_SetVLR(false);
            F_SetGravity(true);
        } else if (nMode == 4)        //右手模式1
        {
            F_SetGravity(false);
            RockeLeft = RockeRightA;
            RockeRight = RockeLeftA;
            int nType = 0;
            nType |= MyRockeViewA.TYPE_No_ADJY;
            if (bHoldHeight)
                nType |= MyRockeViewA.TYPE_RestCentY;
            nType |= MyRockeViewA.TYPE_RestCentX;
            RockeLeft.F_SetType(nType);
            nType = 0;
            nType |= MyRockeViewA.TYPE_RestCentY;
            nType |= MyRockeViewA.TYPE_RestCentX;
            RockeRight.F_SetType(nType);
            RockeRight.F_SetVLR(false);
        }
        else // if (nMode == 5)          //右手模式1+重力
        {
            RockeLeft = RockeRightA;
            RockeRight = RockeLeftA;
            int nType = 0;
            nType |= MyRockeViewA.TYPE_No_ADJY;
            if (bHoldHeight)
                nType |= MyRockeViewA.TYPE_RestCentY;
            nType |= MyRockeViewA.TYPE_X_Acceleration;
            nType |= MyRockeViewA.TYPE_Acceleration_NoDispX;
            RockeLeft.F_SetType(nType);
            nType = 0;
            nType |= MyRockeViewA.TYPE_RestCentX;
            nType |= MyRockeViewA.TYPE_Y_Acceleration;
            nType |= MyRockeViewA.TYPE_Acceleration_NoDispY;
            RockeRight.F_SetType(nType);
            RockeRight.F_SetVLR(false);
            F_SetGravity(true);
        }
        RockeRight.invalidate();
        RockeLeft.invalidate();

    }






    private static int dip2px(Context context, float dpValue) {
        final float scale = context.getResources().getDisplayMetrics().density;
        return (int) (dpValue * scale + 0.5f);
    }


    private void F_DispControlInPath(boolean bDisp) {

        int disp = VISIBLE;
        if (!bDisp)
            disp = INVISIBLE;

        switch (nMode) {
            case 0:
            case 1:

                RockeRightA.setVisibility(disp);
                RockeLeftA.setVisibility(VISIBLE);
                if (!bDisp) {
                    RockeRightA.bPath = false;
                    RockeLeftA.bPath = true;
                } else {
                    RockeRightA.bPath = false;
                    RockeLeftA.bPath = false;
                }
                break;
            case 2:
            case 3:
            case 4:
            case 5:
                RockeLeftA.setVisibility(disp);
                RockeRightA.setVisibility(VISIBLE);
                if (!bDisp) {
                    RockeRightA.bPath = true;
                    RockeLeftA.bPath = false;
                } else {
                    RockeRightA.bPath = false;
                    RockeLeftA.bPath = false;
                }
                break;
        }
        RockeRightA.invalidate();
        RockeLeftA.invalidate();
    }


    public boolean isInPathMode() {
        return (pathView.getVisibility() == VISIBLE);
    }

    public void F_StopPath() {
        pathView.StoppathAnim();
    }

    public void F_DispPahtView(boolean bDisp) {
        if (bDisp) {
            pathView.StoppathAnim();
            pathView.setVisibility(VISIBLE);
            F_DispControlInPath(false);

        } else {
            pathView.StoppathAnim();
            pathView.setVisibility(INVISIBLE);
            F_DispControlInPath(true);
        }
    }


    public class MyRockeViewA extends RelativeLayout {

        private  Handler   mHandler;

        private MyRipple  myRipple;
        public boolean bFlyType = true;
        public boolean isbFlyType_recording = false;

        public boolean bDisText = true;
        public boolean bPath = false;
        public int nPathSpeed = 1;

        public final static int TYPE_RestCentX = 1;
        public final static int TYPE_RestCentY = 2;
        public final static int TYPE_X_Acceleration = 4;
        public final static int TYPE_Y_Acceleration = 8;
        public final static int TYPE_No_ADJY = 0x10;
        public final static int TYPE_Acceleration_NoDispX = 0x20;
        public final static int TYPE_Acceleration_NoDispY = 0x40;


        public String Str1 = "THRO";
        public String Str2 = "RUDO";

        public boolean bDispVbar = true;

        public boolean bLeft = true;
        public boolean bLayoutLeft = true;

        private boolean bCanMoved = false;
        private int adjx, adjy, adjw, adjh;

        public int Radius;  //colorWheelRadius;
        public int CirRadius;
        public int Radius_VV;
        public int Radius_V1;
        private Point cirPoint;
        private Point touchPoint;
        private Point cirPointB;
        private Bitmap backBmp;
        private Bitmap backBmp1;
        private Bitmap cirBmp;
        private Bitmap upBmp;
        private Bitmap downBmp;
        private Bitmap leftBmp;
        private Bitmap rightBmp;
        private Bitmap v_BarBmp;
        private Bitmap h_BarBmp;
        private RectF upRect;
        private RectF downRect;
        private RectF leftRect;
        private RectF rightRect;

        private Bitmap thmbV_Bmp;
        private Bitmap thmbH_Bmp;


        private boolean bAdjMode1 = false;
        private Bitmap bar_V_btn_Bmp;
        private Bitmap bar_H_btn_Bmp;

        private Bitmap PathSpeed1_Bmp;
        private Bitmap PathSpeed2_Bmp;
        private Bitmap PathSpeed3_Bmp;
        //private  Bitmap   PathSpeed4_Bmp;
        //private  Bitmap   PathSpeed5_Bmp;

        private Bitmap thmbV_Bmp_A;
        private Bitmap thmbH_Bmp_A;


        //public  int   nTypeA = 0;


        private boolean bVbarRight = true;

        private  boolean  bTouched=false;

        public int centx;
        public int centy;
        public int nType;
        //public boolean bLimitHeight = false;

        private Activity activity;


        private RelativeLayout.LayoutParams ripple_params;


        public MyRockeViewA(Context context, AttributeSet attrs, int defStyle) {
            super(context, attrs, defStyle);
            //activity = (Activity)context;

            init(true);
        }

        public void F_SetCirR(int r) {
            CirRadius = r;
            invalidate();
        }

        public MyRockeViewA(Context context, AttributeSet attrs) {
            super(context, attrs);
            init(true);
        }

        public MyRockeViewA(Context context) {
            super(context);
            init(true);
        }

        public MyRockeViewA(Context context, Bitmap back, Bitmap cir, boolean btype) {
            super(context);
            bFlyType = btype;
            init(false);
            backBmp = back;//BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.cir_back_jh);
            cirBmp = cir;//BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.cir_jh);

        }

        public void F_SetImage(Bitmap back)
        {
            backBmp = back;
            invalidate();
        }

        public void F_SetImage(Bitmap back, Bitmap cir) {
            backBmp = back;//BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.cir_back_jh);
            cirBmp = cir;//BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.cir_jh);
            invalidate();
        }

        public  void F_SetFlyRecord(boolean b)
        {
            isbFlyType_recording=b;
            if(isbFlyType_recording)
            {
                upBmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.up_fly_jh);
                downBmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.down_fly_jh);
                leftBmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.left_fly_jh);
                rightBmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.right_fly_jh);
            }
            else {

                upBmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.up_fly_jh);
                downBmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.down_fly_jh);
                leftBmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.left_fly_jh);
                rightBmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.right_fly_jh);

                //upBmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.up_fly_jh_b);
                //downBmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.down_fly_jh_b);
                //leftBmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.left_fly_jh_b);
                //rightBmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.right_fly_jh_b);
            }
            invalidate();
        }

        private void init(boolean ba) {

            setWillNotDraw(false);

            touchPoint = new Point(-1,-1);
            myRipple = new MyRipple(getContext());
            ripple_params = new RelativeLayout.LayoutParams(76, 76);
            this.addView(myRipple,ripple_params);

            mHandler = new Handler();

            myRipple.setVisibility(INVISIBLE);
            if (ba) {
                backBmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.cir_back_jh);
                cirBmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.cir_jh);
            }
            if (!bFlyType) {
                upBmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.up_jh);
                downBmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.down_jh);
                leftBmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.left_jh);
                rightBmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.right_jh);
            } else {
                if(isbFlyType_recording)
                {
                    upBmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.up_fly_jh);
                    downBmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.down_fly_jh);
                    leftBmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.left_fly_jh);
                    rightBmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.right_fly_jh);
                }
                else {
                    //upBmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.up_fly_jh_b);
                    //downBmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.down_fly_jh_b);
                    //leftBmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.left_fly_jh_b);
                    //rightBmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.right_fly_jh_b);

                    upBmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.up_fly_jh);
                    downBmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.down_fly_jh);
                    leftBmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.left_fly_jh);
                    rightBmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.right_fly_jh);
                }
            }


            CirRadius = -1;
            cirPoint = new Point(0, 0);
            cirPointB = new Point(0, 0);
            Radius_VV = 1;
            Radius_V1 = 1;
            upRect = new RectF();
            downRect = new RectF();
            leftRect = new RectF();
            rightRect = new RectF();
            adjx = adjy = 0x80;

            if(bFlyType)
            {
                PathSpeed1_Bmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.tra1_1_fly);
                PathSpeed2_Bmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.tra1_2_fly);
                PathSpeed3_Bmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.tra1_3_fly);
            }
            else {
                PathSpeed1_Bmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.tra1_1_jh);
                PathSpeed2_Bmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.tra1_2_jh);
                PathSpeed3_Bmp = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.tra1_3_jh);
            }

            CirRadius = -1;

        }


        @Override
        protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
            super.onMeasure(widthMeasureSpec, heightMeasureSpec);
            /*
            int widthSize = MeasureSpec.getSize(widthMeasureSpec);
            int heightSize = MeasureSpec.getSize(heightMeasureSpec);
           // int size = Math.min(widthSize, heightSize);
           // setMeasuredDimension(size, size);
           */
        }

        private void DrawCir(Canvas canvas) {
            Paint paint = new Paint();
            paint.setAntiAlias(true);
            if(bFlyType) {
                if(JH_App.bFlyDisableAll) {
                    paint.setAlpha(51);
                }
                else
                {
                    paint.setAlpha(255);
                }
            }
            if ((nType & TYPE_Acceleration_NoDispX) != 0) {
                    RectF dstRect = new RectF(centx - CirRadius, cirPoint.y - CirRadius, centx + CirRadius, cirPoint.y + CirRadius);
                    canvas.drawBitmap(cirBmp, null, dstRect, paint);
                return;
            }
            if ((nType & TYPE_Acceleration_NoDispY) != 0) {
                    RectF dstRect = new RectF(cirPoint.x - CirRadius, centy - CirRadius, cirPoint.x + CirRadius, centy + CirRadius);
                    canvas.drawBitmap(cirBmp, null, dstRect, paint);
                return;
            }
            {
                if(bFlyType)
                {

                    if((nType & TYPE_X_Acceleration)!=0 || (nType &TYPE_Y_Acceleration)!=0 )
                    {
                         return;
                    }
                    else
                    {
                        RectF dstRect = new RectF(cirPoint.x - CirRadius, cirPoint.y - CirRadius, cirPoint.x + CirRadius, cirPoint.y + CirRadius);
                        canvas.drawBitmap(cirBmp, null, dstRect, paint);
                    }
                }
                else
                {
                    RectF dstRect = new RectF(cirPoint.x - CirRadius, cirPoint.y - CirRadius, cirPoint.x + CirRadius, cirPoint.y + CirRadius);
                    canvas.drawBitmap(cirBmp, null, dstRect, paint);
                }
            }
        }


        private void DrawV_bar(Canvas canvas) {

            // 创建画笔
            Paint paint = new Paint();
            paint.setAntiAlias(true);
            paint.setStyle(Paint.Style.STROKE);

            if(bFlyType) {
                if(JH_App.bFlyDisableAll) {
                    paint.setAlpha(51);
                }
                else
                {
                    paint.setAlpha(255);
                }
            }

            int dx20 = Storage.dip2px(getContext(), 20);
            int dx20A = Storage.dip2px(getContext(), 25);
            int dx5 = Storage.dip2px(getContext(), (20 - 16) / 2);
            int dx14 = Storage.dip2px(getContext(), 16);
            int dx2 = Storage.dip2px(getContext(), 10);  //5

            int HH = Storage.dip2px(getContext(), 10);
            int WW = Storage.dip2px(getContext(), 28);
            if (bAdjMode1) {
                dx2 = Storage.dip2px(getContext(), 10);
                dx20 = Storage.dip2px(getContext(), 30);
                int H2 = Radius * 2 - dx2 * 2;
                int dh = Storage.dip2px(getContext(), 8);
                RectF dstRect = new RectF();
                int dx = Storage.dip2px(getContext(), 5);
                dx *= 2;
                int dw = dx20 + dx;
                if (bVbarRight) {
                    upRect.left = centx + Radius + dx2;
                    upRect.top = centy - Radius + dx2;
                    upRect.right = upRect.left + dx20;
                    upRect.bottom = upRect.top + H2;
                    canvas.drawBitmap(bar_V_btn_Bmp, null, upRect, null);
                    dstRect.set(upRect);
                    downRect.set(upRect);
                    upRect.bottom = upRect.top + H2 / 2;
                    downRect.top += H2 / 2;

                    float step;
                    adjh = (int) dstRect.height();
                    step = (adjh - dh) / 128.0f;
                    int ch = adjh / 2;
                    int cnt;
                    ch -= (dh / 2);

                    if (adjy < 0x80) {
                        int tp = 0x80 - adjy;
                        ch += (int) (tp * step);
                    } else if (adjy > 0x80) {
                        int tp = adjy - 0x80;
                        ch -= (int) (tp * step);
                    }
                    cnt = ch;
                    dstRect.left -= dx / 2.0f;
                    dstRect.top = cnt + dstRect.top;
                    dstRect.right = dstRect.left + dw;//upRect.left+dx20;
                    dstRect.bottom = dstRect.top + dh;//dx20;
                    canvas.drawBitmap(thmbV_Bmp_A, null, dstRect, null);
                } else {
                    //dx2 = Storage.dip2px(getContext(), 5);
                    upRect.left = centx - Radius - dx2 - dx20;
                    upRect.top = centy - Radius + dx2;
                    upRect.right = upRect.left + dx20;
                    upRect.bottom = upRect.top + H2;
                    dstRect.set(upRect);
                    // NinePatch ninePatch = new NinePatch(bar_V_btn_Bmp, bar_V_btn_Bmp.getNinePatchChunk(), null);
                    // ninePatch.draw(canvas, upRect);
                    canvas.drawBitmap(bar_V_btn_Bmp, null, dstRect, null);
                    //dstRect.set(upRect);
                    downRect.set(upRect);
                    upRect.bottom = upRect.top + H2 / 2;
                    downRect.top += H2 / 2;
                    float step;
                    int w =
                            adjh = (int) dstRect.height();
                    step = (adjh - dh) / 128.0f;
                    int ch = adjh / 2;
                    int cnt;
                    ch -= (dh / 2);
                    if (adjy < 0x80) {
                        int tp = 0x80 - adjy;
                        ch += (int) (tp * step);
                    } else if (adjy > 0x80) {
                        int tp = adjy - 0x80;
                        ch -= (int) (tp * step);
                    }
                    cnt = ch;
                    //  dstRect.left -= dx5;
                    dstRect.left -= dx / 2.0f;
                    dstRect.top = cnt + dstRect.top;
                    dstRect.right = dstRect.left + dw;//upRect.left+dx20;
                    dstRect.bottom = dstRect.top + dh;
                    canvas.drawBitmap(thmbH_Bmp_A, null, dstRect, null);
                }
            } else {
                dx2 = Storage.dip2px(getContext(), 14);  //5
                dx20 = Storage.dip2px(getContext(), 28);
                dx5 = Storage.dip2px(getContext(), (28 - 14) / 2);
                dx14 = Storage.dip2px(getContext(), 14);

                int dxA = Storage.dip2px(getContext(), 20);


                if (bVbarRight) {
                    int dd2 = Storage.dip2px(getContext(), 5);
                    dx2 = Storage.dip2px(getContext(), 5);
                    dx20 = Storage.dip2px(getContext(), 24);
                    dx5 = Storage.dip2px(getContext(), 10);
                    dx20A = Storage.dip2px(getContext(), 12);

                    upRect.left = centx + Radius + dxA;
                    upRect.top = centy - Radius + dx5;



                    if (bFlyType)
                    {
                        upRect.left = centx + Radius+ dd2;// + dxA;
                        upRect.top = centy - (Radius * (2 / 3.0f));

                    }
                    upRect.right = upRect.left + dx20;
                    upRect.bottom = upRect.top + dx20A;



                    canvas.drawBitmap(upBmp, null, upRect, paint);
                    downRect.left = upRect.left;//centx + Radius+dxA;
                    downRect.bottom = centy + Radius - dx2;
                    if (bFlyType) {
                        //downRect.left = centx + Radius + dd2 + dxA;
                        //downRect.left = centx + Radius + dd2 + dxA;
                        downRect.bottom = centy + (Radius * (2 / 3.0f));//-dx20A;
                    }
                    downRect.top = downRect.bottom - dx20A;
                    downRect.right = downRect.left + dx20;
                    canvas.drawBitmap(downBmp, null, downRect, paint);
                    int cir_r = Storage.dip2px(getContext(), 15) * 2;
                    int nsize = Storage.dip2px(getContext(), 15);



                     if(!bFlyType){
                         paint.setColor(Color.BLACK);// 设置
                        canvas.drawLine(upRect.right - dx20 / 2 + dd2, upRect.top - dd2, upRect.right + dd2, upRect.top - dd2, paint);// 画线
                        canvas.drawLine(upRect.right + dd2, upRect.top - dd2, upRect.right + dd2, centy - (cir_r / 2), paint);// 画线
                        canvas.drawLine(downRect.right - dx20 / 2 + dd2, downRect.bottom + dd2, downRect.right + dd2, downRect.bottom + dd2, paint);// 画线
                        canvas.drawLine(downRect.right + dd2, downRect.bottom + dd2, downRect.right + dd2, centy + (cir_r / 2), paint);// 画线
                        canvas.drawCircle(downRect.right + dd2, centy, cir_r / 2, paint);
                         paint.setColor(Color.BLUE);
                    }
                    else
                    {
                        if(JH_App.bFlyDisableAll)
                        {
                            paint.setColor(Color.argb(51,255,255,255));// 设置
                        }
                        else {
                            paint.setColor(Color.WHITE);// 设置
                        }
                    //    canvas.drawCircle(downRect.left + dx20 / 2, centy, cir_r / 2, p);
                    }

                    paint.setTextSize(nsize);
                    paint.setStyle(Paint.Style.FILL);
                    //该方法即为设置基线上那个点究竟是left,center,还是right  这里我设置为center
                    Paint.FontMetrics fontMetrics = paint.getFontMetrics();
                    float top = fontMetrics.top;//为基线到字体上边框的距离,即上图中的top
                    float bottom = fontMetrics.bottom;//为基线到字体下边框的距离,即上图中的bottom
                    int baseLineY = (int) (centy - top / 2 - bottom / 2);//基线中间点的y轴计算公式
                    float xxxx = downRect.right + dd2;
                    if (bFlyType) {
                        xxxx = downRect.left + dx20 / 2;
                    }
                    String s = "";
                    int adj = adjy - 0x80;
                    adj = 0 - adj;
                    s = "" + adj;
                    canvas.rotate(-90, (xxxx), centy);
                    paint.setTextAlign(Paint.Align.CENTER);
                    canvas.drawText(s, xxxx, baseLineY, paint);
                    canvas.rotate(90, xxxx, centy);

                } else {
                    /*
                     dx2 = Storage.dip2px(getContext(), 5);
                     dx20 = Storage.dip2px(getContext(), 24);
                     dx5 = Storage.dip2px(getContext(), 10);
                     dx20A = Storage.dip2px(getContext(), 12);
                   */
                    upRect.left = centx - Radius - dx2 - dx20;
                    upRect.top = centy - Radius;
                    upRect.right = upRect.left + dx20;
                    upRect.bottom = upRect.top + dx20A;
                    canvas.drawBitmap(upBmp, null, upRect, null);
                    downRect.left = centx - Radius - dx2 - dx20;
                    //centx + Radius + dx2;
                    downRect.bottom = centy + Radius;
                    downRect.right = downRect.left + dx20;
                    downRect.top = downRect.bottom - dx20A;
                    canvas.drawBitmap(downBmp, null, downRect, null);
/*
                    RectF dstRect = new RectF();
                    dstRect.left = centx - Radius - dx2 - dx5 - dx14;
                    dstRect.top = upRect.bottom + dx5;
                    dstRect.right = dstRect.left + dx14;
                    dstRect.bottom = downRect.top - dx5;
                    canvas.drawBitmap(v_BarBmp, null, dstRect, null);
                 //   NinePatch ninePatch = new NinePatch(v_BarBmp, v_BarBmp.getNinePatchChunk(), null);
                 //   ninePatch.draw(canvas, dstRect);
                    float step;
                    adjh = (int) dstRect.height();
                    step = (adjh - HH) / 128.0f;
                    int ch = adjh / 2;
                    int cnt;
                    ch -= (HH / 2);
                    if (adjy < 0x80) {
                        int tp = 0x80 - adjy;
                        ch += (int) (tp * step);
                    } else if (adjy > 0x80) {
                        int tp = adjy - 0x80;
                        ch -= (int) (tp * step);
                    }
                    cnt = ch;
                    dstRect.left -= dx5;
                    dstRect.top = cnt + dstRect.top;
                    dstRect.right = dstRect.left + dx20+dx5;//upRect.left+dx20;
                    dstRect.bottom = dstRect.top + HH;
                    canvas.drawBitmap(thmbV_Bmp, null, dstRect, null);
                    */
                }
            }
        }

        private void DrawH_bar(Canvas canvas) {

            int dx20 = Storage.dip2px(getContext(), 20);
            int dx20A = Storage.dip2px(getContext(), 25);
            int dx5 = Storage.dip2px(getContext(), (20 - 16) / 2);
            int dx14 = Storage.dip2px(getContext(), 14);
            int dx2 = Storage.dip2px(getContext(), 5);


            Paint paint = new Paint();
            paint.setAntiAlias(true);
            paint.setColor(Color.BLACK);// 设置

            if(bFlyType) {
                if(JH_App.bFlyDisableAll) {
                    paint.setAlpha(51);
                }
                else
                {
                    paint.setAlpha(255);
                }
            }

            if (bPath) {
                dx14 = Storage.dip2px(getContext(), 6);
                dx2 = Storage.dip2px(getContext(), 38);
                dx20 = Storage.dip2px(getContext(), 100);
                int h = Storage.dip2px(getContext(), 35);
                RectF dstRect = new RectF();
                dstRect.left = centx - Radius + dx2;
                dstRect.top = centy + Radius + dx14;
                dstRect.right = centx + Radius - dx2;
                dstRect.bottom = dstRect.top + h;
                Bitmap bmp = PathSpeed3_Bmp;
                if (nPathSpeed == 1) {
                    bmp = PathSpeed1_Bmp;
                } else if (nPathSpeed == 2) {
                    bmp = PathSpeed2_Bmp;
                }
                //else if(nPathSpeed==3)
                //{
                //    bmp = PathSpeed3_Bmp;
                //}
                //else if(nPathSpeed==5)
                //{
                //    bmp = PathSpeed5_Bmp;
                // }
                else {
                    bmp = PathSpeed3_Bmp;
                }
                canvas.drawBitmap(bmp, null, dstRect, null);


                dx2 = Storage.dip2px(getContext(), 8);
                dx20 = Storage.dip2px(getContext(), 24);
                dx5 = Storage.dip2px(getContext(), 10);


                dx20A = Storage.dip2px(getContext(), 12);

                int dd2 = Storage.dip2px(getContext(), 5);

                leftRect.left = centx - Radius + dx5;
                leftRect.top = centy + Radius + dx2;
                leftRect.right = leftRect.left + dx20A;
                leftRect.bottom = leftRect.top + dx20;
                canvas.drawBitmap(leftBmp, null, leftRect, null);

                rightRect.left = centx + Radius - dx20A - dx5;
                rightRect.top = centy + Radius + dx2;
                rightRect.right = rightRect.left + dx20A;
                rightRect.bottom = rightRect.top + dx20;
                canvas.drawBitmap(rightBmp, null, rightRect, null);

                return;
            }


            if (bAdjMode1) {
/*
                dx2 = Storage.dip2px(getContext(), 8);
                dx20 = Storage.dip2px(getContext(), 30);
                int W2 = Radius*2-dx2*2;
                int dx =  Storage.dip2px(getContext(), 5);
                dx*=2;

                RectF dstRect = new RectF();

                leftRect.left = centx - Radius+dx2;
                leftRect.top = centy + Radius + dx2;
                leftRect.right = leftRect.left +W2;
                leftRect.bottom = leftRect.top + dx20;
                canvas.drawBitmap(bar_H_btn_Bmp, null, leftRect, null);
                dstRect.set(leftRect);
                rightRect.set(leftRect);
                leftRect.right=leftRect.left+W2/2;
                rightRect.left+=W2/2;

                int dw = Storage.dip2px(getContext(), 8);

                float step;
                adjw = (int) dstRect.width();
                step = (adjw - dw) / 128.0f;
                int ch = adjw / 2;
                int cnt;
                ch -= (dw / 2);
                if (adjx < 0x80) {
                    int tp = 0x80 - adjx;
                    ch -= (int) (tp * step);
                } else if (adjx > 0x80) {
                    int tp = adjx - 0x80;
                    ch += (int) (tp * step);
                }
                cnt = ch;
                dstRect.top-=(dx/2.0f);
                dstRect.left = cnt + dstRect.left;
                dstRect.right = dstRect.left + dw;//upRect.left+dx20;
                dstRect.bottom = dstRect.top + dx20+dx;
                canvas.drawBitmap(thmbH_Bmp_A, null, dstRect, null);
*/
            } else {
                int dd2;

                dx2 = Storage.dip2px(getContext(), 5);
                dx20 = Storage.dip2px(getContext(), 24);
                dx5 = Storage.dip2px(getContext(), 10);


                dx20A = Storage.dip2px(getContext(), 12);

                dd2 = Storage.dip2px(getContext(), 5);

                int dxA = Storage.dip2px(getContext(), 20);


                leftRect.left = centx - Radius + dx5;
                leftRect.top = centy + Radius+dxA;
                if (bFlyType)
                {
                    leftRect.top = centy + Radius+dd2;
                    leftRect.left = centx - (Radius * (2 / 3.0f));
                }
                leftRect.right = leftRect.left + dx20A;
                leftRect.bottom = leftRect.top + dx20;
                canvas.drawBitmap(leftBmp, null, leftRect, paint);

                rightRect.left = centx + Radius - dx20A - dx5;
                rightRect.top = leftRect.top;//centy + Radius+dxA;
                if (bFlyType) {
                    //rightRect.top += dd2;
                    rightRect.left = centx + (Radius * (2 / 3.0f)) - dx20A;
                }
                rightRect.right = rightRect.left + dx20A;
                rightRect.bottom = rightRect.top + dx20;
                canvas.drawBitmap(rightBmp, null, rightRect, paint);

                int cir_r = Storage.dip2px(getContext(), 15) * 2;
                int nsize = Storage.dip2px(getContext(), 15);



                if (!bFlyType) {

                    canvas.drawLine(leftRect.left - dd2, leftRect.bottom - dx20 / 2 + dd2, leftRect.left - dd2, leftRect.bottom + dd2, paint);
                    canvas.drawLine(leftRect.left - dd2, leftRect.bottom + dd2, centx - (cir_r / 2), leftRect.bottom + dd2, paint);

                    canvas.drawLine(rightRect.right + dd2, rightRect.bottom - dx20 / 2 + dd2, rightRect.right + dd2, rightRect.bottom + dd2, paint);

                    canvas.drawLine(rightRect.right + dd2, rightRect.bottom + dd2, centx + (cir_r / 2), rightRect.bottom + dd2, paint);
                    paint.setStyle(Paint.Style.STROKE);
                    canvas.drawCircle(centx, rightRect.bottom + dd2 / 2, cir_r / 2, paint);
                }
                /*
                else
                    {
                        paint.setColor(Color.WHITE);// 设置
                        paint.setStyle(Paint.Style.STROKE);
                    //canvas.drawCircle(centx, rightRect.top + dx20 / 2, cir_r / 2, p);
                }
                */



                if (!bFlyType)
                    paint.setColor(Color.BLUE);
                else {
                    if(JH_App.bFlyDisableAll)
                    {
                        paint.setColor(Color.argb(51,255,255,255));// 设置
                    }
                    else
                        paint.setColor(Color.WHITE);
                }
                paint.setTextSize(nsize);
                paint.setStyle(Paint.Style.FILL);
                //该方法即为设置基线上那个点究竟是left,center,还是right  这里我设置为center


                Paint.FontMetrics fontMetrics = paint.getFontMetrics();
                float top = fontMetrics.top;//为基线到字体上边框的距离,即上图中的top
                float bottom = fontMetrics.bottom;//为基线到字体下边框的距离,即上图中的bottom

                int baseLineY = (int) (rightRect.bottom + dd2 - top / 2 - bottom / 2);//基线中间点的y轴计算公式
                if (bFlyType) {
                    baseLineY = (int) (rightRect.bottom - dx20 / 2 - top / 2 - bottom / 2);//基线中间点的y轴计算公式
                }
                String s = "";
                int adj = adjx - 0x80;
                s = "" + adj;


                paint.setTextAlign(Paint.Align.CENTER);
                canvas.drawText(s, centx, baseLineY, paint);



                /*
                RectF dstRect = new RectF();
                dstRect.left = leftRect.right + dx5;
                dstRect.top = leftRect.top + dx5;
                dstRect.right = rightRect.left - dx5;
                dstRect.bottom = leftRect.bottom - dx5;
                canvas.drawBitmap(h_BarBmp, null, dstRect, null);
                */
                //NinePatch ninePatch = new NinePatch(h_BarBmp, h_BarBmp.getNinePatchChunk(), null);
                //ninePatch.draw(canvas, dstRect);
                /*
                float step;
                int w =  Storage.dip2px(getContext(), 10);
                int H = Storage.dip2px(getContext(), 28);
                adjw = (int) dstRect.width();
                step = (adjw - w) / 128.0f;
                int ch = adjw / 2;
                int cnt;
                ch -= (w / 2);
                if (adjx < 0x80) {
                    int tp = 0x80 - adjx;
                    ch -= (int) (tp * step);
                } else if (adjx > 0x80) {
                    int tp = adjx - 0x80;
                    ch += (int) (tp * step);
                }


                dx5 = Storage.dip2px(getContext(), (25-14)/2);

                cnt = ch;
                dstRect.left = cnt + dstRect.left;
                dstRect.top -= dx5;
                dstRect.right = dstRect.left + w;//upRect.left+dx20;
                dstRect.bottom = dstRect.top + H+dx5;
                canvas.drawBitmap(thmbH_Bmp, null, dstRect, null);
                */
            }

        }

        private  RectF  dstrect = new RectF(0,0,1,1);

        @Override
        protected void onDraw(Canvas canvas) {
            Paint paint = new Paint();
            if(bFlyType) {
                if(JH_App.bFlyDisableAll) {
                    paint.setAlpha(51);
                }
                else
                {
                    paint.setAlpha(255);
                }
            }
            paint.setAntiAlias(true);
            int nSize = Storage.dip2px(getContext(), 15);
           // RectF dstRect = new RectF(centx - Radius, centy - Radius, centx + Radius, centy + Radius);
            dstrect.left = centx - Radius;
            dstrect.top = centy - Radius;
            dstrect.right = centx + Radius;
            dstrect.bottom = centy + Radius;

            if(bFlyType)
            {
                if((nType & TYPE_X_Acceleration)!=0 || (nType &TYPE_Y_Acceleration)!=0 )
                {
                    if(bTouched)
                    {
                        canvas.drawBitmap(backBmp, null, dstrect, paint);
                    }
                    else
                    {
                       // Bitmap bb = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.cir_back_fly_jh_fig);
                        if(bbitbmp!=null)
                            canvas.drawBitmap(bbitbmp, null, dstrect, paint);
                    }
                }
                else
                {
                    canvas.drawBitmap(backBmp, null, dstrect, paint);
                }
            }
            else
            {
                canvas.drawBitmap(backBmp, null, dstrect, paint);
            }

            paint.setColor(Color.RED);
            paint.setTextSize(nSize);
            paint.setStrokeWidth(1);
            paint.setStyle(Paint.Style.FILL);
            paint.setTextAlign(Paint.Align.CENTER);
            canvas.drawText(Str1, centx, centy - Radius - 8, paint);
            canvas.rotate(-90, (centx - Radius - 10), centy);
            paint.setTextAlign(Paint.Align.CENTER);
            canvas.drawText(Str2, (centx - Radius - 10), centy, paint);
            canvas.rotate(90, (centx - Radius - 10), centy);
            DrawCir(canvas);
            DrawH_bar(canvas);
            if ((nType & TYPE_No_ADJY) == 0) {
                DrawV_bar(canvas);
            }
        }


        @Override
        protected void onSizeChanged(int width, int height, int oldw, int oldh) {
            int bPitch;
            int dsb = Storage.dip2px(getContext(), 45);

            if (bPath)
            {
                    dsb = Storage.dip2px(getContext(), 50);
            }
            else {
                if (bFlyType)
                {
                    dsb = Storage.dip2px(getContext(), 50);
                } else {
                    dsb = Storage.dip2px(getContext(), 45);
                }
            }

            int dsb1 = (dsb) * 2;
            int size = Math.min(width, height);

            int pitch = Storage.dip2px(getContext(), 70+25);

            CirRadius = (int) ((size * 0.12)/2);// / 12f);   //标点的半径
            Radius = (int)(CirRadius*2*4)/2;//(size - dsb1) / 2;           //摇杆背景的半径
            Radius_VV = Radius - CirRadius;
            Radius_V1 = Radius_VV;
            Radius_VV = Radius_VV * Radius_VV;


           // ripple_params =(LayoutParams)myRipple.getLayoutParams();// LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT);
            ripple_params.width = 200;//(int)(CirRadius*1.2);
            ripple_params.height =200;// (int)(CirRadius*1.2);
            myRipple.post(new Runnable() {
                @Override
                public void run() {
                    myRipple.setLayoutParams(ripple_params);
                }
            });




            if (bFlyType)
            {
                centy = height / 2;
                if (bLeft)
                {
                    centx = (int)(pitch+Radius);
                }
                else {
                    centx = (int) (width - (Radius+pitch));
                }
            }
            else
            {
                centy = height / 2;
                if (bLeft) {
                    centx = (int) (width * 0.45);
                } else {
                    centx = (int) (width * 0.55);
                }
            }




            /*
            if (bFlyType) {
                if (bLayoutLeft) {
                    centx = Radius;
                } else {
                    centx = width - Radius;
                }
            }
            */



            cirPoint.x = centx;
            cirPoint.y = centy;
            F_ReasetEngine();

            /*

            if (bPath)
            {
                dsb = Storage.dip2px(getContext(), 50);
            }
            else {
                if (bFlyType)
                {
                    dsb = Storage.dip2px(getContext(), 50);
                } else {
                    dsb = Storage.dip2px(getContext(), 45);
                }
            }

            int dsb1 = (dsb) * 2;
            int size = Math.min(width, height);

            CirRadius = (int) (size / 12.5f);   //标点的半径

            Radius = (size - dsb1) / 2;             //摇杆背景的半径
            Radius_VV = Radius - CirRadius;
            Radius_V1 = Radius_VV;
            Radius_VV = Radius_VV * Radius_VV;

            bPitch = 0;
            if (bLeft)
            {
                ;
            } else {
                bPitch = 0 - bPitch;
            }

            centx = width / 2 + bPitch;
            if (bFlyType) {
                if (bLayoutLeft) {
                    centx = Radius;
                } else {
                    centx = width - Radius;
                }
            }

            centy = height / 2 - 10;
            cirPoint.x = centx;
            cirPoint.y = centy;
            F_ReasetEngine();
            */

        }


        private void UpReleas() {
            bCanMoved = false;
        /*
        if (nType == Type_Engine) {
            cirPoint.x = centx;
            if (bLimitHeight) {
                cirPoint.y = centy;
            }

        }
        if (nType == TYPE_Direction_Manual) {
            cirPoint.x = centx;
            cirPoint.y = centy;
        }
        */

        }


        @Override
        public boolean onTouchEvent(MotionEvent event) {

            int dxp = Storage.dip2px(this.getContext(), 10);
            int action = event.getAction() & event.ACTION_MASK;

            if(bFlyType)
            {
                if(JH_App.bFlyDisableAll)
                {
                    if (MotionEvent.ACTION_UP == action)
                    {
                        EventBus.getDefault().post("abcc","NeedEnable_123");

                    }
                    return true;
                }
            }


            int x = (int) event.getX();
            int y = (int) event.getY();
            RectF rect = new RectF();
            RectF rectL = new RectF();

            RectF rectR = new RectF();


            rectL.set(leftRect);
            rectL.left -= dxp;
            rectL.top -= dxp;
            rectL.right += dxp;
            rectL.bottom += dxp;

            rectR.set(rightRect);
            rectR.left -= dxp;
            rectR.top -= dxp;
            rectR.right += dxp;
            rectR.bottom += dxp;

            if (bPath) {
                if (rectL.contains(x, y) || rectR.contains(x, y)) {
                    if (MotionEvent.ACTION_UP == action) {

                        if (rectL.contains(x, y)) {
                            if (!bCanMoved) {
                                nPathSpeed--;
                                if (nPathSpeed <= 0) {
                                    nPathSpeed = 1;
                                }
                                UpReleas();
                                invalidate();
                                Integer speed = nPathSpeed;
                                EventBus.getDefault().post(speed, "Send2_path");      //调用第三方库来发送。
                                // EventBus.getDefault().post(0x00, "PlayBtnMusic");
                                return true;
                            }
                        }

                        if (rectR.contains(x, y)) {
                            if (!bCanMoved) {
                                nPathSpeed++;
                                if (nPathSpeed > 3) {
                                    nPathSpeed = 3;
                                }
                                UpReleas();
                                invalidate();
                                Integer speed = nPathSpeed;
                                EventBus.getDefault().post(speed, "Send2_path");      //调用第三方库来发送消图片显示消息。
                                // EventBus.getDefault().post(0x00, "PlayBtnMusic");      //调用第三方库来发送消图片显示消息。
                                return true;
                            }
                        }


                    }
                    return true;
                }
            }


            rect.set(upRect);
            rect.left -= dxp;
            rect.top -= dxp;
            rect.right += dxp;
            rect.bottom += dxp;


            if (rect.contains(x, y) && !bCanMoved) {
                if (MotionEvent.ACTION_UP == action)
                {
                    Integer n = 0x80 + 0;
                    adjy += 1;
                    if (adjy > 0x80 + 32)
                        adjy = 0x80 + 32;
                    if (adjy == 0x80)
                        n = 0x80 + 1;
                    if (RockeRight == this) {
                        n += 0x100;               // Right;
                    }

                    EventBus.getDefault().post(n, "PlayBtnMusic");      //调用第三方库来发送消图片显示消息。
                    UpReleas();
                    invalidate();
                }
                return true;
            }
            rect.set(downRect);
            rect.left -= dxp;
            rect.top -= dxp;
            rect.right += dxp;
            rect.bottom += dxp;
            if (rect.contains(x, y) && !bCanMoved) {
                if (MotionEvent.ACTION_UP == action) {
                    Integer n = 0x70 + 0;
                    adjy -= 1;
                    if (adjy < 0x60)
                        adjy = 0x60;
                    if (adjy == 0x80)
                        n = 0x70 + 1;
                    if (RockeRight == this) {
                        n += 0x100;               // Right;
                    }
                    EventBus.getDefault().post(n, "PlayBtnMusic");      //调用第三方库来发送消图片显示消息。
                    UpReleas();
                    invalidate();
                }
                return true;
            }
            rect.set(leftRect);
            rect.left -= dxp;
            rect.top -= dxp;
            rect.right += dxp;
            rect.bottom += dxp;
            if (rect.contains(x, y) && !bCanMoved) {
                if (MotionEvent.ACTION_UP == action) {
                    Integer n = 0x90;
                    adjx -= 1;
                    if (adjx < 0x60)
                        adjx = 0x60;
                    if (adjx == 0x80)
                        n = 0x91;
                    if (RockeRight == this) {
                        n += 0x100;               // Right;
                    }
                    EventBus.getDefault().post(n, "PlayBtnMusic");      //调用第三方库来发送消图片显示消息。
                    UpReleas();
                    invalidate();
                }
                return true;
            }
            rect.set(rightRect);
            rect.left -= dxp;
            rect.top -= dxp;
            rect.right += dxp;
            rect.bottom += dxp;
            if (rect.contains(x, y) && !bCanMoved) {
                if (MotionEvent.ACTION_UP == action) {
                    Integer n = 0xA0;
                    //EventBus.getDefault().post(n, "PlayBtnMusic");      //调用第三方库来发送消图片显示消息。
                    adjx += 1;
                    if (adjx > 0xA0)
                        adjx = 0xA0;
                    if (adjx == 0x80)
                        n = 0xA1;
                    if (RockeRight == this) {
                        n += 0x100;               // Right;
                    }
                    EventBus.getDefault().post(n, "PlayBtnMusic");      //调用第三方库来发送消图片显示消息。
                    UpReleas();
                    invalidate();
                }
                return true;
            }

            int dx = x - centx;
            int dy = y - centy;


            RectF dstRect = new RectF(centx - Radius_V1, centy - Radius_V1, centx + Radius_V1, centy + Radius_V1);

            int da = dx * dx + dy * dy;
            cirPointB.x = cirPoint.x;
            cirPointB.y = cirPoint.y;
            switch (action)
            {
                case MotionEvent.ACTION_DOWN:

                    bCanMoved = false;
                    if (dstRect.contains(x, y))
                    {
                        if ((nType & TYPE_X_Acceleration) == 0 || (nType & TYPE_Y_Acceleration) == 0)
                            bCanMoved = true;

                        if (da <= Radius_VV) {
                            if (bFlyType) {
                                if ((nType & TYPE_X_Acceleration) != 0 && (nType & TYPE_Y_Acceleration) != 0) {
                                    bTouched = true;
                                    mHandler.removeCallbacksAndMessages(null);
                                    myRipple.F_SetPlay(true);
                                    if (myRipple.getVisibility() != VISIBLE) {
                                        myRipple.resetWave();
                                        myRipple.setVisibility(VISIBLE);
                                    }
                                }
                            }
                        }
                    }

                    if (da > Radius_VV)
                    {
                        EventBus.getDefault().post("abc","MyControlTouched");
                    }


                case MotionEvent.ACTION_MOVE:

                    if (!bCanMoved && !dstRect.contains(x, y))
                    {
                        return true;
                    }
                    bCanMoved = true;
                    if (da <= Radius_VV)
                    {
                        if ((nType & TYPE_X_Acceleration) == 0)
                            cirPoint.x = x;
                        if ((nType & TYPE_Y_Acceleration) == 0)
                            cirPoint.y = y;

                        touchPoint.x = x;
                        touchPoint.y = y;
                    }
                    else
                        {
                        da = (int) Math.sqrt(da);
                        x = dx * Radius_V1 / da;
                        y = dy * Radius_V1 / da;
                        x += centx;
                        y += centy;

                        if ((nType & TYPE_X_Acceleration) == 0)
                            cirPoint.x = x;
                        if ((nType & TYPE_Y_Acceleration) == 0)
                            cirPoint.y = y;
                        touchPoint.x = x;
                        touchPoint.y = y;

                    }
                    if(bFlyType)
                    {
                        if ((nType & TYPE_X_Acceleration) != 0 && (nType & TYPE_Y_Acceleration) != 0)
                        {


                            myRipple.post(new Runnable() {
                                @Override
                                public void run() {
                                    int left = touchPoint.x -ripple_params.width/2;
                                    int top =  touchPoint.y-ripple_params.height/2;
                                    ripple_params.leftMargin=left;
                                    ripple_params.topMargin=top;
                                    myRipple.setLayoutParams(ripple_params);
                                }
                            });
                        }
                    }

                    invalidate();
                    return true;
                case MotionEvent.ACTION_UP:
                    bCanMoved = false;
                    bTouched = false;
                    if(myRipple.getVisibility()==VISIBLE)
                    {
                        myRipple.F_SetPlay(false);
                        mHandler.removeCallbacksAndMessages(null);
                        /*
                        mHandler.postDelayed(new Runnable() {
                            @Override
                            public void run() {
                                myRipple.setVisibility(INVISIBLE);
                            }
                        },1000);
                        */

                    }

                    if ((nType & TYPE_RestCentX) != 0 & (nType & TYPE_X_Acceleration) == 0) {
                        cirPoint.x = centx;
                    }
                    if ((nType & TYPE_RestCentY) != 0 & (nType & TYPE_Y_Acceleration) == 0) {
                        cirPoint.y = centy;
                    }
                 //   touchPoint.x = x;
                 //   touchPoint.y = y;
                    invalidate();
                    break;
            }
            return true;
        }


        public void F_SetVLR(boolean bRight) {
            bVbarRight = bRight;
            invalidate();

        }


        public void F_SetX(int x) {
            if (x < 0)
                x = 0;
            if (x > 0xFF)
                x = 0xff;
            x -= 0x7F;
            x *= Radius_V1;
            x /= 0x7F;
            cirPoint.x = x + centx;
            invalidate();
        }

        public int F_GetX() {
            int x = 0;
            float dx;//= (Radius_V1*2)/(255.0f);
            Point  point = new Point(0,0);
            point.x =  cirPoint.x;
            if(bFlyType)
            {
                if ( (nType & TYPE_Y_Acceleration) != 0 ||
                        (nType & TYPE_Y_Acceleration) != 0 )
                {
                    if(!bTouched)
                    {
                        return 0x80;
                    }

                }
            }

            int zx = centx - Radius_V1;
            int xx = point.x - zx;
            dx = ((float) xx) / (Radius_V1 * 2);
            dx *= 256;
            x = (int) dx;
            int ds = (int) (dx * 10);
            ds %= 10;
            if (ds > 5) {
                x += 1;
            }
            if (x > 255) {
                x = 255;
            }
            return x;
        }

        public void F_SetY(int y) {
            if (y < 0)
                y = 0;
            if (y > 0xFF)
                y = 0xFF;
            y = 0xFF - y;
            y -= 0x7F;
            y *= Radius_V1;
            y = y / 0x7F;
            cirPoint.y = y + centy;
            invalidate();
        }

        public int F_GetY() {

            int y = 0;
            float dy;//= (Radius_V1*2)/(255.0f);


            Point point = new Point(0,0);
            point.y =cirPoint.y;


            if(bFlyType)
            {
                if ( (nType & TYPE_Y_Acceleration) != 0 ||
                        (nType & TYPE_Y_Acceleration) != 0 )
                {
                    if(!bTouched)
                    {
                        return 0x80;
                    }
                }
            }

            int zy = centy + Radius_V1;
            int yy = zy - point.y;



            dy = ((float) yy) / (Radius_V1 * 2);
            dy *= 256;
            y = (int) dy;
            int ds = (int) (dy * 10);
            ds %= 10;
            if (ds > 5) {
                y += 1;
            }
            if (y > 255) {
                y = 255;
            }
            return y;
        }

        public void F_SetAdjX(int x) {
            adjx = x;
            invalidate();
        }

        public void F_SetAdjY(int y) {
            adjy = y;
            invalidate();
        }

        public int F_GetAdjX() {
            int x = adjx;
            //if (x >= 0x7F && x <= 0x81)
            //    x = 0x80;
            return x;
        }

        public int F_GetAdjY() {
            int x = adjy;
            //if (x >= 0x7F && x <= 0x81)
            //    x = 0x80;
            return x;
        }


        public void F_SetType(int n) {
            nType = n;
            if ((nType & TYPE_RestCentX) != 0) {
                cirPoint.x = centx;
            }
            if ((nType & TYPE_RestCentY) != 0) {
                cirPoint.y = centy;
            }
        /*
        if (n == TYPE_Direction_Acceleration ||
                n == Type_Engine ||
                n == TYPE_Direction_Manual) {

            if (n == Type_Engine && nType != Type_Engine)
            {
            //    cirPoint.y = 0xff;
                ;
            }
            nType = n;
        }
        */
        }



        public void F_ReasetEngine()
        {
            if ((nType & TYPE_RestCentY) != 0)
            {
                cirPoint.y = centy;
            }
            else
            {
                cirPoint.y = centy + Radius_V1;
            }
            invalidate();
        }

        public void F_ReasetDir() {
            cirPoint.x = centx;
            cirPoint.y = centy;
            invalidate();
        }


        public void F_AdjReset() {
            adjx = 0x80;
            adjy = 0x80;
            invalidate();
        }

        public void F_Calculator_Acceleration(int y, int x, int z) {
            if ((nType & TYPE_X_Acceleration) == 0 && (nType & TYPE_Y_Acceleration) == 0)
                return;
            if(bFlyType) {
                if (!bTouched) {

                    cirPoint.x = centx;
                    cirPoint.y = centy;
                    return;

                }
            }

        //    Log.e("acceleration", "X:" + x + "   Y=" + y);
            x *= 0.8;
            y *= 0.8;
            if (x != 0 || y != 0) {
                int da = x * x + y * y;
                if (da <= 8 * 8) {
                    x = 0;
                    y = 0;
                } else if (da > Radius_VV) {
                    da = (int) Math.sqrt(da);
                    int r = Radius_V1;
                    x = x * r / da;
                    y = y * r / da;

                }

                Activity activitya = (Activity) this.getContext();
                JH_App.F_GetOrg(activitya);
                if ((nType & TYPE_X_Acceleration) != 0) {
                    if (JH_App.nOrg == ActivityInfo.SCREEN_ORIENTATION_REVERSE_LANDSCAPE) {
                        cirPoint.x = centx - x;
                    } else
                    {
                        cirPoint.x = centx + x;
                    }
                }
                if ((nType & TYPE_Y_Acceleration) != 0) {
                    if (JH_App.nOrg == ActivityInfo.SCREEN_ORIENTATION_REVERSE_LANDSCAPE) {
                        cirPoint.y = centy - y;
                    } else

                    {
                        cirPoint.y = centy + y;
                    }
                }
                invalidate();

            }
        }

    }

    private  class  MyPrivView extends RelativeLayout
    {
        public MyPrivView(Context context, AttributeSet attrs) {
            super(context, attrs);
            init();
        }

        public MyPrivView(Context context, AttributeSet attrs, int defStyle) {
            super(context, attrs, defStyle);
            init();
        }

        public MyPrivView(Context context) {
            super(context);
            init();
        }
        private void  init()
        {


        }

        @Override
        public boolean onTouchEvent(MotionEvent event) {
            return true;

        }

    }

    public class PathViewA extends RelativeLayout {

        public boolean bNeedDrawIcon = false;
        static final int nSTep = 0x65;
        public int nDelaySet = 40;
        public int nDelay;   //50,70,90,110,130
        // 定义画笔
        private Paint mPaint;
        // 用于获取文字的宽和高
        //private Rect mBounds;
        //计数值，每点击一次本控件，其值增加1
        private int mCount;

        public int nX2, nY2;
        private int nPointInx;

        ArrayList<Point> pointArray;
        private Bitmap pathIcon;// = new ImageView(this);

        private Path mPath;// = new Path();
        private PathMeasure mPathMeasure;
        float[] mCurrentPosition;// = new float[2];
        //private  boolean  bStartAnti = false;


        public boolean bFlight = false;

        private ImageView imageView;


        private static final int need_No = 0;
        private static final int need_Reset_Ani = 1;


        private float IconWidth;

        private int need_pro = need_No;

        public PathViewA(Context context, AttributeSet attrs) {
            super(context, attrs);
            init();
        }

        public PathViewA(Context context, AttributeSet attrs, int defStyle) {
            super(context, attrs, defStyle);
            init();
        }

        public PathViewA(Context context) {
            super(context);
            init();
        }

        @Subscriber(tag = "GP4225_GetGsensorData")
        private  void GP4225_GetGsensorData(GP4225_Device.GsensorData data)
        {

        }

        @Subscriber(tag = "Send2_path")
        public void Send2_path(Integer n) {
            if(bFlyType)
            {
                if (n == 2) {
                    nDelaySet = 90;
                } else if (n == 3) {
                    nDelaySet = 110;
                } else {
                    nDelaySet = 70;  //40
                }
            }
            else {
                if (n == 2) {
                    nDelaySet = 70;
                } else if (n == 3) {
                    nDelaySet = 110;
                } else {
                    nDelaySet = 50;  //40
                }
            }
        }

        PathInterpolator pathInterpolator;
        AnimatorSet animSet;
        AnimatorSet animSetActive;

        ValueAnimator valueAnimator;

        private void init() {
            setWillNotDraw(false);
            EventBus.getDefault().register(this);
            IconWidth = Storage.dip2pxF(this.getContext(), 20) / 2;
            nX2 = nY2 = 0x80;
            valueAnimator = null;

            mPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
            mPaint.setAntiAlias(true);
            mPaint.setStyle(Paint.Style.STROKE);
            imageView = new ImageView(this.getContext());




            if(bFlyType)
            {
                nDelaySet = 70;
                pathIcon = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.path_icon_fly);
                imageView.setImageResource(R.mipmap.path_icon_fly);
                mPaint.setStrokeWidth(Storage.dip2px(this.getContext(), 4));
                //mPaint.setColor(Color.WHITE);
                mPaint.setColor(Color.argb(255,200 ,200,200));
            }
            else {
                nDelaySet = 50;
                pathIcon = BitmapFactory.decodeResource(this.getContext().getResources(), R.mipmap.path_jh);
                imageView.setImageResource(R.mipmap.path_jh);
                mPaint.setStrokeWidth(Storage.dip2px(this.getContext(), 8));
                mPaint.setColor(Color.BLACK);
            }
            mPath = new Path();

            pointArray = new ArrayList<Point>();
            mPath.reset();
            mCurrentPosition = new float[2];
            bNeedDrawIcon = false;


            LayoutParams param = new LayoutParams(
                    (int) IconWidth * 2, (int) IconWidth * 2);
            this.addView(imageView, param);
            imageView.setVisibility(INVISIBLE);
            animSet = new AnimatorSet();
            animSet.addListener(new Animator.AnimatorListener() {
                @Override
                public void onAnimationStart(Animator animation) {
                    imageView.setVisibility(VISIBLE);
                    bFlight = true;
                }

                @Override
                public void onAnimationEnd(Animator animation) {
                    F_Remove();
                    bFlight = false;

                    if (need_pro == need_No) {
                        invalidate();
                    }
                }

                @Override
                public void onAnimationCancel(Animator animation) {
                    F_Remove();
                    bFlight = false;
                }

                @Override
                public void onAnimationRepeat(Animator animation) {
                }
            });


            valueAnimator = new ValueAnimator(); //ValueAnimator.ofFloat(0, mPathMeasure.getLength());
            valueAnimator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
                @Override
                public void onAnimationUpdate(ValueAnimator animation) {
                    float value = (Float) animation.getAnimatedValue();
                    mPathMeasure.getPosTan(value, mCurrentPosition, null);
                    postInvalidate();
                }
            });
            valueAnimator.addListener(new Animator.AnimatorListener() {
                @Override
                public void onAnimationStart(Animator animation) {
                    Log.d("ani", "animation start");
                    Log.e("ani", "point size = " + pointArray.size());
                    bNeedDrawIcon = true;
                    bFlight = true;
                }

                @Override
                public void onAnimationEnd(Animator animation) {
                    Log.d("qijian", "animation end");
                    bNeedDrawIcon = false;
                    bFlight = true;
                    nX2 = nY2 = 0x80;
                    if (need_pro == need_No) {
                        //pointArray.clear();
                        invalidate();
                    }
                }

                @Override
                public void onAnimationCancel(Animator animation) {
                    Log.d("qijian", "animation cancel");
                    bNeedDrawIcon = false;
                    bFlight = true;

                }

                @Override
                public void onAnimationRepeat(Animator animation) {
                    Log.d("qijian", "animation repeat");
                }
            });
        }


        private void F_Remove() {
            imageView.setVisibility(INVISIBLE);
        }


        private void F_StartA() {
            bNeedDrawIcon = false;
            need_pro = need_No;
            Path path = new Path();
            path.reset();
            for (int i = 0; i < pointArray.size(); i++) {
                Point pointa = pointArray.get(i);
                if (i == 0) {
                    path.moveTo(pointa.x, pointa.y);
                } else {
                    Point pointb = pointArray.get(i - 1);
                    float cX = (pointa.x + pointb.x) / 2;
                    float cY = (pointa.y + pointb.y) / 2;
                    path.quadTo(cX, cY, pointa.x, pointa.y);

                }
            }
            mPathMeasure = new PathMeasure(path, false);
            int nleng = pointArray.size();

            if (nleng <= 1) {
                bFlight = false;
                return;
            }
            nDelay = nDelaySet;
            long duration = (long) nleng * nDelay;
            valueAnimator.setFloatValues(0, mPathMeasure.getLength());
            valueAnimator.setDuration(duration);
            valueAnimator.setInterpolator(new LinearInterpolator());
            valueAnimator.start();
            nPointInx = 0;
            AdjXY_handler.removeCallbacksAndMessages(null);
            AdjXY_handler.post(AdjXY_Run);
        }


        public void startPathAnimA(long duration) {
            int nleng = 0;
            bNeedDrawIcon = false;

            if (valueAnimator.isRunning()) {
                need_pro = need_Reset_Ani;
                valueAnimator.cancel();
                Handler handler = new Handler();
                Runnable runA = new Runnable() {
                    @Override
                    public void run() {
                        F_StartA();
                    }
                };
                handler.postDelayed(runA, 25);
                return;
            } else {
                F_StartA();
            }
        }


        public void startPathAnimB(long du) {
            if (animSet.isRunning()) {
                need_pro = need_Reset_Ani;
                animSet.cancel();
                Handler handler = new Handler();
                Runnable runB = new Runnable() {
                    @Override
                    public void run() {
                        F_StartB();
                    }
                };
                handler.postDelayed(runB, 25);
            } else {
                F_StartB();
            }
        }

        private void F_StartB() {
            need_pro = need_No;
            Path path = new Path();
            int nLen = pointArray.size();
            Log.v("ddd  ", "nLen = " + nLen);
            if (nLen <= 1) {
                return;
            }

            float preX, preY, XX, YY;
            for (int i = 0; i < pointArray.size(); i++) {
                Point pointa = pointArray.get(i);
                preX = pointa.x - IconWidth;
                preY = pointa.y - IconWidth;
                if (i == 0) {
                    path.moveTo(preX, preY);
                } else {
                    Point pointb = pointArray.get(i - 1);
                    preX = pointb.x - IconWidth;
                    preY = pointb.y - IconWidth;
                    float cX = (pointa.x + pointb.x) / 2;
                    float cY = (pointa.y + pointb.y) / 2;
                    XX = cX - IconWidth;
                    YY = cY - IconWidth;
                    path.lineTo(XX, YY);
                    mPath.quadTo(XX, YY, preX, preY);
                }
            }

            ObjectAnimator traslateAnimator = null;
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                traslateAnimator = ObjectAnimator.ofFloat(imageView, "x", "y", path);
            }
            animSet.playTogether(traslateAnimator);
            int nleng = pointArray.size();
            nDelay = nDelaySet;
            nleng = nleng * nDelay;
            animSet.setInterpolator(new LinearInterpolator());
            animSet.setDuration(nleng);
            animSet.start();
            nPointInx = 0;
            AdjXY_handler.removeCallbacksAndMessages(null);
            AdjXY_handler.post(AdjXY_Run);
        }

        public void StoppathAnim() {
            need_pro = need_No;
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                animSet.cancel();
            } else {
                if (valueAnimator != null) {
                    valueAnimator.cancel();
                }
            }
            bFlight = false;
            AdjXY_handler.removeCallbacksAndMessages(null);
            pointArray.clear();
            invalidate();
        }


        Handler AdjXY_handler = new Handler();
        Runnable AdjXY_Run = new Runnable() {
            @Override
            public void run() {
                boolean bRe = F_AdjXY();
                if (bRe) {
                    if (nPointInx < 3) {
                        AdjXY_handler.postDelayed(this, 10);
                    } else {
                        AdjXY_handler.postDelayed(this, nDelay);
                    }
                }
            }
        };

        private boolean F_AdjXY() {
            if (nPointInx < pointArray.size()) {
                if (nPointInx > 1) {
                    int dx;
                    int dy;
                    Point point = pointArray.get(nPointInx);
                    Point prepoint = pointArray.get(nPointInx - 1);
                    dx = point.x - prepoint.x;
                    dy = -(point.y - prepoint.y);
                    if (dx != 0 && dy != 0) {
                        if (dx != 0) {
                            float angle = (float) Math.atan2((double) dy, (double) dx);
                            float dyy = (float) (Math.abs(Math.sin(angle)) * nSTep);
                            float dxx = (float) (Math.abs(Math.cos(angle)) * nSTep);
                            if (dx > 0) {
                                nX2 = (int) (0x80 + dxx);
                                if (dy > 0) {
                                    nY2 = (int) (0x80 + dyy);
                                } else if (dy < 0.0f) {
                                    nY2 = (int) (0x80 - dyy);
                                } else {
                                    nY2 = 0x80;
                                }
                            } else if (dx < 0) {
                                nX2 = (int) (0x80 - dxx);
                                if (dy > 0) {
                                    nY2 = (int) (0x80 + dyy);
                                } else if (dy < 0) {
                                    nY2 = (int) (0x80 - dyy);
                                } else {
                                    nY2 = 0x80;
                                }
                            }
                        } else {
                            nX2 = 0x80;
                            if (dy > 0) {
                                nY2 = 0x80 + nSTep;
                            } else if (dy < 0) {
                                nY2 = 0x80 - nSTep;
                            } else {
                                nY2 = 0x80;
                            }
                        }
                    }
                } else {
                }

                if (nPointInx < 2) {
                    nX2 = 0x80;
                    nY2 = 0x80;
                }

                //String strX = String.format("%02X",nX2);
                //String strY = String.format("%02X",nY2);
                //Log.e("path","X="+strX+" Y="+strY);
                nPointInx++;
                if (nPointInx >= pointArray.size()) {
                    bFlight = false;
                    return false;
                }
            } else {
                bFlight = false;
                return false;
            }
            return true;
        }


        public void startPathAnim(long duration) {

            AdjXY_handler.removeCallbacksAndMessages(null);
            nPointInx = 0;
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                startPathAnimB(0);
            } else {
                startPathAnimA(0);
            }

        }

        public void F_Clear() {
            bNeedDrawIcon = false;
            mPath.reset();
            pointArray.clear();
            invalidate();
        }

        @Override
        protected void onDraw(Canvas canvas) {
            super.onDraw(canvas);
            if (pointArray.size() > 2) {
                mPath.reset();
                for (int i = 0; i < pointArray.size(); i++) {
                    Point pointa = pointArray.get(i);
                    if (i == 0) {
                        mPath.moveTo(pointa.x, pointa.y);
                    } else {
                        Point pointb = pointArray.get(i - 1);
                        int cX = (pointa.x + pointb.x) / 2;
                        int cY = (pointa.y + pointb.y) / 2;
                        mPath.quadTo(cX, cY, pointa.x, pointa.y);
                    }
                }
                canvas.drawPath(mPath, mPaint);

                if (bNeedDrawIcon) {
                    RectF dstRect = new RectF(mCurrentPosition[0] - IconWidth, mCurrentPosition[1] - IconWidth, mCurrentPosition[0] + IconWidth, mCurrentPosition[1] + IconWidth);
                    canvas.drawBitmap(pathIcon, null, dstRect, null);
                }
            }

        }

        @Override
        protected void onSizeChanged(int width, int height, int oldw, int oldh) {

        }

        int  nActiveInx=-1;

        @Override
        public boolean onTouchEvent(MotionEvent event) {

            int action = event.getAction() & MotionEvent.ACTION_MASK;
            if (MotionEvent.ACTION_DOWN == action) {
                nActiveInx = event.getActionIndex();
                F_TouchBegin(event);
            }

            if (MotionEvent.ACTION_UP == action || MotionEvent.ACTION_POINTER_UP== action) {
                int nix = event.getActionIndex();
                if(nix == nActiveInx) {
                    nActiveInx = -1;
                    F_TouchEnd(event);
                }
            }
            if (MotionEvent.ACTION_MOVE == action) {
                int nix = event.getActionIndex();
                if(nix == nActiveInx) {
                    F_Touchmoved(event);
                }
            }
            return true;
        }

        private void F_TouchBegin(MotionEvent event) {
            imageView.setVisibility(INVISIBLE);
            pointArray.clear();
            bNeedDrawIcon = false;
            Point point = new Point();
            float x = event.getX();
            float y = event.getY();
            point.x = (int) x;
            point.y = (int) y;
            pointArray.add(point);
            invalidate();
        }

        private void F_TouchEnd(MotionEvent event) {
            F_Touchmoved(event);
            startPathAnim(0);
        }


        private void F_Touchmoved(MotionEvent event) {
            Point prevPointA;
            Point point = new Point();

            int x = (int) event.getX();
            int y = (int) event.getY();
            point.x = x;
            point.y = y;

            if (pointArray.size() > 0) {
                prevPointA = pointArray.get(pointArray.size() - 1);
                float dx1, dy1, dx, dy, sq;
                dx1 = dx = point.x - prevPointA.x;
                dy1 = dy = point.y - prevPointA.y;
                dx = dx * dx;
                dy = dy * dy;

                sq = (float) Math.sqrt((double) (dx + dy));
                float dw = Storage.dip2pxF(this.getContext(), 5);
                if (sq < dw) {
                    return;
                } else {
                    int df = (int) (sq / dw);
                    if (df > 1) {
                        float stp = dx1 / df;
                        df -= 1;
                        Point pp = new Point();
                        for (int ix = 0; ix < df; ix++) {
                            pp.x = (int) (prevPointA.x + (ix + 1) * stp);
                            if (dx1 != 0.00f) {
                                pp.y = (int) (prevPointA.y + (ix + 1) * stp * (dy1 / dx1));
                            } else {
                                pp.y = prevPointA.y;
                            }
                            pointArray.add(pp);
                        }
                        pointArray.add(point);
                    } else {
                        pointArray.add(point);
                    }
                }
            } else {
                pointArray.add(point);
            }
            invalidate();
        }
    }
}
