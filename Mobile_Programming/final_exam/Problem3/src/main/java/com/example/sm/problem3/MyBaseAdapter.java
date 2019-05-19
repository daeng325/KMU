package com.example.sm.problem3;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;

import java.util.ArrayList;

/**
 * Created by sm on 12/12/16.
 */

public class MyBaseAdapter extends BaseAdapter{

    Context mContetxt = null;
    ArrayList<Customer> mData = null;
    LayoutInflater mLayoutInflater = null;

    MyBaseAdapter(  Context context, ArrayList<Customer> data){
        mContetxt = context;
        mData = data;
        mLayoutInflater = LayoutInflater.from(mContetxt);
    }

    @Override
    public int getCount() {
        return mData.size();
    }

    @Override
    public Object getItem(int position) {
        return mData.get(position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {

        // need something here
        View itemLayout = convertView; //...?

        return itemLayout;

    }
}
