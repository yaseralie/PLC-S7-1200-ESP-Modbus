/*
  By Yaser Ali Husen
  This program to control PLC Siemens S7-1200 from ESP8266 using modbus TCP communication
  PLC S7 as modbus TCP server
  ESP8266 as modbus Client
  - For connection, dont forget to create bridge between Wifi and ethernet
  - Set PLC IP address same with network
  GPIO Reference: https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/
  Library: https://github.com/emelianov/modbus-esp8266
*/

#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <ModbusIP_ESP8266.h>

//-----------For modbus------------
IPAddress remote(192, 168, 1, 201);  // Address of Modbus Slave device
ModbusIP mb;                         //ModbusIP object
const int REG = 100;                 // Modbus Coils Offset
//-----------For modbus------------

//for DHT sensor--------------------------------------
#include <DHT.h>
#define DHTPIN 2  //GPIO2 atau D4
// Uncomment the type of sensor in use:
//#define DHTTYPE    DHT11     // DHT 11
//#define DHTTYPE    DHT11     // DHT 22 (AM2302)
#define DHTTYPE DHT21  // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE);

// current temperature & humidity, updated in loop()
float t = 0.0;
float h = 0.0;
//for DHT sensor--------------------------------------

//Used Pins
int button1 = 5;  //D1
int button2 = 4;  //D2
int button3 = 0;  //D4
bool status_button1 = false;
bool status_button2 = false;
bool status_button3 = false;

//millis================================
//Set every 5 sec read DHT
unsigned long previousMillis = 0;  // variable to store the last time the task was run
const long interval = 5000;        // time interval in milliseconds (eg 1000ms = 1 second)
//======================================

void setup() {
  //For DHT sensor
  dht.begin();

  Serial.begin(9600);
  WiFi.begin("Yaser", "Yaser1983");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  mb.client();

  //Setup Pin
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);

}

void loop() {
  delay(50);
  unsigned long currentMillis = millis();  // mendapatkan waktu sekarang
  // Checks whether it is time to run the task
  if (currentMillis - previousMillis >= interval) {
    // Save the last time the task was run
    previousMillis = currentMillis;
    //read DHT-11---------------------------------------
    t = dht.readTemperature();
    h = dht.readHumidity();
    Serial.print("Humidity = ");
    Serial.print(h);
    Serial.print("% ");
    Serial.print("Temperature = ");
    Serial.print(t);
    Serial.println(" C ");
    //read DHT-11---------------------------------------
    //split decimal and integer value
    //temperature
    int t_int = int(t);
    int t_dec = int((t - t_int) * 100);
    //humidity
    int h_int = int(h);
    int h_dec = int((h - h_int) * 100);

    //write value temp and humidity to holding registers
    //register 0 and 1 for temperature
    mb.writeHreg(remote,0,t_int);
    delay(50);
    mb.writeHreg(remote,1,t_dec);
    delay(50);
     //register 2 and 3 for humidity
    mb.writeHreg(remote,2,h_int);
    delay(50);
    mb.writeHreg(remote,3,h_dec);   
  }

  if (!mb.isConnected(remote)) {  // Check if connection to Modbus Slave is established
    mb.connect(remote);           // Try to connect if no connection
    Serial.print(".");
  }
  mb.task();  // Common local Modbus task
  check_button();
}

void check_button() {
  //check button1=================================
  int buttonValue1 = digitalRead(button1);
  if (buttonValue1 == LOW) {
    if (status_button1 == false) {
      //turn on
      status_button1 = true;
      Serial.println("Turn ON Coil1");
      mb.writeCoil(remote, 0, true);
    }
  } else if (buttonValue1 == HIGH) {
    if (status_button1 == true) {
      //turn off
      status_button1 = false;
      Serial.println("Turn OFF Coil1");
      mb.writeCoil(remote, 0, false);
    }
  }
  //============================================

  //check button2=================================
  int buttonValue2 = digitalRead(button2);
  if (buttonValue2 == LOW) {
    if (status_button2 == false) {
      //turn on
      status_button2 = true;
      Serial.println("Turn ON Coil2");
      mb.writeCoil(remote, 1, true);
    }
  } else if (buttonValue2 == HIGH) {
    if (status_button2 == true) {
      //turn off
      status_button2 = false;
      Serial.println("Turn OFF Coil2");
      mb.writeCoil(remote, 1, false);
    }
  }
  //============================================

  //check button3=================================
  int buttonValue3 = digitalRead(button3);
  if (buttonValue3 == LOW) {
    if (status_button3 == false) {
      //turn on
      status_button3 = true;
      Serial.println("Turn ON Coil3");
      mb.writeCoil(remote, 2, true);
    }
  } else if (buttonValue3 == HIGH) {
    if (status_button3 == true) {
      //turn off
      status_button3 = false;
      Serial.println("Turn OFF Coil3");
      mb.writeCoil(remote, 2, false);
    }
  }
  //============================================
}
