package com.example.daeng.term2;

/**
 * Created by daeng on 2016-11-26.
 */

import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.os.Bundle;
import android.os.SystemClock;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.Chronometer;
import android.widget.ListView;

import static android.app.Activity.RESULT_OK;

public class FragmentTask extends Fragment {
    public static final String TAG = "this";
    public static final int REQUEST_CODE_ANOTHER = 1001;

    Context mContext;
    private Button addtask;
    private Button editTask;
    private Button endChronometer;
    private Button reset;
    ListView listView;
    TaskTextListAdapter adapter;
    Chronometer chronometer;
    long endTime;

    final DatabaseHelper dbHelper;
    SQLiteDatabase db;


    public FragmentTask(Context context){
        mContext = context;
        //데이터베이스 연결
        dbHelper = new DatabaseHelper(mContext,"TaskDB",null,1);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        final ViewGroup rootView = (ViewGroup) inflater.inflate(R.layout.fragment_task, container, false);

        //참조하는 것들
        addtask = (Button) rootView.findViewById(R.id.addTask);
        listView = (ListView) rootView.findViewById(R.id.list_task) ;
        chronometer = (Chronometer) rootView.findViewById(R.id.chronometer);
        endChronometer = (Button) rootView.findViewById(R.id.end_chronometer);
        editTask = (Button) rootView.findViewById(R.id.edit_task);
        reset = (Button) rootView.findViewById(R.id.reset);



        // 어댑터 객체 생성
        adapter = new TaskTextListAdapter(rootView.getContext(),chronometer);

        //이벤트 추가 버튼 클릭
        addtask.setOnClickListener(new View.OnClickListener()
        {

            @Override
            public void onClick(View v) {
                // 인텐트 객체를 만듭니다.
                Intent intent = new Intent(rootView.getContext(),  AddTask.class);

                // 액티비티를 띄워주도록 startActivityForResult() 메소드를 호출합니다.
                startActivityForResult(intent, REQUEST_CODE_ANOTHER);
            }
        });
        //중지 버튼 클릭
        endChronometer.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                chronometer.stop();
                //시간을 DB에 저장해야 함. DB에 colum을 다시 생각해야 함.

                endTime = System.currentTimeMillis();
                long execute_time = adapter.startTime - endTime;
                //db.execSQL("insert into Time(time) values("+Double.toString(execute_time)+");");
            }
        });

        reset.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                chronometer.setBase(SystemClock.elapsedRealtime());
                chronometer.stop(); //왜 setbase만 넣으면 다시 시작할까..
            }
        });

        db = dbHelper.getReadableDatabase();
        Cursor cursor = db.rawQuery("SELECT * FROM TaskDB", null);
        while (cursor.moveToNext()) {
            adapter.addItem(new TaskTextItem(cursor.getString(1)));
        }
        adapter.notifyDataSetChanged();


        // 리스트뷰에 어댑터 설정
        listView.setAdapter(adapter);

        return rootView;
    }

    public void onDestroy(){
        super.onDestroy();
        chronometer.stop();
    }

    /**
     * 새로운 액티비티에서 돌아올 때 자동 호출되는 메소드
     */
    public void onActivityResult(int requestCode, int resultCode, Intent intent) {
        super.onActivityResult(requestCode, resultCode, intent);

        if (requestCode == REQUEST_CODE_ANOTHER) {
            if (resultCode == RESULT_OK) {
                String task = intent.getStringExtra("task");
                if(task.equals("")){
                    return;
                }
                try {
                    dbHelper.insert(task,"TaskDB");
                    adapter.addItem(new TaskTextItem(task));
                } catch(Exception ex) {
                    Log.e(TAG, "Exception in insert SQL", ex);
                }
            }
        }
    }
}

