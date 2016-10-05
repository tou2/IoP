
//loading libraries
#include <SPI.h>
#include <Ethernet2.h>
#include "DHT.h"

// Local Network Settings
byte mac[] = { 0xD4, 0x28, 0xB2, 0xFF, 0xA0, 0xA1 }; // Must be unique on local network

// ThingSpeak Settings
char thingSpeakAddress[] = "api.thingspeak.com";
String writeAPIKey = "R09Z4A6OS4VLWNX5";
const int updateThingSpeakInterval = 16 * 1000;      // Time interval in milliseconds to update ThingSpeak (number of seconds * 1000 = interval)

// Variable Setup
long lastConnectionTime = 0; 
boolean lastConnected = false;
int failedCounter = 0;

// Initialize Arduino Ethernet Client
EthernetClient client;

//Defining arduino pins
#define DHTPIN 2     // what digital pin we're connected to
#define DHTTYPE DHT11 
int moistsensPin0 = A0;    // select the input pin for the moistuer sensor
int moistsensVal = 0;  // variable to store the value coming from the sensor
int litsensPin0 = A2;  
int litsensVal = 0;

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  // Start Serial for debugging on the Serial Monitor
  Serial.begin(9600);
  dht.begin();
  // Start Ethernet on Arduino
  startEthernet();
}

void loop()
{
  /**************humidity and temperature*************/
    delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
 float h = dht.readHumidity();
 char h_buffer[10];
 String  humid=dtostrf(h,0,5,h_buffer);
 

  // Read temperature as Celsius (the default)
 float t = dht.readTemperature();
 char t_buffer[10];
 String  temp = dtostrf(t,0,5,t_buffer);

  // Read temperature as Fahrenheit (isFahrenheit = true)
  // float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  //if (isnan(h) || isnan(t) ) {
    //Serial.println("Failed to read from DHT sensor!");
    //return;
  //}

   /**************Moistuer sensor*************/
  //moistsensVal = analogRead(moistsensPin); 
  int moistsensVal = map(analogRead(moistsensPin0),0,1024,0,100);
  String moistsensPin = String(moistsensVal);   
  //delay(1000);          
 // Serial.print("soil moistuer = " );                       
  //Serial.println(moistsensVal);               

   /**************light sensor*************/
  //litsensVal = analogRead(litsensPin);   
  int litsensVal = map(analogRead(litsensPin0),0,1024,0,100);
  String litsensPin = String(litsensVal);    
 // delay(1000);          
 // Serial.print("light sensor = " );                       
 // Serial.println(litsensVal);               

 ///////////////////////////////////////////////////////////////////////////////////// 
// Print Update Response to Serial Monitor
  if (client.available())
  {
    char c = client.read();
    Serial.print(c);
  }

  // Disconnect from ThingSpeak
  if (!client.connected() && lastConnected)
  {
    Serial.println("...disconnected");
    Serial.println();
    
    client.stop();
  }
  
  // Update ThingSpeak
  if(!client.connected() && (millis() - lastConnectionTime > updateThingSpeakInterval))
  {
    updateThingSpeak("field1="+moistsensPin+"&field2="+litsensPin+"&field3="+temp+"&field4="+humid);
  }
  
  // Check if Arduino Ethernet needs to be restarted
  if (failedCounter > 3 ) {startEthernet();}
  
  lastConnected = client.connected();
}

void updateThingSpeak(String tsData)
{
  if (client.connect(thingSpeakAddress, 80))
  {         
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+writeAPIKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(tsData.length());
    client.print("\n\n");

    client.print(tsData);
    
    lastConnectionTime = millis();
    
    if (client.connected())
    {
      Serial.println("Connecting to ThingSpeak...");
      Serial.println();
      
      failedCounter = 0;
    }
    else
    {
      failedCounter++;
  
      Serial.println("Connection to ThingSpeak failed ("+String(failedCounter, DEC)+")");   
      Serial.println();
    }
    
  }
  else
  {
    failedCounter++;
    
    Serial.println("Connection to ThingSpeak Failed ("+String(failedCounter, DEC)+")");   
    Serial.println();
    
    lastConnectionTime = millis(); 
  }
}

void startEthernet()
{
  
  client.stop();

  Serial.println("Connecting Arduino to network...");
  Serial.println();  

  delay(1000);
  
  // Connect to network amd obtain an IP address using DHCP
  if (Ethernet.begin(mac) == 0)
  {
    Serial.println("DHCP Failed, reset Arduino to try again");
    Serial.println();
  }
  else
  {
    Serial.println("Arduino connected to network using DHCP");
    Serial.println();
  }
  
  delay(1000);
}
