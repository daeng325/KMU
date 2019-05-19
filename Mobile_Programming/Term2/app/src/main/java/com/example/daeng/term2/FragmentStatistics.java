package com.example.daeng.term2;

/**
 * Created by daeng on 2016-11-26.
 */

import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ListView;

public class FragmentStatistics extends Fragment {
    Context mContext;

    ListView listView;
    StatisticsTextListAdapter adapter;
    DatabaseHelper dbHelper;;
    SQLiteDatabase db;

    public FragmentStatistics(){

    }
    public FragmentStatistics(Context context){
        mContext = context;

        //데이터베이스 연결
        dbHelper = new DatabaseHelper(mContext,"TaskDB",null,1);
    }



    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        ViewGroup rootView = (ViewGroup) inflater.inflate(R.layout.fragment_statistics, container, false);

        listView = (ListView) rootView.findViewById(R.id.list_statistics) ;

        // 어댑터 객체 생성
        adapter = new StatisticsTextListAdapter(rootView.getContext());

        // 리스트뷰에 어댑터 설정
        listView.setAdapter(adapter);

        db = dbHelper.getReadableDatabase();
        Cursor cursor = db.rawQuery("SELECT * FROM TaskDB", null);
        while (cursor.moveToNext()) {
            adapter.addItem(new StatisticsTextItem(cursor.getString(1)));
        }
        adapter.notifyDataSetChanged();

        return rootView;
    }
}

