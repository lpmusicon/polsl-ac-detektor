const char wifissid[] = "UPC0444679";
const char wifipassword[] = "x58nzmvfTzrf";


#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>

#include "config.h"

#define TINY_GSM_MODEM_SIM800

#define GSM_RESET_PIN 15
#define GSM_TX_PIN 4
#define GSM_RX_PIN 5

#include <SoftwareSerial.h>
SoftwareSerial SerialAT(GSM_RX_PIN, GSM_TX_PIN); // RX, TX
#define SerialMon Serial
#define TINY_GSM_DEBUG SerialMon
#include <TinyGsmClient.h>

const uint8_t PIN_LED = 2;
const uint8_t PIN2_LED = 16;

/*
 * Info odnośnie encType
 * TKIP (WPA) = 2
 * WEP = 5
 * CCMP (WPA) = 4
 * NONE = 7
 * AUTO = 8
 * UPC Wi-Free 255 z jakiegoś powodu (Nowy standard UPC!, pewnie -1)
*/

/* 
 * Setup
 * 
 * Trzeba:
 *  > Sprawdzić czy jest konfig
 *  > > Jak jest to wczytać i witaj świecie
 *  > > W innym wypadku trzeba załadować konfigurację podstawową i dać użytkownikowi się wykazać
 *  > Odpalić WiFi dla użytkownika
 *  > Połączyć się z modułem GSM
 *  > Muszę sprawdzać poziom zasilania (dzielnik napięcia na rezystorze wystarczy 1/3 - 2/3)
 *  > Pilnować pinu zasilania (też dzielnik napięcia 5V - 1/3 2/3)
*/

#define AC_PIN 5       // PIN D1
#define BATTERY_PIN A0 // PIN ADC0

#define SERIAL_BAUD 74880

char SSID[33];
char PASSWORD[64];

#define HTTP_SERVER_PORT 80

const int RSSI_MAX = -50;  // define maximum strength of signal in dBm
const int RSSI_MIN = -100; // define minimum strength of signal in dBm
int dBmtoPercentage(int dBm)
{
  int quality;
  if (dBm <= RSSI_MIN)
  {
    quality = 0;
  }
  else if (dBm >= RSSI_MAX)
  {
    quality = 100;
  }
  else
  {
    quality = 2 * (dBm + 100);
  }

  return quality;
}

AsyncWebServer server(HTTP_SERVER_PORT);

void networkToJSONObject(int id, char *string)
{
  snprintf(string, 72, "{\"SSID\":\"%s\",\"dBm\":\"%d\",\"encType\":\"%d\"}", WiFi.SSID(id).c_str(), WiFi.RSSI(id), WiFi.encryptionType(id));
}

uint32_t rate = 0;

void setup()
{
  pinMode(GSM_RESET_PIN, OUTPUT);
  digitalWrite(GSM_RESET_PIN, 1);
  LittleFS.begin();
  Serial.begin(SERIAL_BAUD);
  Serial.setDebugOutput(true);

  if (!rate) {
    rate = TinyGsmAutoBaud(SerialAT);
  }

  if (!rate) {
    SerialMon.println(F("***********************************************************"));
    SerialMon.println(F(" Module does not respond!"));
    SerialMon.println(F("   Check your Serial wiring"));
    SerialMon.println(F("   Check the module is correctly powered and turned on"));
    SerialMon.println(F("***********************************************************"));
    delay(30000L);
    return;
  }

  SerialAT.begin(rate);

  // Access AT commands from Serial Monitor
  SerialMon.println(F("***********************************************************"));
  SerialMon.println(F(" You can now send AT commands"));
  SerialMon.println(F(" Enter \"AT\" (without quotes), and you should see \"OK\""));
  SerialMon.println(F(" If it doesn't work, select \"Both NL & CR\" in Serial Monitor"));
  SerialMon.println(F("***********************************************************"));

  pinMode(AC_PIN, INPUT_PULLUP);
  bool acConnected = digitalRead(AC_PIN);
  int battV = analogRead(BATTERY_PIN);

  if (isConfigured())
  {
    loadWiFiconfig(SSID, PASSWORD);
  }
  else
  {
    loadDefaultWiFiconfig(SSID, PASSWORD);
  }

  Serial.print("ssid: ");
  Serial.println(SSID);
  Serial.print("password: ");
  Serial.println(PASSWORD);

  WiFi.persistent(false);

  // Create network and connect
  WiFi.mode(WIFI_AP_STA);

  WiFi.printDiag(Serial);

  WiFi.begin(wifissid, wifipassword);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Connecting");
    delay(500);
  }

  Serial.print("Connected WiFi IP:");
  Serial.println(WiFi.localIP());

  WiFi.softAP(SSID, PASSWORD);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, String(), "Hello World");
  });

#define SCAN_NOT_TRIGGERED -2
#define SCAN_NOT_FINISHED -1

  server.on("/api/wifi-scan", HTTP_GET, [](AsyncWebServerRequest *request) {
    int scanResult = WiFi.scanComplete();
    if (scanResult == SCAN_NOT_TRIGGERED)
    {
      WiFi.scanDelete();
      WiFi.scanNetworksAsync([](int onComplete) {
        Serial.print("Networks: ");
        Serial.println(onComplete);

        for (int i = 0; i < onComplete; i++)
        {
          Serial.print("WiFi SSID: ");
          Serial.print(WiFi.SSID(i));
          Serial.print(" dBm ");
          Serial.print(WiFi.RSSI(i));
          Serial.print(" signal ");
          Serial.print(dBmtoPercentage(WiFi.RSSI(i)));
          Serial.print(" encType ");
          Serial.println(WiFi.encryptionType(i));
        }
      });

      request->send(200, "application/json", "{ \"status\": \"OK\" }");
    }
    else if (scanResult == SCAN_NOT_FINISHED)
    {
      request->send(200, "application/json", "{ \"status\": \"NOT_FINISHED\" }");
    }
    else
    {
      request->send(200, "application/json", "{ \"status\": \"FINISHED\" }");
    }
  });

  server.on("/api/wifi-rescan", HTTP_GET, [](AsyncWebServerRequest *request) {
    WiFi.scanDelete();
    request->send(200, "application/json", "{\"status\": \"OK\"}");
  });

