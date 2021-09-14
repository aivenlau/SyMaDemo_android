package com.joyhonest.test;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.RectF;
import android.os.Build;
import android.util.AttributeSet;
import android.view.View;
import android.widget.ScrollView;

import androidx.annotation.Nullable;
import androidx.annotation.RequiresApi;

import java.util.ArrayList;
import java.util.List;

public class JH_PicListView extends View
{
    private  float nScal=1.0f;

    private List<Bitmap> list;

    private Paint mBitPaint;

    private  void Init()
    {
        list = new ArrayList<Bitmap>();
        mBitPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        mBitPaint.setFilterBitmap(true);
        mBitPaint.setDither(true);
        nScal = 2.0f;
    }

    public void Clear()
    {
        if(list!=null)
        {
            list.clear();
        }
    }

    public  int size()
    {
        return list.size();
    }

    public void AddBmp(Bitmap bmp)
    {
        if(list!=null)
        {
            list.add(bmp);
        }
    }

    public void F_SetScal(float nScal)
    {
        this.nScal = nScal;
        invalidate();
    }

    public JH_PicListView(Context context) {
        super(context);
        Init();

    }

    public JH_PicListView(Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
        Init();
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        int nH = this.getHeight();
        int nW = this.getWidth();

        if(list.size()>2)
        {


                Rect rectDst = new Rect();
                Rect rectSrc = new Rect();
                rectDst.left=0;
                rectDst.top=0;
                rectDst.bottom = nH;
                rectDst.right=0;
                rectSrc.left = rectSrc.top = 0;

                int nW1=1;
                boolean bFirst = true;
                for(Bitmap bmp :list)
                {
                    if(bFirst) {
                        if ((int) (nScal * 100) == 100) {
                            nW1 = nH;
                        } else {
                            nW1 = (int) (nH / nScal);
                        }
                        rectDst.right = nW1;
                        bFirst = false;
                    }

                    rectSrc.bottom=bmp.getHeight();
                    rectSrc.right=bmp.getWidth();
                    canvas.drawBitmap(bmp,rectSrc,rectDst,mBitPaint);
                    rectDst.offset(nW1,0);
                }
        }
    }
}
