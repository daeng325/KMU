package com.example.daeng.quick_coding2;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

////github
public class MainActivity extends AppCompatActivity {

    int start;
    int last;
    int m;
    int count = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        final Button send = (Button) findViewById(R.id.button);
        final Button bigger = (Button) findViewById(R.id.button2);
        final Button smaller = (Button) findViewById(R.id.button3);
        final Button bingo = (Button) findViewById(R.id.button4);
        final TextView mid = (TextView) findViewById(R.id.textView3);
        final TextView cnt = (TextView) findViewById(R.id.textView4);


        send.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                start = 1;
                last = 500;
                m = (start + last) / 2;
                mid.setText(Integer.toString(m));
            }
        });

        bigger.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                start = m + 1;
                m = (start + last )/2;
                mid.setText(Integer.toString(m));
                count++;
            }
        });

        smaller.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                last = m - 1;
                m = (start + last )/2;
                mid.setText(Integer.toString(m));
                count++;
            }
        });

        bingo.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                cnt.setText(Integer.toString(count));
            }
        });
    }
}

