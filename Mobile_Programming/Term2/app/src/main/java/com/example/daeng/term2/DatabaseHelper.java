package com.example.daeng.term2;

import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.util.Log;
import android.widget.Toast;

/**
 * Created by daeng on 2016-12-01.
 */


public class DatabaseHelper extends SQLiteOpenHelper {

    private Context context;
    public static final String TAG = "this";
    String TableName;


    public DatabaseHelper(Context context, String name, SQLiteDatabase.CursorFactory factory , int version) {
        super(context, name, null, version);
        this.context = context;
        TableName = name;
    }

    public void onCreate(SQLiteDatabase db) {

        try {
            String DROP_SQL = "drop table if exists" + TableName;
            db.execSQL(DROP_SQL);
        } catch(Exception ex) {
            Log.e(TAG, "Exception in DROP_SQL", ex);
        }


        try{
            db.execSQL("create table Location(x double,y double);");
            db.execSQL("create table TaskDB(_id integer PRIMARY KEY autoincrement, task text)");
            db.execSQL("create table Time(_id integer PRIMARY KEY autoincrement, time Real");
            db.execSQL("create table Picture(_id integer PRIMARY KEY autoincrement, Latitude Real, Longitude Real, CameraPath Text)");
        }catch (Exception ex){
            Log.e(TAG, "Exception in CREATE_SQL", ex);
        }

        Toast.makeText(context,"DB생성 완료",Toast.LENGTH_LONG).show();

    }

    public void onOpen(SQLiteDatabase db) {

    }

    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        Log.w(TAG, "Upgrading database from version " + oldVersion +
                " to " + newVersion + ".");
        db.execSQL("DROP TABLE IF EXISTS"+ TableName);
        onCreate(db);
    }

    public void insert(String task,String TableName) {
        // 읽고 쓰기가 가능하게 DB 열기
        SQLiteDatabase db = getWritableDatabase();
        // DB에 입력한 값으로 행 추가
        String str = "insert into "+TableName+"(task) values('"+task +"');";
        db.execSQL(str);
        db.close();
    }


    public void delete(String task,String TableName) {
        SQLiteDatabase db = getWritableDatabase();
        // 입력한 항목과 일치하는 행 삭제
        db.execSQL("DELETE FROM "+TableName+" WHERE task='" + task + "';");
        db.close();
    }

    public String getResult() {
        // 읽기가 가능하게 DB 열기
        SQLiteDatabase db = getReadableDatabase();
        String result = "";

        // DB에 있는 데이터를 쉽게 처리하기 위해 Cursor를 사용하여 테이블에 있는 모든 데이터 출력
        Cursor cursor = db.rawQuery("SELECT * FROM "+TableName, null);
        while (cursor.moveToNext()) {
            result += cursor.getString(0);
        }
        cursor.close();
        return result;
    }
}


