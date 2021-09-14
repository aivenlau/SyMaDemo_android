package com.joyhonest.jh_ui;

import android.annotation.TargetApi;
import android.app.Activity;
import android.content.ContentResolver;
import android.content.ContentUris;
import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.res.Resources;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.Matrix;
import android.graphics.Point;
import android.location.Criteria;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.media.AudioAttributes;
import android.media.AudioManager;
import android.media.MediaMetadataRetriever;
import android.media.SoundPool;
import android.net.ConnectivityManager;
import android.net.LinkProperties;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.net.NetworkRequest;
import android.net.Uri;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.SystemClock;
import android.provider.MediaStore;
import android.provider.Settings;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Gravity;
import android.view.Surface;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Toast;

import com.joyhonest.wifination.R;
import com.joyhonest.wifination.wifination;

import org.simple.eventbus.EventBus;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.lang.reflect.Method;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.List;
import java.util.Locale;

import static android.content.Context.WIFI_SERVICE;


/**
 * Created by aiven on 2017/8/31.
 */

public class JH_App {


    public static boolean bSensor = false;
    public static boolean bAdj = false;
    public static boolean bUp = false;
    public static boolean bDn = false;
    public static boolean bStop = false;
    public static boolean bHeadLess = false;
    public static boolean bHiSpeed = false;
    public static boolean bVR = false;


    public static boolean bGPSMode = false;


    public static int nStyle_ui = 0;
    public static int nStyle_fly = 1;

    public static int nType = nStyle_ui;

    public static boolean bIsSyMa = true;
    public static boolean bInitDispCtrol = true;

    public static final int Status_Connected = 1;
    public static final int LocalRecording = 2;
    public static final int SD_Ready = 4;
    public static final int SD_Recording = 8;
    public static final int SD_SNAP = 0x10;
    public static final int SSID_CHANGED = 0x20;

    public static final int mainFragmeng_Value = 0;
    public static final int select_Fragment_Value = 1;
    public static final int Grid_Fragment_Value = 2;


    public static final int DownLoadNormal = 0;
    public static final int DownLoaded = 1;
    public static final int DownLoading = 2;
    public static final int DownLoaded_NO = 3;
    public static final int DownLoadNeed = 4;

    public static boolean bDownLoading = false;
    public static int nSdStatus = 0;

    public static boolean bPhone_SNAP = false;
    public static boolean bPhone_Video = false;

    public static boolean bNeedStartsasyRecord = false;


    //public static long nRecTime = 0;

    public static boolean bisPathMode = false;


    public static boolean bBrowSD = false;
    public static boolean bBrowPhoto = false;

    public static boolean b720P = false;
    public static boolean bRecordVoice = true;

    // private static JH_App singleton = null;
    public static String sWifi = "";
    public static int nICType = wifination.IC_SN;

    public static List<MyFilesItem> mSD_PhotosList = null;
    public static List<MyFilesItem> mSD_VideosList = null;

    public static List<String> mLocal_PhotosList = null;
    public static List<String> mLocal_VideosList = null;

    public static List<MyItemData> mGridList = null;
    public static List<Integer> mNeedDownLoadList = null;

    public static List<Integer> mDownloadList = null;


    public static List<String> mDispList = null;


    public static boolean bFlyDisableAll = true;

    public static String sRemotePhoto = null;
    public static String sRemoteVideo = null;
    public static String sLocalPhoto = null;
    public static String sLocalVideo = null;
    public static boolean bUserLgLib = true;

    private static SoundPool soundPool = null;
    private static int music_photo = -1;
    private static int music_mid = -1;
    private static int music_adj = -1;
    private static int music_btn = -1;

    private static int music_speed_L = -1;
    private static int music_speed_H = -1;
    private static int music_video_start = -1;
    private static int music_video_stop = -1;


    public static int nOrg;

    public static Context mContext = null;
    public static int nCheckt = 0;

    private static boolean bGpsInited = false;

    private static LocationManager locationManager;  //= (LocationManager) getSystemService(Context.LOCATION_SERVICE);


    private static String sVendor = "SyMa_GO_Data";


    public static boolean bFlying = false;

    public static int nAdjRota = 0x80;
    public static int nAdjLeftRight = 0x80;
    public static int nAdjForwardBack = 0x80;

    public static void F_ReadSaveSetting(boolean bSave) {
        if (mContext == null)
            return;
        SharedPreferences settings = mContext.getSharedPreferences("AdjSave", 0);
        if (bSave) {
            SharedPreferences.Editor editor = settings.edit();
            editor.putInt("nAdjRota", nAdjRota);
            editor.putInt("nAdjLeftRight", nAdjLeftRight);
            editor.putInt("nAdjForwardBack", nAdjForwardBack);

// 提交本次编辑
            editor.commit();
        } else {
            nAdjRota = settings.getInt("nAdjRota", 0x80);
            nAdjLeftRight = settings.getInt("nAdjLeftRight", 0x80);
            nAdjForwardBack = settings.getInt("nAdjForwardBack", 0x80);
        }


    }

    public JH_App(Context context) {

    }


    public static void F_Clear_not_videoFiles() {
        File f = new File(JH_App.sLocalVideo);
        File[] files = f.listFiles();// 列出所有文件
        if (files == null)
            return;
        String fileName = " ";
        for (File file : files) {
            if (file.exists() && !file.isDirectory()) {
                fileName = file.getAbsolutePath();
                fileName = fileName.toLowerCase();
                if (!fileName.endsWith(".mp4")) {
                    file.delete();
                }
            }
        }
    }


