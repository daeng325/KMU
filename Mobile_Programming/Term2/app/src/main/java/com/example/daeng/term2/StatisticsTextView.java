package com.example.daeng.term2;

import android.content.Context;
import android.view.LayoutInflater;
import android.widget.LinearLayout;
import android.widget.TextView;

/**
 * Created by daeng on 2016-11-28.
 */

public class StatisticsTextView extends LinearLayout {

    //list에 보일 task text
    private TextView mText01;


    public StatisticsTextView(Context context, StatisticsTextItem aItem) {
        super(context);

        // Layout Inflation
        LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        inflater.inflate(R.layout.list_item_statistics, this, true);


        // Set Text 01
        mText01 = (TextView) findViewById(R.id.statistics_task);
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
