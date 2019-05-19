package com.example.daeng.mylogger2;

/**
 * Created by daeng on 2016-11-26.
 */

import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.widget.Toast;


public class MyDB extends SQLiteOpenHelper {
    Context context;

    public MyDB(Context context) {
        super(context, "myLocation", null, 1);
        this.context = context;
    }


    @Override
    public void onCreate(SQLiteDatabase db) {
        String sql = "Create table location" +
                "( _id integer primary key autoincrement" +
                ", latitude real" +
                ", longitude real" +
                ", activity char(10)" +
                ")";
        db.execSQL(sql);
        Toast.makeText(context, "DB가 생성되었습니다~~와아~~", Toast.LENGTH_LONG).show();
    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        String sql = "DROP TABLE IF EXISTS location";
        db.execSQL(sql);
        onCreate(db);
    }
}
