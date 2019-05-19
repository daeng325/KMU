package com.example.daeng.mylogger2;


import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.os.Bundle;
import android.support.v4.app.FragmentActivity;
import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.MarkerOptions;

public class MapsActivity extends FragmentActivity implements OnMapReadyCallback {

    MyDB mydb;
    SQLiteDatabase sqlite;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_maps);
        SupportMapFragment mapFragment = (SupportMapFragment) getSupportFragmentManager()
                .findFragmentById(R.id.map);
        mapFragment.getMapAsync(this);
    }

    @Override
    public void onMapReady(GoogleMap map) {

        mydb = new MyDB(this);

        sqlite = mydb.getReadableDatabase();
        String sql = "SELECT * FROM location";

        Cursor cursor;

        cursor = sqlite.rawQuery(sql, null);
        String sequecne;
        String latitude;
        String longitude;
        String activity;

        while(cursor.moveToNext()){

            sequecne = cursor.getString(0);

            latitude = cursor.getString(1);

            longitude = cursor.getString(2);

            activity = cursor.getString(3);

            LatLng loca = new LatLng(Double.parseDouble(latitude), Double.parseDouble(longitude));
            map.addMarker(new MarkerOptions().position(loca).title(sequecne + "번:" + activity));
            map.moveCamera(CameraUpdateFactory.newLatLng(loca));
        }

    }
}