package com.example.daeng.quick_coding01;

import android.support.annotation.IntegerRes;
import android.widget.EditText;
import android.widget.TextView;

/**
 * Created by daeng on 2016-10-06.
 */

public class MyAverage extends MyValues {
    public void getResult(TextView v, EditText e){
        String s = e.getText().toString();
        String arr[] = s.split(" ");
        int arr1[] = new int[arr.length];
        int sum = 0;

        for(int i=0; i<arr.length; i++){
            arr1[i] = Integer.parseInt(arr[i]);
        }

        for(int i=0; i<arr1.length; i++){
            sum += arr1[i];
        }

        double res = sum / arr1.length;
        v.setText(Double.toString(res));
    }
}
