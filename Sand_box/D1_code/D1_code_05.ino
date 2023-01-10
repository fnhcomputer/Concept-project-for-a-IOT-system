
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

// ThingSpeak ----------------start---
#include <ThingSpeak.h>
#define CHANNEL_ID 1526074
#define CHALLEL_API_KEY "''''''''''"
WiFiClient client;
// ThingSpeak ------------------End --



// display ---------------start--
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// display ----------------End--


  //Declar the veriable 
  int Temperature = 0; 
  int Humidity = 0;
  int WaterLevel = 0;
  int SoilTemp = 0;
  int SoilMoisture = 0;
  int light = 0;


// ledPin refers to D1 GPIO 2
int ledPin = 2;
  
  
ESP8266WebServer server;
char* ssid = "--------";
char* password = "--------";

void setup()
{
  WiFi.begin(ssid,password);
  Serial.begin(9600);
  while(WiFi.status()!=WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/",handleIndex);
  server.begin();

  ThingSpeak.begin(client);

// Display --------------strat---------------------------

 // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
// Display ---------------End----------------------------

  // initialize digital pin ledPin as an output.
  pinMode(ledPin, OUTPUT);
}

void loop()
{
  server.handleClient();
  
  delay(1000);
  handleIndex();
  
  delay(1000);
  LcdDispay();

  LEDlight();


 ThingSpeak.setField(1,Temperature);  // insert data 
 ThingSpeak.setField(2,Humidity);
 ThingSpeak.setField(3,WaterLevel);
 ThingSpeak.setField(4,SoilTemp);
 ThingSpeak.setField(5,SoilMoisture);
 ThingSpeak.setField(6,light);
 
 ThingSpeak.writeFields(CHANNEL_ID, CHALLEL_API_KEY);   // send data
 
 delay (1000);
  
}

void handleIndex(void)
{
  // Send a JSON-formatted request with key "type" and value "request"
  // then parse the JSON-formatted response with keys "gas" and "distance"
  DynamicJsonDocument doc(1024);
  
  // Sending the request
  doc["type"] = "request";
  serializeJson(doc,Serial);
  // Reading the response
  boolean messageReady = false;
  String message = "";
  while(messageReady == false) { // blocking but that's ok
    if(Serial.available()) {
      message = Serial.readString();
      messageReady = true;
    }
  }
  // Attempt to deserialize the JSON-formatted message
  DeserializationError error = deserializeJson(doc,message);
  if(error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  
  Temperature = doc["Temperature"];
  Humidity = doc["Humidity"];
  WaterLevel =  doc["Water Level"];
  SoilTemp = doc["Soil Temparature"];
  SoilMoisture = doc["Soil Moisture"];
  light = doc["Sun light"];
  
  // Prepare the data for serving it over HTTP
  String output = "Room Temperature: " + String(Temperature) + "\n";
  output += "Room Humidity: " + String(Humidity)+ "\n";
  output += "Water Level: " + String(WaterLevel)+ "\n";
  output += "Soil Temparature: " + String(SoilTemp)+ "\n";
  output += "Soil Moisture: " + String(SoilMoisture)+ "\n";
  output += "Sun light: " + String(light)+ "\n";
  
    // Serve the data as plain text, for example
  server.send(200,"text/plain",output);
}

void LcdDispay(void)
{
    // Clear the display
  display.clearDisplay();
  //Set the color - always use white despite actual display color
  display.setTextColor(WHITE);
  //Set the font size
  display.setTextSize(1);
  //Set the cursor coordinates

  display.setCursor(0,0);
  display.print("Room Temp.:  "); 
  display.print(Temperature);
  display.print(" C");

  display.setCursor(0,10); 
  display.print("Humidity:    "); 
  display.print(Humidity);
  display.print(" %"); 

  display.setCursor(0,20);
  display.print("Sun light:   "); 
  display.print(light);
  display.print("  ADC ");
  
  display.setCursor(0,26);
  display.print("--------------------"); 

  display.setCursor(0,33);
  display.print("Soil Mois.:  "); 
  display.print(SoilMoisture);
  display.print("  %");

  display.setCursor(0,43);
  display.print("Soil Temp.:  "); 
  display.print(SoilTemp);
  display.print(" C");

  display.setCursor(0,53);
  display.print("Water dis:   "); 
  display.print(WaterLevel);
  display.print(" cm");

  display.setCursor(0,60);
  display.print("--------------------"); 

  display.display();
}

void LEDlight(void) 
{
  digitalWrite(ledPin, LOW);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                  // wait for a second
  digitalWrite(ledPin, HIGH);    // turn the LED off by making the voltage LOW
  delay(1000);                  // wait for a second
}
