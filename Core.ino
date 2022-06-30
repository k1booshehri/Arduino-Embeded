#include <DHT.h>
#include <DHT_U.h>
#include "DHT.h"
#include <WiFiEspClient.h>
#include <WiFiEsp.h>
#include <WiFiEspUdp.h>
#include "SoftwareSerial.h"
#include "HX711.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <SPI.h>


#define AIO_SERVER      "45.149.77.235"
#define AIO_SERVERPORT  1883           
#define AIO_USERNAME    "97521387"
#define AIO_KEY         "euCh4kvA"

// Initialize the Ethernet client object
WiFiEspClient espClient;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&espClient, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Publish Light = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/Keyvan");

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 6;
const int LOADCELL_SCK_PIN = 7;

HX711 scale;

#define WIFI_AP "Abed"
#define WIFI_PASSWORD "12345678"


// DHT
#define DHTPIN 4
#define DHTTYPE DHT11


// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);


SoftwareSerial soft(2, 3); // RX, TX

int status = WL_IDLE_STATUS;
uint32_t breath ;
long reading;

void setup() {
  // initialize serial for debugging
  Serial.begin(57600);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  dht.begin();
  InitWiFi();
}

void loop() {
    MQTT_connect();
  status = WiFi.status();
  if ( status != WL_CONNECTED) {
    while ( status != WL_CONNECTED) {
      Serial.print("Attempting to connect to WPA SSID: ");
      Serial.println(WIFI_AP);
      // Connect to WPA/WPA2 network
      status = WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      delay(500);
    }
    Serial.println("Connected to AP");
  }
    getAndSendTemperatureAndHumidityData();
    breath=0;
    for(int i=0 ; i<80 ; i++){
    StrainGauge();
    delay(200);
    }

    if ( breath ==1 || breath ==2)
    {
      breath=0;
    }

    Serial.println("breath:");
    Serial.print(breath);
    
  if (!Light.publish(breath*4)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }


  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds

  if (! mqtt.ping()) {
    mqtt.disconnect();
  }
    delay(5000);
}

void getAndSendTemperatureAndHumidityData()
{
  Serial.println("Collecting temperature data.");

  // Reading temperature or humidity takes about 250 milliseconds!
  float humidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float temperature = dht.readTemperature();


  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.println("");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C ");

    if (!Light.publish(humidity)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

      if (!Light.publish(temperature)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
  
}



void StrainGauge(){
  scale.set_gain(32);
  if (scale.is_ready()) {
  long newreading = scale.get_value();
if (newreading-reading>=100 || reading-newreading>=100 ){
    breath++;
  }
  reading=newreading;
  Serial.print("HX711 reading: ");
  Serial.println(newreading);
  } 
  else {
  Serial.println("HX711 not found.");
  }
  
}




void InitWiFi()
{
  // initialize serial for ESP module
  soft.begin(9600);
  // initialize ESP module
  WiFi.init(&soft);
  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(WIFI_AP);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(WIFI_AP, WIFI_PASSWORD);
    delay(500);
  }
  Serial.println("Connected to AP");
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
