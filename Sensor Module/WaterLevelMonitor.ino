//#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#define ECHOPIN                 D6 // Echo Pin D5
#define TRIGPIN                 D5 // Trigger Pin D6
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "";
char pass[] = "";
long duration, distance;
BlynkTimer timer;
WidgetBridge bridge1(V20);
IPAddress arduino_ip ( 192,  168,   1,  109);
IPAddress dns_ip     (  8,   8,   8,   8);
IPAddress gateway_ip ( 192,  168,   1,   1);
IPAddress subnet_mask(255, 255, 0,   0);


BLYNK_CONNECTED() {
  Blynk.email("Tank Sensor", "Online just now");
  bridge1.setAuthToken(""); // Token of the Main Module
}

void setup()
{
  // Debug console
  Serial.begin(115200);
  pinMode(TRIGPIN, OUTPUT);
  pinMode(ECHOPIN, INPUT);
  WiFi.mode(WIFI_STA);
  WiFi.config(arduino_ip, gateway_ip, subnet_mask, dns_ip);
  Blynk.begin(auth, ssid, pass);
  while (Blynk.connect() == false) {
    // Wait until Blynk is connected
  }
  timer.setInterval(1000L, getDepth);

}

void loop()
{
  Blynk.run();
  timer.run();
}

void getDepth() {
  digitalWrite(TRIGPIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIGPIN, LOW);

  duration = pulseIn(ECHOPIN, HIGH);
  distance = duration / 58.2;
  Blynk.virtualWrite(V0, distance);
  Blynk.virtualWrite(V1, WiFi.RSSI());
  bridge1.virtualWrite(V20, distance);
  Serial.print(distance);
  Serial.println("cm");
}
