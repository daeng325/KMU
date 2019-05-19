package com.example.daeng.term2;

/**
 * Created by daeng on 2016-11-26.
 */

import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.graphics.Color;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v4.app.Fragment;
import android.support.v4.content.ContextCompat;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;

import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.CircleOptions;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.PolylineOptions;

import java.util.ArrayList;

public class FragmentPicture extends Fragment implements OnMapReadyCallback {
    Context mContext;
    Activity mActivity;
    public FragmentPicture(){

    }
    public FragmentPicture(Context context, Activity activity){
        mContext = context;
        mActivity = activity;
    }

    GoogleMap mMap;
    SQLiteDatabase db;
    DatabaseHelper helper;
    ArrayList<LatLng> list_latlng = new ArrayList<LatLng>();
    PolylineOptions polylineOptions;
    View v;


    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        ViewGroup rootView = (ViewGroup) inflater.inflate(R.layout.fragment_picture, container, false);
        v = rootView;

        ((SupportMapFragment)getChildFragmentManager().findFragmentById(R.id.map)).getMapAsync(this);

        helper = new DatabaseHelper(mContext, "Location",null,1);
        db = helper.getWritableDatabase();

        return rootView;
    }

    public void onMapReady(GoogleMap googleMap) {
        mMap = googleMap;
        helper = new DatabaseHelper(mContext, "Location",null,1);
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

    /** permission 체크해주는 함수입니다. **/
    public void checkDangerousPermissions() {
        String[] permissions = {
                android.Manifest.permission.ACCESS_FINE_LOCATION,
                android.Manifest.permission.ACCESS_COARSE_LOCATION
        };

        int permissionCheck = PackageManager.PERMISSION_GRANTED;
        for (int i = 0; i < permissions.length; i++) {
            permissionCheck = ContextCompat.checkSelfPermission(mContext, permissions[i]);
            if (permissionCheck == PackageManager.PERMISSION_DENIED) {
                break;
            }
        }

        if (permissionCheck == PackageManager.PERMISSION_GRANTED) {
            Toast.makeText(mContext, "권한 있음", Toast.LENGTH_LONG).show();
        } else {
            Toast.makeText(mContext, "권한 없음", Toast.LENGTH_LONG).show();

            if (!ActivityCompat.shouldShowRequestPermissionRationale(mActivity, permissions[0]))
                ActivityCompat.requestPermissions(mActivity, permissions, 1);
            else {
                Toast.makeText(mContext, "권한 설명 필요함.", Toast.LENGTH_LONG).show();
            }
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        if (requestCode == 1) {
            for (int i = 0; i < permissions.length; i++) {
                if (grantResults[i] == PackageManager.PERMISSION_GRANTED) {
                    Toast.makeText(mContext, permissions[i] + " 권한이 승인됨.", Toast.LENGTH_LONG).show();
                } else {
                    Toast.makeText(mContext, permissions[i] + " 권한이 승인되지 않음.", Toast.LENGTH_LONG).show();
                }
            }
        }
    }


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

    public void startLocationService() {

        checkDangerousPermissions();
        // 위치 관리자 객체 참조
        LocationManager manager = (LocationManager) mContext.getSystemService(Context.LOCATION_SERVICE);

        // 위치 정보를 받을 리스너 생성
        GPSListener gpsListener = new GPSListener(db,mMap,list_latlng);
        long minTime = 10000;
        float minDistance = 0;

        try {
            // GPS를 이용한 위치 요청
            manager.requestLocationUpdates(LocationManager.GPS_PROVIDER, minTime, minDistance, gpsListener);

            // 네트워크를 이용한 위치 요청
            manager.requestLocationUpdates(LocationManager.NETWORK_PROVIDER, minTime, minDistance, gpsListener);

            // 위치 확인이 안되는 경우에도 최근에 확인된 위치 정보 먼저 확인
            Location lastLocation = manager.getLastKnownLocation(LocationManager.GPS_PROVIDER);
            if (lastLocation != null) {
                Double latitude = lastLocation.getLatitude();
                Double longitude = lastLocation.getLongitude();

                Toast.makeText(mContext, "Last Known Location : " + "Latitude : " + latitude + "\nLongitude:" + longitude, Toast.LENGTH_LONG).show();
            }
        } catch(SecurityException ex) {
            ex.printStackTrace();
        }

        Toast.makeText(mContext, "위치 확인이 시작되었습니다.", Toast.LENGTH_SHORT).show();

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

            db.execSQL("insert into Location(x,y) values ("+ Double.toString(latitude)+","+Double.toString(longitude)+");");

            list_latlng.add(new LatLng(latitude,longitude));
            drawPolyline();

            mMap.addCircle(new CircleOptions().center(new LatLng(latitude, longitude)).radius(3).strokeColor(Color.RED).fillColor(Color.BLUE));
            mMap.moveCamera(CameraUpdateFactory.newLatLng(new LatLng(latitude, longitude)));


            String msg = "Latitude : "+ latitude + "\nLongitude:"+ longitude;

            Toast.makeText(mContext, msg, Toast.LENGTH_SHORT).show();
        }


        public void onProviderDisabled(String provider) {
        }

        public void onProviderEnabled(String provider) {
        }

        public void onStatusChanged(String provider, int status, Bundle extras) {
        }
    }


}

