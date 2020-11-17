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
MqttClient mqttClient = MqttClient(wifiClient);
AsyncWebServer server(HTTP_SERVER_PORT);

void setup()
{
  Serial.begin(SERIAL_BAUD);
  auto configManager = ConfigManager::GetInstance();
  AC::GetInstance();
  Battery::GetInstance();
  ResetManager::GetInstance();
  GsmManager::GetInstance();
  WifiManager::GetInstance();

  configureHttpServer(server);

  if (configManager->mqttEnabled())
  {
    Serial.printf("%s:%d - Connect MQTT\n", __FILE__, __LINE__);
    mqttClient.connect(configManager->getMqttServerIP().c_str(), configManager->getMqttServerPort());
  }
}

void loop()
{
  mqttClient.keepAlive();
  AC::GetInstance()->check();
  Battery::GetInstance()->measure();
  ResetManager::GetInstance()->check();
  EventManager::GetInstance()->loop();
}