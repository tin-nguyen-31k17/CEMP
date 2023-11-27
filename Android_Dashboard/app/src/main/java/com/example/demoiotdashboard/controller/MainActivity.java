package com.example.demoiotdashboard.controller;

import android.annotation.SuppressLint;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.SwitchCompat;

import com.android.wateriotdashboard.R;
import com.example.demoiotdashboard.mqtt.MQTTHelper;
import com.jjoe64.graphview.GraphView;
import com.jjoe64.graphview.series.DataPoint;
import com.jjoe64.graphview.series.LineGraphSeries;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallbackExtended;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;

import java.nio.charset.StandardCharsets;
import java.time.Instant;
import java.time.LocalDateTime;
import java.time.ZoneOffset;
import java.util.Date;

public class MainActivity extends AppCompatActivity {
    MQTTHelper mqttHelper;
    TextView txtWaterTemp, txtEC, txtPH, txtORP;
    SwitchCompat buttonPUMP;
    GraphView waterTempGraph, ecGraph, phGraph, orpGraph;

    final LineGraphSeries<DataPoint> seriesWaterTemp = new LineGraphSeries<>();
    final LineGraphSeries<DataPoint> seriesEC = new LineGraphSeries<>();
    final LineGraphSeries<DataPoint> seriesPH = new LineGraphSeries<>();
    final LineGraphSeries<DataPoint> seriesORP = new LineGraphSeries<>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        txtWaterTemp = findViewById(R.id.txtWaterTemp);
        txtEC = findViewById(R.id.txtEC);
        txtPH = findViewById(R.id.txtPH);
        txtORP = findViewById(R.id.txtORP);

        buttonPUMP = findViewById(R.id.buttonPUMP);

        waterTempGraph = (GraphView) findViewById(R.id.graph1);
        ecGraph = (GraphView) findViewById(R.id.graph2);
        phGraph = (GraphView) findViewById(R.id.graph3);
        orpGraph = (GraphView) findViewById(R.id.graph4);

        // Set up the graphs as per your requirements

        buttonPUMP.setOnCheckedChangeListener(
                (compoundButton, b) -> {
                    if (b) {
                        sendDataMQTT("Fusioz/feeds/wms-feed.relay-485", "ON");
                    } else {
                        sendDataMQTT("Fusioz/feeds/wms-feed.relay-485", "OFF");
                    }
                }
        );
        startMQTT();
    }
    private void startMQTT() {
        mqttHelper = new MQTTHelper(getApplicationContext());
        mqttHelper.setCallback(new MqttCallbackExtended() {
            @Override
            public void connectComplete(boolean b, String s) {
                Log.w("Debug", s);
            }

            @Override
            public void connectionLost(Throwable throwable) {

            }

            @Override
            public void messageArrived(String topic, MqttMessage message) {
                Log.d("TEST", topic + "***" + message.toString());
                if (topic.contains("waterTemp")) {
                    double x = Double.parseDouble(message.toString());
                    LocalDateTime time = null;
                    if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O) {
                        time = LocalDateTime.now();
                    }
                    Date y = convertLocalDateTime(time);
                    seriesWaterTemp.appendData(new DataPoint(y, x), true, 15, true);
                    waterTempGraph.addSeries(seriesWaterTemp);
                    waterTempGraph.onDataChanged(true, true);
                    txtWaterTemp.setText("Water Temp: " + message + " °C");
                } else if (topic.contains("EC")) {
                    double x = Double.parseDouble(message.toString());
                    LocalDateTime time = null;
                    if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O) {
                        time = LocalDateTime.now();
                    }
                    Date y = convertLocalDateTime(time);
                    seriesEC.appendData(new DataPoint(y, x), true, 15, true);
                    ecGraph.addSeries(seriesEC);
                    ecGraph.onDataChanged(true, true);
                    txtEC.setText("EC: " + message + " µS/cm");
                } else if (topic.contains("PH")) {
                    double x = Double.parseDouble(message.toString());
                    LocalDateTime time = null;
                    if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O) {
                        time = LocalDateTime.now();
                    }
                    Date y = convertLocalDateTime(time);
                    seriesPH.appendData(new DataPoint(y, x), true, 15, true);
                    phGraph.addSeries(seriesPH);
                    phGraph.onDataChanged(true, true);
                    txtPH.setText("PH: " + message);
                } else if (topic.contains("ORP")) {
                    double x = Double.parseDouble(message.toString());
                    LocalDateTime time = null;
                    if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O) {
                        time = LocalDateTime.now();
                    }
                    Date y = convertLocalDateTime(time);
                    seriesORP.appendData(new DataPoint(y, x), true, 15, true);
                    orpGraph.addSeries(seriesORP);
                    orpGraph.onDataChanged(true, true);
                    txtORP.setText("ORP: " + message + " mV");
                }
            }
            @Override
            public void deliveryComplete(IMqttDeliveryToken token) {
            }
        });
    }

    private Date convertLocalDateTime(LocalDateTime time) {
        return Date.from(time.toInstant(ZoneOffset.UTC));
    }
    private void sendDataMQTT(String topic, String message) {
        try {
            mqttHelper.mqttAndroidClient.publish(topic, new MqttMessage(message.getBytes()));
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }
}