    public static void init(Context contextA, String LocalPhoto, String LocalVideo, String SDPhoto, String SDVideo) {


        if (contextA == null) {
            mContext = null;
            if (soundPool != null) {
                if (music_mid > 0)
                    soundPool.unload(music_mid);
                if (music_adj > 0)
                    soundPool.unload(music_adj);
                if (music_photo > 0)
                    soundPool.unload(music_photo);

            }
            return;
        }
        if (mContext != null)
            return;
        mContext = contextA.getApplicationContext();
        F_ReadSaveSetting(false);


        if (mGridList == null) {
            mSD_PhotosList = new ArrayList<MyFilesItem>();
            mSD_VideosList = new ArrayList<MyFilesItem>();
            mLocal_PhotosList = new ArrayList<String>();
            mLocal_VideosList = new ArrayList<String>();
            mDownloadList = new ArrayList<Integer>();
            mDispList = new ArrayList<String>();
            mNeedDownLoadList = new ArrayList<Integer>();
            mGridList = new ArrayList<MyItemData>();
        }
        F_CreateLocalDir(LocalPhoto, LocalVideo, SDPhoto, SDVideo);
        //  F_InitMusic();
    }

    public static void F_CreateLocalFlyDefalutDir() {
        if (mContext == null)
            return;
        String StroragePath = "";

        try {
            StroragePath = Storage.getNormalSDCardPath();
        } catch (Exception e) {
            return;
        }
        if (StroragePath.length() == 0) {
            StroragePath = Storage.getNormalSDCardPath();
        }

        File fdir;
        boolean bCreateOK = false;
        sLocalPhoto = String.format("%s/SYMA fly/SYMA fly_P", StroragePath);
        fdir = new File(sLocalPhoto);
        if (!fdir.exists()) {
            fdir.mkdirs();
        }
        sLocalVideo = String.format("%s/SYMA fly/SYMA fly_V", StroragePath);
        fdir = new File(sLocalVideo);
        if (!fdir.exists()) {
            fdir.mkdirs();
        }


    }

    public static void F_CreateLocalDir(String LocalPhoto, String LocalVideo, String SDPhoto, String SDVideo) {
        if (mContext == null)
            return;
        boolean bRemote = true;
        String StroragePath = "";

        if (sVendor.length() != 0) {

            File fdir;
            String recDir;
            boolean bCreateOK = false;

            try {
                StroragePath = Storage.getNormalSDCardPath();
            } catch (Exception e) {
                return;
            }
            if (StroragePath.length() == 0) {
                StroragePath = Storage.getNormalSDCardPath();
            }

            bCreateOK = false;
            if (LocalPhoto != null) {
                fdir = new File(LocalPhoto);
                if (!fdir.exists()) {
                    fdir.mkdirs();
                }
                if (fdir.exists()) {
                    sLocalPhoto = LocalPhoto;
                    bCreateOK = true;
                }
            }
            if (!bCreateOK) {
                recDir = String.format("%s/%s/Local/SYMA_Photo_JH", StroragePath, sVendor);
                sLocalPhoto = recDir;
                fdir = new File(recDir);
                if (!fdir.exists()) {
                    fdir.mkdirs();
                }
            }

            bCreateOK = false;
            if (LocalVideo != null) {
                fdir = new File(LocalVideo);
                if (!fdir.exists()) {
                    fdir.mkdirs();
                }
                if (fdir.exists()) {
                    sLocalVideo = LocalVideo;
                    bCreateOK = true;
                }
            }
            if (!bCreateOK) {
                recDir = String.format("%s/%s/Local/SYMA_Video_JH", StroragePath, sVendor);
                sLocalVideo = recDir;
                fdir = new File(recDir);
                if (!fdir.exists()) {
                    fdir.mkdirs();
                }
            }

            bCreateOK = false;
            if (SDPhoto != null) {
                fdir = new File(SDPhoto);
                if (!fdir.exists()) {
                    fdir.mkdirs();
                }
                if (fdir.exists()) {
                    sRemotePhoto = SDPhoto;
                    bCreateOK = true;
                }
            }
            if (!bCreateOK) {
                recDir = String.format("%s/%s/SD/SYMA_SDPhoto_JH", StroragePath, sVendor);
                sRemotePhoto = recDir;
                fdir = new File(recDir);
                if (!fdir.exists()) {
                    fdir.mkdirs();
                }
            }

            bCreateOK = false;
            if (SDVideo != null) {
                fdir = new File(SDVideo);
                if (!fdir.exists()) {
                    fdir.mkdirs();
                }
                if (fdir.exists()) {
                    sRemoteVideo = SDVideo;
                    bCreateOK = true;
                }
            }
            if (!bCreateOK) {
                recDir = String.format("%s/%s/SD/SYMA_SDVideo_JH", StroragePath, sVendor);
                sRemoteVideo = recDir;
                fdir = new File(recDir);
                if (!fdir.exists()) {
                    fdir.mkdirs();
                }
            }
        }
    }

/*
    public static void openGPSSettings() {
        if (nType != JH_App.nStyle_fly)
            return;
        if (locationManager == null)
            locationManager = (LocationManager) mContext.getSystemService(Context.LOCATION_SERVICE);
        if (locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER) || locationManager.isProviderEnabled(LocationManager.NETWORK_PROVIDER)) {
            getLocation();
            return;
        }
        Intent intent = new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS);// ActionLocationSourceSettings);
        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        mContext.startActivity(intent);
    }


     // 定位位置监听器

    private static final LocationListener locationListener = new LocationListener() {
        public void onLocationChanged(Location location) {
            EventBus.getDefault().post(location, "GPS_LocationChanged");
        }

        public void onProviderDisabled(String provider) {
            Location location = new Location(provider);
            location.setLatitude(-400);
            location.setLongitude(-400);
            EventBus.getDefault().post(location, "GPS_LocationChanged");
        }

        public void onProviderEnabled(String provider) {
        }

        @Override
        public void onStatusChanged(String provider, int status, Bundle extras) {
            // 通过GPS获取位置
            Location location = locationManager.getLastKnownLocation(provider);
            EventBus.getDefault().post(location, "GPS_LocationChanged");
        }
    };




    // 如果开启正常，则会直接进入到显示页面，如果开启不正常，则会进行到GPS设置页面

    private static void getLocation() {
        //String serviceName = Context.LOCATION_SERVICE;
        // 查找到服务信息
        Criteria criteria = new Criteria();
        // 高精度
        criteria.setAccuracy(Criteria.ACCURACY_FINE);
        criteria.setAltitudeRequired(false);
        criteria.setBearingRequired(false);
        criteria.setCostAllowed(true);
        // 低功耗
        criteria.setPowerRequirement(Criteria.POWER_LOW);
        // 获取GPS信息
        try {
            String provider = locationManager.getBestProvider(criteria, true);
            // 通过GPS获取位置
            Location location = locationManager.getLastKnownLocation(LocationManager.GPS_PROVIDER);
            if (location == null) {
                location = locationManager.getLastKnownLocation(LocationManager.NETWORK_PROVIDER);
                if (location == null) {
                    location = new Location(provider);
                    location.setLatitude(-400);
                    location.setLongitude(-400);
                }
            }
            EventBus.getDefault().post(location, "GPS_LocationChanged");
            // 设置监听*器，自动更新的最小时间为间隔N秒(1秒为1*1000，这样写主要为了方便)或最小位移变化超过N米
            nCheckt = 0;
            if (bGpsInited) {
                locationManager.removeUpdates(locationListener);
            }
            bGpsInited = true;
            locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, 1000, 2, locationListener);
            locationManager.requestLocationUpdates(LocationManager.NETWORK_PROVIDER, 1000, 2, locationListener);
        } catch (SecurityException e) {
            e.printStackTrace();
        } catch (RuntimeException e) {
            e.printStackTrace();
        }
    }

 */

