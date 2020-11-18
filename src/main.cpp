#include <FS.h>
#include <ESPAsyncWebServer.h>

#include "http/http.h"
#include "mqtt/mqtt-client.h"
#include "gsm/gsm-manager.h"
#include "battery/battery.h"
#include "wifi/wifi-manager.h"
#include "ac/ac.h"
#include "event/event-manager.h"
#include "reset-manager/reset-manager.h"
#include "config/config-manager.h"

#define SERIAL_BAUD 115200
#define HTTP_SERVER_PORT 80

WiFiClient wifiClient;
AsyncWebServer server(HTTP_SERVER_PORT);

void setup()
{
  Serial.begin(SERIAL_BAUD);
  ConfigManager::GetInstance()->refresh();
  AC::GetInstance();
  Battery::GetInstance();
  ResetManager::GetInstance();
  GsmManager::GetInstance();
  WifiManager::GetInstance();
  MqttClient::GetInstance()->setClient(wifiClient);
  MqttClient::GetInstance()->connect();
  configureHttpServer(server);
}

void loop()
{
  MqttClient::GetInstance()->checkNewConnection();
  MqttClient::GetInstance()->keepAlive();
  AC::GetInstance()->check();
  Battery::GetInstance()->measure();
  ResetManager::GetInstance()->check();
  EventManager::GetInstance()->loop();
}