package com.joyhonest.jh_fly;

import android.animation.ObjectAnimator;
import android.content.Context;
import android.graphics.Bitmap;

import androidx.annotation.Nullable;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.widget.ImageView;
import android.widget.RelativeLayout;

import com.joyhonest.wifination.R;
import com.joyhonest.jh_ui.Storage;

import org.simple.eventbus.EventBus;

/**
 * Created by aiven on 2017/11/18.
 */

public class MySwitch extends RelativeLayout {

    private Bitmap bmpLeft;
    private Bitmap bmpRight;
    private ImageView img;

    public boolean bMenu = false;

    int firstX = 0;
    int firstY = 0;
    boolean bLeft = true;

    public MySwitch(Context context) {
        super(context);
        F_Init();
    }

    public MySwitch(Context context, AttributeSet attrs) {
        super(context, attrs);
        F_Init();
    }

    public MySwitch(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        F_Init();
    }

    private int nWidth = 0;
    private int nHeght = 0;

    public void F_SetMenu(boolean b) {
        bMenu = b;
        if (bMenu)
            img.setBackgroundResource(R.mipmap.left_menu_icon_fly_jh);
        else
            img.setBackgroundResource(R.mipmap.photo_icon_a_fly_jh);

    }

    private void F_Init() {
        bLeft = true;
        ImageView imgb = new ImageView(getContext());
        imgb.setBackgroundResource(R.mipmap.switch_back_1_fly_jh);
        this.addView(imgb);
        img = new ImageView(getContext());
        img.setBackgroundResource(R.mipmap.photo_icon_a_fly_jh);
        this.addView(img);
    }

    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        super.onSizeChanged(w, h, oldw, oldh);
        nWidth = w;
        nHeght = h;

    }

    int nOp = 0;

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        int action = event.getAction();
        if (action == MotionEvent.ACTION_DOWN) {
            firstX = (int) event.getX();
        }
        nOp = 0;
        if (action == MotionEvent.ACTION_UP) {
            int da = Storage.dip2px(getContext(), 8);
            int x = (int) event.getX();
            if (x - firstX < -da) {

                if (!bLeft) {
                    bLeft = true;
                    nOp = 1;
                    F_DispIcon();
                }
            } else if (x - firstX > da) {
                if (bLeft) {
                    bLeft = false;
                    nOp = 2;
                    F_DispIcon();
                }
            } else {
                bLeft = !bLeft;
                if (bLeft) {
                    nOp = 1;
                } else {
                    nOp = 2;
                }
                F_DispIcon();

            }
            SwitchMesage msg = new SwitchMesage(this, bLeft);
            EventBus.getDefault().post(msg, "SwitchChanged");
        }
        return true;
    }


    private void F_DispIcon() {
        if (nOp == 1) {
            if (bMenu) {
                img.setBackgroundResource(R.mipmap.left_menu_icon_fly_jh);
            } else
                img.setBackgroundResource(R.mipmap.photo_icon_a_fly_jh);
            ObjectAnimator.ofFloat(img, "X", nWidth - (nHeght), 0).setDuration(200).start();
        }
        if (nOp == 2) {
            if (bMenu) {
                img.setBackgroundResource(R.mipmap.right_menu_icon_fly_jh);
            } else
                img.setBackgroundResource(R.mipmap.record_icon_fly_jh);
            ObjectAnimator.ofFloat(img, "X", 0, nWidth - (nHeght)).setDuration(200).start();
        }

    }

    public void F_SetPhoto(boolean b) {
        bLeft = b;
        F_DispIcon();

    }


}