    public static void F_InitMusic() {
        if (soundPool == null) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                SoundPool.Builder builder = new SoundPool.Builder();
                builder.setMaxStreams(10);//传入音频数量
                AudioAttributes.Builder attrBuilder = new AudioAttributes.Builder();
                attrBuilder.setLegacyStreamType(AudioManager.STREAM_MUSIC);//设置音频流的合适的属性
                builder.setAudioAttributes(attrBuilder.build());//加载一个AudioAttributes
                soundPool = builder.build();
            } else {
                soundPool = new SoundPool(10, AudioManager.STREAM_MUSIC, 0);
            }
        }

        if (music_mid != -1) {
            soundPool.unload(music_mid);
            music_mid = -1;
        }
        if (music_adj != -1) {
            soundPool.unload(music_adj);
            music_adj = -1;
        }
        if (music_photo != -1) {
            soundPool.unload(music_photo);
            music_photo = -1;
        }

        if (music_speed_L != -1) {
            soundPool.unload(music_speed_L);
            music_speed_L = -1;
        }

        if (music_speed_H != -1) {
            soundPool.unload(music_speed_H);
            music_speed_H = -1;
        }

        if (music_video_start != -1) {
            soundPool.unload(music_video_start);
            music_video_start = -1;
        }

        if (music_video_stop != -1) {
            soundPool.unload(music_video_stop);
            music_video_stop = -1;
        }

        music_mid = soundPool.load(mContext, R.raw.center_fly, 1);
        music_adj = soundPool.load(mContext, R.raw.adjian_fly, 1);
        if (nType == JH_App.nStyle_fly) {
            music_photo = soundPool.load(mContext, R.raw.photo_fly, 1);
            music_speed_L = soundPool.load(mContext, R.raw.speed_low_fly, 1);
            music_speed_H = soundPool.load(mContext, R.raw.speed_h_fly, 1);
            music_video_start = soundPool.load(mContext, R.raw.video_start_fly, 1);
            music_video_stop = soundPool.load(mContext, R.raw.video_stop_fly, 1);
        } else {
            music_photo = soundPool.load(mContext, R.raw.photo_m_fly, 1);
        }

    }


    //把图片或者视频添加到系统图库

    public static void F_Save2ToGallery(String filename, boolean bPhoto) {
        //保存图片后发送广播通知更新数据库
        if (mContext == null)
            return;
        try {
            ContentResolver contentResolver = mContext.getContentResolver();
            final ContentValues values = new ContentValues();
            if (bPhoto) {
                values.put(MediaStore.Images.Media.MIME_TYPE, "image/jpeg");
                values.put(MediaStore.Images.Media.DATA, filename);
                //Uri uri =
                contentResolver.insert(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, values);
            } else {
                values.put(MediaStore.Video.Media.MIME_TYPE, "video/mp4");
                values.put(MediaStore.Video.Media.DATA, filename);
                //Uri uri =
                contentResolver.insert(MediaStore.Video.Media.EXTERNAL_CONTENT_URI, values);
            }
            Uri uri1 = Uri.parse("file://" + filename);
            mContext.sendBroadcast(new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, uri1));
        } catch (NullPointerException e) {
            e.printStackTrace();
        }
    }


    //删除图片或者视频，并且把它从系统图库中清除
    public static void DeleteImage(String imgPath) {
        String stype = imgPath.substring(imgPath.length() - 3, imgPath.length());
        stype = stype.toUpperCase();
        boolean bImage = false;
        ContentResolver resolver = mContext.getContentResolver();
        Cursor cursor;
        {
            // cursor = MediaStore.Images.Media.query(resolver, MediaStore.Images.Media.EXTERNAL_CONTENT_URI, new String[]{MediaStore.Images.Media._ID}, MediaStore.Images.Media.DATA + "=?",
            //        new String[]{imgPath}, null);
            if (stype.equalsIgnoreCase("jpg") || stype.equalsIgnoreCase("png")) {
                bImage = true;
                cursor = resolver.query(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, new String[]{MediaStore.Images.Media._ID}, MediaStore.Images.Media.DATA + "=?",
                        new String[]{imgPath}, null);
            } else {
                cursor = resolver.query(MediaStore.Video.Media.EXTERNAL_CONTENT_URI, new String[]{MediaStore.Video.Media._ID}, MediaStore.Video.Media.DATA + "=?",
                        new String[]{imgPath}, null);
            }
        }

        //boolean result = false;
        if (cursor != null) {
            if (cursor.moveToFirst()) {
                long id = cursor.getLong(0);
                Uri contentUri = MediaStore.Images.Media.EXTERNAL_CONTENT_URI;
                if (!bImage) {
                    contentUri = MediaStore.Video.Media.EXTERNAL_CONTENT_URI;
                }
                Uri uri = ContentUris.withAppendedId(contentUri, id);
                //int count =
                resolver.delete(uri, null, null);
                //result = count == 1;
            }
            cursor.close();
        }
        {
            File file = new File(imgPath);
            if (file.isFile() && file.exists()) {
                file.delete();
            }
        }

    }

    public static void F_PlayCenter() {

        if (music_mid < 0) {
            music_mid = soundPool.load(mContext, R.raw.center_fly, 1);
        }
        if (music_mid != -1)
            soundPool.play(music_mid, 1, 1, 0, 0, 1);

    }

    public static void F_PlayAdj() {
        if (music_adj < 0) {
            music_adj = soundPool.load(mContext, R.raw.adjian_fly, 1);
        }
        if (music_adj != -1)
            soundPool.play(music_adj, 1, 1, 0, 0, 1);

    }

    public static void F_PlayPhoto() {

        if (music_photo > 0)
            soundPool.play(music_photo, 1, 1, 0, 0, 1);
    }

    public static void F_PlayStartRecord() {
        if (music_video_start > 0)
            soundPool.play(music_video_start, 1, 1, 0, 0, 1);
    }

    public static void F_PlayStopRecord() {
        if (music_video_stop > 0)
            soundPool.play(music_video_stop, 1, 1, 0, 0, 1);
    }

    public static void F_PlaySpeed(boolean bHiSpeedA) {
        if (bHiSpeedA) {
            if (music_speed_H > 0)
                soundPool.play(music_speed_H, 1, 1, 0, 0, 1);
        } else {
            if (music_speed_L > 0)
                soundPool.play(music_speed_L, 1, 1, 0, 0, 1);
        }

    }

    public static void F_PlayButton() {

        if (music_btn > 0)
            soundPool.play(music_btn, 1, 1, 0, 0, 1);
    }

    public static void Clear() {
        if (mGridList == null) {
            mSD_PhotosList.clear();
            mSD_VideosList.clear();
            mLocal_PhotosList.clear();
            mLocal_VideosList.clear();
            mDownloadList.clear();
            mDispList.clear();
            mNeedDownLoadList.clear();
            mGridList.clear();
        }
    }


    private static String intToIp(int i) {
        return (i & 0xFF) + "." + ((i >> 8) & 0xFF) + "." + ((i >> 16) & 0xFF) + "." + ((i >> 24) & 0xFF);
    }

    public static int F_GetWifiRssi() {
        if (mContext == null)
            return -1;

        if (nICType == wifination.IC_NO) {
            return -1;
        }
        WifiManager wifi_service = (WifiManager) mContext.getSystemService(WIFI_SERVICE);
        WifiInfo info = null;
        if (wifi_service != null)
            info = wifi_service.getConnectionInfo();

        if (info == null)
            return -1;

        int level = info.getRssi();
        int nrssi;
        //根据获得的信号强度发送信息
        if (level <= 0 && level >= -50) {
            nrssi = 4;
        } else if (level < -50 && level >= -70) {
            nrssi = 3;
        } else if (level < -70 && level >= -80) {
            nrssi = 2;
        } else if (level < -80 && level >= -90) {
            nrssi = 1;
        } else {
            nrssi = -1;
        }
        return nrssi;
    }

    public static int F_GetWifiType() {
        nICType = wifination.IC_NO;
        if (mContext == null)
            return nICType;
        WifiManager wifi_service = (WifiManager) mContext.getSystemService(WIFI_SERVICE);

        WifiInfo info = null;
        if (wifi_service != null)
            info = wifi_service.getConnectionInfo();

        String wifiId;
        wifiId = (info != null ? info.getSSID() : null);
        if (wifiId != null) {
            wifiId = wifiId.replace("\"", "");
            if (wifiId.length() > 4)
                wifiId = wifiId.substring(wifiId.length() - 4);
        } else {
            wifiId = "nowifi";
            return nICType;
        }
        sWifi = wifiId;

        String sIP = intToIp(info.getIpAddress());

        if (sIP.startsWith("175.16.10")) {
            nICType = wifination.IC_GKA;
        } else if (sIP.startsWith("192.168.234")) {
            nICType = wifination.IC_GK;
        } else if (sIP.startsWith("192.168.25")) {
            nICType = wifination.IC_GP;
        } else if (sIP.startsWith("192.168.26")) {
            nICType = wifination.IC_GPRTSP;
        } else if (sIP.startsWith("192.168.27")) {
            nICType = wifination.IC_GPH264;
        } else if (sIP.startsWith("192.168.28")) {
            nICType = wifination.IC_GPRTP;
        } else if (sIP.startsWith("192.168.29")) {
            nICType = wifination.IC_GPRTPB;
        } else if (sIP.startsWith("192.168.30")) {
            nICType = wifination.IC_GPH264A;
        } else if (sIP.startsWith("192.168.123")) {
            nICType = wifination.IC_SN;
        } else if (sIP.startsWith("192.168.16")) {
            nICType = wifination.IC_GK_UDP;
            //nICType = wifination.IC_GPH264;
        }
        return nICType;
    }

    // public static JH_App getInstance() {
    //    return singleton;
    // }


    //获取是否存在NavigationBar
    public static void checkDeviceHasNavigationBar(Context context) {

        /*
        Activity activity = (Activity) context;
        //隐藏虚拟按键，并且全屏
        if (Build.VERSION.SDK_INT > 11 && Build.VERSION.SDK_INT < 19) { // lower api
            View v = activity.getWindow().getDecorView();
            v.setSystemUiVisibility(View.GONE);
        } else if (Build.VERSION.SDK_INT >= 19) {
            //for new api versions.
            View decorView = activity.getWindow().getDecorView();
            int uiOptions = View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                    | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY | View.SYSTEM_UI_FLAG_FULLSCREEN;
            decorView.setSystemUiVisibility(uiOptions);
        }

        Window window =activity.getWindow();
        window.setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON, WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
*/

        if (context == null)
            return;

        boolean hasNavigationBar = false;

        Activity activity = (Activity) context;

        Window window = activity.getWindow();
        window.setFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS, WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS);
        window.setFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_NAVIGATION, WindowManager.LayoutParams.FLAG_TRANSLUCENT_NAVIGATION);

        window.setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON, WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        Resources rs = context.getResources();
        int id = rs.getIdentifier("config_showNavigationBar", "bool", "android");
        if (id > 0) {
            hasNavigationBar = rs.getBoolean(id);
        } else {
            try {
                Class<?> systemPropertiesClass = Class.forName("android.os.SystemProperties");
                Method m = systemPropertiesClass.getMethod("get", String.class);
                String navBarOverride = (String) m.invoke(systemPropertiesClass, "qemu.hw.mainkeys");
                if ("1".equals(navBarOverride)) {
                    hasNavigationBar = false;
                } else if ("0".equals(navBarOverride)) {
                    hasNavigationBar = true;


                }
            } catch (Exception e) {

            }
        }

        if (hasNavigationBar) {

            int uiOptions = View.SYSTEM_UI_FLAG_LAYOUT_STABLE |
                    //布局位于状态栏下方
                    View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION |
                    //全屏
                    View.SYSTEM_UI_FLAG_FULLSCREEN |
                    //隐藏导航栏
                    View.SYSTEM_UI_FLAG_HIDE_NAVIGATION |
                    View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN;
            if (Build.VERSION.SDK_INT >= 19) {
                uiOptions |= 0x00001000;
            } else {
                uiOptions |= View.SYSTEM_UI_FLAG_LOW_PROFILE;
            }
            window.getDecorView().setSystemUiVisibility(uiOptions);


            // activity.getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_HIDE_NAVIGATION);


        }



