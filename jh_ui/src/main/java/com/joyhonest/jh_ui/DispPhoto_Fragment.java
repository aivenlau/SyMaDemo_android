package com.joyhonest.jh_ui;


import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
//import android.support.v4.app.Fragment;
//import android.support.v4.view.PagerAdapter;
//import android.support.v4.view.ViewPager;
import androidx.fragment.app.Fragment;
import androidx.viewpager.widget.PagerAdapter;
import androidx.viewpager.widget.ViewPager;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;

import com.joyhonest.wifination.R;

import org.simple.eventbus.EventBus;

import java.io.FileInputStream;
import java.io.FileNotFoundException;


/**
 * A simple {@link Fragment} subclass.
 */
public class DispPhoto_Fragment extends Fragment {


    ViewPager viewpager;
    PagerAdapter adapter;


    public DispPhoto_Fragment() {
        // Required empty public constructor
    }


    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View view = inflater.inflate(R.layout.fragment_disp_photo_jh, container, false);
        viewpager = (ViewPager) view.findViewById(R.id.viewpager1);


        adapter = new PagerAdapter() {
            @Override
            public int getItemPosition(Object object) {
                return POSITION_NONE;
            }

            @Override
            public int getCount() {
                return JH_App.mDispList.size();
            }

            @Override
            public boolean isViewFromObject(View arg0, Object arg1) {
                return arg0 == arg1;
            }

            /**
             * 销毁当前page的相隔2个及2个以上的item时调用
             */
            @Override
            public void destroyItem(ViewGroup container, int position, Object o) {
                //container.removeViewAt(position);
                container.removeView((View) o);
                o = null;
            }

            //设置ViewPager指定位置要显示的view
            @Override
            public Object instantiateItem(ViewGroup container, int position) {
                ImageView im = new ImageView(getActivity());
                String filename = JH_App.mDispList.get(position);
                FileInputStream fis = null;
                BitmapFactory.Options options = new BitmapFactory.Options();
                Bitmap bitmap = null;
                try {
                    fis = new FileInputStream(filename);
                    options.inSampleSize = 1;
                    bitmap = BitmapFactory.decodeStream(fis, null, options);
                } catch (FileNotFoundException e) {
                    e.printStackTrace();
                }
                im.setImageBitmap(bitmap);
                im.setOnClickListener(new View.OnClickListener() {

                    @Override
                    public void onClick(View v) {

                        Integer i = JH_App.Grid_Fragment_Value;
                        EventBus.getDefault().post(i, "Return_Back");
                    }
                });
                im.setScaleType(ImageView.ScaleType.FIT_XY);
                container.addView(im);

                return im;
            }
        };
        viewpager.setAdapter(adapter);
        return view;
    }


    public void UpdateData(int inx) {
        if (inx < 0)
            inx = 0;
        adapter.notifyDataSetChanged();
        viewpager.setCurrentItem(inx, false);


    }

    @Override
    public void onDestroy() {
        super.onDestroy();

    }

    //属性初始化完成之后，我们需要给ViewPager设置adapter对象。ViewPager如同ListView一般需要设置适配器，ViewPager的adapter是PagerAdapter。


}
