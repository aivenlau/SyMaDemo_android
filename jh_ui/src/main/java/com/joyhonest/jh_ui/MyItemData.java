package com.joyhonest.jh_ui;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

import com.joyhonest.wifination.R;

/**
 * Created by aiven on 2017/6/5.
 */

public class MyItemData {
    public String sSDPath;
    public String sPhonePath;
    public Bitmap image;

    public int nDownloadStatus;


    public int nSelectedStatus;
    public boolean bSelected;
    public boolean bNeedDelete;
    public float fPrecent;
    public int nDuration;


    public MyItemData() {
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inSampleSize = 2;
        Bitmap bitmap = BitmapFactory.decodeResource(JH_App.mContext.getResources(), R.mipmap.damageimg_jh, options);
        image = bitmap;
        nDownloadStatus = JH_App.DownLoadNormal;
        bSelected = false;
        bNeedDelete = false;
        sSDPath = "";
        sPhonePath = "";
        nDuration = -1;
        nSelectedStatus = 0;
    }

}
