package com.example.daeng.term2;

/**
 * Created by daeng on 2016-12-05.
 */

import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import android.support.v7.app.AppCompatActivity;
import android.util.DisplayMetrics;
import android.view.Display;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.Gallery;
import android.widget.ImageView;
import android.widget.Toast;

/**
 * 갤러리 위젯을 사용해 사진을 보여주는 방법을 알 수 있습니다.
 */
public class ShowPictureView extends AppCompatActivity{
    /**
     * 이미지 커서
     */
    private Cursor imageCursor;

    /**
     * 이미지 커서
     */
    private Cursor actualImageCursor;
    private int imageColumnIndex;
    private int actualImageColumnIndex;

    /**
     * 갤러리 객체
     */
    Gallery gallery;

    /**
     * 갯수
     */
    private int count;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        //checkDangerousPermissions();

        init();
    }

//    private void checkDangerousPermissions() {
//        int permissionCheck = ContextCompat.checkSelfPermission(this, Manifest.permission.READ_EXTERNAL_STORAGE);
//
//        if (permissionCheck == PackageManager.PERMISSION_GRANTED) {
//            Toast.makeText(this, "권한 있음", Toast.LENGTH_LONG).show();
//        } else {
//            Toast.makeText(this, "권한 없음", Toast.LENGTH_LONG).show();
//
//            if (ActivityCompat.shouldShowRequestPermissionRationale(this, android.Manifest.permission.READ_EXTERNAL_STORAGE)) {
//                Toast.makeText(this, "권한 설명 필요함.", Toast.LENGTH_LONG).show();
//            } else {
//                String[] permissions = {
//                        android.Manifest.permission.READ_EXTERNAL_STORAGE
//                };
//
//                ActivityCompat.requestPermissions(this, permissions, 1);
//            }
//        }
//    }
    //왜 안되는지..ㅇㅁㅇ...

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        if (requestCode == 1) {
            for (int i = 0; i < permissions.length; i++) {
                if (grantResults[i] == PackageManager.PERMISSION_GRANTED) {
                    Toast.makeText(this, permissions[i] + " 권한이 승인됨.", Toast.LENGTH_LONG).show();
                } else {
                    Toast.makeText(this, permissions[i] + " 권한이 승인되지 않음.", Toast.LENGTH_LONG).show();
                }
            }
        }
    }

    /**
     * 초기화
     */
    private void init() {
        // 갤러리에 대한 이미지 커서
        String[] img = { MediaStore.Images.Thumbnails._ID };
        imageCursor = managedQuery(MediaStore.Images.Thumbnails.EXTERNAL_CONTENT_URI, img, null,
                null, MediaStore.Images.Thumbnails.IMAGE_ID + "");

        imageColumnIndex = imageCursor.getColumnIndexOrThrow(MediaStore.Images.Thumbnails._ID);
        count = imageCursor.getCount();

        gallery = (Gallery) findViewById(R.id.gallery);
        gallery.setAdapter(new ImageAdapter(getApplicationContext()));
        gallery.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            public void onItemClick(AdapterView parent, View v, int position, long id) {
                String[] proj = {MediaStore.Images.Media.DATA};
                actualImageCursor = managedQuery(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, proj, null, null, null);
                actualImageColumnIndex = actualImageCursor.getColumnIndexOrThrow(MediaStore.Images.Media.DATA);
                actualImageCursor.moveToPosition(position);
                String filename = actualImageCursor.getString(actualImageColumnIndex);

                Intent intent = new Intent(getApplicationContext(), OriginalPictureView.class);
                intent.putExtra("filename", filename);

                startActivity(intent);
            }
        });
    }


    public class ImageAdapter extends BaseAdapter {
        private Context mContext;
        private Gallery.LayoutParams params;

        public ImageAdapter(Context c) {
            mContext = c;

            WindowManager wm = (WindowManager) getSystemService(Context.WINDOW_SERVICE);
            DisplayMetrics metrics = new DisplayMetrics();
            Display display = wm.getDefaultDisplay();
            display.getMetrics(metrics);

            params = new Gallery.LayoutParams(
                    metrics.widthPixels/3,
                    Gallery.LayoutParams.MATCH_PARENT);
        }

        public int getCount() {
            return count;
        }

        public Object getItem(int position) {
            return position;
        }

        public long getItemId(int position) {
            return position;
        }

        public View getView(int position, View convertView, ViewGroup parent) {
            ImageView iv = null;
            if (convertView == null) {
                iv = new ImageView(mContext);
            } else {
                iv = (ImageView) convertView;
            }

            imageCursor.moveToPosition(position);
            int id = imageCursor.getInt(imageColumnIndex);

            iv.setImageURI(Uri.withAppendedPath(MediaStore.Images.Thumbnails.EXTERNAL_CONTENT_URI, String.valueOf(id)));
            iv.setScaleType(ImageView.ScaleType.FIT_CENTER);
            iv.setLayoutParams(params);

            return iv;
        }
    }

}
