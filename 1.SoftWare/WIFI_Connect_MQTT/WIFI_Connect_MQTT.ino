#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
WiFiManager wifiManager;

typedef struct{
  unsigned char soiltemp_int;   //土壤温度的整数部分
  unsigned char soiltemp_deci;  //土壤温度的小数部分
  unsigned char soilhumi_int;   //土壤湿度的整数部分
  unsigned char soilhumi_deci;  //土壤湿度的小数部分
  unsigned char ph_int;         //酸碱度的整数部分
  unsigned char ph_deci;        //酸碱度的小数部分
  unsigned char air_humi_int;   //空气湿度的整数部分
  unsigned char air_humi_deci;  //空气湿度的小数部分
  unsigned char air_temp_int;   //空气温度的整数部分
  unsigned char air_temp_deci;  //空气温度的小数部分
  unsigned char Illumination;           //光照强度
  unsigned char Worm;           //虫害数据
  unsigned char air_check_sum;  //校验和
}Monitor_Data_TypeDef;
unsigned char RxState=0;

uint8_t RxBuffer[50]={0};
Monitor_Data_TypeDef monitor;

// MQTT Broker
const char *mqtt_broker = "*.*.*.*";
const char *topic = "testtopic/test";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;
unsigned long previousMillis = 0;
WiFiClient espClient;
PubSubClient client(espClient);


//匿名传输协议解析
void Data_Receive_Prepare(uint8_t data)
{
  static uint8_t _data_len = 0,_data_cnt = 0;

    if(RxState==0)           //帧头
    {
      if(data==0xAA)
      {
        RxState=1;
        RxBuffer[0]=data;
      }else RxState==0;
    }
    else if(RxState==1)     //目标地址
    {
      if(data==0x05)
      {
        RxState=2;
        RxBuffer[1]=data;
      }else RxState==0;
    }
    else if(RxState==2)                 //功能码
    {
      RxState=3;
      RxBuffer[2]=data;
    }
    else if(RxState==3&&data<50)        //数据长度
    {
      RxState = 4;
      RxBuffer[3]=data;
      _data_len = data;
      _data_cnt = 0;
    }
    else if(RxState==4&&_data_len>0)    //数据内容
    {
      _data_len--;
      RxBuffer[4+_data_cnt++]=data;     //数据长度
      if(_data_len==0) RxState = 5;
    }
    else if(RxState==5)                 //和检验
    {
      RxState = 6;
      RxBuffer[4+_data_cnt]=data;  //校验和
    }else if(RxState==6)                 //附加检验
    {
      RxBuffer[5+_data_cnt]=data;  //校验和
//      if(AND_SUM_Check(RxBuffer)) Serial.println("AND_Check OK!!!");
//      else Serial.println("AND_Check ERROR!!!");
      if(AND_SUM_Check(RxBuffer)) RxState=7;
      else RxState=0;
    }
}

//匿名协议检验和
unsigned char AND_SUM_Check(uint8_t *data_buf)
{
    unsigned char sumcheck=0;
    unsigned char addcheck=0;
    for(unsigned char i=0; i < ((*(data_buf+3))+4); i++)
    {
        sumcheck += (*(data_buf+i));    //从帧头开始，对每一字节进行求和，直到DATA区结束
        addcheck += sumcheck;           //每一字节的求和操作，进行一次sumcheck的累加
    }
    if(sumcheck == (*(data_buf+((*(data_buf+3))+4))) && addcheck == (*(data_buf+((*(data_buf+3))+5))))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
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
//  unsigned long currentMillis = millis();
  // temperature and humidity data are publish every five second
    while (Serial.available() > 0)  
    {
        unsigned char temp;
        temp = Serial.read();
        Data_Receive_Prepare(temp);  
    }
    if(RxState==7)
    {
      RxState = 0;
      DynamicJsonDocument data(256);
      data["soil_temp_int"]   = RxBuffer[4];       //土壤温度整数
      data["soil_temp_deci"]  = RxBuffer[5];       //土壤温度小数
      data["soil_hum_int"]    = RxBuffer[6];      //土壤湿度整数
      data["soil_hum_deci"]   = RxBuffer[7];      //土壤湿度小数
      data["ph_int"]          = RxBuffer[8];     //ph小数
      data["ph_deci"]         = RxBuffer[9];    //ph整数
      data["air_hum_int"]     = RxBuffer[10];     //空气湿度整数
      data["air_hum_deci"]    = RxBuffer[11];     //空气湿度小数
      data["air_temp_int"]    = RxBuffer[12];      //空气温度整数
      data["air_temp_deci"]   = RxBuffer[13];     //空气温度小数
      data["illu"]            = RxBuffer[14];    //光照强度
      data["worm"]            = RxBuffer[15];    //虫害监测
      // publish temperature and humidity
      char json_string[256];
      serializeJson(data, json_string);
      Serial.println(json_string);
      client.publish(topic, json_string, false);
    }
//  if (currentMillis - previousMillis >= 5000) {
//      previousMillis = currentMillis;
//      // json serialize
//      DynamicJsonDocument data(256);
//      data["soil_temp_int"]   = 1;       //土壤温度整数
//      data["soil_temp_deci"]  = 2;       //土壤温度小数
//      data["soil_hum_int"]    = 3;      //土壤湿度整数
//      data["soil_hum_deci"]   = 4;      //土壤湿度小数
//      data["air_temp_int"]    = 5;      //空气温度整数
//      data["air_temp_deci"]   = 6;     //空气温度小数
//      data["air_hum_int"]     = 7;     //空气湿度整数
//      data["air_hum_deci"]    = 8;     //空气湿度小数
//      data["ph_int"]          = 9;     //ph小数
//      data["ph_deci"]          = 10;    //ph整数
//      data["illu"]            = 11;    //光照强度
//      data["worm"]            = 12;    //虫害监测
//      
//      // publish temperature and humidity
//      char json_string[256];
//      serializeJson(data, json_string);
//      Serial.println(json_string);
//      client.publish(topic, json_string, false);
//  }
}
