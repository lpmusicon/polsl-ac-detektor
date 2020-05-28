const char wifissid[] = "UPC0444679";
const char wifipassword[] = "x58nzmvfTzrf";

#include <FS.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

#include "config.h"
#include "notification.h"

#define GSM_RESET_PIN 15
#define GSM_TX_PIN 4
#define GSM_RX_PIN 5

#define ADC_CALIBRATION_OFFSET 0

// #define DUMP_AT_COMMANDS
// #define TINY_GSM_DEBUG Serial
#define TINY_GSM_MODEM_SIM800
#include <TinyGsmClient.h>
#include <PubSubClient.h>

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(Serial2, Serial);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(Serial2);
#endif
TinyGsmClient client(modem);
PubSubClient mqtt(client);

#define AC_PIN 34      
#define BATTERY_PIN 32

#define SERIAL_BAUD 115200

char SSID[33];
char PASSWORD[64];

#define HTTP_SERVER_PORT 80

AsyncWebServer server(HTTP_SERVER_PORT);

void networkToJSONObject(int id, char *string)
{
  snprintf(string, 72, "{\"SSID\":\"%s\",\"dBm\":\"%d\",\"encType\":\"%d\"}", WiFi.SSID(id).c_str(), WiFi.RSSI(id), WiFi.encryptionType(id));
}

uint32_t rate = 0;

int8_t networks = 0;
uint16_t batteryVoltage = 0;
bool acOn = false;
String gsmNumber;

void setup()
{
  pinMode(BATTERY_PIN, INPUT);
  pinMode(AC_PIN, INPUT);

  WiFi.softAPdisconnect();

  pinMode(GSM_RESET_PIN, OUTPUT);
  digitalWrite(GSM_RESET_PIN, 1);
  SPIFFS.begin();
  Serial.begin(SERIAL_BAUD);
  Serial.setDebugOutput(true);

  pinMode(AC_PIN, INPUT_PULLUP);
  bool acConnected = digitalRead(AC_PIN);
  int battV = analogRead(BATTERY_PIN);

  Serial.println(acConnected);
  Serial.println(battV);

  loadDefaultWiFiconfig(SSID, PASSWORD);

  Serial.print("ssid: ");
  Serial.println(SSID);
  Serial.print("password: ");
  Serial.println(PASSWORD);

  WiFi.persistent(false);

  WiFi.mode(WIFI_AP_STA);
  WiFi.setAutoReconnect(false);

  WiFi.printDiag(Serial);

  WiFi.begin(wifissid, wifipassword);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Connecting");
    delay(1000);
  }

  Serial.print("Connected WiFi IP:");
  Serial.println(WiFi.localIP());

  WiFi.softAP(SSID, PASSWORD);

