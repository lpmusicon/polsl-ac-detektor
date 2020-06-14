#include <FS.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

#include "config.h"
#include "notification.h"

#define BUTTON_PIN 5
#define GSM_RESET_PIN 13

#define TINY_GSM_MODEM_SIM800
#include <TinyGsmClient.h>

TinyGsm modem(Serial2);
TinyGsmClient client(modem);

#define AC_PIN 34
#define BATTERY_PIN 32
#define SERIAL_BAUD 115200
#define HTTP_SERVER_PORT 80
#define MAX_MAPPED_VOLTAGE 5050

/**
 * Infrastructure SSID
 */
char SSID[33];

/**
 * Infrastructure Password
 */
char PASSWORD[64];

/**
 * AP SSID
 */
char AP_SSID[33];

/**
 * AP Password
 */
char AP_PASSWORD[64];

/**
 * Device SIM number
 */
String gsmNumber;

AsyncWebServer server(HTTP_SERVER_PORT);

/**
 * This function gets formatted time and date from GSM module
 * @returns current time/date string HH:mm dd/MM/YYYY
 */
String getTimeString()
{
  /**
   * Have to get every part of time/date string
   */
  int year = 0, month = 0, day = 0, hour = 0, min = 0, sec = 0;
  float timezone = 0.0;
  if (!modem.getNetworkTime(&year, &month, &day, &hour, &min, &sec, &timezone))
  {
    Serial.printf("%s:%d - cannot get network time\n", __FILE__, __LINE__);
  }

  char buf[20];
  sprintf(buf, "%02d:%02d %02d/%02d/%d", hour, min, day, month, year);

  return String(buf);
}

/**
 * This function gets SIM number and sets it to local variable
 * TONS of side effect coding
 * but it works
 */
void getSIMNumber()
{
  modem.sendAT("+CNUM");
  modem.stream.readStringUntil('\n');
  modem.stream.readStringUntil('"');
  modem.stream.readStringUntil('"');
  modem.stream.readStringUntil('"');
  String data = modem.stream.readStringUntil('"');
  modem.streamClear();
  Serial.printf("%s:%d - DATA %s\n", __FILE__, __LINE__, data.c_str());

  gsmNumber = data;
}

/**
 * Formats network to JSON per ID
 * Outputs to string
 */
void networkToJSONObject(int id, char *string)
{
  snprintf(string, 72, "{\"SSID\":\"%s\",\"dBm\":\"%d\",\"encType\":\"%d\"}", WiFi.SSID(id).c_str(), WiFi.RSSI(id), WiFi.encryptionType(id));
}

int8_t networks = 0;
uint16_t batteryVoltage = 0;
bool acOn = false;

