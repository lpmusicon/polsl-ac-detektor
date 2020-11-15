#pragma once

#include <FS.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#include "helpers.h"
#include "../config.h"
#include "event/event-manager.h"

#define API_EVENTS "/api/entries"
#define API_EVENTS_CLEAR "/api/entries/clear"

void configureEventEndpoints(AsyncWebServer &server)
{
    server.on(API_EVENTS, HTTP_GET, [](AsyncWebServerRequest *request) {
        File f = SPIFFS.open(NOTIFICATION_DATA, "r");
        std::string response = "{\"entries\":[";
        EventManager::GetInstance()->forEach([&response](uint8_t type, std::string date, bool isLast) {
            Notification n(type, date);
            response += n.toJSON();
            if (!isLast)
            {
                response += ",";
            }
        });
        response += "]}";

        request->send(HTTP_OK, CONTENT_TYPE_JSON, response.c_str());
    });

    server.on(API_EVENTS_CLEAR, HTTP_DELETE, [](AsyncWebServerRequest *request) {
        if (!SPIFFS.remove(NOTIFICATION_DATA))
        {
            request->send(HTTP_SERVER_ERROR);
        }
        request->send(HTTP_NO_CONTENT);
    });
}