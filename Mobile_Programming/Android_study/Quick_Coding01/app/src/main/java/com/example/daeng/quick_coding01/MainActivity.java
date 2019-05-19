package com.example.daeng.quick_coding01;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import org.w3c.dom.Text;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button btn = (Button) findViewById(R.id.button);
        Button btn2 = (Button) findViewById(R.id.button2);
        final TextView result = (TextView) findViewById(R.id.textView2);
        final EditText input = (EditText) findViewById(R.id.editText);

        btn.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v){
                MyMinimum min = new MyMinimum();

                min.getResult(result,input);
            }
        });

        btn2.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v){
                MyAverage avg = new MyAverage();

                avg.getResult(result, input);
            }
        });
    }
}
