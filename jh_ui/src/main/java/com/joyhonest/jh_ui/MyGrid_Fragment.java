package com.joyhonest.jh_ui;


//import android.app.Fragment;

import android.graphics.ColorMatrixColorFilter;
import android.os.Bundle;
//import android.support.v4.app.Fragment;
import androidx.fragment.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.joyhonest.wifination.R;

import org.simple.eventbus.EventBus;


/**
 * A simple {@link Fragment} subclass.
 */
public class MyGrid_Fragment extends Fragment implements View.OnClickListener, AdapterView.OnItemClickListener, AdapterView.OnItemLongClickListener {


    public MyAdapter adapter;
    Button but_exit;
    Button but_select;
    Button but_delete;
    Button but_add2album;
    ProgressBar progressBar;
    ImageView logoImgView;
    TextView precentView;
    GridView GridView;

    RelativeLayout AlartView;
    Button button_yes;
    Button button_no;


    boolean bCannSelected = false;

    public MyGrid_Fragment() {
        // Required empty public constructor
    }


    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View view = inflater.inflate(R.layout.fragment_my_grid_jh, container, false);
        AlartView = (RelativeLayout) view.findViewById(R.id.AlartView);
        AlartView.setVisibility(View.INVISIBLE);
        button_yes = (Button) view.findViewById(R.id.button_yes);
        button_no = (Button) view.findViewById(R.id.button_no);

        button_yes.setOnClickListener(this);
        button_no.setOnClickListener(this);


        but_exit = (Button) view.findViewById(R.id.But_Exit);
        but_select = (Button) view.findViewById(R.id.But_Select);
        but_delete = (Button) view.findViewById(R.id.But_Delete);
        but_add2album = (Button) view.findViewById(R.id.But_Add2ALBUM);
        progressBar = (ProgressBar) view.findViewById(R.id.progressBar);
        logoImgView = (ImageView) view.findViewById(R.id.logo_ImgView);
        precentView = (TextView) view.findViewById(R.id.DownLoad_precent_View);
        GridView = (GridView) view.findViewById(R.id.GridView1);
        but_exit.setOnClickListener(this);
        but_select.setOnClickListener(this);
        but_delete.setOnClickListener(this);
        but_add2album.setOnClickListener(this);
        GridView.setOnItemClickListener(this);
        GridView.setOnItemLongClickListener(this);
        precentView.setText("0%");

        adapter = new MyAdapter(getActivity().getApplicationContext(), GridView);

        GridView.setAdapter(adapter);
        but_exit.setOnClickListener(this);

        but_add2album.setEnabled(false);
        but_delete.setEnabled(false);
        F_SetSelectIcon();


