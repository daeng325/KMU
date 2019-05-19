package com.example.daeng.term2;

import android.content.Context;
import android.content.Intent;
import android.os.SystemClock;
import android.provider.MediaStore;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.Chronometer;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by daeng on 2016-11-28.
 */

public class TaskTextListAdapter extends BaseAdapter {
    private Context mContext;
    Chronometer chronometer;
//    Button start;
//    Button cameraIntent;
    long startTime;




    private List<TaskTextItem> mItems = new ArrayList<TaskTextItem>();

    public TaskTextListAdapter(Context context, Chronometer chronometer) {
        mContext = context;
        this.chronometer = chronometer;
    }

    public void addItem(TaskTextItem it) {
        mItems.add(it);
    }

    public void setListItems(List<TaskTextItem> lit) {
        mItems = lit;
    }

    public int getCount() {
        return mItems.size();
    }

    public Object getItem(int position) {
        return mItems.get(position);
    }

    public boolean areAllItemsSelectable() {
        return false;
    }

    public boolean isSelectable(int position) {
        try {
            return mItems.get(position).isSelectable();
        } catch (IndexOutOfBoundsException ex) {
            return false;
        }
    }

    public long getItemId(int position) {
        return position;
    }

    public View getView(int position, final View convertView, final ViewGroup parent) {
        //final int pos = position;
        TaskTextView itemView;
        if (convertView == null) {
            itemView = new TaskTextView(mContext, mItems.get(position));
        } else {
            itemView = (TaskTextView) convertView;

            itemView.setText(0, mItems.get(position).getData(0));

            // 시작 버튼을 터치 했을 때 이벤트 발생
            Button start = (Button) convertView.findViewById(R.id.start_chronometer);
            start.setOnClickListener(new View.OnClickListener() {

                @Override
                public void onClick(View v) {
                    chronometer.setBase(SystemClock.elapsedRealtime());
                    chronometer.start();

                    startTime = System.currentTimeMillis();
                }
            });


            // 카메라 버튼을 터치 했을 때 이벤트 발생
            Button cameraIntent = (Button) convertView.findViewById(R.id.camera_intent);
            cameraIntent.setOnClickListener(new View.OnClickListener() {

                @Override
                public void onClick(View v) {
                    //인텐트로 카메라를 띄워야 함
                    Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
                    mContext.startActivity(intent);
                }
            });



        }

        return itemView;
    }
}
