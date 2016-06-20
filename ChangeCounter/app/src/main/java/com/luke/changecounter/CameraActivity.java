package com.luke.changecounter;

import android.app.Activity;
import android.content.Intent;
import android.graphics.PixelFormat;
import android.graphics.Rect;
import android.hardware.Camera;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;

import android.widget.FrameLayout;
import android.widget.ImageButton;
import android.widget.Toast;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;


public class CameraActivity extends Activity implements SensorEventListener {
    private Camera mCamera;
    private CameraPreview mPreview;
    Sensor accel;
    SensorManager sm;
    ImageButton captureButton, flashButton, retakePhoto, usePhoto;
    FrameLayout cameraControlFrame, photoControlFrame, cameraPreview;
    boolean flashOn = true;
    private static final int FOCUS_AREA_SIZE = 300;
    private File sdRoot;
    private String fileName, dir;
    String filePath;


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.camera_preview);

        captureButton = (ImageButton) findViewById(R.id.capture);
        flashButton = (ImageButton) findViewById(R.id.flashButton);
        retakePhoto = (ImageButton) findViewById(R.id.retakePhoto);
        usePhoto = (ImageButton) findViewById(R.id.usePhoto);

        cameraControlFrame = (FrameLayout) findViewById(R.id.cameraControlFrame);
        photoControlFrame = (FrameLayout) findViewById(R.id.photoActionFrame);
        cameraPreview = (FrameLayout)findViewById(R.id.camera_preview);

        sdRoot = Environment.getExternalStorageDirectory();
        dir = "/Android/data/" + getApplicationContext().getPackageName() + "/Files/";

        sm = (SensorManager) getSystemService(SENSOR_SERVICE);
        accel = sm.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        sm.registerListener(this, accel, SensorManager.SENSOR_DELAY_NORMAL);

        captureButton.setEnabled(false);
        cameraPreview.setOnTouchListener(
                new View.OnTouchListener(){
                    @Override
                    public boolean onTouch(View v, MotionEvent event){
                        if (event.getAction() == MotionEvent.ACTION_DOWN) {
                            focusOnTouch(event);
                        }
                        return true;
                    }
                }
        );
    }

    private void createCamera() {
        //Create instance of  a camera
        mCamera = getCameraInstance();
        // Setting the right parameters in the camera
        Camera.Parameters params = mCamera.getParameters();
        params.setFlashMode(params.FLASH_MODE_ON);
        params.setPictureFormat(PixelFormat.JPEG);
        params.setJpegQuality(100);
        Log.d("TAG", String.valueOf(params.getAutoExposureLock()));
        mCamera.setParameters(params);
        // Create our Preview view and set it as the content of our activity.
        mPreview = new CameraPreview(this, mCamera);
        FrameLayout preview = (FrameLayout) findViewById(R.id.camera_preview);
        preview.addView(mPreview, 0);
    }


    private Camera.PictureCallback mPicture = new Camera.PictureCallback() {
        @Override
        public void onPictureTaken(byte[] data, Camera camera) {
            cameraControlFrame.setVisibility(View.GONE);
            photoControlFrame.setVisibility(View.VISIBLE);

            // File name of the image that we just took.
            fileName = "IMG_" + new SimpleDateFormat("yyyyMMdd_HHmmss").format(new Date()).toString() + ".jpg";

            // Creating the directory where to save the image. Sadly in older
            // version of Android we can not get the Media catalog name
            File mkDir = new File(sdRoot, dir);
            mkDir.mkdirs();

            // Main file where to save the data that we recive from the camera
            File pictureFile = new File(sdRoot, dir + fileName);
            filePath = pictureFile.getAbsolutePath();
            try {
                FileOutputStream fos = new FileOutputStream(pictureFile);
                fos.write(data);
                fos.close();
            } catch (FileNotFoundException e) {
                Log.d("TAG", "File not found: " + e.getMessage());
            } catch (IOException e) {
                Log.d("TAG", "Error accessing file: " + e.getMessage());
            }
        }
    };

    /**
     * A safe way to get an instance of the Camera object.
     */
    public static Camera getCameraInstance() {
        Camera c = null;
        try {
            // attempt to get a Camera instance
            c = Camera.open();
        } catch (Exception e) {
            // Camera is not available (in use or does not exist)
        }

        // returns null if camera is unavailable
        return c;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        releaseCamera();
        // removing the inserted view - so when we come back to the app we
        // won't have the views on top of each other.
        FrameLayout preview = (FrameLayout) findViewById(R.id.camera_preview);
        preview.removeViewAt(0);
        Log.d("TAG", "Destory");
    }

    @Override
    public void onResume() {
        super.onResume();
        createCamera();
    }

    private void releaseCamera() {
        if (mCamera != null) {
            mCamera.release(); // release the camera for other applications
            mCamera = null;
        }
    }

    public void cameraButtonPressed(View view) {
        mCamera.takePicture(null, null, mPicture);
    }

    public void retakePictureButtonPressed(View view){
        // Deleting the image from the SD card/
        File discardedPhoto = new File(sdRoot, dir + fileName);
        discardedPhoto.delete();

        // Restart the camera preview.
        mCamera.startPreview();
        photoControlFrame.setVisibility(View.GONE);
        cameraControlFrame.setVisibility(View.VISIBLE);
    }

    public void flashButtonPressed(View view){
        Camera.Parameters parameters = mCamera.getParameters();
        if (flashOn) {
            flashButton.setImageResource(R.drawable.flash_off_button);
            flashOn = false;
            Toast.makeText(getApplicationContext(), "Flash OFF", Toast.LENGTH_LONG).show();

            parameters.setFlashMode(parameters.FLASH_MODE_OFF);
        } else {
            flashButton.setImageResource(R.drawable.flash_on_button);
            flashOn = true;
            Toast.makeText(getApplicationContext(), "Flash ON", Toast.LENGTH_LONG).show();
            parameters.setFlashMode(parameters.FLASH_MODE_ON);
        }
        mCamera.setParameters(parameters);

    }

    public void usePictureButtonPressed(View view){
        Intent intent = new Intent(getApplicationContext(),CoinDetection.class);
        intent.putExtra("filePath",filePath);
        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TASK);
        startActivity(intent);
    }

    private void focusOnTouch(MotionEvent event) {
        if (mCamera != null ) {

            Camera.Parameters parameters = mCamera.getParameters();
            if (parameters.getMaxNumMeteringAreas() > 0){
                Rect rect = calculateFocusArea(event.getX(), event.getY());

                parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_AUTO);
                List<Camera.Area> meteringAreas = new ArrayList<Camera.Area>();
                meteringAreas.add(new Camera.Area(rect, 800));
                parameters.setFocusAreas(meteringAreas);

                mCamera.setParameters(parameters);
                mCamera.autoFocus(mAutoFocusTakePictureCallback);
            }else {
                mCamera.autoFocus(mAutoFocusTakePictureCallback);
            }
        }
    }

    private Rect calculateFocusArea(float x, float y) {
        int left = clamp(Float.valueOf((x / mPreview.getWidth()) * 2000 - 1000).intValue(), FOCUS_AREA_SIZE);
        int top = clamp(Float.valueOf((y / mPreview.getHeight()) * 2000 - 1000).intValue(), FOCUS_AREA_SIZE);

        return new Rect(left, top, left + FOCUS_AREA_SIZE, top + FOCUS_AREA_SIZE);
    }

    private int clamp(int touchCoordinateInCameraReper, int focusAreaSize) {
        int result;
        if (Math.abs(touchCoordinateInCameraReper)+focusAreaSize/2>1000){
            if (touchCoordinateInCameraReper>0){
                result = 1000 - focusAreaSize/2;
            } else {
                result = -1000 + focusAreaSize/2;
            }
        } else{
            result = touchCoordinateInCameraReper - focusAreaSize/2;
        }
        return result;
    }

    private Camera.AutoFocusCallback mAutoFocusTakePictureCallback = new Camera.AutoFocusCallback() {
        @Override
        public void onAutoFocus(boolean success, Camera camera) {
            if (success) {

            } else {

            }
        }
    };

    @Override
    public void onSensorChanged(SensorEvent event) {
        float[] g = new float[3];
        g = event.values.clone();
        double norm_Of_g = Math.sqrt(g[0] * g[0] + g[1] * g[1] + g[2] * g[2]);

        g[2] = g[2] / (float) norm_Of_g;

        int inclination = (int) Math.round(Math.toDegrees(Math.acos(g[2])));

        if (inclination < 30 || inclination > 155) {
            // device is flat
            captureButton.setEnabled(true);
            captureButton.setImageResource(R.drawable.take_picture_yes_button);
        } else {
            captureButton.setEnabled(false);
            captureButton.setImageResource(R.drawable.take_picture_no_button);
        }

    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
    }


}
