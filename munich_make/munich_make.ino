#include <DHT.h>
#include <SPI.h>
#include <math.h>

#include <Ethernet.h>
byte mac[] = { 0xD4, 0x28, 0xB2, 0xFF, 0xA0, 0xA1 }; // Must be unique on local network


#define soilMoisturePIN 0 // soil moisture sensor connected to Analog Pin 0
#define photoresistorPIN 2 // photoresistor connected to Analog Pin 0
#define DHTPIN 3 // what pin we're connected to


 //thingspeak
// replace with your channel's thingspeak API key, 
char thingSpeakAddress[] = "api.thingspeak.com";
String writeAPIKey = "CMKRR51G4VAYJOEP";

DHT dht(DHTPIN, DHT22,15);
EthernetClient client;


void setup()
{
    pinMode(soilMoisturePIN, INPUT);
  
    pinMode(photoresistorPIN, INPUT);
    Serial.begin(9600);
  
     dht.begin();
     
     startEthernet();
    
    
    
  }



String dht22TandH = "";
String soilMoist = "";
String soilTemp = "";
String illuminance = "";

String GET = "GET /update?key=[THINGSPEAK_KEY]";
String GET1 = "&field1="; //Ambient temperature chart
String GET2 = "&field2="; //Humidity temperature chart
String GET3 = "&field3="; //Soil moisture chart
String GET5 = "&field4="; //photoresistor chart

String cmd = "";



//detects ambient temperature and humidity and then converts them into strings
void loop() {
   
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

String soilMoisture (){

    float moisture;
    char moisture_c[6];
    //calculates the percentage of soil moisture
    moisture = analogRead(soilMoisturePIN);
    moisture = 100*(1-(moisture)/1023);

    dtostrf(moisture, 0, 1, moisture_c); //converts floats to strings

    return (String) moisture_c;
}

String photoresistor () {

    char luxValue_c[6];
    double voltageOUT = (5.04/1023)*analogRead(photoresistorPIN);
    double photoResistance = 12000*((5.04/voltageOUT)-1);
    double a = 0.7;
    double luxValue = pow((30000/photoResistance), 1/a);
    dtostrf(luxValue, 0, 1, luxValue_c); //converts floats to strings

    return (String) luxValue_c;
}


//sends ambient temperature and humidity to ThingSpeak
void upadatevValues(String temperature_c,
                    String humidity_c,
                    String moisture_c,
                    String soilTemperature_c,
                    String luxValue_c)
                    {
    cmd = "AT+CIPSTART=\"TCP\",\"";
    cmd += IP;
    cmd += "\",80";
    esp8266.println(cmd);
    delay(2000);

        if(esp8266.find("Error")){
            Serial.print("Error1");
        return;
        }

    cmd = GET + GET1;
    cmd += temperature_c;
    cmd += GET2;
    cmd += humidity_c;
    cmd += GET3;
    cmd += moisture_c;
    cmd += GET4;
    cmd += soilTemperature_c;
    cmd += GET5;
    cmd += luxValue_c;

    cmd += "\r\n";

    Serial.print(cmd);
    esp8266.print("AT+CIPSEND=");
    esp8266.println(cmd.length());

        if(esp8266.find(">")){
            esp8266.print(cmd);
        }
        else{
            esp8266.println("AT+CIPCLOSE");
        }
}

boolean connectWiFi(){
  esp8266.println("AT+CWMODE=1");
  delay(2000);
  String cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  esp8266.println(cmd);
  delay(5000);
  if(esp8266.find("OK")){
    Serial.println("OK");
    return true;
  }else{
    Serial.println("KO");
    return false;
  }
}
}
void setup()
{
    pinMode(soilMoisturePIN, INPUT);
  
    pinMode(photoresistorPIN, INPUT);
    Serial.begin(9600);
    esp8266.begin(115200);
    //sensors.begin();
    esp8266.println("AT");
    delay(5000);
    if(esp8266.find("OK")){
    connectWiFi();
  }
}

void loop(){

    dht22TandH = dht22();
    soilMoist = soilMoisture();
  
    illuminance = photoresistor();

    upadatevValues(getValue(dht22TandH, ':', 0), getValue(dht22TandH, ':', 1),soilMoist, soilTemp, illuminance);
    delay(2000);
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

