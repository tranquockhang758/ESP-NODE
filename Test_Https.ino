//D0  GPIO16
//D1  GPIO5
//D2  GPIO4
//D3,D15  GPIO0
//D4,D14  GPIO2
//D5,D13  GPIO14
//D6,D12  GPIO12
//D7,D11  GPIO13
//D8  GPIO15
//D9  GPIO3
//D10 GPIO1
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Arduino.h>
#include <ESP8266WiFiMulti.h>

#include <WiFiClient.h>
#include <Arduino_JSON.h>
#include <SoftwareSerial.h>
#include <ModbusMaster.h>

#include <WiFiClientSecureBearSSL.h>
ESP8266WiFiMulti WiFiMulti;


//======================================Connecting to Server
const char* ssid = "C201";
const char* password ="C201nlttao";
//String url = "https://plc-spkt.herokuapp.com/api/ViewDataArduino";
String json_string;
int Status_Code = 0;
int Freqyency = 0;
bool Stop;
bool Reverse;
bool Forward;
int Setpoint=0;
int Feedback = 0;
//======================================Connecting to Server

//======================================Modbus RTU
ModbusMaster node;
#define TSEN 13 // Khai Báo Chân Điều Hướng D3 GPIO0
#define IDSLAVE1 1  // Khai Báo ID Giao Tiếp Của Biến Tần iG5A (I60=1)
void preTransmission(); //=> Cho phép truyền nhận
void postTransmission();
//======================================Modbus RTU
void setup() {

    Serial.begin(19200,SERIAL_8N1);
  //Đèn báo truyền nhận dữ liệu
  //Chân điều hướng D3
    pinMode(TSEN,OUTPUT);
    digitalWrite(TSEN, 0);
  //
  //  pinMode(12,OUTPUT);
  //  digitalWrite(12,LOW);
      pinMode(4,OUTPUT);//D2
      digitalWrite(4,HIGH);
      pinMode(0,OUTPUT);//D3
      digitalWrite(0,HIGH);
      pinMode(2,OUTPUT);//D4
      digitalWrite(2,HIGH);
      pinMode(14,OUTPUT);//D5
      digitalWrite(14,HIGH);
      Serial.println();
      Serial.println();
      Serial.println();
      //=================================Config Truyền thông
      node.begin(1, Serial);
      node.preTransmission(preTransmission);
      node.postTransmission(postTransmission);
      //=================================Config Truyền thông
    
      //=================================Config Wifi 
      WiFi.begin(ssid, password);
      Serial.println("");
      Serial.print("Connecting");
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("");
      Serial.print("Connected to ");
      Serial.println(ssid);
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      //Biến ESP thành Device như điện thoại
      WiFi.mode(WIFI_STA);
      WiFiMulti.addAP("C201", "C201nlttao");
}

void loop() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    //=====================================================================Set thuộc tính cho SSL
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    client->setInsecure();
    //=====================================================================Set thuộc tính cho SSL
    
    HTTPClient https;
    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, "https://plc-spkt.herokuapp.com/api/ViewDataArduino?id=1&Feedback=100")) {  // HTTPS
      Serial.print("[HTTPS] GET...\n");
      int httpCode = https.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          JSONVar my_obj = JSON.parse(payload);
            if (JSON.typeof(my_obj) == "undefined") {
            Serial.println("Parsing input failed!");
            return;
            }
            Serial.println(payload);
            //{"Feedback":"100", "data":{"id":1,"Stop":true,"Reverse":false,"Forward":false,"Setpoint":1199,"Feedback":100,"Current":0,"Voltage":0,"Frequency":0},
            //"message":"Data Fetch in DB","errCode":0}
                   Stop = my_obj["data"]["Stop"];
                   Reverse = my_obj["data"]["Reverse"];
                   Forward = my_obj["data"]["Forward"];
                   Setpoint =my_obj["data"]["Setpoint"];
                   Feedback = my_obj["data"]["Feedback"];
                  int Current = my_obj["data"]["Current"];
                  int Voltage = my_obj["data"]["Voltage"];
                  int Frequency = my_obj["data"]["Frequency"];
                  
                  if (Stop == true) { 
                    //D2 = STOP = K1 = I4 
                    digitalWrite(4,LOW);
                    digitalWrite(0,HIGH);//Reverse
                    digitalWrite(2,HIGH);//Forward
//                    Status_Code = 1;
//                    node.writeSingleRegister(4, 0);
//                    node.writeSingleRegister(5, 1);
//                      digitalWrite(4,HIGH);
//                      digitalWrite(0,HIGH);
//                      digitalWrite(2,HIGH);
//                      digitalWrite(14,HIGH);
                  }
                  if (Reverse == true) {
//                    Status_Code = 4;
//                    node.writeSingleRegister(4, Setpoint);
//                    node.writeSingleRegister(5, 4);
                      //D3 = REVERSE = K2 = I5 
                    digitalWrite(0,LOW);
                    digitalWrite(4,HIGH);
                    digitalWrite(2,HIGH);
                  }
                  if (Forward == true) {
//                    Status_Code = 2;
                    //4 Frequency , 5 Command Code
//                    node.writeSingleRegister(4, Setpoint);
//                    node.writeSingleRegister(5, 2);
//                      digitalWrite(2,LOW);
//                      digitalWrite(14,LOW);
                      //D4 = Forward = K3 = I6
                      digitalWrite(2,LOW);
                      digitalWrite(4,HIGH);
                      digitalWrite(0,HIGH);
                  }
        }
        //Nếu server trả về lỗi
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }
      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }
  Serial.println("Wait 1s before next round...");
  delay(200);
}
void preTransmission()
{
  digitalWrite(TSEN, 1);
}

void postTransmission()
{
  digitalWrite(TSEN, 0);
}
