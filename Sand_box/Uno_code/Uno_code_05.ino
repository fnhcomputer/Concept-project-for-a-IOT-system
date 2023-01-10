
#include <ArduinoJson.h>   // json library 
#include <dht.h>   //  DHT11 temparature and humidity sensor library
#include <OneWire.h>   // Soil temp sensor 
#include <DallasTemperature.h>  // Soil temp sensor 

// json library 
String message = "";
bool messageReady = false;
// json library


dht DHT;  //Environment Humidity and temparature sensor 
#define DHT11_PIN 9   // Environment Humidity and temparature sensor 


// Resgister 4.7k  must be conected with data and VCC conected 
#define ONE_WIRE_BUS 7 // soil temp sensor data digital pin 7
OneWire oneWire(ONE_WIRE_BUS);  // Setup a oneWire instance to communicate with any OneWire devices - soil temp sensor
DallasTemperature sensorsSoil(&oneWire); // Pass our oneWire reference to soil temp sensor  
int SoilTemp; // soil temp 


// sun light detection -----------
int light;  // light sensor output in serial 
// ------------------- setup --------------------


// water level sensor -----start------
int trigPin = 11;    // Trigger -  water sensor 
int echoPin = 12;    // Echo    -  water sensor 
long duration; 
float cm;
int WaterLevel;
// water level sensor--------End-----


// Soil-Moisture sensor -----Start---

const int AirValue = 590;   //you need to replace this value with Value_1
const int WaterValue = 310;  //you need to replace this value with Value_2
int soilMoistureValue = 0;
int soilmoisturepercent=0;
int SoilMoisture;   // soil moisture percentage 

// Soil-Moisture sensor -----End-----



void setup() {
  //Serial Port begin
  Serial.begin(9600);


 // water level sensor ----------start-------
  //Define inputs and outputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  // water level sensor ----------end-------


pinMode (8,OUTPUT);  // confirm the sun light

  sensorsSoil.begin();  // soil temp sensor start
}


void loop() {


// Environment Humidity and temparature sensor ------------start-----
   
    int chk = DHT.read11(DHT11_PIN);   
     
// Environment Humidity and temparature sensor------------end-------


// Water level sensor ---------start-------------------

// The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
 
  // Convert the time into a distance - water level sensor 
  cm = (duration/2) / 29.1;     // Divide by 29.1 or multiply by 0.0343

  if (cm > 2 && cm < 300 ){       // water level display; sensor 2-300 cm; 
    WaterLevel = cm;
  }else{
  WaterLevel = 0;
  }

// water level sensor ---------End---------------------


// soil temp -------------start----------
sensorsSoil.requestTemperatures(); // Send the command to get temperature readings 
 //Serial.print("Soil Temp : ");
 SoilTemp = sensorsSoil.getTempCByIndex(0)- 2 ;  // Error mesurement (-)
// soil temp -------------End-------


// SoilMoisture sensor ---------start-------------
soilMoistureValue = analogRead(A0);  //put Sensor insert into soil
soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
if(soilmoisturepercent >= 100)
{
  SoilMoisture = 100;
}
else if(soilmoisturepercent <=0)
{
  SoilMoisture = 0;
}
else if(soilmoisturepercent >0 && soilmoisturepercent < 100)
{
  SoilMoisture = soilmoisturepercent;
}
// SoilMoisture sensor -------------End---------

// sunlight -----------start----------
  light = analogRead(A3);
if (light < 250){                 // sensor output high in sun 
  digitalWrite(8, HIGH); 
}else{
  digitalWrite(8,LOW);
}
//Serial.print(light);  //output high when 250 and low  in sun  

// sunlight -----------End----------



  // Monitor serial communication with D1 mini 
  while(Serial.available()) {
    message = Serial.readString();
    messageReady = true;
  }
  // Only process message if there's one
  if(messageReady) {
    // The only messages we'll parse will be formatted in JSON
    DynamicJsonDocument doc(1024); // ArduinoJson version 6+
    // Attempt to deserialize the message
    DeserializationError error = deserializeJson(doc,message);
    if(error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      messageReady = false;
      return;
    }
    if(doc["type"] == "request") {
      doc["type"] = "response";
      // Get data from analog sensors
      
      doc["Temperature"] = DHT.temperature;    // Environment temparature 
      doc["Humidity"] = DHT.humidity;          // Environment Humidity 
      doc["Water Level"] = WaterLevel;         // Water level 
      doc["Soil Temparature"] = SoilTemp;      // Soil temparature 
      doc["Soil Moisture"] = SoilMoisture;      // Soil Moisture  
      doc["Sun light"] = light;                // sun light if 250 and high  

      serializeJson(doc,Serial);
    }
    messageReady = false;
  }

//delay(1000);
  
}
