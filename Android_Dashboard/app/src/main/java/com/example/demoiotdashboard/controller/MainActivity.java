package com.example.demoiotdashboard.controller;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.SwitchCompat;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.widget.CompoundButton;
import android.widget.ScrollView;
import android.widget.TextView;

import com.example.demoiotdashboard.R;

import com.example.demoiotdashboard.alert.Alerts;
import com.example.demoiotdashboard.mqtt.MQTTHelper;
import com.jjoe64.graphview.GraphView;
import com.jjoe64.graphview.series.DataPoint;
import com.jjoe64.graphview.series.LineGraphSeries;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallbackExtended;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;

import java.nio.charset.Charset;
import java.time.Instant;
import java.time.LocalDateTime;
import java.time.ZoneOffset;
import java.util.Date;


public class MainActivity extends AppCompatActivity {
    MQTTHelper mqttHelper;
    TextView txtAirTemp, txtAirHumidity, txtAI, txtSoilTemperature;
    SwitchCompat buttonPUMP;
    GraphView airHumidityGraph, airTemperatureGraph, soilTemperature;

    Alerts alertDialog;

    LineGraphSeries seriesAirTemperature = new LineGraphSeries<DataPoint>();
    LineGraphSeries seriesAirHumidity = new LineGraphSeries<DataPoint>();
    LineGraphSeries seriesSoilTemperature = new LineGraphSeries<DataPoint>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        txtAirTemp = findViewById(R.id.txtAirTemperature);
        txtAirHumidity = findViewById(R.id.txtAirHumidity);
        txtSoilTemperature = findViewById(R.id.txtSoilTemperature);

//        txtAI = findViewById(R.id.txtAI);
        buttonPUMP = findViewById(R.id.buttonPUMP);

        airHumidityGraph = (GraphView) findViewById(R.id.graph1);
        airHumidityGraph.getGridLabelRenderer().setHorizontalLabelsVisible(false);
        airHumidityGraph.getGridLabelRenderer().setGridColor(Color.WHITE);
        airHumidityGraph.getGridLabelRenderer().setVerticalLabelsColor(Color.WHITE);
        airHumidityGraph.getGridLabelRenderer().setHorizontalLabelsColor(Color.WHITE);
        airHumidityGraph.getGridLabelRenderer().setVerticalLabelsColor(Color.WHITE);
        airHumidityGraph.getGridLabelRenderer().setHorizontalLabelsColor(Color.WHITE);

        airTemperatureGraph = (GraphView) findViewById(R.id.graph2);
        airTemperatureGraph.getGridLabelRenderer().setHorizontalLabelsVisible(false);
        airTemperatureGraph.getGridLabelRenderer().setGridColor(Color.WHITE);
        airTemperatureGraph.getGridLabelRenderer().setVerticalLabelsColor(Color.WHITE);
        airTemperatureGraph.getGridLabelRenderer().setHorizontalLabelsColor(Color.WHITE);
        airTemperatureGraph.getGridLabelRenderer().setVerticalLabelsColor(Color.WHITE);
        airTemperatureGraph.getGridLabelRenderer().setHorizontalLabelsColor(Color.WHITE);

        soilTemperature = (GraphView) findViewById(R.id.graph3);
        soilTemperature.getGridLabelRenderer().setHorizontalLabelsVisible(false);
        soilTemperature.getGridLabelRenderer().setGridColor(Color.WHITE);
        soilTemperature.getGridLabelRenderer().setVerticalLabelsColor(Color.WHITE);
        soilTemperature.getGridLabelRenderer().setHorizontalLabelsColor(Color.WHITE);
        soilTemperature.getGridLabelRenderer().setVerticalLabelsColor(Color.WHITE);
        soilTemperature.getGridLabelRenderer().setHorizontalLabelsColor(Color.WHITE);

        buttonPUMP.setOnCheckedChangeListener(
                new CompoundButton.OnCheckedChangeListener() {
                    @Override
                    public void onCheckedChanged(CompoundButton compoundButton,
                                                 boolean b) {
                        if (b == true) {
                            sendDataMQTT("Fusioz/feeds/wms-feed.relay-485", "ON");
                        } else {
                            sendDataMQTT("Fusioz/feeds/wms-feed.relay-485", "OFF");
                        }
                    }
                });


        startMQTT();
    }

    public Date convertLocalDateTime (LocalDateTime y){
       Instant instant = y.toInstant(ZoneOffset.of("+07:00"));
       Date out = Date.from(instant);
        return out;
    }

    public void sendDataMQTT(String topic, String value){
        MqttMessage msg = new MqttMessage();
        msg.setId(1234);
        msg.setQos(0);
        msg.setRetained(false);

        byte[] b = value.getBytes(Charset.forName("UTF-8"));
        msg.setPayload(b);

        try {
            mqttHelper.mqttAndroidClient.publish(topic, msg);
        }catch (MqttException e){
        }
    }
    public void startMQTT(){
        mqttHelper = new MQTTHelper(this);
        mqttHelper.setCallback(new MqttCallbackExtended() {
            @Override
            public void connectComplete(boolean reconnect, String serverURI) {

            }

            @Override
            public void connectionLost(Throwable cause) {

            }

            @Override
            public void messageArrived(String topic, MqttMessage message) throws Exception {
                Log.d("TEST",topic+ "***" +message.toString());
                if (topic.contains("ph")){
                    double x = Double.parseDouble(message.toString());
                    LocalDateTime time = LocalDateTime.now();
                    Date y = convertLocalDateTime(time);
                    seriesAirHumidity.appendData(new DataPoint(y,x), true, 15, true);
                    airHumidityGraph.addSeries(seriesAirHumidity);
                    airHumidityGraph.onDataChanged(true, true);
                    txtAirHumidity.setText("pH Value: "  + message.toString() + " °Degree");
                }


                else if(topic.contains("turbidity")){

                    double x = Double.parseDouble(message.toString());
                    LocalDateTime time = LocalDateTime.now();
                    Date y = convertLocalDateTime(time);
                    seriesAirTemperature.appendData(new DataPoint(y,x), true, 6, true);
                    airTemperatureGraph.addSeries(seriesAirTemperature);
                    airTemperatureGraph.onDataChanged(true, true);
                    txtAirTemp.setText("Turbidity Value: " + message.toString() + " NTU" );
                }

                else if(topic.contains("ammonia")){

                    double x = Double.parseDouble(message.toString());
                    LocalDateTime time = LocalDateTime.now();
                    Date y = convertLocalDateTime(time);
                    seriesSoilTemperature.appendData(new DataPoint(y,x), true, 11, true);
                    soilTemperature.addSeries(seriesSoilTemperature);
                    soilTemperature.onDataChanged(true, true);
                    txtSoilTemperature.setText("Ammonia Value: " + message.toString() + " mg/L" );
                }

                else if(topic.contains("AI")){
                    txtAI.setText(message.toString());
                }

                else if(topic.contains("wms-feed.relay-485")){
                    if(message.toString().equals("ON")) {
                        buttonPUMP.setChecked(true);
                    }else{
                        buttonPUMP.setChecked(false);
                    }
                }
            }
            @Override
            public void deliveryComplete(IMqttDeliveryToken token) {
            }
        });
    }


}