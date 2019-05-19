package com.example.daeng.term2;

import android.content.Context;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by daeng on 2016-11-28.
 */

public class StatisticsTextListAdapter extends BaseAdapter {
    private Context mContext;


    private List<StatisticsTextItem> mItems = new ArrayList<StatisticsTextItem>();

    public StatisticsTextListAdapter(Context context) {
        mContext = context;
    }

    public void addItem(StatisticsTextItem it) {
        mItems.add(it);
    }

    public void setListItems(List<StatisticsTextItem> lit) {
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
        StatisticsTextView itemView;
        if (convertView == null) {
            itemView = new StatisticsTextView(mContext, mItems.get(position));
        } else {
            itemView = (StatisticsTextView) convertView;
            itemView.setText(0, mItems.get(position).getData(0));
        }

        return itemView;
    }
}
