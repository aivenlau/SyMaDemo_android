package com.joyhonest.jh_ui;


//import android.app.Fragment;

import android.net.Uri;
import android.os.Bundle;
//import android.support.v4.app.Fragment;
import android.os.Handler;
import androidx.fragment.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.MediaController;
import android.widget.RelativeLayout;
import android.widget.VideoView;

import com.joyhonest.wifination.R;

import org.simple.eventbus.EventBus;

import java.io.File;


/**
 * A simple {@link Fragment} subclass.
 */
public class DispVideo_Fragment extends Fragment {


    VideoView videoView;
    MediaController mc;

    Button backBut;
    Handler handler;
    Runnable runnable;

    public DispVideo_Fragment() {
        // Required empty public constructor
    }


    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View view = inflater.inflate(R.layout.fragment_disp_video_jh, container, false);
        mc = new MediaController(getActivity());
        videoView = (VideoView) view.findViewById(R.id.videoView);
        backBut = (Button) view.findViewById(R.id.button_ExitPaly);

        mc.setAnchorView(view);
        videoView.setMediaController(mc);
        videoView.setBackgroundColor(0xFFFFFFFF);


        videoView.setZOrderOnTop(true);
        videoView.setZOrderMediaOverlay(true);

        backBut.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Integer i = JH_App.Grid_Fragment_Value;
                EventBus.getDefault().post(i, "Return_Back");
            }
        });
        handler = new Handler();
        runnable = new Runnable() {
            @Override
            public void run() {
                if (!mc.isShowing())
                    JH_App.checkDeviceHasNavigationBar(getActivity());
                handler.postDelayed(this, 400);
            }
        };
        //handler.post(runnable);
        if (JH_App.nType == JH_App.nStyle_fly) {
            backBut.setBackgroundResource(R.mipmap.return_icon_fly_jh);
            RelativeLayout.LayoutParams para = (RelativeLayout.LayoutParams) backBut.getLayoutParams();
            para.width = Storage.dip2px(getActivity(), 30);
            para.height = Storage.dip2px(getActivity(), 30);
            backBut.setLayoutParams(para);

        }
        return view;
    }


    public void F_SetBackImg(int id) {
        backBut.setBackgroundResource(id);
    }

    public void F_Play(String str) {

        File file = new File(str);
//判断文件夹是否存在,如果不存在则创建文件夹
        if (!file.exists()) {
            JH_App.F_DispMesate("file is not exist。。。");
            return;
        }

        Uri uri = Uri.parse(str);
        videoView.setVideoURI(uri);
        videoView.start();
        videoView.requestFocus();
        videoView.setBackgroundColor(0x00000000);

        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                mc.hide();
            }
        }, 100);

        handler.removeCallbacksAndMessages(null);
        handler.post(runnable);


    }

    public void F_Stop() {
        handler.removeCallbacksAndMessages(null);
        videoView.stopPlayback();
        mc.hide();

    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        F_Stop();

    }
}