#define MINIMUM_RESPONSE_SIZE 30
#define MAXIMUM_OBJECT_SIZE 72

  server.on("/api/wifi-networks", HTTP_GET, [](AsyncWebServerRequest *request) {
    int scanResult = WiFi.scanComplete();
    if (scanResult >= 0)
    {
      uint16_t responseSize = MINIMUM_RESPONSE_SIZE + (MAXIMUM_OBJECT_SIZE + 1) * scanResult + 1;
      char *response = new char[responseSize];
      memset(response, 0, responseSize);

      strcat(response, "{\"status\":\"OK\",\"networks\":[");

      for (int i = 0; i < scanResult; i++)
      {
        char *object = new char[MAXIMUM_OBJECT_SIZE];
        networkToJSONObject(i, object);
        strcat(response, object);

        // Nie można dodać przecinka do ostatniego, więc sprawdzam
        if (i + 1 < scanResult)
        {
          strcat(response, ",");
        }

        delete[] object;
      }

      strcat(response, "]}");

      request->send(200, "application/json", response);
      delete[] response;
    }
    else if (scanResult == SCAN_NOT_FINISHED)
    {
      request->send(200, "application/json", "{ \"status\": \"NOT_FINISHED\" }");
    }
    else if (scanResult == SCAN_NOT_TRIGGERED)
    {
      request->send(200, "application/json", "{ \"status\": \"NOT_TRIGGERED\" }");
    }
  });

  server.on("/api/wifi-status", HTTP_GET, [](AsyncWebServerRequest *request) {
    uint8_t wifiStatus = WiFi.status();
    char *response = new char[18];
    sprintf(response, "{\"status\":%d}", wifiStatus);
    request->send(200, "application/json", response);
    delete[] response;
  });

  server.on("/api/wifi-connect", HTTP_POST, [](AsyncWebServerRequest *request) {
    bool isConnected = WiFi.isConnected();
    Serial.print("isConnected: ");
    Serial.println(isConnected ? "true" : "false");

    if (isConnected)
      WiFi.disconnect();

    
    String ssid;
    String password;

    if (request->hasParam("ssid", true) && request->hasParam("password", true)) {
      AsyncWebParameter * pssid = request->getParam("ssid", true);
      AsyncWebParameter * ppassword = request->getParam("password", true);
      ssid = pssid->value();
      password = ppassword->value();

      Serial.println(ssid);
      Serial.println(password);

      WiFi.begin(ssid, password);
    }

    uint8_t wifiStatus = WiFi.status();
    char *response = new char[18];
    sprintf(response, "{\"status\":%d}", wifiStatus);
    request->send(200, "application/json", response);
    
  });

  server.on("/api/wifi-disconnect", HTTP_GET, [](AsyncWebServerRequest *request) {
    WiFi.disconnect();

    request->send(200, "application/json", "{ \"status\": \"OK\" }");
  });

  server.on("/api/battery-status", HTTP_GET, [](AsyncWebServerRequest *request) {
    //TODO
    request->send(200, "application/json", "{ \"status\": \"3.8V\" }");
  });

  server.on("/api/ac-status", HTTP_GET, [](AsyncWebServerRequest *request) {
    //TODO
    request->send(200, "application/json", "{ \"status\": \"ON\" }");
  });

  server.on("/api/wifi-network", HTTP_POST, [](AsyncWebServerRequest *request) {
    // TODO
    // SAVE FROM FORM TO MEMORKI

    request->send(200, "application/json", "{ \"status\": \"OK\" }");
  });

  server.on("/api/gsm-contact", HTTP_POST, [](AsyncWebServerRequest *request) {
    // TODO
    // SAVE FROM FORM TO MEMORKI KONTAKT USERA
    request->send(200, "application/json", "{ \"status\": \"OK\" }");
  });

  server.on("/api/gsm-contact", HTTP_GET, [](AsyncWebServerRequest *request) {
    // TODO
    // GET SAVED USER TEL NAME
    request->send(200, "application/json", "{ \"status\": \"OK\" }");
  });

  server.on("/api/gsm-signal", HTTP_GET, [](AsyncWebServerRequest *request) {
    // TODO
    // GET SIGNAL STRENGTH
    request->send(200, "application/json", "{ \"status\": \"OK\" }");
  });

  server.on("/api/gsm-status", HTTP_GET, [](AsyncWebServerRequest *request) {
    // TODO
    // GET SIGNAL STRENGTH
    request->send(200, "application/json", "{ \"status\": \"OK\" }");
  });

  server.begin();

  // gsmSetup
  // serverSetup
}

void loop()
{
  if (SerialAT.available()) {
    SerialMon.write(SerialAT.read());
  }
  
  if (SerialMon.available()) {
    SerialAT.write(SerialMon.read());
  }

}