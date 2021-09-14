package com.joyhonest.jh_fly;


import android.os.Bundle;
import androidx.fragment.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.GridView;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.joyhonest.jh_ui.JH_App;
import com.joyhonest.jh_ui.MyItemData;
import com.joyhonest.wifination.R;

import org.simple.eventbus.EventBus;

import com.joyhonest.jh_ui.MyAdapter;

/**
 * A simple {@link Fragment} subclass.
 */
public class BrowFilesFragment extends Fragment implements View.OnClickListener, AdapterView.OnItemClickListener, AdapterView.OnItemLongClickListener {


    public MyAdapter adapter;
    private GridView gridView;
    private Button return_btn;
    private Button phone_btn;
    private Button sd_btn;


    private Button button_all;
    private TextView textView_all;
    private Button button_selected;
    private Button button_trash_2;
    private Button button_trash;
    private boolean bSelectAll;


    public boolean bCannSelected = false;

    RelativeLayout AlartView;
    private Button button_yes;
    private Button button_no;

    private RelativeLayout Layout_Bar;


    public BrowFilesFragment() {
        // Required empty public constructor
    }


    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View view = inflater.inflate(R.layout.fragment_brow_files, container, false);
        bSelectAll = false;


        Layout_Bar = (RelativeLayout) view.findViewById(R.id.Layout_Bar);


        AlartView = (RelativeLayout) view.findViewById(R.id.AlartView);
        AlartView.setVisibility(View.INVISIBLE);

        button_yes = (Button) view.findViewById(R.id.button_yes);
        button_no = (Button) view.findViewById(R.id.button_no);

        button_all = (Button) view.findViewById(R.id.button_all);
        button_selected = (Button) view.findViewById(R.id.button_selected);
        button_trash_2 = (Button) view.findViewById(R.id.button_trash_2);
        button_trash = (Button) view.findViewById(R.id.button_trash);
        textView_all = (TextView) view.findViewById(R.id.textView_all);

        gridView = (GridView) view.findViewById(R.id.Gridview);

        adapter = new MyAdapter(getActivity().getApplicationContext(), gridView, JH_App.nStyle_fly);
        gridView.setAdapter(adapter);

        return_btn = (Button) view.findViewById(R.id.return_button);
        sd_btn = (Button) view.findViewById(R.id.button_sd);
        phone_btn = (Button) view.findViewById(R.id.button_phone);


        return_btn.setOnClickListener(this);
        sd_btn.setOnClickListener(this);
        phone_btn.setOnClickListener(this);

        gridView.setOnItemClickListener(this);
        gridView.setOnItemLongClickListener(this);


        button_all.setOnClickListener(this);
        button_selected.setOnClickListener(this);
        button_trash_2.setOnClickListener(this);
        button_trash.setOnClickListener(this);
        button_yes.setOnClickListener(this);
        button_no.setOnClickListener(this);
        F_DispSelectSDorPhone();

        F_DispInit();

