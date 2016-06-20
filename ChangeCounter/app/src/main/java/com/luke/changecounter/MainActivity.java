package com.luke.changecounter;

import android.Manifest;
import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.view.View;
import android.widget.TextView;


public class MainActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.mainmenu);
        ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.CAMERA}, 0);
        ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, 1);
        ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.READ_EXTERNAL_STORAGE}, 2);
        ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.INTERNET}, 3);
        ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.ACCESS_NETWORK_STATE}, 4);
    }

    public void takePhoto(View view) {
        Intent i = new Intent(getApplicationContext(), CameraActivity.class);
        startActivity(i);
    }

    @Override
    protected void onResume() {
        super.onResume();

    }
}
