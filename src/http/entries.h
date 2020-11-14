#pragma once

#include <FS.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#include "helpers.h"
#include "../config.h"

#define API_ENTRIES "/api/entries"
#define API_ENTRIES_CLEAR "/api/entries/clear"

void configureEntriesEndpoints(AsyncWebServer &server)
{
    server.on(API_ENTRIES, HTTP_GET, [](AsyncWebServerRequest *request) {
        File f = SPIFFS.open(NOTIFICATION_DATA, "r");
        String response = "{\"entries\":[";

        while (f.available())
        {
            int type = f.readStringUntil(',').toInt();
            String date = f.readStringUntil('\n');
            Notification n(type, date);
            response += n.toJSON();
            if (f.available())
            {
                response += ",";
            }
        }

        response += "]}";

        request->send(HTTP_OK, CONTENT_TYPE_JSON, response);
    });

    server.on(API_ENTRIES_CLEAR, HTTP_DELETE, [](AsyncWebServerRequest *request) {
        if (!SPIFFS.remove(NOTIFICATION_DATA))
        {
            request->send(HTTP_SERVER_ERROR);
        }

        request->send(HTTP_NO_CONTENT);
    });
}