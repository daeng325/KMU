package com.example.daeng.mylogger2;

/**
 * Created by daeng on 2016-11-26.
 */

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends Activity {

    Button button01, button02, resetBtn;
    MyDB mydb;
    SQLiteDatabase sqlite;
    TextView textId, textLatitude, textLongitude, textActivity;
    Intent i;
    RadioGroup radioGroup;
    RadioButton radioButton;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        button01 = (Button) findViewById(R.id.button01);
        button02 = (Button) findViewById(R.id.button02);
        resetBtn = (Button) findViewById(R.id.resetBtn);

        textId = (TextView) findViewById(R.id.textId);
        textLatitude = (TextView) findViewById(R.id.textLatitude);
        textLongitude = (TextView) findViewById(R.id.textLongitude);
        textActivity = (TextView) findViewById(R.id.textActivity);
        radioGroup = (RadioGroup) findViewById(R.id.radioGroup);


        // 데이터베이스 연결
        mydb = new MyDB(this);

        // 초기화
        resetBtn.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {

                sqlite = mydb.getWritableDatabase();
                mydb.onUpgrade(sqlite, 1, 2);   // 1번 버전 지우고 2번 버전 만들겠다.
                sqlite.close();
            }
        });

        // 입력
        button01.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                startLocationService();
            }
        });

        // 조회
        button02.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                sqlite = mydb.getReadableDatabase();
                String sql = "SELECT * FROM location";

                // 테이블을 돌아다니면서 데이터를 읽어 올 Cursor
                Cursor cursor;
                cursor = sqlite.rawQuery(sql, null);

                String _idStr = "번호\r\n";
                String latitudeStr = "위도\r\n";
                String longitudeStr = "경도\r\n";
                String activityStr = "활동\r\n";

                while (cursor.moveToNext()) { // cursor가 이동하며 각 column의 내용 읽어옴.

                    _idStr += cursor.getString(0) + "\r\n";

                    latitudeStr += cursor.getString(1) + "\r\n";

                    longitudeStr += cursor.getString(2) + "\r\n";

                    activityStr += cursor.getString(3) + "\r\n";

                }
                // Text문 변환.
                textId.setText(_idStr);
                textLatitude.setText(latitudeStr);
                textLongitude.setText(longitudeStr);
                textActivity.setText(activityStr);

                cursor.close();
                sqlite.close();
            }
        });
    }


    private class GPSListener implements LocationListener{

        @Override
        public void onLocationChanged(Location location) {
            Double latitude = location.getLatitude();
            Double longitude = location.getLongitude();

            String msg = "Latitude: " + latitude + "\nLongitude: " + longitude;
            Log.i("GPSListener", msg);

            radioButton = (RadioButton)findViewById(radioGroup.getCheckedRadioButtonId());
            String activity = radioButton.getText().toString();

            sqlite = mydb.getWritableDatabase();

            String sql = "INSERT INTO location(latitude, longitude, activity) VALUES('" +latitude+ "', '" +longitude+ "', '" +activity+"') " ;

            Log.d("myLocation", sql);

            sqlite.execSQL(sql);
            sqlite.close();
            Toast.makeText(getApplicationContext(), msg + " 데이터가 저장되었습니다.", Toast.LENGTH_SHORT).show();
        }

        @Override
        public void onStatusChanged(String provider, int status, Bundle extras) {

        }

        @Override
        public void onProviderEnabled(String provider) {

        }

        @Override
        public void onProviderDisabled(String provider) {

        }
    }

    private void startLocationService(){
        LocationManager manager = (LocationManager)getSystemService(Context.LOCATION_SERVICE);

        GPSListener gpsListener = new GPSListener();

        long minTime = 10000;
        float minDistance = 10;

        try{
            manager.requestLocationUpdates(
                    LocationManager.GPS_PROVIDER,
                    minTime, minDistance, gpsListener);
            manager.requestLocationUpdates(
                    LocationManager.NETWORK_PROVIDER,
                    minTime, minDistance, gpsListener);
        }
        catch (SecurityException ex){
            ex.printStackTrace();
        }

        Toast.makeText(getApplicationContext(), "위치 정보 저장", Toast.LENGTH_SHORT).show();
    }
}