void setup()
{
  /**
   * GPIO setup
   */
  pinMode(BATTERY_PIN, INPUT);
  pinMode(AC_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(GSM_RESET_PIN, OUTPUT);

  /**
   * This PIN can't be LOW
   * Otherwise GSM will not start
   */
  digitalWrite(GSM_RESET_PIN, HIGH);

  analogSetCycles(255);

  batteryVoltage = map(analogRead(BATTERY_PIN), 0, 4096, 0, MAX_MAPPED_VOLTAGE);
  acOn = digitalRead(AC_PIN);

  WiFi.persistent(false);
  WiFi.setAutoReconnect(false);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPdisconnect();
  WiFi.disconnect();

  SPIFFS.begin();
  Serial.begin(SERIAL_BAUD);

  if (loadWiFiconfig(SSID, PASSWORD))
  {
    WiFi.begin(SSID, PASSWORD);
    Serial.printf("%s:%d - SSID %s, PASSWORD %s\n", __FILE__, __LINE__, SSID, PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("Connecting");
      delay(500);
    }
    Serial.printf("%s:%d - Connected WiFi IP: %s\n", __FILE__, __LINE__, WiFi.localIP().toString().c_str());
  }

  loadDefaultWiFiconfig(AP_SSID, AP_PASSWORD);
  Serial.printf("%s:%d - AP SSID %s, AP PASSWORD %s\n", __FILE__, __LINE__, AP_SSID, AP_PASSWORD);
  WiFi.softAP(AP_SSID, AP_PASSWORD);

  /**
   * Ogólnie to najchętniej bym te metody przeniósł do innego pliku i tam to wykonywał.
   * Tutaj znajdują się wszystkie endpointy wymagane do działania aplikacji
   */
  server.on("/api/setup-completed", HTTP_GET, [](AsyncWebServerRequest *request) {
    String response = "{ \"status\": \"";
    response += isConfigured();
    response += "\" }";

    request->send(200, "application/json", response);
  });

  /**
   * [UNUSED] Ta funkcja nie jest obecnie wykorzystywana
   * Skanuje dostępne sieci WiFi
   */
  server.on("/api/wifi-scan", HTTP_GET, [](AsyncWebServerRequest *request) {
    int scanResult = WiFi.scanComplete();
    if (scanResult == WIFI_SCAN_NOT_TRIGGERED)
    {
      WiFi.scanDelete();
      networks = WiFi.scanNetworks(true, false);

      request->send(200, "application/json", "{ \"status\": \"OK\" }");
    }
    else if (scanResult == WIFI_SCAN_NOT_FINISHED)
    {
      request->send(200, "application/json", "{ \"status\": \"NOT_FINISHED\" }");
    }
    else
    {
      request->send(200, "application/json", "{ \"status\": \"FINISHED\" }");
    }
  });

  /**
   * [UNUSED]
   * Usuwa obecnie zapisany stan zapisanych sieci
   */ 
  server.on("/api/wifi-rescan", HTTP_GET, [](AsyncWebServerRequest *request) {
    WiFi.scanDelete();
    request->send(200, "application/json", "{\"status\": \"OK\"}");
  });

  /**
   * [UNUSED]
   * Zwraca listę sieci WiFi
   */
  server.on("/api/wifi-networks", HTTP_GET, [](AsyncWebServerRequest *request) {
    int scanResult = WiFi.scanComplete();
    if (scanResult >= 0)
    {
      uint16_t responseSize = MINIMUM_WIFI_NETWORK_RESPONSE_SIZE + (MAXIMUM_WIFI_NETWORK_OBJECT_SIZE + 1) * scanResult + 1;
      char *response = new char[responseSize];
      memset(response, 0, responseSize);

      strcat(response, "{\"status\":\"OK\",\"networks\":[");

      for (int i = 0; i < scanResult; i++)
      {
        char *object = new char[MAXIMUM_WIFI_NETWORK_OBJECT_SIZE];
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
    else if (scanResult == WIFI_SCAN_NOT_FINISHED)
    {
      request->send(200, "application/json", "{ \"status\": \"NOT_FINISHED\" }");
    }
    else if (scanResult == WIFI_SCAN_NOT_TRIGGERED)
    {
      request->send(200, "application/json", "{ \"status\": \"NOT_TRIGGERED\" }");
    }
  });

  /**
   * Zwraca status modułu WiFi
   * Zgodny z enum ESP32 WiFi Status
   */
  server.on("/api/wifi-status", HTTP_GET, [](AsyncWebServerRequest *request) {
    uint8_t wifiStatus = WiFi.status();
    char *response = new char[18];
    sprintf(response, "{\"status\":%d}", wifiStatus);
    request->send(200, "application/json", response);
    delete[] response;
  });

  /**
   * Zwraca obecnie podłączone SSID
   */
  server.on("/api/wifi-current", HTTP_GET, [](AsyncWebServerRequest *request) {
    String response = "{\"ssid\":\"";
    response += WiFi.SSID();
    response += "\"}";
    request->send(200, "application/json", response);
  });

  /**
   * Łączy do danego STA podanego w formularzu
   */
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

      Serial.printf("%s:%d - SSID: %s, PASSWORD: %s\n", __FILE__, __LINE__, ssid.c_str(), password.c_str());

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

  /**
   * Rozłącza sieć WiFi
   */
  server.on("/api/wifi-disconnect", HTTP_DELETE, [](AsyncWebServerRequest *request) {
    WiFi.disconnect();
    request->send(200, "application/json", "{ \"status\": \"ok\" }");
  });

  /**
   * Zwraca MAC modułu WiFi
   */
  server.on("/api/wifi-mac", HTTP_GET, [](AsyncWebServerRequest *request) {
    String response = "{ \"mac\": \"";
    response += WiFi.macAddress();
    response += "\" }";

    request->send(200, "application/json", response);
  });

  /**
   * Zwraca stan baterii w mV
   */
  server.on("/api/battery-status", HTTP_GET, [](AsyncWebServerRequest *request) {
    String response = "{ \"status\": \"";
    response += batteryVoltage;
    response += "\"}";
    request->send(200, "application/json", response);
  });

  /**
   * Zwraca stan zasilania AC
   */
  server.on("/api/ac-status", HTTP_GET, [](AsyncWebServerRequest *request) {
    String response = "{ \"status\": \"";
    response += acOn;
    response += "\" }";
    request->send(200, "application/json", response);
  });

  /**
   * Zapisuje SSID/Hasło WiFi w pamięci Flash modułu
   */
  server.on("/api/wifi-save", HTTP_POST, [](AsyncWebServerRequest *request) {
    String ssid, password;

    if (request->hasParam("ssid", true) && request->hasParam("password", true))
    {
      AsyncWebParameter *pssid = request->getParam("ssid", true);
      AsyncWebParameter *ppassword = request->getParam("password", true);
      ssid = pssid->value();
      password = ppassword->value();

      Serial.printf("%s:%d - SSID: %s, PASSWORD: %s\n", __FILE__, __LINE__, ssid.c_str(), password.c_str());

      if (!saveWiFiconfig(ssid.c_str(), password.c_str()))
      {
        Serial.println("DID NOT SAVE WIFI CONFIG");
      }
    }

    request->send(200, "application/json", "{ \"status\": \"OK\" }");
  });

  /**
   * Zapisuje kontakt GSM w pamięci Flash
   */
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
        Serial.printf("%s:%d - %s\n", __FILE__, __LINE__, "did not save gsm config");
      }
    }

    request->send(200, "application/json", "{ \"status\": \"ok\" }");
  });

  /**
   * Zwraca nazwę, nr telefonu zapisany w module
   */
  server.on("/api/gsm-contact", HTTP_GET, [](AsyncWebServerRequest *request) {
    char name[64];
    memset(name, 0, 64);
    char number[18];
    memset(number, 0, 18);

    if (!loadGSMconfig(name, number))
    {
      Serial.printf("%s:%d - %s\n", __FILE__, __LINE__, "couldn't load gsm config");
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

  /**
   * Zwraca listę powiadomień z modułu
   */
  server.on("/api/notification", HTTP_GET, [](AsyncWebServerRequest *request) {
    File f = SPIFFS.open(NOTIFICATION_DATA, "r");
    String response = "{\"entries\":[";

    while (f.available())
    {
      int type = f.readStringUntil(',').toInt();
      String date = f.readStringUntil('\n');
      Notification n(type, date);
      response += n.toJSON();
      if (f.available())
      {
        response += ",";
      }
    }

    response += "]}";
    request->send(200, "application/json", response);
  });

  /**
   * Zwraca frontEnd - czyt. aplikację
   */
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html");
  });

  /**
   * Powinno naprawiać problemy z CORS na niektórych konfiguracjach
   */
  server.on("/api/*", HTTP_OPTIONS, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "OK");
    response->addHeader("Access-Control-Allow-Origin", "*");
    response->addHeader("Access-Control-Expose-Headers", "*");
    response->addHeader("Access-Control-Allow-Credentials", "true");
    request->send(response);
  });

  /**
   * Pozwala na serwowanie potrzebnych assetów przygotowanych w pamięci urządzenia
   */
  server.serveStatic("/", SPIFFS, "/");

  Serial2.begin(SERIAL_BAUD, SERIAL_8N1);
  modem.sendAT("+IPR=115200");

  if (!modem.restart())
  {
    Serial.printf("%s:%d - %s\n", __FILE__, __LINE__, "couldn't restart modem");
    ESP.restart();
  }
  Serial.printf("%s:%d - %s\n", __FILE__, __LINE__, "modem restarted");

  if (!modem.waitForNetwork(300000L))
  {
    Serial.printf("%s:%d - %s\n", __FILE__, __LINE__, "couldn't connect modem");
    ESP.restart();
  }
  Serial.printf("%s:%d - %s\n", __FILE__, __LINE__, "modem connected to network");
  Serial.printf("%s:%d - Current Time: %s\n", __FILE__, __LINE__, getTimeString().c_str());

  getSIMNumber();
  server.begin();
}

