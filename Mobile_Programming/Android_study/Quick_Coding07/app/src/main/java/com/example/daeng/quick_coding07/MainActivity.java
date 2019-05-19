package com.example.daeng.quick_coding07;

import android.app.Activity;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Toast;

public class MainActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        View vw = new View(this);
        vw.setOnTouchListener(new TouchListenerClass());
        setContentView(vw);
    }

    class TouchListenerClass implements View.OnTouchListener{
        public boolean onTouch(View v, MotionEvent event){
            Toast.makeText(getApplicationContext(), "Touch Event Received",Toast.LENGTH_LONG).show();
            return true;
        }
    }
}
