package com.example.sm.problem2;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.EditText;
import android.widget.ListView;
import java.util.ArrayList;


public class MainActivity extends AppCompatActivity implements View.OnClickListener {
    MyBaseAdapter adapter;
    ListView listview;
    ArrayList<Employee> emp_list;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // need something here
        emp_list = new ArrayList<Employee>(); //직원 리스트

        adapter = new MyBaseAdapter(this, emp_list);
        listview = (ListView) findViewById(R.id.listView1) ;
        listview.setAdapter(adapter);
        listview.setOnItemClickListener((AdapterView.OnItemClickListener)adapter);
    }
    @Override
    public void onClick(View v){
        EditText edit_name = (EditText) findViewById(R.id.edit_name);
        EditText edit_age = (EditText) findViewById(R.id.edit_age);
        EditText edit_salary = (EditText) findViewById(R.id.edit_salary);



        Employee employee;

        switch (v.getId()){
            case R.id.btn_inc:
                // need something here
                break;

            case R.id.btn_dec:
                // need something here
                break;

            case R.id.btn_store:
                // need something here
//                emp_list.add(new Employee("추가하는 액티비티를 만들어줘야돼..?",21,2000));
//                adapter = new MyBaseAdapter(this,emp_list);
//                listview.setAdapter(adapter);
                break;

            case R.id.btn_modify:
                // need something here
                break;

            case R.id.btn_delete:
                // need something here
                break;
        }
    }
}

interface Payment {
    void increase();
    void decrease();
}
