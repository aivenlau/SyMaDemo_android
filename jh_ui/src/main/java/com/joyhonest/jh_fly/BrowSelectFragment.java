package com.joyhonest.jh_fly;


import android.os.Bundle;
import androidx.fragment.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

import com.joyhonest.jh_ui.JH_App;
import com.joyhonest.wifination.R;

import org.simple.eventbus.EventBus;

/**
 * A simple {@link Fragment} subclass.
 */
public class BrowSelectFragment extends Fragment implements View.OnClickListener {

    private Button return_but;
    private Button photo_btn;
    private Button video_btn;


    private Button btn_Phone;
    private Button btn_SD;

    public BrowSelectFragment() {
        // Required empty public constructor
    }


    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View view = inflater.inflate(R.layout.fragment_brow_select, container, false);
        return_but = (Button) view.findViewById(R.id.return_button);
        photo_btn = (Button) view.findViewById(R.id.button_photo);
        video_btn = (Button) view.findViewById(R.id.button_video);

        btn_Phone = (Button) view.findViewById(R.id.button_phone);
        btn_SD = (Button) view.findViewById(R.id.button_sd);

        btn_Phone.setOnClickListener(this);
        btn_SD.setOnClickListener(this);

        return_but.setOnClickListener(this);
        photo_btn.setOnClickListener(this);
        video_btn.setOnClickListener(this);
        F_DispSelectSDorPhone();
        return view;
    }


    public void F_DispSelectSDorPhone() {
        if (JH_App.bBrowSD) {
            btn_Phone.setBackgroundResource(R.mipmap.phone_2_fly_jh);
            btn_SD.setBackgroundResource(R.mipmap.sd_sel_fly_jh);
        } else {
            btn_Phone.setBackgroundResource(R.mipmap.phone_2_sel_fly_jh);
            btn_SD.setBackgroundResource(R.mipmap.sd_fly_jh);
        }
    }

    @Override
    public void onClick(View v) {
        Fly_PlayActivity activity = (Fly_PlayActivity) getActivity();
        if (v == return_but) {
            EventBus.getDefault().post("exit", "Exit");
        }
        if (v == photo_btn) {
            JH_App.bBrowPhoto = true;
            activity.F_GetFilesNumber();
            if (!JH_App.bBrowSD) {
                activity.F_FillFilesData();
            }
            Integer nFragment = JH_Fly_Setting.Brow_Files_Fragment;
            EventBus.getDefault().post(nFragment, "gotoFragment");
        }
        if (v == video_btn) {
            JH_App.bBrowPhoto = false;
            activity.F_GetFilesNumber();
            if (!JH_App.bBrowSD) {
                activity.F_FillFilesData();
            }
            Integer nFragment = JH_Fly_Setting.Brow_Files_Fragment;
            EventBus.getDefault().post(nFragment, "gotoFragment");
        }
        if (v == btn_Phone) {
            if (JH_App.bBrowSD) {
                JH_App.bBrowSD = false;
            }
            F_DispSelectSDorPhone();

        }
        if (v == btn_SD) {
            if (!JH_App.bBrowSD) {
                JH_App.bBrowSD = true;
                //  Fly_PlayActivity activity = (Fly_PlayActivity) getActivity();
                //activity.F_GetFilesNumber();
            }
            F_DispSelectSDorPhone();
        }
    }
}
