#pragma once

#include <ESPAsyncWebServer.h>
#include "http/helpers.h"

#define API_MQTT_STATUS "/api/mqtt/status"
#define API_MQTT_CONNECT "/api/mqtt/connect"

void configureMqttEndpoints(AsyncWebServer &server)
{
    server.on(API_MQTT_STATUS, HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *web = request->beginResponse(HTTP_OK, CONTENT_TYPE_JSON);
        addCORS(web);
        request->send(web);
    });

    server.on(API_MQTT_CONNECT, HTTP_POST, [](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *web = request->beginResponse(HTTP_OK, CONTENT_TYPE_JSON);
        addCORS(web);
        request->send(web);
    });
}