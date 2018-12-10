// This #include statement was automatically added by the Particle IDE.
#include <neopixel.h>

#include <ThingSpeak.h>
#include <Adafruit_DHT.h>
#include <ThingSpeak.h>

TCPClient client;


#include "Particle.h"
#include "neopixel.h"

SYSTEM_MODE(AUTOMATIC);

// IMPORTANT: Set pixel COUNT, PIN and TYPE
#define PIXEL_PIN D2
#define PIXEL_COUNT 16
#define PIXEL_TYPE WS2812

Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
unsigned long myChannelNumber = 000000;		/*Thingspeak channel id*/
const char * myWriteAPIKey = "zzzzzzzzzzzzzzzz";/*Channel's write API key*/


int photoCell = A3;
int power = A5; // Photocell power.  An analog pin to gives a more steady voltage.
int light; // Light
int Moisture = A0;

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
	
	  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
// Wait a few seconds between measurements.
	delay(2000);
  light = analogRead(photoCell);
// String light1 = String(light);
  delay(2000);  // is this needed?

  if (light <35){
             uint16_t i;

    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, 255, 0, 255);   
    }
    strip.show();
     Particle.publish("state", "grrow light on");
  }else {
              uint16_t i;

      for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, 0, 0, 0);   
    }
         Particle.publish("state", "grrow light OFFFFF");

    strip.show();
  }
  
  
  moist = analogRead(Moisture);
// String light1 = String(light);
     Particle.publish("state", "Moituer done");

  delay(2000);  // is this needed?
// Reading temperature or humidity takes about 250 milliseconds!
// Sensor readings may also be up to 2 seconds 'old' (its a 
// very slow sensor)
	float h = dht.getHumidity();
	     Particle.publish("state", "Humidiy done");

// Read temperature as Celsius
//	float t = dht.getTempCelcius();
// Read temperature as Farenheit
	float f = dht.getTempFarenheit();
	     Particle.publish("state", "temp done");

  
// Check if any reads failed and exit early (to try again).
 if (isnan(h) || isnan(f)) {
		//Serial.println("Failed to read from DHT sensor!");
				Particle.publish("Failed to read from DHT sensor!");

		return;
	}


 ThingSpeak.setField(1,h); // humidity 
  ThingSpeak.setField(2,f);//  temperature as Farenheit
   ThingSpeak.setField(3,light); // light
  ThingSpeak.setField(4,moist); // soil moisture

  // Write the fields that you've set all at once.
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);  
  delay(20000);	
 
	String timeStamp = Time.timeStr();


delay(1500);
	loopCount++;
	if(loopCount >= 5){ // the kit will read 5 times before going to power saver sleep mode, you can edit the number to any number you like.
	  Particle.publish("state", "Going to sleep for 1/2 minutes");
	  delay(10000);
	  System.sleep(SLEEP_MODE_DEEP, 5);  // The kit will go into deep sleep mode to save power for 5 seconds, 
					     // please edit to change the sleep time, example: 3600 seconds is one houre sleep time. 
	}
}