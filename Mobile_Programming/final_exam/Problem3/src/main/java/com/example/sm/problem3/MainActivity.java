package com.example.sm.problem3;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.ListView;

import java.util.ArrayList;


public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ArrayList<CustomerThread> list = new ArrayList<CustomerThread>();
        Manager manager = new Manager();

        for(int i = 0 ; i < 10 ; i++){
            Customer customer = new Customer("Customer" + i);
            CustomerThread ct = new CustomerThread(customer);
            list.add(ct);
            manager.add_customer(customer);
            ct.start();
        }


        for(CustomerThread ct : list){

            try {
                // need something here
                Thread.sleep((int)Math.random()*1000);
            } catch (InterruptedException e) { }
        }

        manager.sort();

        MyBaseAdapter adapter = new MyBaseAdapter(this, manager.list);
        ListView listview = (ListView) findViewById(R.id.listView1) ;
        listview.setAdapter(adapter);


    }
}

class CustomerThread extends Thread{

    Customer customer;

    CustomerThread(Customer customer){
        this.customer = customer;
    }
    // need something here
    public void run(){
        customer.work();
    }
}

abstract class Person{

    static int money = 100000;
    int spent_money = 0;
    abstract void work();

}


class Customer extends Person{

    String name;
    Customer(String name){
        this.name = name;
    }

    @Override
    void work() {
        spent_money = (int) Math.random()*1000;

    }
}


class Manager extends Person{
    ArrayList <Customer> list = new ArrayList<Customer>();

    void add_customer(Customer customer) {
        list.add(customer);
    }

    void sort(){ // 직접 소팅 알고리즘을 이용하여 코딩해야함. 자바 기본 정렬 메소드 이용시 감

        // need something here
        ArrayList<Integer> money= new ArrayList<Integer>();
        for(int i=0; i<10; i++){
            money.add(list.get(i).spent_money);
        }

        for(int i=0; i<9; i++) {
            if (money.indexOf(i) > money.indexOf(i + 1)) {
                int temp = money.indexOf(i);
//                money.indexOf(i+1) = money.indexOf(i);
//                money.indexOf(i) = temp;
            }
        }

    }

    @Override
    void work() {
        sort();
    }
}

// need something here

