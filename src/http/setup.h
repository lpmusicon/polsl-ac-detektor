#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#include "helpers.h"
#include "battery/battery.h"
#include "config.h"
#include "gsm/gsm-manager.h"

#define API_SETUP "/api/setup"
#define API_SETUP_CONFIG "/api/setup/config"
#define API_SETUP_BATTERY "/api/setup/battery"

void configureSetupEndpoints(AsyncWebServer &server)
{
    server.on(API_SETUP_CONFIG, HTTP_GET, [](AsyncWebServerRequest *request) {
        String response;
        const size_t size = JSON_OBJECT_SIZE(8);
        DynamicJsonDocument json(size);
        json["ssid"] = "network ssid";
        json["port"] = 3306;
        json["server"] = "mqtt.example.com";
        json["user"] = "sip.user.mqtt";
        json["phone"] = GsmManager::GetInstance()->getSIMNumber();
        json["battery"] = 4020;
        json["ac"] = 0;
        json["mac"] = WiFi.macAddress();
        serializeJson(json, response);
        request->send(HTTP_OK, CONTENT_TYPE_JSON, response);
    });

    server.on(API_SETUP_BATTERY, HTTP_GET, [](AsyncWebServerRequest *request) {
        String response;
        const size_t size = JSON_OBJECT_SIZE(1);
        DynamicJsonDocument json(size);
        json["status"] = Battery::GetInstance()->getVoltage();
        serializeJson(json, response);
        request->send(HTTP_OK, CONTENT_TYPE_JSON, response);
    });

    server.on(API_SETUP, HTTP_GET, [](AsyncWebServerRequest *request) {
        String response;
        const size_t size = JSON_OBJECT_SIZE(1);
        DynamicJsonDocument json(size);
        json["status"] = isConfigured();
        serializeJson(json, response);
        request->send(HTTP_OK, CONTENT_TYPE_JSON, response);
    });
}