/*
            activity.getWindow().getDecorView().setSystemUiVisibility(
                    View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                            | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                            | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                            | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION // hide navbar
                            | View.SYSTEM_UI_FLAG_FULLSCREEN // hide status bar
                            | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
                            */

    }

    public static String getFileName(String pathandname) {

        int start = pathandname.lastIndexOf("/");
        int end = pathandname.length();
        if (start == -1)
            return pathandname;
        if (end != 0) {
            return pathandname.substring(start + 1, end);
        } else {
            return null;
        }
    }


    public static void F_ClearDataSDArray() {
        for (MyFilesItem filesItem : mSD_PhotosList) {
            filesItem.sSDPath = null;
            filesItem.sPhonePath = null;
        }
        mSD_PhotosList.clear();

        for (MyFilesItem filesItem : mSD_VideosList) {
            filesItem.sSDPath = null;
            filesItem.sPhonePath = null;
        }
        mSD_VideosList.clear();

        for (MyItemData itemData : mGridList) {
            itemData.image = null;
            itemData.sSDPath = null;
            itemData.sPhonePath = null;
        }
        mGridList.clear();

    }

    public static String getsVendor() {
        return sVendor;
    }


    public static boolean F_CheckFileIsExist(String sFileName, int size) {
        File file = new File(sFileName);
        int nSize = -1;
        if (file.exists() && !file.isDirectory()) {
            FileInputStream fis;
            try {
                fis = new FileInputStream(file);
                nSize = fis.available();
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }
            return nSize == size;
        }
        return false;
    }


    public static String F_GetSaveName(boolean bJpg) {

        File file = mContext.getExternalFilesDir(sVendor);
        if (file != null)
        {
            sLocalPhoto = sLocalPhoto;//file.getAbsolutePath();
            sLocalVideo = sLocalVideo;
        }

        Date d = new Date();
        SimpleDateFormat f = new SimpleDateFormat("yyyy_MM_dd_HH_mm_ss_SSS", Locale.getDefault());
        String str = f.format(d);

        if (bJpg)
            return sLocalPhoto + "/" + str + ".png";
        else
            return sLocalVideo + "/" + str + ".mp4";
    }


    public static void F_DispMesate(String str) {
        if (mContext == null)
            return;


        WindowManager wm = (WindowManager) (mContext.getSystemService(Context.WINDOW_SERVICE));


        DisplayMetrics metrics = new DisplayMetrics();
/**
 * getRealMetrics - 屏幕的原始尺寸，即包含状态栏。
 * version >= 4.2.2
 */
        int width = 0;
        int height = 1;
        if (Build.VERSION.SDK_INT >= 18) {
            wm.getDefaultDisplay().getRealMetrics(metrics);
            width = metrics.widthPixels;
            height = metrics.heightPixels;
        } else {
            Point outSize = new Point();
            wm.getDefaultDisplay().getSize(outSize);
            width = outSize.x;
            height = outSize.y;
        }

        float dad = width / (float) height;
        Toast toast = Toast.makeText(mContext, str, Toast.LENGTH_SHORT);
        //第一个参数：设置toast在屏幕中显示的位置。我现在的设置是居中靠顶
        //第二个参数：相对于第一个参数设置toast位置的横向X轴的偏移量，正数向右偏移，负数向左偏移
        //第三个参数：同的第二个参数道理一样
        //如果你设置的偏移量超过了屏幕的范围，toast将在屏幕内靠近超出的那个边界显示
        //toast.setGravity(Gravity.TOP|Gravity.CENTER, -50, 100);
        //屏幕居中显示，X轴和Y轴偏移量都是0
        //toast.setGravity(Gravity.BOTTOM, 80, 100);
        if (dad < 1.81) {
            toast.setGravity(Gravity.CENTER, 0, 100);
        } else {
            toast.setGravity(Gravity.CENTER, 80, 100);
        }
        //toast.setGravity(Gravity.BOTTOM|Gravity.CENTER_HORIZONTAL, 0, 100);

        toast.show();
    }

    public static String getAppVersionName() {
        String versionName = "";
        try {
            if (mContext != null) {
                // ---get the package info---
                PackageManager pm = mContext.getPackageManager();
                PackageInfo pi = pm.getPackageInfo(mContext.getPackageName(), 0);
                versionName = pi.versionName;
                //versioncode = pi.versionCode;
                if (versionName == null || versionName.length() <= 0) {
                    return "";
                }
            } else {
                return "";
            }
        } catch (Exception e) {
            Log.e("VersionInfo", "Exception", e);
        }
        return versionName;
    }

    public static void F_GetOrg(Activity activity) {
        int sf = activity.getWindowManager().getDefaultDisplay().getRotation();
        if (sf == Surface.ROTATION_90) {
            JH_App.nOrg = ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE;
        }
        if (sf == Surface.ROTATION_270) {
            JH_App.nOrg = ActivityInfo.SCREEN_ORIENTATION_REVERSE_LANDSCAPE;
        }
    }

    public static boolean CheckConnectedDevice() {
        if (mContext == null)
            return false;
        WifiManager wifi_service = (WifiManager) mContext.getSystemService(WIFI_SERVICE);
        WifiInfo info = null;
        try {
            info = wifi_service.getConnectionInfo();
        } catch (NullPointerException e) {
            ;
        }
        String wifiId;
        wifiId = (info != null ? info.getSSID() : null);
        if (wifiId != null) {
            wifiId = wifiId.replace("\"", "");
            if (wifiId.length() > 4)
                wifiId = wifiId.substring(wifiId.length() - 4);
        } else {
            wifiId = "nowifi";
        }
        String sIP = "";
        try {
            sIP = intToIp(info.getIpAddress());
        } catch (NullPointerException e) {
            ;
        }

        int x = sIP.lastIndexOf(".");
        sIP = sIP.substring(0, x);
        if (sIP.equalsIgnoreCase("192.168.27") ||
                sIP.equalsIgnoreCase("192.168.30") ||
                sIP.equalsIgnoreCase("192.168.33") ||         //MJ  支持SD
                sIP.equalsIgnoreCase("192.168.34"))           //H264 支持SD卡
        {
            if (sIP.equalsIgnoreCase("192.168.27")) {
                Command.sIP = "192.168.27.1";
            }
            if (sIP.equalsIgnoreCase("192.168.30")) {
                Command.sIP = "192.168.30.1";
            }

            if (sIP.equalsIgnoreCase("192.168.33")) {
                Command.sIP = "192.168.33.1";
            }
            if (sIP.equalsIgnoreCase("192.168.34")) {
                Command.sIP = "192.168.34.1";
            }
            return true;
        } else {
            Command.sIP = "";
            return false;
        }
    }

    @TargetApi(21)
    public static void forceSendRequestByWifiData(Context context, final boolean b, final RequestWifi_Interface requestWifi_interface) {

        if (Build.VERSION.SDK_INT >= 21) {
            final ConnectivityManager connectivityManager = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
            NetworkRequest.Builder builder = new NetworkRequest.Builder();

            // 设置指定的网络传输类型(蜂窝传输) 等于手机网络
            if (b) {
                builder.addTransportType(NetworkCapabilities.TRANSPORT_WIFI);
            } else {
                builder.addTransportType(NetworkCapabilities.TRANSPORT_CELLULAR);
                builder.addCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET);

            }


            // 设置感兴趣的网络功能
            // builder.addCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET);

            // 设置感兴趣的网络：计费网络
            // builder.addCapability(NetworkCapabilities.NET_CAPABILITY_NOT_METERED);

            NetworkRequest request = builder.build();
            ConnectivityManager.NetworkCallback callback = new ConnectivityManager.NetworkCallback() {
                @Override
                public void onUnavailable() {
                    super.onUnavailable();
                    Log.e("aa","bbbb");
                }

//                @Override
//                public void onLosing(Network network, int maxMsToLive) {
//                    super.onLosing(network, maxMsToLive);
//                }
//
//                @Override
//                public void onLost(Network network) {
//                    super.onLost(network);
//                }
//
//                @Override
//                public void onLinkPropertiesChanged(Network network, LinkProperties linkProperties) {
//                    super.onLinkPropertiesChanged(network, linkProperties);
//                }
//
//                @Override
//                public void onCapabilitiesChanged(Network network, NetworkCapabilities networkCapabilities) {
//                    super.onCapabilitiesChanged(network, networkCapabilities);
//                }

                /**
                 * Called when the framework connects and has declared a new network ready for use.
                 * This callback may be called more than once if the {@link Network} that is
                 * satisfying the request changes.
                 */


                @TargetApi(Build.VERSION_CODES.M)
                @Override
                public void onAvailable(Network network) {
                    super.onAvailable(network);
                    //if(b)
                    {
                        Log.i("test", "已根据功能和传输类型找到合适的网络");

                        // 可以通过下面代码将app接下来的请求都绑定到这个网络下请求
                        if (Build.VERSION.SDK_INT >= 23) {
                            connectivityManager.bindProcessToNetwork(network);
                        } else {
                            // 23后这个方法舍弃了
                            ConnectivityManager.setProcessDefaultNetwork(network);
                        }

                        // 也可以在将来某个时间取消这个绑定网络的设置
                        // if (Build.VERSION.SDK_INT >= 23) {
                        //      onnectivityManager.bindProcessToNetwork(null);
                        //} else {
                        //     ConnectivityManager.setProcessDefaultNetwork(null);
                        //}

                        // 只要一找到符合条件的网络就注销本callback
                        // 你也可以自己进行定义注销的条件
                        connectivityManager.unregisterNetworkCallback(this);
                    }
//                    else
//                    {
//                        if (Build.VERSION.SDK_INT >= 23) {
//                            connectivityManager.bindProcessToNetwork(null);
//                        } else {
//                            ConnectivityManager.setProcessDefaultNetwork(null);
//                        }
//                        connectivityManager.unregisterNetworkCallback(this);
//                    }
                    requestWifi_interface.onResult();
                }
            };
            connectivityManager.requestNetwork(request, callback);
        }
    }


    //public static void F_OpenStream(Surface surface) {
    public static void F_OpenStream() {



        //wifination.naSetRecordWH(640,480);
        //F_OpenStream_A();


        forceSendRequestByWifiData(mContext.getApplicationContext(), true, new RequestWifi_Interface() {
            @Override
            public void onResult() {
                F_OpenStream_A();
            }
        });


//        forceSendRequestByWifiData(mContext.getApplicationContext(), false, new RequestWifi_Interface() {
//            @Override
//            public void onResult() {
//                forceSendRequestByWifiData(mContext.getApplicationContext(), true, new RequestWifi_Interface() {
//                    @Override
//                    public void onResult() {
//                        F_OpenStream_A();
//                    }
//                });
//            }
//        });


//        new Handler().postDelayed(new Runnable() {
//            @Override
//            public void run() {
//                F_OpenStream_A();
//            }
//        },200);
    }


    private  static Handler sendHander = new Handler();
    private static Runnable snedRunnable = new Runnable() {
        @Override
        public void run() {
            byte msg[]=new byte[7];
            msg[0] = 'J';
            msg[1] = 'H';
            msg[2] = 'C';
            msg[3] = 'M';
            msg[4] = 'D';
            msg[5] = (byte)0xD0;
            msg[6] = (byte)0x01;
            Sent_Udp("192.168.29.1",20001,msg);
            sendHander.postDelayed(this,500);
        }
    };

    public static void F_OpenStream_A() {

        sendHander.removeCallbacksAndMessages(null);
        sendHander.post( snedRunnable);


        int ictype = JH_App.F_GetWifiType();

        wifination.naSetAdjFps(false); //针对GKA模块 有效


        CheckConnectedDevice();

        JH_App.nICType = ictype;

        if (JH_App.bIsSyMa)
            wifination.naSetCustomer("sima");
        else
            wifination.naSetCustomer(" ");
        String str = "2";
        if (JH_App.nICType == wifination.IC_GP)
            str = "http://192.168.25.1:8080/?action=stream";
        wifination.naSetRevBmp(true);
        wifination.na4225_SetMode((byte) 0);
        wifination.naSetRevBmp(true);
        //wifination.naStop();
        wifination.naSetGsensorType(2);
        wifination.naSetCameraDataRota(0);
        wifination.naSetRecordWH(3840,2160);
        wifination.naInit("");

//        wifination.naSetCameraDataRota(180);
//        wifination.naSetMirror(false);
//        wifination.naSetSensor(true);
//        wifination.naSetEnableRotate(true);
//        wifination.naSetCircul(true);
//        wifination.naSetsquare(true);
        wifination.naSet3DDenoiser(false);



//        wifination.naSetEnableRotate(true);
//        wifination.naSetCircul(true);
//        wifination.naSetSensor(true);
//        wifination.naSetsquare(true);


        //wifination.naSetGsensorPara(40,4);
        //wifination.naSetEnableRotate(true);
//        wifination.naSetCircul(true);
//        wifination.naSetSensor(true);
        //wifination.naSetEnableRotate(true);
        //wifination.naSetCircul(true);
        //wifination.naSetSensor(true);
        //wifination.naSetAcdetection(false);

        //wifination.naStartReadUdp(20001);  //SDK  版本
        //wifination.naGetRtl_Mode();
    }


    static InetAddress mAddress;
    static DatagramSocket socket = null;

    private static void Sent_Udp(String sIP,final int nPort,final byte[] sendBuf)
    {
        //初始化socket

        try {
            socket = new DatagramSocket();
        } catch (SocketException e) {
            e.printStackTrace();
        }
        try {
            mAddress = InetAddress.getByName(sIP);
        } catch (UnknownHostException e) {
            e.printStackTrace();
        }

        new Thread() {


            public void run() {
                DatagramPacket recvPacket1 = new DatagramPacket(sendBuf, sendBuf.length, mAddress, nPort);
                try {
                    socket.send(recvPacket1);
                    socket.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }.start();

    }


    public static Bitmap F_AdjBitmp(Bitmap bitmap) {
        // 获得图片的宽高
        int width = bitmap.getWidth();
        int height = bitmap.getHeight();
        // 设置想要的大小
        int newWidth = 720 / 8;
        int newHeight = 1280 / 8;
        // 计算缩放比例
        float scaleWidth = ((float) newWidth) / width;
        float scaleHeight = ((float) newHeight) / height;
        // 取得想要缩放的matrix参数
        Matrix matrix = new Matrix();
        matrix.postScale(scaleWidth, scaleHeight);
        // 得到新的图片
        Bitmap newbm = Bitmap.createBitmap(bitmap, 0, 0, width, height, matrix, true);
        return newbm;
    }

    public static Bitmap getVideoThumbnail(String videoPath) {
        Bitmap bitmap = null;
        MediaMetadataRetriever retriever = new MediaMetadataRetriever();
        try {
            retriever.setDataSource(videoPath);
            // bitmap = retriever.getFrameAtTime(0, MediaMetadataRetriever.OPTION_CLOSEST_SYNC);
            bitmap = retriever.getFrameAtTime();
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        } catch (RuntimeException e) {
            e.printStackTrace();
        } finally {
            try {
                retriever.release();
            } catch (RuntimeException e) {
                e.printStackTrace();
            }
        }
        if (bitmap != null) {
            return F_AdjBitmp(bitmap);
        }
        return bitmap;
    }

    public static int F_GetVideoCountTime(String path) {
        MediaMetadataRetriever mmr = new MediaMetadataRetriever();
        mmr.setDataSource(path);
        String duration = mmr.extractMetadata(MediaMetadataRetriever.METADATA_KEY_DURATION); // 播放时长单位为毫秒
        int du = -1;
        try {
            du = Integer.parseInt(duration);
        } catch (NumberFormatException e) {
            e.printStackTrace();
        }
        if (du != -1)
            du /= 1000;
        return du;
    }


    private static List<String> lstFile = new ArrayList<String>();  //结果 List


    public static void GetFilesA(String Path, String Extension, boolean IsIterative)  //搜索目录，扩展名，是否进入子文件夹
    {
        if (lstFile == null) {
            lstFile = new ArrayList<String>();
        }
        File fa = new File(Path);
        if (fa.isFile()) {
            lstFile.clear();
            return;
        }
        File[] files = new File(Path).listFiles();


        File wfile = new File(sLocalVideo + "/s8.txt");

        if (wfile.exists()) {
            wfile.delete();
        }
        try {
            wfile.createNewFile();
        } catch (IOException e) {
            e.printStackTrace();
        } catch (SecurityException e) {
            e.printStackTrace();
        }

        wfile.setWritable(true);

        BufferedWriter wr = null;
        try {
            wr = new BufferedWriter(new FileWriter(wfile));
        } catch (IOException e) {
            e.printStackTrace();
        }
        for (int i = 0; i < files.length; i++) {
            File f = files[i];
            if (f.isFile()) {
                if (Extension != null) {
                    if (f.getPath().substring(f.getPath().length() - Extension.length()).equals(Extension))  //判断扩展名
                        lstFile.add(f.getName());
                } else {
                    StringBuffer sb = new StringBuffer();
                    try {
                        BufferedReader br = new BufferedReader(new FileReader(f));
                        String line = "";
                        while ((line = br.readLine()) != null) {
                            sb.append(line);
                        }
                        br.close();
                    } catch (FileNotFoundException e) {
                        e.printStackTrace();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                    String str = f.getName() + "  " + sb + "\n";

                    lstFile.add(str);
                    try {
                        if (wr != null)
                            wr.write(str);
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            } else if (f.isDirectory() && !f.getPath().contains("/."))  //忽略点文件（隐藏文件/文件夹）
            {
                if (IsIterative) {
                    //GetFilesA(f.getPath(), Extension, IsIterative);
                    GetFilesA(f.getPath(), Extension, true);
                }
            }
        }
        try {
            wr.close();
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    /*
    private static Handler handler1 = new Handler();
    public static void  F_OpenCamera(boolean  b)
    {
        if(b)
        {
            if(bIsOpen)
            {
                return;
            }
            bIsOpen = true;
            handler1.removeCallbacksAndMessages(null);
            forceSendRequestByWifiData(singleton.getApplicationContext(), false, new RequestWifi_Interface() {
                @Override
                public void onResult() {
                    forceSendRequestByWifiData(singleton.getApplicationContext(), true, new RequestWifi_Interface() {
                        @Override
                        public void onResult() {
                            wifination.naSetRevBmp(true);
                            wifination.naStop();
                            SystemClock.sleep(150);
                            wifination.naInit("");
                            wifination.na4225_SetMode((byte) 0);
                            Log.e("MyApp","Open Camera");
                        }
                    });

                }
            });


//            handler1.postDelayed(new Runnable() {
//                @Override
//                public void run() {
//                    forceSendRequestByWifiData(singleton.getApplicationContext(),true);
//                    handler1.postDelayed(new Runnable() {
//                        @Override
//                        public void run() {
//                            wifination.na4225_SetMode((byte) 0);
//                            wifination.naSetRevBmp(true);
//                            wifination.naInit("");
//                            Log.e("MyApp","Open Camera");
//                        }
//                    },100);
//                }
//            },150);
        }
        else
        {
            if(!bIsOpen)
                return;
            //MyApp.forceSendRequestByWifiData(singleton,false);
            // wifination.naStopRecord(wifination.TYPE_ONLY_PHONE);
            wifination.naStop();
            bIsOpen = false;
            new Handler().postDelayed(new Runnable() {
                @Override
                public void run() {
                    bIsConnect = false;
                }
            },200);
            Log.e("MyApp","Close Camera");
        }
    }
*/


}