#define SCAN_NOT_TRIGGERED -2
#define SCAN_NOT_FINISHED -1

  server.on("/api/setup-completed", HTTP_GET, [](AsyncWebServerRequest *request) {
    // GOT HELP ME REFACTOR THIS
    // STRING BAD
    // CHAR* GOOD

    String response = "{ \"status\": \"";
    response += isConfigured();
    response += "\" }";

    request->send(200, "application/json", response);
  });

  server.on("/api/wifi-scan", HTTP_GET, [](AsyncWebServerRequest *request) {
    int scanResult = WiFi.scanComplete();
    if (scanResult == SCAN_NOT_TRIGGERED)
    {
      WiFi.scanDelete();
      networks = WiFi.scanNetworks(true, false);

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

  server.on("/api/wifi-current", HTTP_GET, [](AsyncWebServerRequest *request) {
    String response = "{\"ssid\":\"";
    response += WiFi.SSID();
    response += "\"}";
    request->send(200, "application/json", response);
  });

  server.on("/api/notification", HTTP_GET, [](AsyncWebServerRequest *request) {
    Notification one(0, "13:30 13/03/2020");
    Notification two(1, "12:30 13/03/2020");
    Notification three(2, "11:30 13/03/2020");
    Notification four(0, "10:50 13/03/2020");

    String response = "{\"entries\":[";
    response += one.toJSON() + ",";
    response += two.toJSON() + ",";
    response += three.toJSON() + ",";
    response += four.toJSON();
    response += "]}";
    request->send(200, "application/json", response);
  });

  server.on("/api/wifi-connect", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (WiFi.isConnected())
      WiFi.disconnect();

    String ssid;
    String password;

    if (request->hasParam("ssid", true) && request->hasParam("password", true))
    {
      AsyncWebParameter *pssid = request->getParam("ssid", true);
      AsyncWebParameter *ppassword = request->getParam("password", true);

      ssid = pssid->value();
      password = ppassword->value();

      Serial.print(__FILE__);
      Serial.print(":");
      Serial.print(__LINE__);
      Serial.print("param ssid: ");
      Serial.println(ssid);
      Serial.print("password: ");
      Serial.println(password);

      WiFi.persistent(false);
      WiFi.begin(ssid.c_str(), password.c_str());
    }
    else
    {
      Serial.println("Mamma mia, it's me disconectario");
    }

    uint8_t wifiStatus = WiFi.status();
    char *response = new char[18];
    sprintf(response, "{\"status\":%d}", wifiStatus);
    request->send(200, "application/json", response);
  });

  server.on("/api/wifi-disconnect", HTTP_DELETE, [](AsyncWebServerRequest *request) {
    WiFi.disconnect();

    request->send(200, "application/json", "{ \"status\": \"ok\" }");
  });

  server.on("/api/wifi-mac", HTTP_GET, [](AsyncWebServerRequest *request) {
    String response = "{ \"mac\": \"";
    response += WiFi.macAddress();
    response += "\" }";

    request->send(200, "application/json", response);
  });

  server.on("/api/battery-status", HTTP_GET, [](AsyncWebServerRequest *request) {
    String response = "{ \"status\": \""; response += batteryVoltage; response += "\"}";
    request->send(200, "application/json", response);
  });

  server.on("/api/ac-status", HTTP_GET, [](AsyncWebServerRequest *request) {
    String response = "{ \"status\": \"";
    response += acOn;
    response += "\" }";
    request->send(200, "application/json", response);
  });

  server.on("/api/wifi-save", HTTP_POST, [](AsyncWebServerRequest *request) {
    String ssid, password;

    if (request->hasParam("ssid", true) && request->hasParam("password", true))
    {
      AsyncWebParameter *pssid = request->getParam("ssid", true);
      AsyncWebParameter *ppassword = request->getParam("password", true);
      ssid = pssid->value();
      password = ppassword->value();

      Serial.print(__FILE__);
      Serial.print(":");
      Serial.print(__LINE__);
      Serial.println(ssid);
      Serial.println(password);

      if (!saveWiFiconfig(ssid.c_str(), password.c_str()))
      {
        Serial.println("DID NOT SAVE WIFI CONFIG");
      }
    }

    request->send(200, "application/json", "{ \"status\": \"OK\" }");
  });

  server.on("/api/gsm-contact", HTTP_POST, [](AsyncWebServerRequest *request) {
    String name, number;

    if (request->hasParam("name", true) && request->hasParam("number", true))
    {
      AsyncWebParameter *pname = request->getParam("name", true);
      AsyncWebParameter *pnumber = request->getParam("number", true);
      name = pname->value();
      number = pnumber->value();

      Serial.println(name);
      Serial.println(number);

      if (!saveGSMconfig(name.c_str(), number.c_str()))
      {
        Serial.print(__FILE__);
        Serial.print(":");
        Serial.print(__LINE__);
        Serial.println(" DID NOT SAVE GSM CONFIG");
      }
    }

    request->send(200, "application/json", "{ \"status\": \"ok\" }");
  });

  server.on("/api/gsm-contact", HTTP_GET, [](AsyncWebServerRequest *request) {
    char name[64];
    memset(name, 0, 64);
    char number[18];
    memset(number, 0, 18);

    if (!loadGSMconfig(name, number))
    {
      Serial.print(__FILE__);
      Serial.print(":");
      Serial.print(__LINE__);
      Serial.println(" cannot load gsm config");
    }

    String response = "{ \"name\": \"";
    response += name;
    response += "\", \"number\" : \"";
    response += number;
    response += "\" }";

    request->send(200, "application/json", response);
  });

  server.on("/api/gsm-signal", HTTP_GET, [](AsyncWebServerRequest *request) {
    String response = "{ \"status\": \"";
    response += modem.getSignalQuality();
    response += "\" }";
    request->send(200, "application/json", response);
  });

  server.on("/api/gsm-number", HTTP_GET, [](AsyncWebServerRequest *request) {
    String response = "{ \"number\": \"";
    response += gsmNumber;
    response += "\"}";
    request->send(200, "application/json", response);
  });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html");
  });

  server.serveStatic("/", SPIFFS, "/");

  server.on("/api/*", HTTP_OPTIONS, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "OK");
    response->addHeader("Access-Control-Allow-Origin", "*");
    response->addHeader("Access-Control-Expose-Headers", "*");
    response->addHeader("Access-Control-Allow-Credentials", "true");
    request->send(response);
  });

  server.begin();

  // gsmSetup
  // serverSetup

  pinMode(5, INPUT_PULLUP);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  uint32_t rate = 115200;

  Serial2.begin(rate, SERIAL_8N1);
  modem.sendAT("+IPR=115200");

  if (!modem.restart())
  {
    Serial.print(__FILE__);
    Serial.print(":");
    Serial.print(__LINE__);
    Serial.println(" couldn't restart");
  }
  else
  {
    Serial.println("restarted");
  }

  if (!modem.waitForNetwork(300000L))
  {
    Serial.print(__FILE__);
    Serial.print(":");
    Serial.print(__LINE__);
    Serial.println(" couldn't connect");
  }
  else
  {
    Serial.println("network reg");
  }

  // if(!modem.sendSMS("+48786153444", "Siemanko witam w mojej kuchni")) {
  //   Serial.print(__FILE__);
  //   Serial.print(":");
  //   Serial.print(__LINE__);
  //   Serial.println(" SMS not sent");
  // };

  // if (!modem.gprsConnect("internet"))
  // {
  //   Serial.print(__FILE__);
  //   Serial.print(":");
  //   Serial.print(__LINE__);
  //   Serial.println(" can't connect gprs");
  // }
  // else
  // {
  //   Serial.print(__FILE__);
  //   Serial.print(":");
  //   Serial.print(__LINE__);
  //   Serial.println(" connected GPRS");
  // }

  // Serial.print("isGRPSConnected ");
  // Serial.println(modem.isGprsConnected() ? "YES" : "NO");

  // mqtt.setServer("broker.hivemq.com", 1883);
  // mqtt.setCallback(mqttCallback);

  // if (!mqtt.connect("clientid-client-test"))
  // {
  //   Serial.print(__FILE__);
  //   Serial.print(":");
  //   Serial.print(__LINE__);
  //   Serial.println(" can't connect mqtt");
  // }

  // if (!!mqtt.publish("/esp32lpm/esp32-test", "hello world"))
  // {
  //   Serial.print(__FILE__);
  //   Serial.print(":");
  //   Serial.print(__LINE__);
  //   Serial.println(" can't publish mqtt");
  // }

  modem.sendAT("+CNUM");
  modem.stream.readStringUntil('\n');
  modem.stream.readStringUntil('"');
  modem.stream.readStringUntil('"');
  modem.stream.readStringUntil('"');
  String data = modem.stream.readStringUntil('"');
  modem.streamClear();
  Serial.print(__FILE__);
  Serial.print(":");
  Serial.print(__LINE__);
  Serial.print(" DATA: "); Serial.println(data);

  gsmNumber = data;
}

