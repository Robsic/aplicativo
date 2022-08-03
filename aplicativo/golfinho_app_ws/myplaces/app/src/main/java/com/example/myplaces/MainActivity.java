/*
Golfinho Teleop Android Node - Android Studio and ROS2 Foxy Integration
Author: José Gabriel de Jesus Flores - gabrielfloresbassanello@gmail.com
Github: https://github.com/jgabrielflores
Laboratory of Robotics, Intelligent and Complex Systems - RobSIC
Universidade Federal de Itajubá
Created: 2022/04/28
*/

package com.example.myplaces;

import android.annotation.SuppressLint;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import java.io.IOException;
import java.io.PrintWriter;
import java.net.Socket;

import io.github.controlwear.virtual.joystick.android.JoystickView;

public class MainActivity extends AppCompatActivity {

    Socket s;
    PrintWriter writer;
    TextView number;
    String mens, mensx, mensy;

    private TextView mTextViewAngleRight;
    private TextView mTextViewStrengthRight;
    private TextView mTextViewCoordinateRight;
    JoystickView joystickRight;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        number = findViewById(R.id.number);

        mTextViewAngleRight = findViewById(R.id.textView_angle_right);
        mTextViewStrengthRight = findViewById(R.id.textView_strength_right);
        mTextViewCoordinateRight = findViewById(R.id.textView_coordinate_right);

        joystickRight = findViewById(R.id.joystickView_right);

        OnBtnMove();
    }

    public void OnBtnMove(){
        joystickRight.setOnMoveListener(new JoystickView.OnMoveListener() {
            @SuppressLint("DefaultLocale")
            @Override
            public void onMove(int angle, int strength) {

                class BackGroundNormalizedCoordinates extends AsyncTask<String, Void, Void> {
                    Handler h = new Handler();
                    @Override
                    protected Void doInBackground(String... voids) {
                        try {

                            mensx = String.valueOf(((float)joystickRight.getNormalizedX()-50)/50);
                            mensy = String.valueOf(((float)joystickRight.getNormalizedY()-50)/50);
                            mens = mensx + ";" + mensy;

                            if(s == null){
                                //change it to your IP
                                s = new Socket("192.168.1.111",6000);
                                writer = new PrintWriter(s.getOutputStream());
                                Log.i("NormalizedXY", "CONNECTED");
                            }
                            writer.write(mens);
                            writer.flush();
                            //writer.close();
                            h.post(new Runnable() {
                                @Override
                                public void run() {
                                    number.setText(mens);
                                }
                            });
                        }
                        catch (IOException e) {
                            e.printStackTrace();
                        }
                        return null;
                    }
                }

                BackGroundNormalizedCoordinates b = new BackGroundNormalizedCoordinates();
                b.execute();

                mTextViewAngleRight.setText(angle + "°");
                mTextViewStrengthRight.setText(strength + "%");
                mTextViewCoordinateRight.setText(
                        String.format("x%.2f:y%.2f",
                                (((float)joystickRight.getNormalizedX()-50)/50)*-1,
                                ((float)joystickRight.getNormalizedY()-50)/50)
                );
            }
        });
    }

    }
