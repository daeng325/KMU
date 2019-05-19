package com.example.daeng.term2;

import android.os.Bundle;
import android.support.design.widget.TabLayout;
import android.support.v4.app.Fragment;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;


public class MainActivity extends AppCompatActivity {

    Toolbar toolbar;

    FragmentTask fragmentTask;
    FragmentPicture fragmentPicture;
    FragmentStatistics fragmentStatistics;

    //private GoogleMap map;

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        actionBar.setDisplayShowTitleEnabled(false);

        fragmentTask = new FragmentTask(this);
        fragmentPicture = new FragmentPicture(this, this);
        fragmentStatistics = new FragmentStatistics(this);

        getSupportFragmentManager().beginTransaction().replace(R.id.container, fragmentTask).commit();

        TabLayout tabs = (TabLayout) findViewById(R.id.tabs);
        tabs.addTab(tabs.newTab().setText("Task"));
        tabs.addTab(tabs.newTab().setText("Picture"));
        tabs.addTab(tabs.newTab().setText("Statistics"));//[탭] 버튼 추가

        tabs.setOnTabSelectedListener(new TabLayout.OnTabSelectedListener() { //탭이 선택될 때마다 그 리스너 안에 있는 onTabSelected()메소드가 호출됨
            @Override
            public void onTabSelected(TabLayout.Tab tab) { //현재 선택된 탭 객체가 전달
                int position = tab.getPosition();

                Fragment selected = null;
                if (position == 0) {
                    selected = fragmentTask;
                } else if (position == 1) {
                    selected = fragmentPicture;
                } else if (position == 2) {
                    selected = fragmentStatistics;
                }

                getSupportFragmentManager().beginTransaction().replace(R.id.container, selected).commit();
            }

            @Override
            public void onTabUnselected(TabLayout.Tab tab) { }

            @Override
            public void onTabReselected(TabLayout.Tab tab) { }
        });


    }
}
