#include <FS.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

#include "config.h"
#include "notification.h"
#include "http/http.h"
#include "mqtt/mqtt-client.h"
#include "gsm/gsm-manager.h"
#include "battery/battery.h"
#include "wifi/wifi-manager.h"
#include "ac/ac.h"
#include "event/event-manager.h"
#include "reset-manager/reset-manager.h"

#define SERIAL_BAUD 115200
#define HTTP_SERVER_PORT 80

WiFiClient wifiClient;
MqttClient mqttClient = MqttClient(wifiClient);
AsyncWebServer server(HTTP_SERVER_PORT);

void setup()
{
  AC::GetInstance();
  Battery::GetInstance();
  ResetManager::GetInstance();
  GsmManager::GetInstance();
  WifiManager::GetInstance();

  Serial.begin(SERIAL_BAUD);
  SPIFFS.begin();

  configureHttpServer(server);
  mqttClient.connect("192.168.4.2", 1883);
}

unsigned long check_time_now = 0;
int check_interval = 1000;
bool lowBatterySMSsent = false;

#define BATTERY_LOW_CHARGE_VOLTAGE 3500

void loop()
{
  mqttClient.keepAlive();
  AC::GetInstance()->check();
  Battery::GetInstance()->measure();
  ResetManager::GetInstance()->check();

  if (millis() - check_time_now > check_interval)
  {
    check_time_now += check_interval;

    /**
     * Detekcja jest możliwa tylko gdy urządzenie jest ustawione
     */
    if (false)
    {
      /**
       * State detection change
       * two power states are recognized
       */
      if (AC::GetInstance()->justConnected())
      {
        Serial.printf("%s:%d - Power loss\n", __FILE__, __LINE__);
        EventManager::GetInstance()->add(EVENT_TYPE::POWER_LOSS, GsmManager::GetInstance()->getTimeString());
      }
      else if (AC::GetInstance()->justDisconnected())
      {
        Serial.printf("%s:%d - Power on\n", __FILE__, __LINE__);
        EventManager::GetInstance()->add(EVENT_TYPE::POWER_CONNECT, GsmManager::GetInstance()->getTimeString());
      }

      /**
      * We have to sens SMS when battery is low
      * best case scenario we would have to go to sleep
      * and wait for power interrupt
      * not here however as it may never happen with 3000mAh
      * battery and uptime of 10+hrs
      */
      if (!AC::GetInstance()->connected() && Battery::GetInstance()->getVoltage() < BATTERY_LOW_CHARGE_VOLTAGE && !lowBatterySMSsent)
      {
        lowBatterySMSsent = true;
        EventManager::GetInstance()->add(EVENT_TYPE::LOW_BATTERY, GsmManager::GetInstance()->getTimeString());
      }
    }
  }
}