#pragma once

#include <FS.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#include "http/helpers.h"
#include "config/config-manager.h"
#include "event/event-manager.h"

#define API_EVENTS "/api/events"
#define API_EVENTS_CLEAR "/api/events/clear"

void configureEventEndpoints(AsyncWebServer &server)
{
    server.on(API_EVENTS, HTTP_GET, [](AsyncWebServerRequest *request) {
        std::string response = "{\"events\":[";
        EventManager::GetInstance()->forEach([&response](uint8_t type, std::string date, bool isLast) {
            const size_t size = JSON_OBJECT_SIZE(2);
            DynamicJsonDocument json(size);
            json["type"] = type;
            json["date"] = date;
            serializeJson(json, response);
            if (!isLast)
            {
                response += ",";
            }
        });
        response += "]}";
        AsyncWebServerResponse *web = request->beginResponse(HTTP_OK, CONTENT_TYPE_JSON, response.c_str());
        addCORS(web);
        request->send(web);
    });

    server.on(API_EVENTS_CLEAR, HTTP_DELETE, [](AsyncWebServerRequest *request) {
        EventManager::GetInstance()->clear();
        AsyncWebServerResponse *web = request->beginResponse(HTTP_NO_CONTENT);
        addCORS(web);
        request->send(web);
    });
}