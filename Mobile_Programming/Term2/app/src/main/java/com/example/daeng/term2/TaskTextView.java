package com.example.daeng.term2;

import android.content.Context;
import android.view.LayoutInflater;
import android.widget.Chronometer;
import android.widget.LinearLayout;
import android.widget.TextView;

/**
 * Created by daeng on 2016-11-28.
 */

public class TaskTextView extends LinearLayout {

    //list에 보일 task text
    private TextView mText01;
    Chronometer chronometer;


    public TaskTextView(Context context, TaskTextItem aItem) {
        super(context);

        // Layout Inflation
        LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        inflater.inflate(R.layout.list_item_task, this, true);


        // Set Text 01
        mText01 = (TextView) findViewById(R.id.tasks);
        mText01.setText(aItem.getData(0));

    }

    /**
     * set Text
     *
     * @param index
     * @param data
     */
    public void setText(int index, String data) {
        if (index == 0) {
            mText01.setText(data);
        } else {
            throw new IllegalArgumentException();
        }
    }
}
