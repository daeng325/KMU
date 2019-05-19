package com.example.igx.problem1;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.net.Uri;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import static android.content.Intent.ACTION_SENDTO;

/**
 * 센서 값 어떻게 읽는지, 메시지 보낼 때 어떻게 보내는지 해결해야 함
 */

public class MainActivity extends AppCompatActivity /* implements Something1, Something2 */ {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button btn_getLocation = (Button) findViewById(R.id.btn_getLocation);
        Button btn_getSensors = (Button) findViewById(R.id.btn_getSensors);
        Button btn_sendMessage = (Button) findViewById(R.id.btn_sendMessage);

        final TextView text_selectedData = (TextView) findViewById(R.id.text_selectedData);
        final TextView text_selectedType = (TextView) findViewById(R.id.text_selectedType);
        final EditText edit_phoneNumber = (EditText) findViewById(R.id.edit_phoneNumber);


        btn_getLocation.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                text_selectedType.setText("Location");

                long minTime = 10;
                float minDistance = 600;

                GpsListener gpsListener = new GpsListener(text_selectedData);
                LocationManager locationManager = (LocationManager) getSystemService(Context.LOCATION_SERVICE);
                if (ActivityCompat.checkSelfPermission(getApplicationContext(), Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED
                        && ActivityCompat.checkSelfPermission(getApplicationContext(), Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
                    return;
                }
                locationManager.requestLocationUpdates(locationManager.GPS_PROVIDER, minTime, minDistance, gpsListener);
            }
        });

        btn_getSensors.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                text_selectedType.setText("Sensors");

            }
        });

        btn_sendMessage.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String phoneNumber = edit_phoneNumber.getText().toString();
                Intent intent = new Intent(ACTION_SENDTO, Uri.parse(phoneNumber));
            }
        });


    }


    public class GpsListener implements LocationListener {
        TextView view;

        public GpsListener(TextView v){
            view = v;
    }
        @Override
        public void onLocationChanged(Location location) {

            Toast.makeText(getApplicationContext(),"gpslistener",Toast.LENGTH_LONG).show();

            Double latitude = location.getLatitude();
            Double longitude = location.getLongitude();

            String msg = "latitude : "+ Double.toString(latitude) + "\nlongitude" + Double.toString(longitude);
            view.setText(msg);
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

    public class SensorActivity extends Activity implements SensorEventListener {
        private final SensorManager mSensorManager;
        private final Sensor mAccelerometer;
        private final Sensor mGyroscope;
        private final Sensor mTemperature;
        private final Sensor mProximity;

        public SensorActivity(Sensor mGyroscope) {
            mSensorManager = (SensorManager)getSystemService(SENSOR_SERVICE);
            mAccelerometer = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
            this.mGyroscope = mSensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE);
            mTemperature = mSensorManager.getDefaultSensor(Sensor.TYPE_AMBIENT_TEMPERATURE);
            mProximity = mSensorManager.getDefaultSensor(Sensor.TYPE_PROXIMITY);
        }

        protected void onResume() {
            super.onResume();
            mSensorManager.registerListener(this, mAccelerometer, SensorManager.SENSOR_DELAY_NORMAL);
            mSensorManager.registerListener(this, mGyroscope, SensorManager.SENSOR_DELAY_NORMAL);
            mSensorManager.registerListener(this, mTemperature, SensorManager.SENSOR_DELAY_NORMAL);
            mSensorManager.registerListener(this, mProximity, SensorManager.SENSOR_DELAY_NORMAL);
        }

        protected void onPause() {
            super.onPause();
            mSensorManager.unregisterListener(this);
        }

        public void onAccuracyChanged(Sensor sensor, int accuracy) {
        }

        public void onSensorChanged(SensorEvent event) {
        }
    }
}
