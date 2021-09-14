package com.joyhonest.jh_ui;

/**
 * Created by aivenlau on 2017/2/9.
 */

import android.content.Context;
import android.os.Environment;
import android.os.StatFs;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;


public class Storage {

    public static final String VENDOR = "JH-WIFI-FPV";

    /**
     * 获取手机自身内存路径
     */

    public static String getPhoneCardPath() {
        return Environment.getDataDirectory().getPath();

    }

    /**
     * 获取sd卡路径 双sd卡时，根据”设置“里面的数据存储位置选择，获得的是内置sd卡或外置sd卡
     *
     * @return
     */
    public static String getNormalSDCardPath() {
        return Environment.getExternalStorageDirectory().getPath();
    }


    // 查看所有的外部sd路径
    public static String getSDCardPathEx() {
        String mount = new String();
        try {
            Runtime runtime = Runtime.getRuntime();
            Process proc = runtime.exec("mount");
            InputStream is = proc.getInputStream();
            InputStreamReader isr = new InputStreamReader(is);
            String line;
            long minSize = (1024L * 1024L * 1024L);

            BufferedReader br = new BufferedReader(isr);
            while ((line = br.readLine()) != null) {
                if (line.contains("secure"))
                    continue;
                if (line.contains("asec"))
                    continue;

                if (line.contains("fat") || line.contains("fuse")) {
                    String columns[] = line.split(" ");
                    if (columns != null && columns.length > 1) {


                        if ((getTotalSize(columns[1]) > minSize) && (!getNormalSDCardPath().equals(columns[1]))) {
                            mount = mount.concat(columns[1] + ";");

                        }
                    }
                }
            }
        } catch (FileNotFoundException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        return mount;
    }

    // 获取当前路径，整个空间
    public static long getTotalSize(String path) {
        try {
            File base = new File(path);
            StatFs stat = new StatFs(base.getPath());
            long nTotalCount = stat.getBlockSizeLong() * stat.getBlockCountLong();
            return nTotalCount;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return 0;
    }

    // 获取当前路径，可用空间
    public static long getAvailableSize(String path) {
        try {
            File base = new File(path);
            StatFs stat = new StatFs(base.getPath());
            long nAvailableCount = stat.getBlockSizeLong() * stat.getAvailableBlocksLong();
            long nTotalCount = stat.getBlockSizeLong() * stat.getBlockCountLong();
            return nAvailableCount;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return 0;
    }


    /**
     * 根据手机的分辨率从 dp 的单位 转成为 px(像素)
     */
    public static int dip2px(Context context, float dpValue) {
        final float scale = context.getResources().getDisplayMetrics().density;
        return (int) (dpValue * scale + 0.5f);
    }

    public static float dip2pxF(Context context, float dpValue) {
        final float scale = context.getResources().getDisplayMetrics().density;
        return dpValue * scale;
    }


    /**
     * 根据手机的分辨率从 px(像素) 的单位 转成为 dp
     */
    public static int px2dip(Context context, float pxValue) {
        final float scale = context.getResources().getDisplayMetrics().density;
        return (int) (pxValue / scale + 0.5f);
    }
}
