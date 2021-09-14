package com.joyhonest.test;

import androidx.appcompat.app.AppCompatActivity;

import android.app.ActionBar;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import com.joyhonest.jh_ui.JH_App;
import com.joyhonest.jh_ui.Storage;
import com.joyhonest.wifination.PictureFromVideo_Interface;
import com.joyhonest.wifination.R;
import com.joyhonest.wifination.wifination;

import org.simple.eventbus.EventBus;
import org.simple.eventbus.Subscriber;

import java.util.ArrayList;
import java.util.List;

public class MyTestActivity extends AppCompatActivity {

    int nInx = 0;
     List<Bitmap> list = new ArrayList<Bitmap>();

     int nFrames;
     int nTimes;

     TextView  Frams_text;
     TextView  Times_text;
    ImageView DispImageView;
    JH_PicListView listView;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_my_test);
        EventBus.getDefault().register(this);
        Frams_text = findViewById(R.id.Frames_text);
        Times_text = findViewById(R.id.Times_text);
        listView = findViewById(R.id.listView);

        Frams_text.setText("Frames = "+nFrames);
        Times_text.setText("Times = "+nTimes);
//        Frams_text.setVisibility(View.INVISIBLE);
//        Times_text.setVisibility(View.INVISIBLE);
        DispImageView = findViewById(R.id.DispImageView);
        wifination.naSetRevBmp(true);

        findViewById(R.id.btn_test).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
//                list.clear();
//                nInx = 0;
//                nFrames = 0;
//                nTimes = 0;
//                listView.Clear();
//
//                String str = Storage.getNormalSDCardPath();
//                wifination.naGetPictListFromVideo("/storage/emulated/0/1/12345.mp4", new PictureFromVideo_Interface() {
//                    @Override
//                    public void onStart(int nFrame_,int nTimes_) {
//                     //   Log.e("TAG","onStart! nFrame = "+nFrame_ +"  nTimes = "+nTimes_);
//                        nFrames = nFrame_;
//                        nTimes = nTimes_;
//                        EventBus.getDefault().post("","onGetStart");
//                    }
//
//                    @Override
//                    public void onEnd() {
//                      //  Log.e("TAG","onEnd!");
//
//                        EventBus.getDefault().post("","OnEnd");
//                    }
//
//                    @Override
//                    public void onGetaPicture(Bitmap bmp) {
//                        listView.AddBmp(bmp);
//                        EventBus.getDefault().post(bmp,"ReviceBMP");
//                        nInx++;
//                    }
//
//                    @Override
//                    public void onError(int nErrorNo) {
//                       // Log.e("TAGG","error = "+nErrorNo);
//
//                    }

//                });

                wifination.naPlayFlie("/storage/emulated/0/Sports-Camera/Sports_Camera_SD/MOVI0001-5440512.mov");

            }
        });
    }


    @Subscriber(tag = "OnEnd")
    private void OnEnd(String str)
    {
        int nsize = 4;
        int W =nsize*listView.getHeight();
        ViewGroup.LayoutParams  params = listView.getLayoutParams();
        params.width = W;
        listView.setLayoutParams(params);
        listView.invalidate();
    }
    @Subscriber(tag = "onGetStart")
    private void onGetStart(String str)
    {
        Frams_text.setVisibility(View.VISIBLE);
        Times_text.setVisibility(View.VISIBLE);
        Frams_text.setText("Frames = "+nFrames);
        Times_text.setText("Times = "+nTimes);
    }
    @Subscriber (tag = "ReviceBMP")
    private void ReviceBMP(Bitmap bmp)
    {
        //listView.invalidate();
        DispImageView.setImageBitmap(bmp);
    }



    @Override
    protected void onDestroy() {
        super.onDestroy();
        EventBus.getDefault().unregister(this);
    }

    @Override
    protected void onResume() {
        super.onResume();
        JH_App.checkDeviceHasNavigationBar(this);
    }
}