        return view;
    }


    private void F_SetSelectIcon() {

        // final float[] BT_NOT_SELECTED = new float[] {1,0,0,0,0.8f,0,1,0,0,0.8f,0,0,1,0,0.8f,0,0,0,1,0};
        // final float[] BT_SELECTED = new float[] {1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0};

        final float[] BT_NOT_SELECTED = new float[]{1, 0, 0, 0, -50, 0, 1, 0, 0, -50, 0, 0, 1, 0, -50, 0, 0, 0, 1, 0};
        final float[] BT_SELECTED = new float[]{1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0};

        if (bCannSelected) {
            but_add2album.setEnabled(true);
            but_delete.setEnabled(true);
            //but_delete.setBackgroundResource(R.mipmap.delete_nor_jh);
            //but_add2album.setBackgroundResource(R.mipmap.addtoalbum_nor_jh);
            but_delete.getBackground().setColorFilter(
                    new ColorMatrixColorFilter(BT_SELECTED));
            but_add2album.getBackground().setColorFilter(
                    new ColorMatrixColorFilter(BT_SELECTED));

            but_select.setBackgroundResource(R.mipmap.selected_sel_jh);


        } else {
            but_select.setBackgroundResource(R.mipmap.selected_nor_jh);
            but_add2album.setEnabled(true);
            but_delete.setEnabled(true);
            but_delete.getBackground().setColorFilter(
                    new ColorMatrixColorFilter(BT_NOT_SELECTED));
            but_add2album.getBackground().setColorFilter(
                    new ColorMatrixColorFilter(BT_NOT_SELECTED));
            but_add2album.setEnabled(false);
            but_delete.setEnabled(false);
        }
    }

    @Override
    public void onClick(View view) {
        if (view == but_exit) {
            Integer i = JH_App.select_Fragment_Value;

            EventBus.getDefault().post(i, "Return_Back");
        }
        if (view == but_select) {

            if (JH_App.bDownLoading) {
                JH_App.F_DispMesate("正在下载，请稍候");
                return;
            }
            bCannSelected = !bCannSelected;
            F_SetSelectIcon();
            if (!bCannSelected) {
                for (MyItemData data : JH_App.mGridList) {
                    data.bSelected = false;
                }
                F_UpdateLisetViewData();


            }
        }
        if (view == button_yes) {
            EventBus.getDefault().post("", "Grid_Delete");
            AlartView.setVisibility(View.INVISIBLE);
        }
        if (view == button_no) {
            AlartView.setVisibility(View.INVISIBLE);
            F_ResetSelect();
            F_UpdateLisetViewData();
        }
        if (view == but_delete) {
            int nCount = 0;
            for (MyItemData data : JH_App.mGridList) {
                if (data.bSelected) {
                    nCount++;
                }
            }
            if (nCount > 0) {
                AlartView.setVisibility(View.VISIBLE);

            }
        }

    }

    public void F_SetIcon(boolean bPhoto) {
        if (bPhoto) {
            logoImgView.setBackgroundResource(R.mipmap.photofolder_nor_jh);
        } else {
            logoImgView.setBackgroundResource(R.mipmap.videofolder_nor_jh);
        }
    }

    public void F_ResetSelect() {
        bCannSelected = false;
        F_SetSelectIcon();
        if (!bCannSelected) {
            for (MyItemData data : JH_App.mGridList) {
                data.bSelected = false;
            }


        }
    }


    public void F_Init() {
        bCannSelected = false;
        F_SetSelectIcon();
    }


    @Override
    public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
        Log.v("dd", "Item Click");
        //if(JH_App.bBrowSD)
        {
            if (bCannSelected) {
                MyItemData data = JH_App.mGridList.get(i);
                data.bSelected = !data.bSelected;
                F_UpdateLisetViewData();
                return;
            }
            Integer ix = i;

            EventBus.getDefault().post(ix, "GridItem_Click");
        }
    }

    @Override
    public boolean onItemLongClick(AdapterView<?> adapterView, View view, int i, long l) {

        Log.v("dd", "Item LongClick");
        return true;
    }

    public void F_UpdateLisetViewData() {
        adapter.notifyDataSetChanged();
    }

    public void F_UpdateLisetView() {
        if (JH_App.bBrowSD) {
            GridView.setNumColumns(3);
        } else {
            GridView.setNumColumns(4);
        }

        adapter.notifyDataSetChanged();
        if (JH_App.bBrowSD) {
            logoImgView.setVisibility(View.INVISIBLE);
            precentView.setVisibility(View.VISIBLE);
            progressBar.setVisibility(View.VISIBLE);
            int nPrecent = 0;
            if (JH_App.bBrowPhoto) {

                int nRead = JH_App.mGridList.size();
                int nCount = JH_App.mSD_PhotosList.size();
                if (nCount != 0) {
                    nPrecent = (nRead * 100) / nCount;
                }
            } else {
                int nRead = JH_App.mGridList.size();
                int nCount = JH_App.mSD_VideosList.size();

                if (nCount != 0) {
                    nPrecent = (nRead * 100) / nCount;

                    precentView.setText(nPrecent + "%");
                    progressBar.setProgress(nPrecent);
                }
            }
            if (nPrecent > 100)
                nPrecent = 100;
            precentView.setText(nPrecent + "%");
            progressBar.setProgress(nPrecent);
        } else {
            logoImgView.setVisibility(View.VISIBLE);
            precentView.setVisibility(View.INVISIBLE);
            progressBar.setVisibility(View.INVISIBLE);
        }


    }
}
