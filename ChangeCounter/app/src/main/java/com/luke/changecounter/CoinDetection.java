package com.luke.changecounter;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;

import android.view.View;
import android.widget.ProgressBar;
import android.widget.TextView;


import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;


public class CoinDetection extends Activity {
    //get path to external storage (SD card)
    //File sdIconStorageDir = new File(iconsStoragePath);
    String pack;
    byte[] imgBytes;
    Float total;
    String filePath;
    private Socket socket;
    private static final int SERVERPORT = 27015;
    private static final String SERVER_IP = "192.168.0.5";
    private TextView totalCountTextView;
    private ProgressBar progressBarPP;


    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.picture_preview);
        totalCountTextView = (TextView) findViewById(R.id.TotalTextView);
        totalCountTextView.setText("Waiting for value");
        progressBarPP = (ProgressBar) findViewById(R.id.progressBarPP);
        pack = this.getPackageName();
        Bundle extras = getIntent().getExtras();
        if (extras != null) {
            filePath = extras.getString("filePath");
        }
        Bitmap d = BitmapFactory.decodeFile(filePath);
        ByteArrayOutputStream blob = new ByteArrayOutputStream();
        d.compress(Bitmap.CompressFormat.JPEG, 80, blob);
        imgBytes = blob.toByteArray();
        new Thread(new ClientThread()).start();

    }

    class ClientThread implements Runnable {
        @Override
        public void run() {
            try {
                InetAddress severAddress = InetAddress.getByName(SERVER_IP);
                socket = new Socket(severAddress, SERVERPORT);

                DataOutputStream dOut = new DataOutputStream(socket.getOutputStream());
                dOut.write(Integer.toString(imgBytes.length).getBytes());
                dOut.write(imgBytes);

                DataInputStream dIn = new DataInputStream(socket.getInputStream());
                total = dIn.readFloat();
                progressBarPP.setVisibility(View.GONE);
            } catch (UnknownHostException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }

        }
    }
}
