#include <Arduino.h>

#include <TFT_eSPI.h> 
#include <SPI.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
#include <Wire.h>
#include <Button2.h>
#include "esp_adc_cal.h"

#ifndef TFT_DISPOFF
#define TFT_DISPOFF 0x28
#endif

#ifndef TFT_SLPIN
#define TFT_SLPIN   0x10
#endif

#define ADC_EN          14
#define ADC_PIN         34
#define BUTTON_1        35
#define BUTTON_2        0

TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library

const char *serverName = "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin%2Cethereum%2Ctezos&vs_currencies=usd";

const char* root_ca= \
"-----BEGIN CERTIFICATE-----\n" \
"MIIEpTCCBEygAwIBAgIQCuZHyV28hPT5tDMlUvhIwzAKBggqhkjOPQQDAjBKMQsw\n" \
"CQYDVQQGEwJVUzEZMBcGA1UEChMQQ2xvdWRmbGFyZSwgSW5jLjEgMB4GA1UEAxMX\n" \
"Q2xvdWRmbGFyZSBJbmMgRUNDIENBLTMwHhcNMjAwOTIwMDAwMDAwWhcNMjEwOTIw\n" \
"MTIwMDAwWjBlMQswCQYDVQQGEwJVUzELMAkGA1UECBMCQ0ExFjAUBgNVBAcTDVNh\n" \
"biBGcmFuY2lzY28xGTAXBgNVBAoTEENsb3VkZmxhcmUsIEluYy4xFjAUBgNVBAMT\n" \
"DWNvaW5nZWNrby5jb20wWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAASH6nzJGwEr\n" \
"hxNj2Jf8fUTY3qntUAmHnsx7YWx6ZQy5xIOHZgJ8g3XJlA3jjV5RzTiVmCMCHiHu\n" \
"Iv45JWb4Cc2To4IC9zCCAvMwHwYDVR0jBBgwFoAUpc436uuwdQ6UZ4i0RfrZJBCH\n" \
"lh8wHQYDVR0OBBYEFPMXv/wyVHfkqX2En1k7gDcek87TMCkGA1UdEQQiMCCCDWNv\n" \
"aW5nZWNrby5jb22CDyouY29pbmdlY2tvLmNvbTAOBgNVHQ8BAf8EBAMCB4AwHQYD\n" \
"VR0lBBYwFAYIKwYBBQUHAwEGCCsGAQUFBwMCMHsGA1UdHwR0MHIwN6A1oDOGMWh0\n" \
"dHA6Ly9jcmwzLmRpZ2ljZXJ0LmNvbS9DbG91ZGZsYXJlSW5jRUNDQ0EtMy5jcmww\n" \
"N6A1oDOGMWh0dHA6Ly9jcmw0LmRpZ2ljZXJ0LmNvbS9DbG91ZGZsYXJlSW5jRUND\n" \
"Q0EtMy5jcmwwTAYDVR0gBEUwQzA3BglghkgBhv1sAQEwKjAoBggrBgEFBQcCARYc\n" \
"aHR0cHM6Ly93d3cuZGlnaWNlcnQuY29tL0NQUzAIBgZngQwBAgIwdgYIKwYBBQUH\n" \
"AQEEajBoMCQGCCsGAQUFBzABhhhodHRwOi8vb2NzcC5kaWdpY2VydC5jb20wQAYI\n" \
"KwYBBQUHMAKGNGh0dHA6Ly9jYWNlcnRzLmRpZ2ljZXJ0LmNvbS9DbG91ZGZsYXJl\n" \
"SW5jRUNDQ0EtMy5jcnQwDAYDVR0TAQH/BAIwADCCAQQGCisGAQQB1nkCBAIEgfUE\n" \
"gfIA8AB2APZclC/RdzAiFFQYCDCUVo7jTRMZM7/fDC8gC8xO8WTjAAABdKr9eLQA\n" \
"AAQDAEcwRQIgARuYm3cnuH4WGVgDjE8yr5pjfRRXmBUQLvjQbBhO8lUCIQDKFRYy\n" \
"MKwT1P9EbAmKySHDmtxPVrPFe1H8FU2DPcaE5gB2AFzcQ5L+5qtFRLFemtRW5hA3\n" \
"+9X6R9yhc5SyXub2xw7KAAABdKr9ePsAAAQDAEcwRQIhAKUyUAbP99vsNMq6iPp/\n" \
"KGCVrvLfgFH4Q88ws5Qo/T02AiAMTyN/6QwWTuf0VGfY8ZV35KtU8s3aLG8ZDOlc\n" \
"CCxTzjAKBggqhkjOPQQDAgNHADBEAiBunS4tLHkZhszQn/m2Ie0LB+ED1tfdLaVj\n" \
"7mVIjGMx3AIgApaDszooVmZy9DfRUX5MhR+NMeNTBNwC9US0QCgA3DA=\n" \
"-----END CERTIFICATE-----\n";

const char* ssid = "foo";
const char* password = "bar";

//! Long time delay, it is recommended to use shallow sleep, which can effectively reduce the current consumption
void espDelay(int ms) //use-> espDelay(6000);
{   
    esp_sleep_enable_timer_wakeup(ms * 1000);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH,ESP_PD_OPTION_ON);
    esp_light_sleep_start();
}

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

void setup()
{
    Serial.begin(115200);
    setup_wifi();

    Serial.println("Start");
    tft.init();
    tft.fontHeight(2);
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
}

void loop()
{

    HTTPClient http;

    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.GET();

    String payload = http.getString();
    http.end();

    StaticJsonDocument<200> doc;
    deserializeJson(doc, payload);
    String btc = doc["bitcoin"]["usd"];
    String xtz = doc["tezos"]["usd"];
    String eth = doc["ethereum"]["usd"];


    // note: height = 135; Every row = 33 px; Margin top = 5
    int xpos = 0;
    xpos += tft.drawString("BTC: ", 0, 5, 4);  //string,start x,start y, font weight {1;2;4;6;7;8}
    tft.drawString(btc, xpos, 5, 4);

    xpos = 0;
    xpos += tft.drawString("XTZ: ", 0, 38, 4);  //string,start x,start y, font weight {1;2;4;6;7;8}
    tft.drawString(xtz, xpos, 38, 4);

    xpos = 0;
    xpos += tft.drawString("ETH: ", 0, 71, 4);  //string,start x,start y, font weight {1;2;4;6;7;8}
    tft.drawString(eth, xpos, 71, 4);

    delay(5000);
}
