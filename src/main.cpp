#include <FS.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

#include "config.h"
#include "notification.h"
#include "httpserver.h"

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

  Serial2.begin(SERIAL_BAUD, SERIAL_8N1);
  modem.sendAT("+IPR=115200");

  if (!modem.restart())
  {
    Serial.printf("%s:%d - %s\n", __FILE__, __LINE__, "couldn't restart modem");
  }
  Serial.printf("%s:%d - %s\n", __FILE__, __LINE__, "modem restarted");

  if (!modem.waitForNetwork(300000L))
  {
    Serial.printf("%s:%d - %s\n", __FILE__, __LINE__, "couldn't connect modem");
  }
  Serial.printf("%s:%d - %s\n", __FILE__, __LINE__, "modem connected to network");
  Serial.printf("%s:%d - Current Time: %s\n", __FILE__, __LINE__, getTimeString().c_str());

  getSIMNumber();
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

      if (justConnected)
      {
        writeNotification(NOTIFICATION_TYPE::POWER_LOSS, getTimeString());
      }
      else if (justDisconnected)
      {
        writeNotification(NOTIFICATION_TYPE::POWER_CONNECT, getTimeString());
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
        writeNotification(NOTIFICATION_TYPE::LOW_BATTERY, getTimeString());
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