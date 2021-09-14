package com.joyhonest.jh_ui;

import android.annotation.SuppressLint;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AbsListView;
import android.widget.BaseAdapter;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.joyhonest.wifination.R;


/**
 * Created by aiven on 2017/6/5.
 */

public class MyAdapter extends BaseAdapter {


    public int nStyle = JH_App.nStyle_ui;

    private GridView gridView;
    public boolean bTypeDisoSD;


    ViewHolder holder = null;
    Context context;
    //private List<String> scrollViews;
    /*
    public MyAdapter(Context context) {
        this.context = context;
    }
    */

    public MyAdapter(Context context, GridView gridView) {
        this.context = context;
        this.gridView = gridView;
        nStyle = JH_App.nStyle_ui;
    }

    public MyAdapter(Context context, GridView gridView, int _nType) {
        this.context = context;
        this.gridView = gridView;
        this.nStyle = _nType;
    }

    @Override
    public int getCount() {
        return JH_App.mGridList.size();
    }

    @Override
    public Object getItem(int arg0) {
        return JH_App.mGridList.get(arg0);
    }

    @Override
    public long getItemId(int arg0) {
        return arg0;
    }

    @SuppressLint("InflateParams")
    @Override
    public View getView(final int position, View convertView, ViewGroup parent) {
        if (convertView == null) {
            holder = new ViewHolder();
            LayoutInflater layoutInflater = LayoutInflater.from(context);
            if (nStyle == JH_App.nStyle_ui) {
                convertView = layoutInflater.inflate(R.layout.list_item_jh, null);
            } else {
                convertView = layoutInflater.inflate(R.layout.list_item_fly_jh, null);
            }
            holder.ImageViewA = (ImageView) convertView.findViewById(R.id.imageView3);
            holder.ImageView = (ImageView) convertView.findViewById(R.id.imageView1);
            holder.SelectedImageView = (ImageView) convertView.findViewById(R.id.imageView2);
            holder.Name_View = (TextView) convertView.findViewById(R.id.textView_name);
            holder.Precent_View = (TextView) convertView.findViewById(R.id.textView_Precent);
            holder.Time_View = (TextView) convertView.findViewById(R.id.textView_Time);
            holder.ProgressBar = (ProgressBar) convertView.findViewById(R.id.progressBar);
            convertView.setTag(holder);
        } else {
            holder = (ViewHolder) convertView.getTag();
        }

        if (holder.ImageViewA != null) {
            if (JH_App.bBrowPhoto) {
                holder.ImageViewA.setVisibility(View.INVISIBLE);
            } else {
                holder.ImageViewA.setVisibility(View.VISIBLE);
            }
            holder.ImageViewA.setBackgroundResource(R.mipmap.videotype_icon_fly_jh);
        }
        {
            MyItemData item = null;
            if (position < JH_App.mGridList.size())
                item = JH_App.mGridList.get(position);

            String sTime = " ";

            if (item != null) {
                if (JH_App.bBrowSD) {
                    if (item.nDownloadStatus == JH_App.DownLoaded) {
                        String sName = JH_App.getFileName(item.sPhonePath);
                        holder.Name_View.setTextColor(0xFFFFFFFF);

                        holder.ImageView.setImageBitmap(item.image);
                        if (item.nDuration >= 0) {
                            int m = item.nDuration / 60;
                            int s = item.nDuration % 60;
                            sTime = String.format("%02d:%02d", m, s);
                        } else {
                            sTime = "";
                        }

                        holder.Time_View.setText(sTime);
                        holder.Name_View.setVisibility(View.VISIBLE);
                        if (sName != null)
                            holder.Name_View.setText(sName);
                        else
                            holder.Name_View.setText("");

                        holder.Precent_View.setText("100%");

                        holder.ProgressBar.setVisibility(View.VISIBLE);
                        if (JH_App.bBrowPhoto) {
                            holder.ProgressBar.setProgress(0);
                            holder.Time_View.setVisibility(View.INVISIBLE);
                            holder.Precent_View.setVisibility(View.INVISIBLE);
                        } else {
                            holder.ProgressBar.setProgress(100);
                            holder.Time_View.setVisibility(View.VISIBLE);
                            holder.Precent_View.setVisibility(View.VISIBLE);
                        }
                    }
                    if (item.nDownloadStatus == JH_App.DownLoading) {
                        holder.Name_View.setTextColor(0xFFFFFFFF);
                        holder.ImageView.setImageBitmap(item.image);

                        String sName = JH_App.getFileName(item.sSDPath);
                        holder.Name_View.setVisibility(View.VISIBLE);

                        if (sName != null)
                            holder.Name_View.setText(sName);
                        else
                            holder.Name_View.setText("");

                        int nPreA = (int) (item.fPrecent);
                        if (nPreA > 100)
                            nPreA = 100;

                        holder.ProgressBar.setProgress(nPreA);

                        if (!JH_App.bBrowPhoto) {
                            holder.Time_View.setVisibility(View.INVISIBLE);
                            holder.Precent_View.setVisibility(View.VISIBLE);
                            holder.Precent_View.setText(nPreA + "%");
                        } else {
                            holder.Precent_View.setVisibility(View.INVISIBLE);
                            holder.Time_View.setVisibility(View.INVISIBLE);
                        }
                    }
                    if (item.nDownloadStatus == JH_App.DownLoadNormal || item.nDownloadStatus == JH_App.DownLoaded_NO || item.nDownloadStatus == JH_App.DownLoadNeed) {
                        holder.ImageView.setImageBitmap(item.image);

                        String sName = JH_App.getFileName(item.sSDPath);
                        holder.Name_View.setVisibility(View.VISIBLE);
                        if (sName != null)
                            holder.Name_View.setText(sName);
                        else
                            holder.Name_View.setText("");

                        holder.ProgressBar.setProgress(0);
                        holder.ProgressBar.setVisibility(View.VISIBLE);

                        holder.Time_View.setVisibility(View.INVISIBLE);
                        holder.Precent_View.setVisibility(View.INVISIBLE);
                        if (item.nDownloadStatus == JH_App.DownLoadNeed) {
                            holder.Name_View.setTextColor(0xFFFF0000);
                        } else {
                            holder.Name_View.setTextColor(0xFFFFFFFF);
                        }
                    }

                } else {
                    holder.ProgressBar.setVisibility(View.INVISIBLE);
                    holder.Precent_View.setVisibility(View.INVISIBLE);
                    holder.Name_View.setVisibility(View.INVISIBLE);
                    holder.Time_View.setVisibility(View.INVISIBLE);
                    if (JH_App.bBrowPhoto) {
                        holder.ImageView.setImageBitmap(item.image);
                    } else {
                        holder.ImageView.setImageBitmap(item.image);
                    }
                }

                if (nStyle == JH_App.nStyle_ui) {
                    if (item.bSelected) {
                        holder.SelectedImageView.setVisibility(View.VISIBLE);

                    } else {
                        holder.SelectedImageView.setVisibility(View.INVISIBLE);
                    }
                } else {
                    if (item.nSelectedStatus == 0) {
                        holder.SelectedImageView.setVisibility(View.INVISIBLE);
                    }
                    if (item.nSelectedStatus == 1) {
                        holder.SelectedImageView.setVisibility(View.VISIBLE);
                        holder.SelectedImageView.setBackgroundResource(R.mipmap.selectedflag_fly_0_jh);
                    }
                    if (item.nSelectedStatus == 2) {
                        holder.SelectedImageView.setVisibility(View.VISIBLE);
                        holder.SelectedImageView.setBackgroundResource(R.mipmap.selectedflag_fly_jh);
                    }
                }
            }
        }

        int w = gridView.getWidth();
        int h = gridView.getHeight();

        //int w =View.MeasureSpec.makeMeasureSpec(0,View.MeasureSpec.UNSPECIFIED);
        //int h =View.MeasureSpec.makeMeasureSpec(0,View.MeasureSpec.UNSPECIFIED);
//        gridView.measure(w,h);

        //h=gridView.getMeasuredHeight();
        //w=gridView.getMeasuredWidth();


        //gridView.setStretchMode(GridView.STRETCH_COLUMN_WIDTH); // 设置为禁止拉伸模式
        int ws = gridView.getHorizontalSpacing();
        int hs = gridView.getVerticalSpacing();
        if (nStyle == JH_App.nStyle_ui) {
            if (JH_App.bBrowSD) {
                w -= (ws * 2);
                h -= hs;
                w /= 3;
                h /= 2;

            } else {
                w -= (ws * 3);
                h -= hs;
                w /= 4;
                h /= 2;
            }
        } else {

            w -= (ws * 3);
            h -= hs;
            w /= 4;
            h /= 2;

        }

        AbsListView.LayoutParams param = new AbsListView.LayoutParams(
                w,
                h);
        convertView.setLayoutParams(param);
        return convertView;
    }

    private final class ViewHolder {
        private ImageView ImageView;
        private ImageView ImageViewA;
        private ImageView SelectedImageView;
        private TextView Precent_View;
        private TextView Name_View;
        private TextView Time_View;
        private ProgressBar ProgressBar;
    }

}
