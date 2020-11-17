#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#include "http/helpers.h"
#include "battery/battery.h"
#include "ac/ac.h"
#include "config/config-manager.h"
#include "gsm/gsm-manager.h"

#define API_SETUP "/api/setup"
#define API_SETUP_CONFIG "/api/setup/config"
#define API_SETUP_BATTERY "/api/setup/battery"

void configureSetupEndpoints(AsyncWebServer &server)
{
    server.on(API_SETUP_CONFIG, HTTP_GET, [](AsyncWebServerRequest *request) {
        String response;
        const size_t size = JSON_OBJECT_SIZE(12);
        DynamicJsonDocument json(size);
        json["ssid"] = ConfigManager::GetInstance()->getWifiSsid();
        json["port"] = ConfigManager::GetInstance()->getMqttServerPort();
        json["server"] = ConfigManager::GetInstance()->getMqttServerIP();
        json["user"] = ConfigManager::GetInstance()->getMqttServerUser();
        json["phone"] = GsmManager::GetInstance()->getSIMNumber();
        json["battery"] = Battery::GetInstance()->getVoltage();
        json["mac"] = ConfigManager::GetInstance()->getWifiMac();
        json["ac"] = AC::GetInstance()->connected();
        serializeJson(json, response);

        AsyncWebServerResponse *web = request->beginResponse(HTTP_OK, CONTENT_TYPE_JSON, response.c_str());
        addCORS(web);
        request->send(web);
    });

    server.on(API_SETUP_BATTERY, HTTP_GET, [](AsyncWebServerRequest *request) {
        String response;
        const size_t size = JSON_OBJECT_SIZE(1);
        DynamicJsonDocument json(size);
        json["status"] = Battery::GetInstance()->getVoltage();
        serializeJson(json, response);

        AsyncWebServerResponse *web = request->beginResponse(HTTP_OK, CONTENT_TYPE_JSON, response.c_str());
        addCORS(web);
        request->send(web);
    });

    server.on(API_SETUP, HTTP_GET, [](AsyncWebServerRequest *request) {
        String response;
        const size_t size = JSON_OBJECT_SIZE(1);
        DynamicJsonDocument json(size);
        json["status"] = ConfigManager::GetInstance()->isConfigured();
        serializeJson(json, response);

        AsyncWebServerResponse *web = request->beginResponse(HTTP_OK, CONTENT_TYPE_JSON, response.c_str());
        addCORS(web);
        request->send(web);
    });
}