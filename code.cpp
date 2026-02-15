#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

const char* ssid = "YOUR_WIFI_NAME";
const char* pass = "YOUR_WIFI_PASSWORD";

const char* sid  = "AC11a60a8e4012f9dc08ed65ca4c213d26";
const char* tok  = "bfcc6130cff6e1ec1cea5eb1d30af320";

const char* fromN = "+";
const char* toN   = "+";

#define DHTPIN  27
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

float TH = 35.0;
bool sent = false;

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(ssid, pass);
  Serial.print(F("WiFi"));

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }

  Serial.println(F("\nConnected"));
}

void loop() {
  delay(1000);

  float t = dht.readTemperature();
  if (isnan(t)) return;

  Serial.print(F("Temp: "));
  Serial.println(t);

  if (t >= TH && !sent) {
    sms(t);
    sent = true;
  }

  if (t < TH - 5) sent = false;
}

void sms(float t) {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient h;
  String url = "https://api.twilio.com/2010-04-01/Accounts/" + String(sid) + "/Messages.json";

  h.begin(url);
  h.setAuthorization(sid, tok);
  h.addHeader("Content-Type", "application/x-www-form-urlencoded");

  String body = "From=" + String(fromN) +
                "&To=" + String(toN) +
                "&Body=FIRE ALERT! Temp=" + String(t) + "C";

  int r = h.POST(body);
  Serial.println(r == 201 ? F("SMS OK") : F("SMS FAIL"));
  h.end();
}