        return view;
    }

    public void F_DispSelectSDorPhone() {
        if (JH_App.bBrowSD) {
            phone_btn.setBackgroundResource(R.mipmap.phone_2_fly_jh);
            sd_btn.setBackgroundResource(R.mipmap.sd_sel_fly_jh);
        } else {
            phone_btn.setBackgroundResource(R.mipmap.phone_2_sel_fly_jh);
            sd_btn.setBackgroundResource(R.mipmap.sd_fly_jh);
        }
    }

    public void F_DispInit() {
        AlartView.setVisibility(View.INVISIBLE);
        bSelectAll = false;
        bCannSelected = false;
        for (MyItemData data : JH_App.mGridList) {
            data.bSelected = false;
            data.nSelectedStatus = 0;
        }
        F_DispSelectUI(false);
        F_UpdateLisetViewData();
    }

    private void F_DispSelectNumber() {
        int nCount = 0;
        for (MyItemData data : JH_App.mGridList) {
            if (data.bSelected)
                nCount++;
        }
        int nC = JH_App.mGridList.size();
        if (nC > nCount) {
            bSelectAll = false;
            button_all.setBackgroundResource(R.mipmap.all_backgound_fly_jh);
        } else {
            bSelectAll = true;
            button_all.setBackgroundResource(R.mipmap.all_backgound_sel_fly_jh);
        }
        button_selected.setText("" + nCount);
    }

    private void F_DispSelectUI(boolean b) {
        if (b) {
            if (bSelectAll) {
                button_all.setBackgroundResource(R.mipmap.all_backgound_sel_fly_jh);
            } else {
                button_all.setBackgroundResource(R.mipmap.all_backgound_fly_jh);
            }

            Layout_Bar.setVisibility(View.VISIBLE);
            /*

            button_all.setVisibility(View.VISIBLE);
            button_selected.setVisibility(View.VISIBLE);
            button_trash_2.setVisibility(View.VISIBLE);
            button_trash.setVisibility(View.VISIBLE);
            textView_all.setVisibility(View.VISIBLE);
            */
        } else {
            bCannSelected = false;
            Layout_Bar.setVisibility(View.INVISIBLE);
            /*
            button_all.setVisibility(View.INVISIBLE);
            button_selected.setVisibility(View.INVISIBLE);
            button_trash_2.setVisibility(View.INVISIBLE);
            button_trash.setVisibility(View.INVISIBLE);
            textView_all.setVisibility(View.INVISIBLE);
            */
        }
    }


    private void F_TranSelect2System() {
        String sFileName;
        for (MyItemData data : JH_App.mGridList) {
            if (data.bSelected) {
                sFileName = data.sPhonePath;
                if (JH_App.bBrowPhoto) {
                    JH_App.F_Save2ToGallery(sFileName, true);
                } else {
                    JH_App.F_Save2ToGallery(sFileName, false);
                }
                data.bSelected = false;
                data.nSelectedStatus = 0;

            }
        }
        F_DispInit();


    }

    @Override
    public void onClick(View v) {
        Integer nFragment;
        if (v == phone_btn) {
            if (JH_App.bBrowSD) {
                JH_App.bBrowSD = false;
                F_DispSelectSDorPhone();
                Fly_PlayActivity activity = (Fly_PlayActivity) getActivity();
                activity.F_CancelDownLoad();
                activity.F_GetFilesNumber();
                activity.F_FillFilesData();
            }

        }
        if (v == sd_btn) {
            if (!JH_App.bBrowSD) {
                JH_App.bBrowSD = true;
                F_DispSelectSDorPhone();
                Fly_PlayActivity activity = (Fly_PlayActivity) getActivity();
                activity.F_CancelDownLoad();
                JH_App.mSD_PhotosList.clear();
                JH_App.mSD_VideosList.clear();
                JH_App.mGridList.clear();
                F_UpdateLisetView();
                activity.F_GetFilesNumber();
                //activity.F_FillFilesData();
            }
        }

        if (v == button_trash_2) {
            F_TranSelect2System();
            JH_App.F_DispMesate("Download Finished!");
        }

        if (v == button_yes) {
            if (JH_App.bDownLoading) {
                JH_App.F_DispMesate("正在下载，请稍候");
                return;
            }
            EventBus.getDefault().post("", "Grid_Delete");
            AlartView.setVisibility(View.INVISIBLE);
        }
        if (v == button_no) {
            AlartView.setVisibility(View.INVISIBLE);
            F_DispInit();

        }
        if (button_all == v) {
            bSelectAll = !bSelectAll;
            if (bCannSelected) {
                F_DispSelectUI(true);
                for (MyItemData data : JH_App.mGridList) {
                    if (bSelectAll) {
                        data.bSelected = true;
                        data.nSelectedStatus = 2;
                    } else {
                        data.bSelected = false;
                        data.nSelectedStatus = 1;
                    }
                }
                F_DispSelectNumber();
                F_UpdateLisetViewData();
            }
        }
        if (button_trash == v) {
            if (bCannSelected) {
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
        if (v == return_btn) {
            if (bCannSelected) {
                F_DispInit();
                return;
            }

            EventBus.getDefault().post("exit", "Exit");
        }

    }

    public void F_UpdateLisetViewData() {
        adapter.notifyDataSetChanged();
    }

    public void F_UpdateLisetView() {
        if (!JH_App.bBrowPhoto) {
            gridView.setNumColumns(3);
        } else {
            gridView.setNumColumns(4);
        }

        adapter.notifyDataSetChanged();
        if (JH_App.bBrowSD) {
            //logoImgView.setVisibility(View.INVISIBLE);
            //precentView.setVisibility(View.VISIBLE);
            //progressBar.setVisibility(View.VISIBLE);
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

                    //precentView.setText(nPrecent+"%");
                    //progressBar.setProgress(nPrecent);
                }
            }
            if (nPrecent > 100)
                nPrecent = 100;
            //precentView.setText(nPrecent+"%");
            //progressBar.setProgress(nPrecent);
        } else {
            //   logoImgView.setVisibility(View.VISIBLE);
            //   precentView.setVisibility(View.INVISIBLE);
            //   progressBar.setVisibility(View.INVISIBLE);
        }


    }

    @Override
    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {

        if (bCannSelected) {
            MyItemData data = JH_App.mGridList.get(position);

            data.bSelected = !data.bSelected;
            if (data.bSelected)
                data.nSelectedStatus = 2;
            else
                data.nSelectedStatus = 1;
            F_DispSelectNumber();
            F_UpdateLisetViewData();
            return;
        }
        Integer ix = position;
        EventBus.getDefault().post(ix, "GridItem_Click");

    }

    @Override
    public boolean onItemLongClick(AdapterView<?> parent, View view, int position, long id) {
        //bCannSelected = !bCannSelected;
        if (!bCannSelected) {
            bCannSelected = true;
            bSelectAll = false;
            F_DispSelectUI(true);

            MyItemData dat = JH_App.mGridList.get(position);
            for (MyItemData data : JH_App.mGridList) {
                if (dat == data) {
                    data.bSelected = true;
                    data.nSelectedStatus = 2;
                } else {
                    data.bSelected = false;
                    data.nSelectedStatus = 1;
                }
            }
            F_DispSelectNumber();
            F_UpdateLisetViewData();
        }

        return true;
    }
}