int timePassed = 0;
int period = 5000;
unsigned long time_now = 0;

void loop()
{
  if (!digitalRead(5))
  {
    if (millis() - timePassed > 5000)
    {
      deleteWiFiconfig();                                                                                                                                                                                            
      deleteGSMconfig();
      ESP.restart();
    }
  }
  else
  {
    timePassed = millis();
  }

  // mqtt.loop();
  if (millis() - time_now > period)
  {
    time_now += period;
    batteryVoltage = batteryVoltage * 0.4 + 0.6 * map(analogRead(BATTERY_PIN), 0, 4096, 0, 5000) + ADC_CALIBRATION_OFFSET;
    bool acOn = digitalRead(AC_PIN);
    Serial.print(__FILE__); Serial.print(":"); Serial.print(__LINE__); Serial.print(" - BATTERY PIN mV "); Serial.println(batteryVoltage);
    Serial.print(__FILE__); Serial.print(":"); Serial.print(__LINE__); Serial.print(" - AC PIN "); Serial.println(acOn ? "ON" : "OFF");
    Serial.print(__FILE__); Serial.print(":"); Serial.print(__LINE__); Serial.print(" - Signal Quality "); Serial.println(modem.getSignalQuality());
    Serial.println("-----");
    // mqtt.publish("/esp32lpm/esp32-test", modem.getGSMDateTime(TinyGSMDateTimeFormat::DATE_FULL).c_str());
  }
}