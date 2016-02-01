
//loading libraries
#include <SPI.h>
#include <Ethernet2.h>


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
int tmp36Pin = A1;
int moistsensPin0 = A0;    // select the input pin for the moistuer sensor
int moistsensVal = 0;  // variable to store the value coming from the sensor
int litsensPin0 = A2;  
int litsensVal = 0;



void setup()
{
  // Start Serial for debugging on the Serial Monitor
  Serial.begin(9600);
 // dht.begin();
  // Start Ethernet on Arduino
  startEthernet();
}

void loop()
{
  /**************temperature*************/
    delay(2000);
    
     //getting the voltage reading from the temperature sensor
 int reading = analogRead(tmp36Pin);  
 
 // converting that reading to voltage, for 3.3v arduino use 3.3
  float voltage = reading * 5.0;
 voltage /= 1024.0; 

  float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
                                               //to degrees ((voltage - 500mV) times 100)
 String temp = String(temperatureC);   

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
    updateThingSpeak("field1="+moistsensPin+"&field2="+litsensPin+"&field3="+temp);
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

