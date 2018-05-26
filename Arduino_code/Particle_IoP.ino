// This #include statement was automatically added by the Particle IDE.
#include <Adafruit_DHT.h>


#include <ThingSpeak.h>


//#define DHTPIN D2     // what pin we're connected to

//#define DHTTYPE DHT22		// DHT 22 (AM2302)
#include "ThingSpeak.h"

TCPClient client;

#include "Adafruit_DHT.h"

unsigned long myChannelNumber = 443542;		/*Thingspeak channel id*/
const char * myWriteAPIKey = "4V4UQ5VL59D22LNR";/*Channel's write API key*/


int photoCell = A3;
int power = A5; // Photocell power.  An analog pin to gives a more steady voltage.
int light; // Light

int Moisture = A0;
//int power = A5; // Photocell power.  An analog pin to gives a more steady voltage.
int moist; // Light

DHT dht(D2, DHT22);

//DHT dht(DHTPIN, DHTTYPE);
int loopCount;

void setup() {
//	Serial.begin(9600); 
//	Serial.println("DHTxx test!");
	//Particle.publish("state", "DHTxx test start");
    ThingSpeak.begin(client);
	dht.begin();
	
	pinMode(photoCell,INPUT);
	
	pinMode(Moisture,INPUT);
	
    pinMode(power,OUTPUT);
    digitalWrite(power,HIGH); // Turn on power source for photoCell

	loopCount = 0;
	delay(2000);
}

void loop() {
// Wait a few seconds between measurements.
	delay(5000);
  light = analogRead(photoCell);
// String light1 = String(light);
  delay(100);  // is this needed?
  
  moist = analogRead(Moisture);
// String light1 = String(light);
  delay(100);  // is this needed?
// Reading temperature or humidity takes about 250 milliseconds!
// Sensor readings may also be up to 2 seconds 'old' (its a 
// very slow sensor)
	float h = dht.getHumidity();
// Read temperature as Celsius
//	float t = dht.getTempCelcius();
// Read temperature as Farenheit
	float f = dht.getTempFarenheit();
  
// Check if any reads failed and exit early (to try again).
 if (isnan(h) || isnan(f)) {
		Serial.println("Failed to read from DHT sensor!");
		return;
	}

// Compute heat index
// Must send in temp in Fahrenheit!
/*	float hi = dht.getHeatIndex();
	float dp = dht.getDewPoint();
	float k = dht.getTempKelvin();
	*/

 ThingSpeak.setField(1,h);
  ThingSpeak.setField(2,f);
   ThingSpeak.setField(3,light);
  ThingSpeak.setField(4,moist);

  // Write the fields that you've set all at once.
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);  
  delay(20000);	
 

//ThingSpeak.writeField(myChannelNumber, 1, light, myWriteAPIKey);
 //delay(1500); /* ThingSpeak will only accept updates every 15 seconds.*/
 
 //ThingSpeak.writeField(myChannelNumber, 2, moist, myWriteAPIKey);
// delay(1500); /* ThingSpeak will only accept updates every 15 seconds.*/
 
// ThingSpeak.writeField(myChannelNumber, 3, h, myWriteAPIKey);
 //delay(1500); /* ThingSpeak will only accept updates every 15 seconds*/
 //
// ThingSpeak.writeField(myChannelNumber, 4, f, myWriteAPIKey);
 //delay(1500); /* ThingSpeak will only accept updates every 15 seconds.*/
 

  
  /*
    Serial.print("light"); 
	Serial.print(light);
	Serial.print("Humid: "); 
	Serial.print(h);
	Serial.print("% - ");
//	Serial.print("Temp: "); 
//	Serial.print(t);
//	Serial.print("*C ");
	Serial.print(f);
	Serial.print("*F ");
//	Serial.print(k);
//	Serial.print("*K - ");
//	Serial.print("DewP: ");
//	Serial.print(dp);
//	Serial.print("*C - ");
//	Serial.print("HeatI: ");
//	Serial.print(hi);
//	Serial.println("*C");
//	Serial.println(Time.timeStr());
	*/
//	String timeStamp = Time.timeStr();
//	Particle.publish("readings", String::format("{\light: light, \"Hum(\%)\": %4.2f}",light, h, f));
delay(1500);
	loopCount++;
	if(loopCount >= 5){
	  Particle.publish("state", "Going to sleep for 1/2 minutes");
	  delay(10000);
	  System.sleep(SLEEP_MODE_DEEP, 5);  
	}
}

