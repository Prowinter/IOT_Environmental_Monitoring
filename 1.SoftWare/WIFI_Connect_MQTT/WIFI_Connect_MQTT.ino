#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
WiFiManager wifiManager;

// MQTT Broker
const char *mqtt_broker = "*.*.*.*";
const char *topic = "testtopic/test";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;
unsigned long previousMillis = 0;
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  wifiManager.autoConnect();

  client.setServer(mqtt_broker, mqtt_port);
  while (!client.connected()) {
      String client_id = "esp8266-client-";
      client_id += String(WiFi.macAddress());
      Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
      if (client.connect(client_id.c_str())) {
          Serial.println("Public emqx mqtt broker connected");
      } else {
          Serial.print("failed with state ");
          Serial.print(client.state());
          delay(2000);
      }
  }
  
}

void loop() {
  unsigned long currentMillis = millis();
  // temperature and humidity data are publish every five second
  if (currentMillis - previousMillis >= 5000) {
      previousMillis = currentMillis;
      // json serialize
      DynamicJsonDocument data(256);
      data["soil_temp_int"]   = 1;       //土壤温度整数
      data["soil_temp_deci"]  = 2;       //土壤温度小数
      data["soil_hum_int"]    = 3;      //土壤湿度整数
      data["soil_hum_deci"]   = 4;      //土壤湿度小数
      data["air_temp_int"]    = 5;      //空气温度整数
      data["air_temp_deci"]   = 6;     //空气温度小数
      data["air_hum_int"]     = 7;     //空气湿度整数
      data["air_hum_deci"]    = 8;     //空气湿度小数
      data["ph_int"]          = 9;     //ph小数
      data["ph_deci"]          = 10;    //ph整数
      data["illu"]            = 11;    //光照强度
      data["worm"]            = 12;    //虫害监测
      
      // publish temperature and humidity
      char json_string[256];
      serializeJson(data, json_string);
      Serial.println(json_string);
      client.publish(topic, json_string, false);
  }
}
