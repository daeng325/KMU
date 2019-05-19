package com.example.daeng.term2;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

/**
 * Created by daeng on 2016-11-28.
 */

public class AddTask extends Activity {
    private Button backButton;
    private EditText addedTask;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.add_task);

        this.backButton = (Button) findViewById(R.id.backButton);
        this.addedTask = (EditText) findViewById(R.id.addedTask);



        // 버튼을 눌렀을 때 메인 액티비티로 돌아갑니다.
        backButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {

                String str = addedTask.getText().toString();

                // 객체를 만듭니다.
                Intent resultIntent = new Intent();
                resultIntent.putExtra("task",str);

                // 응답을 전달하고 이 액티비티를 종료합니다.
                setResult(RESULT_OK, resultIntent);
                finish();
            }
        });

    }
}
