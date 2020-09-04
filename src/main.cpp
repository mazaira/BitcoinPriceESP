#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>


// Config
const char* ssid = "foo";
const char* password = "bar";

//API URL
const char *serverName = "https://api.coingecko.com/api/v3/ping";

unsigned long time1;
unsigned long time2;

WiFiClient espClient;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
}

void loop() {
  HTTPClient http;
  http.begin(serverName);
  http.addHeader("Content-Type", "application/json");
  time1 = millis();
  int rcode = http.GET();
  time2 = millis();
  Serial.println(time2 - time1);
  Serial.println(rcode);
}
