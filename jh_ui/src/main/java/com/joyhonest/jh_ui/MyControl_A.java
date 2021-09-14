package com.joyhonest.jh_ui;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Point;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;
import android.widget.RelativeLayout;

/**
 * Created by aiven on 2017/10/11.
 */

public class MyControl_A extends RelativeLayout {

    private RockeView rock1;
    private RockeView rock2;
    private int nWW;
    private int nCWW;
    private int nRectW = 1;
    private int nRectH = 1;


    private Rect rectleft;
    private Rect rectright;


    private Bitmap bmp1;
    private Bitmap bmp2;


    public void F_SetBackFly(Bitmap bmp1, Bitmap bmp2) {
        this.bmp1 = bmp1;
        this.bmp2 = bmp2;
    }

    public MyControl_A(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public MyControl_A(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        init();
    }

    public MyControl_A(Context context) {
        super(context);
        init();
    }

    /////////////
    //获取坐标
    public int F_GetX1() {
        return rock1.F_GetX();
    }

    public int F_GetY1() {
        return rock1.F_GetY();
    }

    public int F_GetX2() {
        return rock2.F_GetX();
    }

    public int F_GetY2() {
        return rock2.F_GetY();
    }

    //设定背景图片
    public void F_SetBackGround(int backbmo, int cirbmp) {
        rock1.F_SetBackBmp_CirBmpId(backbmo, cirbmp);
        rock2.F_SetBackBmp_CirBmpId(backbmo, cirbmp);
    }

    public void F_SetBackGroundbyColor(int backbmo, int cirbmp) {
        rock1.F_SetBack_Cir_Color(backbmo, cirbmp);
        rock2.F_SetBack_Cir_Color(backbmo, cirbmp);
    }

    //设定图标大小，W 背景，W1 游标
    public void F_SetW_W(int W, int W1) {
        nWW = W;
        nCWW = W1;
        F_SetSize();

    }
/////////////

    private void init() {
        rectleft = new Rect();
        rectright = new Rect();
        rock1 = new RockeView(getContext());
        rock2 = new RockeView(getContext());
        addView(rock1);
        addView(rock2);
        nWW = -1; //Storage.dip2px(getContext(),200);
        nCWW = -1;// Storage.dip2px(getContext(),200/8);
        rock1.F_SetWidth_Height(nWW, nCWW);
        rock2.F_SetWidth_Height(nWW, nCWW);
        setBackgroundColor(0x00FFFFFF);

        //     rock1.F_SetBackBmp_CirBmpId(R.mipmap.cir_back_jh,R.mipmap.cir_jh);
        //     rock2.F_SetBackBmp_CirBmpId(R.mipmap.cir_back_jh,R.mipmap.cir_jh);
    }


    private void F_SetSize() {
        if (nCWW > 0 && nWW > 0) {
            rock1.F_SetWidth_Height(nWW, nCWW);
            rock2.F_SetWidth_Height(nWW, nCWW);

        } else {
            int ws = nRectW / 2;
            int hs = nRectH;
            int ww = Math.min(ws, hs);
            ww = (ww * 2) / 3;
            int cw = ww / 6;
            rock1.F_SetWidth_Height(ww, cw);
            rock2.F_SetWidth_Height(ww, cw);
        }
    }


    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {

        nRectW = w;
        nRectH = h;

        rectleft.top = 0;
        rectleft.left = 0;
        rectleft.bottom = nRectH;
        rectleft.right = nRectW / 2;

        rectright.top = 0;
        rectright.left = nRectW / 2;
        rectright.bottom = nRectH;
        rectright.right = nRectW;
        F_SetSize();
        super.onSizeChanged(w, h, oldw, oldh);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {

        int action = event.getAction();
        int action_code = action & MotionEvent.ACTION_MASK;
        int pointIndex = action >> 8;

        int x = (int) event.getX(pointIndex);
        int y = (int) event.getY(pointIndex);
        int ida = event.getPointerId(pointIndex);

        int index1;
        int index2;

        switch (action_code) {
            case MotionEvent.ACTION_DOWN:

                if (rectleft.contains(x, y)) {
                    rock1.nTouchID = ida;
                    rock1.F_SetCenter(x, y);
                } else if (rectright.contains(x, y)) {
                    rock2.nTouchID = ida;
                    rock2.F_SetCenter(x, y);
                }
                break;
            case MotionEvent.ACTION_POINTER_DOWN:

                if (rectleft.contains(x, y)) {
                    if (rock1.nTouchID < 0) {
                        rock1.nTouchID = ida;
                        rock1.F_SetCenter(x, y);
                    }
                } else if (rectright.contains(x, y)) {
                    if (rock2.nTouchID < 0) {
                        rock2.nTouchID = ida;
                        rock2.F_SetCenter(x, y);
                    }
                }
                break;
            case MotionEvent.ACTION_MOVE:

                index1 = event.findPointerIndex(rock1.nTouchID);
                index2 = event.findPointerIndex(rock2.nTouchID);
                if (index1 != -1) {
                    x = (int) event.getX(index1);
                    y = (int) event.getY(index1);
                    rock1.F_Update(x, y);

                }

                if (index2 != -1) {
                    x = (int) event.getX(index2);
                    y = (int) event.getY(index2);
                    rock2.F_Update(x, y);
                }
                break;
            case MotionEvent.ACTION_POINTER_UP:


                if (rock1.nTouchID == ida) {
                    rock1.nTouchID = -1;
                    rock1.F_SetCenter(-1, -1);
                }
                if (rock2.nTouchID == ida) {
                    rock2.nTouchID = -1;
                    rock2.F_SetCenter(-1, -1);
                }

                break;
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_CANCEL:

                rock1.nTouchID = -1;
                rock2.nTouchID = -1;
                rock1.F_SetCenter(-1, -1);
                rock2.F_SetCenter(-1, -1);
                break;
        }
        return true;
    }

    private class RockeView extends View {
        public int nTouchID = -1;
        private Point mCenter;
        private Bitmap mBackBmp = null;
        private Bitmap mCirBmp = null;

        private int mBackColor = Color.LTGRAY;
        private int ietmCirColor = Color.WHITE;

        private int mWidth;
        //private  int  mHeight;
        private int mWidth_cir;
        //private  int  mHeight_cir;

        public int XX = 0x80;
        public int YY = 0x80;


        private int nPointX = -1;
        private int nPointY = -1;

        private int nCirX = -1;
        private int nCirY = -1;

        private boolean bDisplay = true;


        public int F_GetX() {

            int w = mWidth / 2 - mWidth_cir / 2;
            if (mCenter.x == nCirX) {
                return 0x80;
            }
            float nb = (float) (nCirX - (mCenter.x - w));

            int re = (int) (255 * (nb / (w * 2)));
            if (re < 0)
                re = 0;
            if (re > 255)
                re = 255;
            return re;

        }

        public int F_GetY() {

            int w = mWidth / 2 - mWidth_cir / 2;
            if (mCenter.y == nCirY) {
                return 0x80;
            }
            float nb = (float) (nCirY - (mCenter.y - w));
            int re = (int) (255 * (nb / (w * 2)));
            re = 255 - re;
            if (re < 0)
                re = 0;
            if (re > 255)
                re = 255;
            return re;

        }


        public RockeView(Context context, AttributeSet attrs) {
            super(context, attrs);
            init();
        }

        public RockeView(Context context, AttributeSet attrs, int defStyle) {
            super(context, attrs, defStyle);
            init();
        }

        public RockeView(Context context) {
            super(context);
            init();
        }

        private void init() {
            XX = 0x80;
            YY = 0x80;
            mWidth = -1;
            //mHeight=-1;
            nPointX = -1;
            nPointY = -1;
            mCenter = new Point(-1, -1);
        }

        public void F_SetBackBmp_CirBmpId(int background, int cirbmp) {
            if (background < 0) {
                mBackBmp = null;
            } else {
                mBackBmp = BitmapFactory.decodeResource(getResources(), background);
            }
            if (cirbmp < 0) {
                mCirBmp = null;
            } else {
                mCirBmp = BitmapFactory.decodeResource(getResources(), cirbmp);
            }

            invalidate();
        }

        public void F_SetBack_Cir_Color(int backgroundcolor, int cirbmpcolor) {
            mBackColor = backgroundcolor;
            ietmCirColor = cirbmpcolor;
            invalidate();
        }


        public void F_SetWidth_Height(int w, int wcir) {
            //mHeight =
            mWidth = w;
            //mHeight_cir =
            mWidth_cir = wcir;
            invalidate();
        }

        public void F_Update(int x, int y) {
            nPointX = x;
            nPointY = y;
            if (x < 0 || y < 0) {
                nPointX = mCenter.x;
                nPointY = mCenter.y;
                bDisplay = false;
            } else {
                bDisplay = true;
            }

            int cirx, ciry;
            int centx = mCenter.x;
            int centy = mCenter.y;
            int dx = nPointX - centx;
            int dy = nPointY - centy;


            int da = dx * dx + dy * dy;
            da = (int) Math.sqrt(da);


            int r = mWidth / 2 - mWidth_cir / 2;
            if (da < r) {
                nCirX = nPointX;
                nCirY = nPointY;
            } else {
                cirx = dx * r / da;
                cirx += centx;

                ciry = dy * r / da;
                ciry += centy;

                nCirX = cirx;
                nCirY = ciry;
            }

            invalidate();
        }


        public void F_SetCenter(int x, int y) {
            mCenter.x = x;
            mCenter.y = y;
            nCirX = x;
            nCirY = y;
            invalidate();
        }


        @Override
        protected void onSizeChanged(int w, int h, int oldw, int oldh) {
            super.onSizeChanged(w, h, oldw, oldh);
            //mCenter.x=-1;
            //mCenter.y=-1;
        }

        @Override
        protected void onDraw(Canvas canvas) {
            super.onDraw(canvas);
            Paint paint = new Paint();
            paint.setAntiAlias(true);
            if (mCenter.x < 0 || mCenter.y < 0 || mWidth < 0 || nCirX < 0 || nCirY < 0 || !bDisplay) {
                return;
            }
            if (mBackBmp != null) {
                Rect dstRect = new Rect(mCenter.x - mWidth / 2, mCenter.y - mWidth / 2, mCenter.x + mWidth / 2, mCenter.y + mWidth / 2);
                canvas.drawBitmap(mBackBmp, null, dstRect, null);
            } else {
                paint.setColor(mBackColor);
                canvas.drawCircle(mCenter.x, mCenter.y, mWidth / 2, paint);
            }

            if (mCirBmp != null) {
                Rect dstRect = new Rect(nCirX - mWidth_cir / 2, nCirY - mWidth_cir / 2, nCirX + mWidth_cir / 2, nCirY + mWidth_cir / 2);
                canvas.drawBitmap(mCirBmp, null, dstRect, null);
            } else {
                paint.setColor(ietmCirColor);
                canvas.drawCircle(nCirX, nCirY, mWidth_cir / 2, paint);
            }
        }
    }
}