/**
 * This needs cleanup,
 * two periods are not needed
 */
int timePassed = 0;
int period = 5000;
unsigned long time_now = 0;
unsigned long check_time_now = 0;
int check_interval = 1000;
bool lowBatterySMSsent = false;

#define BATTERY_LOW_CHARGE_VOLTAGE 3500
#define BUTTON_RESET_TIME_THRESHOLD 5000

void loop()
{
  /**
   * Reset za pomocą przycisku
   */
  if (!digitalRead(BUTTON_PIN))
  {
    if (millis() - timePassed > BUTTON_RESET_TIME_THRESHOLD)
    {
      /**
       * Delete everything, reboot uC
       */
      deleteWiFiconfig();
      deleteGSMconfig();
      deleteNotifications();
      WiFi.disconnect();
      WiFi.softAPdisconnect();
      ESP.restart();
    }
  }
  else
  {
    timePassed = millis();
  }

  /**
   * Main power handling loop
   */
  bool currentAC = digitalRead(AC_PIN);
  if (millis() - check_time_now > check_interval)
  {
    /**
     * Filtr uśredniający z poprzedniej wartości ze względu na dość duży szum ADC w ESP32
     * Wartości są mapowane z 10 bitowego zakresu 3.3V do 5V (ze względu na dzielnik napięcia)
     */
    batteryVoltage = 0.4 * batteryVoltage + 0.6 * map(analogRead(BATTERY_PIN), 0, 4096, 0, MAX_MAPPED_VOLTAGE);

    /**
     * Detekcja jest możliwa tylko gdy urządzenie jest ustawione
     */
    if (isConfigured() == 1)
    {
      char name[64], number[18];
      memset(name, 0, 64);
      memset(number, 0, 18);

      /**
       * GSM config is crucial to proper device functioning
       * if it is not present delete config
       * start over
       */
      if (!loadGSMconfig(name, number))
      {
        Serial.printf("%s:%d - GSM Config %s\n", __FILE__, __LINE__, "");
        deleteWiFiconfig();
        deleteGSMconfig();
        deleteNotifications();
        WiFi.disconnect();
        WiFi.softAPdisconnect();
        ESP.restart();
      }

      /**
       * State detection change
       * two power states are recognized
       */
      bool justConnected = acOn && !currentAC;
      bool justDisconnected = !acOn && currentAC;
      String smsText = "Hej ";
      smsText += name;
      if (justConnected)
      {
        smsText += ", prad wlasnie zostal odlaczony\n";
        writeNotification(NOTIFICATION_TYPE::POWER_LOSS, getTimeString());
      }
      else if (justDisconnected)
      {
        smsText += ", prad wlasnie zostal podlaczony\n";
        writeNotification(NOTIFICATION_TYPE::POWER_CONNECT, getTimeString());
        lowBatterySMSsent = false;
      }

      /**
       * When state changes we send SMS to client
       * this ensures sms flow
       */
      if (justConnected || justDisconnected)
      {
        smsText += getTimeString();
        Serial.printf("%s:%d - %s\n", __FILE__, __LINE__, smsText.c_str());
        modem.sendSMS(number, smsText);
      }

      /**
     * We have to sens SMS when battery is low
     * best case scenario we would have to go to sleep
     * and wait for power interrupt
     * not here however as it may never happen with 3000mAh
     * battery and uptime of 10+hrs
     */
      if (!acOn && batteryVoltage < BATTERY_LOW_CHARGE_VOLTAGE && !lowBatterySMSsent)
      {
        lowBatterySMSsent = true;
        String smsBatteryText = "Hej ";
        smsBatteryText += name;
        smsBatteryText += ", niski stan baterii. Urzadzenie moze sie wkrotce wylaczyc\n";
        smsBatteryText += getTimeString();
        writeNotification(NOTIFICATION_TYPE::LOW_BATTERY, getTimeString());
        Serial.printf("%s:%d - %s\n", __FILE__, __LINE__, smsBatteryText.c_str());
        modem.sendSMS(number, smsBatteryText);
      }
    }

    /**
     * Zapisuje aktualny stan do porównania
     */
    acOn = currentAC;
  }

  /**
   * Periodic diagnostic print on screen
   */
  if (millis() - time_now > period)
  {
    time_now += period;
    Serial.println("-----");
    Serial.printf("%s:%d - BATTERY PIN mV %d\n", __FILE__, __LINE__, batteryVoltage);
    Serial.printf("%s:%d - AC PIN %s\n", __FILE__, __LINE__, acOn ? "ON" : "OFF");
  }
}