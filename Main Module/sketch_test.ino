//#define BLYNK_PRINT Serial // Defines the object that is used for printing
//#define BLYNK_DEBUG        // Optional, this enables more detailed prints
#include <PZEM004Tv30.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include "secret.h"
#include "settings.h"

PZEM004Tv30 pzem(RX_PIN_NODEMCU, TX_PIN_NODEMCU);

float voltage_blynk = 0;
float current_blynk = 0;
float power_blynk = 0;
float energy_blynk = 0;
float frequency_blynk = 0;
float pf_blynk = 0;
bool AUTO_MODE = false;
int lastConnectionAttempt = millis();
int connectionDelay = 2000;
int waterFilllevel = 0;
int sensor_distance = 0;
int tank_volume = 0;
int tank_percent = 0;

IPAddress arduino_ip ( 192,  168,   1,  102);
IPAddress dns_ip     (  8,   8,   8,   8);
IPAddress gateway_ip ( 192,  168,   1,   1);
IPAddress subnet_mask(255, 255, 0,   0);


BlynkTimer timer;
DHT dht(DHTPIN, DHTTYPE);

BLYNK_CONNECTED() {
  Blynk.email("PUMP CONTROLLER", "Connected just now");
  Blynk.syncVirtual(vPIN_PUMP, vPIN_12VLED, vPIN_TANK_TOGGLE);
}

BLYNK_WRITE(vPIN_TANK_TOGGLE) {
  int buttonState = param.asInt(); //pinData variable will store value that came via Bridge
  if (buttonState == 0)
    AUTO_MODE = false;
  else AUTO_MODE = true;
  digitalWrite(D7, buttonState);

}

BLYNK_WRITE(vPIN_TANK_LEVEL) {
  sensor_distance = param.asInt(); //pinData variable will store value that came via Bridge
}

BLYNK_WRITE(vPIN_PUMP) {
  int relayState2 = param.asInt();
  digitalWrite(RELAY_PIN_1, relayState2);
  digitalWrite(D4, relayState2);
}

BLYNK_WRITE(vPIN_12VLED) {
  int relayState3 = param.asInt();
  digitalWrite(RELAY_PIN_2, relayState3);
}

void setup() {
  Serial.begin(115200);
  Serial.println("SETUP");
  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(D4, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFi.config(arduino_ip, gateway_ip, subnet_mask, dns_ip);
  Blynk.begin(AUTH, WIFI_SSID, WIFI_PASS);
  while (Blynk.connect() == false) {
    // Wait until Blynk is connected
  }
  //WiFi.setSleepMode(WIFI_NONE_SLEEP);
  //WiFi.begin(WIFI_SSID, WIFI_PASS);         // Non-blocking if no WiFi available
  //delay(5);
  //Blynk.config(AUTH);
  //Blynk.connect();
  dht.begin();
  // Setup a function to be called every second
  timer.setInterval(2000L, getDHTSensor);
  timer.setInterval(1500L, get_pzem_data);
  timer.setInterval(1000L, getWifiStats);
  timer.setInterval(1200L, getTankLevel);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED || Blynk.connected() == false)
  {
    // (optional) "offline" part of code

    // check delay:
    if (millis() - lastConnectionAttempt >= connectionDelay)
    {
      //Blynk.disconnect();
      Serial.print("Blynk connect");
      Serial.println(Blynk.connected());
      Serial.print("Wifi connect");
      Serial.println(WiFi.status());
      lastConnectionAttempt = millis();
      // attempt to connect to Wifi network:
      resetWifi();
      //ESP.restart();  //In my case I was using a wireless Repeater, it seems to have problems with Blynk. Only way to fix is restart the board everytime it disconnects.
    }
  }
  else {
    Blynk.run();
    timer.run();
  }

}

void get_pzem_data() {
  Serial.println("get pzem");
  if (!isnan(pzem.voltage()))
    voltage_blynk = pzem.voltage();
  if (!isnan(pzem.current()))
    current_blynk = pzem.current();
  if (!isnan(pzem.power()))
    power_blynk = pzem.power();
  //if (!isnan(pzem.energy()))energy_blynk = pzem.energy();
  if (!isnan(pzem.frequency()))
    frequency_blynk = pzem.frequency();
  //if (!isnan(pzem.pf()))pf_blynk = pzem.pf();


  Blynk.virtualWrite(vPIN_VOLTAGE, voltage_blynk);
  Blynk.virtualWrite(vPIN_CURRENT_USAGE, current_blynk  );
  Blynk.virtualWrite(vPIN_ACTIVE_POWER, power_blynk);
  //Blynk.virtualWrite(vPIN_ACTIVE_ENERGY, energy_blynk  );
  //Blynk.virtualWrite(vPIN_POWER_FACTOR, pf_blynk  );
  Blynk.virtualWrite(vPIN_FREQUENCY, frequency_blynk);
}

void getDHTSensor()
{
  Serial.println("get DHT11");
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(vPIN_TEMPERATURE, t);
  Blynk.virtualWrite(vPIN_HUMIDITY, h);
}


void getTankLevel() {       //Modify according to your Tank Dimensions
  Serial.println("get tank level");
  waterFilllevel = WATER_TANK_HEIGHT_IN_CM - sensor_distance;
  if (sensor_distance >= WATER_TANK_HEIGHT_IN_CM) {
    tank_volume = 0;
    tank_percent = 0;
  }
  if (sensor_distance < 10 && sensor_distance > 0) {
    tank_volume = FULL_VOLUME_IN_LITRE;
    tank_percent = 100;
  }
  if (sensor_distance >= 10 && sensor_distance <= WATER_TANK_HEIGHT_IN_CM) {
    tank_volume = (3.14 * TANK_RADIUS * TANK_RADIUS * waterFilllevel) / 1000;
    tank_percent = (((float)tank_volume / FULL_VOLUME_IN_LITRE) * 100);
  }

  Blynk.virtualWrite(vPIN_TANK_PERCENT, tank_percent);
  //  Serial.println(waterFilllevel);
  //  Serial.println(tank_volume);
  //  Serial.println(tank_percent);
  //  Serial.println(sensor_distance);

  if (sensor_distance < 10  && AUTO_MODE) {
    Blynk.virtualWrite(vPIN_PUMP, 0);
    digitalWrite(RELAY_PIN_1, 0);
    digitalWrite(D4, 0);
  }

}

void getWifiStats() {
  Serial.println("get wifi level");
  Serial.print("HEAP : ");
  Serial.println(ESP.getFreeHeap());
  Blynk.virtualWrite(vPIN_SIGNAL, WiFi.RSSI());
}

void resetWifi() {
  WiFi.disconnect();
  Blynk.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.config(arduino_ip, gateway_ip, subnet_mask, dns_ip);
  Blynk.begin(AUTH, WIFI_SSID, WIFI_PASS);
}
