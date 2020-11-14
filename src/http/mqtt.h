#pragma once

#include <ESPAsyncWebServer.h>
#include "helpers.h"

#define API_MQTT_STATUS "/api/mqtt/status"
#define API_MQTT_CONNECT "/api/mqtt/connect"

extern void mqttConnect();

void configureMqttEndpoints(AsyncWebServer &server)
{
    server.on(API_MQTT_STATUS, HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(HTTP_OK, CONTENT_TYPE_JSON);
    });

    server.on(API_MQTT_CONNECT, HTTP_POST, [](AsyncWebServerRequest *request) {
        mqttConnect();
        request->send(HTTP_OK, CONTENT_TYPE_JSON);
    });
}