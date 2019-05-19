package com.example.daeng.quick_coding05;

import android.renderscript.ScriptGroup;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;



public class MainActivity extends AppCompatActivity {

    List<Integer> list_int = new ArrayList<Integer>();
    List<String> list_str = new ArrayList<String>();
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        final Button input_button = (Button) findViewById(R.id.input); //activity가 생성되기 전에 메소드를 이용하면 실행오류가 남!! 오... 뷰를 못 찾아간대.
        final Button finish = (Button) findViewById(R.id.input_well);
        final TextView integer = (TextView) findViewById(R.id.textView3);
        final TextView string = (TextView) findViewById(R.id.textView5);
        final EditText input = (EditText) findViewById(R.id.editText);

        input_button.setOnClickListener(new View.OnClickListener(){
            public void onClick(View v){
                String input_str = input.getText().toString();

                if(isInteger(input_str)){
                    list_int.add(Integer.parseInt(input_str));
                }
                else{
                    list_str.add(input_str);
                }
                input.setText("");
            }
        });

        finish.setOnClickListener(new View.OnClickListener(){
            public void onClick(View v){
                integer.setText(list_int.toString());
                string.setText(list_str.toString());
            }
        });




    }

    public static boolean isInteger(String s) {          // int형(숫자)인지 판단
        try {
            Integer.parseInt(s);
        }
        catch(NullPointerException e1) {
            return false;
        }
        catch(NumberFormatException e2) {
            return false;
        }
        return true;
    }

}

