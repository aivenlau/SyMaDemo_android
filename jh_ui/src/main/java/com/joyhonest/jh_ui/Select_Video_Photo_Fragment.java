package com.joyhonest.jh_ui;


//import android.app.Fragment;

import android.os.Bundle;
//import android.support.v4.app.Fragment;
import androidx.fragment.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import com.joyhonest.wifination.R;

import org.simple.eventbus.EventBus;


public class Select_Video_Photo_Fragment extends Fragment {


    Button Btn_Return;
    Button Btn_BrowPhoto;
    Button Btn_BrowVideo;

    TextView Text_Photo_Number_Label;
    TextView Text_Videos_Number_Label;

    public Select_Video_Photo_Fragment() {

    }


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_select_video_photo_jh, container, false);


        Text_Photo_Number_Label = (TextView) view.findViewById(R.id.photo_number_label);
        Text_Videos_Number_Label = (TextView) view.findViewById(R.id.video_number_label);

        view.findViewById(R.id.button_Exit).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                JH_App.F_ClearDataSDArray();
                EventBus.getDefault().post("abc", "GoTo_Main");
            }
        });

        view.findViewById(R.id.Photo).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                JH_App.bBrowPhoto = true;
                F_Sent_SelectType();
            }
        });

        view.findViewById(R.id.Video).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                JH_App.bBrowPhoto = false;
                F_Sent_SelectType();

            }
        });


        return view;
    }


    private void F_Sent_SelectType() {
        EventBus.getDefault().post("abc", "GoTo_DispGrid");
    }

    public void F_Update_number(int nPhotos, int nVideos) {
        int df = Text_Photo_Number_Label.getVisibility();
        if (nPhotos < 0) {
            Text_Photo_Number_Label.setText("");
        } else {
            Text_Photo_Number_Label.setText("" + nPhotos);
        }
        if (nVideos < 0) {
            Text_Videos_Number_Label.setText("");
        } else {
            Text_Videos_Number_Label.setText("" + nVideos);
        }
    }


}
