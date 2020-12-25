#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "POCO"
#define WLAN_PASS       "amitrawat"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "nerbots_1234"
#define AIO_KEY         "aio_fMWX44qgQUKyUEn7blazdv2qBOVE"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiClientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/


// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe car_control = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/control_car");

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

void setup() {
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(D8, OUTPUT);
  Serial.begin(9600);
  delay(10);

  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&car_control);
}

uint32_t x=0;

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &car_control) {
      Serial.print(F("Got: "));
      char* res = (char *)car_control.lastread;
      Serial.println(res);
      if(res[0] == 'F' && res[1] == 'o'){
        Serial.println("Moving Forward");
        digitalWrite(D2, HIGH);
        digitalWrite(D1, LOW);

        digitalWrite(D4, HIGH);
        digitalWrite(D3, LOW);

        digitalWrite(D6, HIGH);
        digitalWrite(D5, LOW);

        digitalWrite(D8, HIGH);
        digitalWrite(D7, LOW);
           
      }
      else if(res[0] == 'B' && res[1] == 'a'){
        Serial.println("Moving Backward");
        digitalWrite(D2, LOW);
        digitalWrite(D1, HIGH); 
        
        digitalWrite(D3, HIGH);
        digitalWrite(D4, LOW);

        digitalWrite(D5, HIGH);
        digitalWrite(D6, LOW);

        digitalWrite(D7, HIGH);
        digitalWrite(D8, LOW);
       }
      else if(res[0] == 'R' && res[1] == 'o'){
        Serial.println("Rotating");
        digitalWrite(D2, HIGH);
        digitalWrite(D1, LOW);

        digitalWrite(D4, LOW);
        digitalWrite(D3, LOW);

        digitalWrite(D5,LOW);
        digitalWrite(D6, HIGH);

        digitalWrite(D7, LOW);
        digitalWrite(D8, LOW);
        }
    }
  }
}



void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
