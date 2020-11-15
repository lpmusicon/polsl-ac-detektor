#include <FS.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

#include "config.h"
#include "notification.h"
#include "http/http.h"
#include "mqtt/mqtt-client.h"
#include "gsm/gsm.h"
#include "wifi/wifi.h"

#define BUTTON_PIN 5
#define GSM_RESET_PIN 13

#define AC_PIN 34
#define BATTERY_PIN 32
#define SERIAL_BAUD 115200
#define HTTP_SERVER_PORT 80
#define MAX_MAPPED_VOLTAGE 5050

WiFiClient wifiClient;
MqttClient mqttClient = MqttClient(wifiClient);

AsyncWebServer server(HTTP_SERVER_PORT);

uint16_t batteryVoltage = 0;
bool acOn = false;

void setup()
{
  Serial.begin(SERIAL_BAUD);

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

  SPIFFS.begin();

  configureWifi();
  configureGsmModem();
  configureHttpServer(server);
  mqttClient.connect("192.168.4.2", 1883);
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
    check_time_now += check_interval;
    /**
     * Filtr uśredniający z poprzedniej wartości ze względu na dość duży szum ADC w ESP32
     * Wartości są mapowane z 10 bitowego zakresu 3.3V do 5V (ze względu na dzielnik napięcia)
     */
    batteryVoltage = 0.4 * batteryVoltage + 0.6 * map(analogRead(BATTERY_PIN), 0, 4096, 0, MAX_MAPPED_VOLTAGE);

    /**
     * Detekcja jest możliwa tylko gdy urządzenie jest ustawione
     */
    if (false)
    {
      /**
       * State detection change
       * two power states are recognized
       */
      bool justConnected = acOn && !currentAC;
      bool justDisconnected = !acOn && currentAC;

      if (justConnected)
      {
        Serial.printf("%s:%d - Write power loss\n", __FILE__, __LINE__);
        writeNotification(NOTIFICATION_TYPE::POWER_LOSS, getTimeString());
      }
      else if (justDisconnected)
      {
        Serial.printf("%s:%d - Write power on\n", __FILE__, __LINE__);
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

    mqttClient.keepAlive();
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
    mqttClient.printDiagnostic();
  }
}