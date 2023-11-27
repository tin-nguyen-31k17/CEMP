package com.example.demoiotdashboard.controller;
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
import java.time.LocalDateTime;
import java.time.ZoneOffset;
import java.util.Date;
import org.json.JSONObject;
import org.json.JSONArray;

public class MainActivity extends AppCompatActivity {
    MQTTHelper mqttHelper;
    TextView txtWaterTemp, txtEC, txtPH, txtORP, txtLon, txtLat;
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
        txtLon = findViewById(R.id.txtLon);
        txtLat = findViewById(R.id.txtLat);
        txtWaterTemp = findViewById(R.id.txtWaterTemp);
        txtEC = findViewById(R.id.txtEC);
        txtPH = findViewById(R.id.txtPH);
        txtORP = findViewById(R.id.txtORP);
        buttonPUMP = findViewById(R.id.buttonPUMP);
        waterTempGraph = (GraphView) findViewById(R.id.graph1);
        ecGraph = (GraphView) findViewById(R.id.graph2);
        phGraph = (GraphView) findViewById(R.id.graph3);
        orpGraph = (GraphView) findViewById(R.id.graph4);
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
                // Implement your reconnect strategy here
                Log.w("Debug", "Connection lost, reconnecting...");
                mqttHelper.connect();
            }
            @Override
            public void messageArrived(String topic, MqttMessage message) throws Exception {
                Log.d("TEST", topic + "***" + message.toString());
                JSONObject jsonMessage = new JSONObject(message.toString());
                String Lon = jsonMessage.getString("gps_longitude");
                String Lat = jsonMessage.getString("gps_latitude");
                txtLon.setText("Longitude: " + Lon);
                txtLat.setText("Latitude: " + Lat);
                JSONArray sensors = jsonMessage.getJSONArray("sensors");
                for (int i = 0; i < sensors.length(); i++) {
                    JSONObject sensor = sensors.getJSONObject(i);
                    String sensor_id = sensor.getString("sensor_id");
                    String sensor_value = sensor.getString("sensor_value");
                    if (sensor_id.contains("TEMP_0001")) {
                        updateGraphAndText(seriesWaterTemp, waterTempGraph, txtWaterTemp, sensor_value, "Water Temp: ", " °C");
                    } else if (sensor_id.contains("ec_0001")) {
                        updateGraphAndText(seriesEC, ecGraph, txtEC, sensor_value, "EC: ", " µS/cm");
                    } else if (sensor_id.contains("ph_0001")) {
                        updateGraphAndText(seriesPH, phGraph, txtPH, sensor_value, "PH: ", "");
                    } else if (sensor_id.contains("ORP_0001")) {
                        updateGraphAndText(seriesORP, orpGraph, txtORP, sensor_value, "ORP: ", " mV");
                    }
                }
            }
            private void updateGraphAndText(LineGraphSeries<DataPoint> series, GraphView graph, TextView textView, String value, String textPrefix, String textSuffix) {
                double x = Double.parseDouble(value);
                LocalDateTime time;
                time = LocalDateTime.now();
                Date y = convertLocalDateTime(time);
                series.appendData(new DataPoint(y, x), true, 15, true);
                graph.addSeries(series);
                graph.onDataChanged(true, true);
                textView.setText(textPrefix + value + textSuffix);
            }
            @Override
            public void deliveryComplete(IMqttDeliveryToken token) {
                Log.d("Debug", "Delivery complete");
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