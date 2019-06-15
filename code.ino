//Wi-fi
#include <LWiFi.h>
#include <WiFiClient.h>
#include "MCS.h"
//DHT
#include "DHT.h"

// Assign AP ssid / password here
#define _SSID "ASUS23114"
#define _KEY  "11111166"

// Assign device id / key of your test device
MCSDevice mcs("DW823zSR", "x5VKTgqyUl975aoC");

// Assign channel id 
// The test device should have 2 channel
// the first channel should be "Display" - "Temperature"
// the secord channel should be "Display" - "Hhumidity"
MCSDisplayFloat temperature_1("Temperature_1");
MCSDisplayFloat humidity_1("Humidity_1");

#define DHTPIN 2     // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

// Read humidity
float h;
// Read temperature as Celsius (Fahreheit)
float t,f;
// Compute heat index in Celsius (Fahreheit)
float hic,hif;

//timer
#include "Arduino.h"
#include "LTimer.h"

// instantiation
LTimer timer0(LTIMER_0);
LTimer timer1(LTIMER_1);

int flag0 = 0;
int flag1 = 0;

// callback function for timer0
void _callback0(void *usr_data)
{
   flag0=!flag0;
}

// callback function for timer1
void _callback1(void *usr_data)
{
   flag1=!flag1;;
}
void setup() {
  Serial.begin(9600);

  // setup Wifi connection
  while(WL_CONNECTED != WiFi.status())
  {
    Serial.print("WiFi.begin(");
    Serial.print(_SSID);
    Serial.print(",");
    Serial.print(_KEY);
    Serial.println(")...");
    WiFi.begin(_SSID, _KEY);
  }
  Serial.println("WiFi connected !!");

  // setup MCS connection
  mcs.addChannel(temperature_1);
  mcs.addChannel(humidity_1);
  while(!mcs.connected())
  {
    Serial.println("MCS.connect()...");
    mcs.connect();
  }
  
  // initialization
  timer0.begin();
  timer1.begin();

  // start the execution
  timer0.start(1000, LTIMER_REPEAT_MODE, _callback0, NULL);
  timer1.start(10000, LTIMER_REPEAT_MODE, _callback1, NULL);

  Serial.println(F("DHT11 test!"));
  dht.begin();
}

void loop() {
  // Wait a few seconds between measurements.
  delay(1000);
    
  // call process() to allow background processing, add timeout to avoid high cpu usage
  Serial.print("process(");
  Serial.print(millis());
  Serial.println(")");
  mcs.process(100);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  hic = dht.computeHeatIndex(t, h, false);
  
  if(flag0) {
    delay(300);
    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.print(F("%  Temperature: "));
    Serial.print(t);
    Serial.print(F("°C "));
    Serial.print(f);
    Serial.print(F("°F  Heat index: "));
    Serial.print(hic);
    Serial.print(F("°C "));
    Serial.print(hif);
    Serial.println(F("°F"));

    flag0=!flag0;
  }
  
  if(flag1) {
    delay(300);
    if(!temperature_1.set(t)) 
    {
      Serial.print("Failed to update Temperature");
    }
    if(!humidity_1.set(h)) 
    {
      Serial.print("Failed to update Hhumidity");
    }
    flag1=!flag1;
  }
  // check if need to re-connect
  while(!mcs.connected())
  {
    Serial.println("re-connect to MCS...");
    mcs.connect();
    if(mcs.connected())
      Serial.println("MCS connected !!");
  } 
}
