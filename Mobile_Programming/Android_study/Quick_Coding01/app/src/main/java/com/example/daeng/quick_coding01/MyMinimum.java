package com.example.daeng.quick_coding01;

import android.support.annotation.IntegerRes;
import android.widget.EditText;
import android.widget.TextView;

/**
 * Created by daeng on 2016-10-06.
 */

public class MyMinimum extends MyValues {
    public void getResult(TextView v, EditText e){
        String s= e.getText().toString();
        String arr[] = s.split(" ");
        int arr1[] = new int[arr.length];

        for(int i=0; i<arr.length; i++){
            arr1[i] = Integer.parseInt(arr[i]);
        }

        for(int i=0; i<arr1.length-1; i++){
            if(arr1[i]>arr1[i+1]){
                int temp = arr1[i];
                arr1[i] = arr1[i+1];
                arr1[i+1] = temp;
            }
        }

        int res = arr1[0];
        v.setText(Integer.toString(res));

    }
}
