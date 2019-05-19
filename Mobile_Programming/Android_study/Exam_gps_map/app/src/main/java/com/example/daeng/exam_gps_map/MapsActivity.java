package com.example.daeng.exam_gps_map;

import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.graphics.Color;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.support.v4.app.FragmentActivity;
import android.os.Bundle;
import android.support.v7.app.ActionBarActivity;
import android.view.View;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.Toast;


import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.CircleOptions;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.PolylineOptions;

import java.util.ArrayList;


public class MapsActivity extends FragmentActivity implements OnMapReadyCallback {

    GoogleMap mMap;
    static LatLng SEOUL = new LatLng( 37.56, 126.97);
    SQLiteDatabase db;
    DB helper;
    ArrayList<LatLng> list_latlng = new ArrayList<LatLng>();
    ArrayList<String> list_str = new ArrayList<String>();
    PolylineOptions polylineOptions;

    EditText schedule = (EditText) findViewById(R.id.editText);
    Button store = (Button) findViewById(R.id.store);
    String str;
    ListView listview = (ListView) findViewById(R.id.listview);



    public void initialize(){
        Cursor rs = db.rawQuery("select * from Location;", null);
        while(rs.moveToNext()){
            mMap.addCircle(new CircleOptions().center(new LatLng(rs.getDouble(0),rs.getDouble(1))).radius(3).strokeColor(Color.RED).fillColor(Color.BLUE));
            list_latlng.add(new LatLng(rs.getDouble(0), rs.getDouble(1)));
        }
        drawPolyline();
    }

    public void drawPolyline(){
        polylineOptions = new PolylineOptions();
        polylineOptions.color(Color.RED);
        polylineOptions.width(5);
        polylineOptions.addAll(list_latlng);
        mMap.addPolyline(polylineOptions);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_maps);
        // Obtain the SupportMapFragment and get notified when the map is ready to be used.
        SupportMapFragment mapFragment = (SupportMapFragment) getSupportFragmentManager()
                .findFragmentById(R.id.map);
        mapFragment.getMapAsync(this);

        store.setOnClickListener(new View.OnClickListener(){
            public void onClick(View v){
                String str = schedule.getText().toString();
                list_str.add(str); //이걸 한 이유가 없어...

                //db.execSQL( "insert into  Location(x,y,z) values ("+Double.toString(location.latitude)+"," +Double.toString(location.longitude)+ "," + str +");" );

                schedule.setText("");
            }
        }); // 저장버튼을 누르면 db에 저장하고 싶었는데...저장을 어떻게 하지?
    }


    @Override
    public void onMapReady(GoogleMap googleMap) {
        // ↑매개변수로 GoogleMap 객체가 넘어옵니다.
        mMap = googleMap;
        helper = new DB(MapsActivity.this, "Location",null,1);
        db = helper.getWritableDatabase(); // 쓸 수 있는 db를 리턴


        GPSListener gps = new GPSListener(db, mMap, list_latlng);
        initialize();

        if(list_latlng.size()!=0){
            mMap.moveCamera(CameraUpdateFactory.newLatLngZoom(list_latlng.get(0),15));
        }else{
            mMap.moveCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(37.610215, 126.997202),5));
        }

        startLocationService();
    }

    private void startLocationService() {
        LocationManager manager = (LocationManager) getSystemService(Context.LOCATION_SERVICE);

        GPSListener gpsListener = new GPSListener(db,mMap,list_latlng);
        long minTime = 10000;
        float minDistance = 10;

        try {
            manager.requestLocationUpdates(LocationManager.GPS_PROVIDER, minTime, minDistance, gpsListener);
            manager.requestLocationUpdates(LocationManager.NETWORK_PROVIDER, minTime, minDistance, gpsListener);

            Location lastLocation = manager.getLastKnownLocation(LocationManager.GPS_PROVIDER);
            if (lastLocation != null) {
                Double latitude = lastLocation.getLatitude();
                Double longitude = lastLocation.getLongitude();

                Toast.makeText(getApplicationContext(), "Last Known Location : " + "Latitude : " + latitude + "\nLongitude:" + longitude, Toast.LENGTH_LONG).show();
            }
        } catch(SecurityException e) {
            e.printStackTrace();
        }
    }

    class GPSListener implements LocationListener {
        SQLiteDatabase db;
        GoogleMap mMap;
        ArrayList<LatLng> save;


        GPSListener(SQLiteDatabase db, GoogleMap mMap, ArrayList<LatLng> save){
            this.db = db;
            this.mMap = mMap;
            this.save = save;
        }

        public void onLocationChanged(Location location) {
            Double latitude = location.getLatitude();
            Double longitude = location.getLongitude();


            if(str!=null) {
                db.execSQL("insert into  Location(x,y,z) values (" + Double.toString(latitude) + "," + Double.toString(longitude) + "," + str + ");"); //이게 위치가 변할 때마다 db에 저장하는건데..
            }else{
                db.execSQL("insert into Location(x,y) values ("+ Double.toString(latitude)+","+Double.toString(longitude)+");");
            }

            list_latlng.add(new LatLng(latitude,longitude));
            drawPolyline();

            mMap.addCircle(new CircleOptions().center(new LatLng(latitude, longitude)).radius(3).strokeColor(Color.RED).fillColor(Color.BLUE));
            mMap.moveCamera(CameraUpdateFactory.newLatLng(new LatLng(latitude, longitude)));


            String msg = "Latitude : "+ latitude + "\nLongitude:"+ longitude;

            Toast.makeText(getApplicationContext(), msg, Toast.LENGTH_SHORT).show();
        }


        public void onProviderDisabled(String provider) {
        }

        public void onProviderEnabled(String provider) {
        }

        public void onStatusChanged(String provider, int status, Bundle extras) {
        }
    }


    class DB extends SQLiteOpenHelper{
        Context context;

        public DB(Context context, String name, SQLiteDatabase.CursorFactory factory, int version){
            super(context, name, factory, version);
            this.context = context;
        }

        @Override
        public void onCreate(SQLiteDatabase db) {
            String a = "create table Location(x double,y double,z String);";
            db.execSQL(a);
            Toast.makeText(context,"DB가 생성되었습니다!! 와아~~", Toast.LENGTH_LONG).show();
        }//table생성

        @Override
        public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
            String a = "create table Location(x double,y double, z String);";
            db.execSQL(a);
            Toast.makeText(context,"DB가 업데이트 되었습니다.", Toast.LENGTH_LONG).show();
        }
    }
    }