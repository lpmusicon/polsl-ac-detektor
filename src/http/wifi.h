#pragma once

#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#include "http/helpers.h"
#include "config/config-manager.h"
#include "wifi/wifi-manager.h"

#define API_WIFI_SCAN "/api/wifi/scan"
#define API_WIFI_RESCAN "/api/wifi/rescan"
#define API_WIFI_CONNECT "/api/wifi/connect"
#define API_WIFI_STATUS "/api/wifi/status"
#define API_WIFI_NETWORKS "/api/wifi/networks"

#define MINIMUM_WIFI_NETWORK_RESPONSE_SIZE 30
#define MAXIMUM_WIFI_NETWORK_OBJECT_SIZE 72

#define WIFI_SCAN_NOT_TRIGGERED -2
#define WIFI_SCAN_NOT_FINISHED -1

volatile int8_t networks = 0;

void configureWifiEndpoints(AsyncWebServer &server)
{
    server.on(API_WIFI_SCAN, HTTP_POST, [](AsyncWebServerRequest *request) {
        int scanResult = WiFi.scanComplete();
        switch (scanResult)
        {
        case WIFI_SCAN_NOT_TRIGGERED:
        {
            WiFi.scanDelete();
            networks = WiFi.scanNetworks(true);
            AsyncWebServerResponse *web = request->beginResponse(HTTP_OK);
            addCORS(web);
            request->send(web);
            break;
        }
        case WIFI_SCAN_NOT_FINISHED:
        {
            AsyncWebServerResponse *web = request->beginResponse(HTTP_ACCEPTED);
            addCORS(web);
            request->send(web);
            break;
        }
        default:
        {
            AsyncWebServerResponse *web = request->beginResponse(HTTP_NO_CONTENT);
            addCORS(web);
            request->send(web);
            break;
        }
        }
    });

    server.on(API_WIFI_RESCAN, HTTP_POST, [](AsyncWebServerRequest *request) {
        WiFi.scanDelete();
        networks = WiFi.scanNetworks(true);
        AsyncWebServerResponse *web = request->beginResponse(HTTP_OK);
        addCORS(web);
        request->send(web);
    });

    server.on(API_WIFI_CONNECT, HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("ssid", true) && request->hasParam("password", true))
        {
            AsyncWebParameter *paramSsid = request->getParam("ssid", true);
            AsyncWebParameter *paramPassword = request->getParam("password", true);

            String ssid = paramSsid->value();
            String password = paramPassword->value();

            WifiManager::GetInstance()->tryConnect(ssid, password);

            String response;
            const size_t size = JSON_OBJECT_SIZE(1);
            DynamicJsonDocument json(size);
            json["status"] = WiFi.status();
            serializeJson(json, response);

            AsyncWebServerResponse *web = request->beginResponse(HTTP_OK, CONTENT_TYPE_JSON, response);
            addCORS(web);
            request->send(web);
        }
        else
        {
            AsyncWebServerResponse *web = request->beginResponse(HTTP_BAD_REQUEST, "text/plain", "");
            addCORS(web);
            request->send(web);
        }
    });

    server.on(API_WIFI_STATUS, HTTP_GET, [](AsyncWebServerRequest *request) {
        std::string response;
        const size_t size = JSON_OBJECT_SIZE(1);
        DynamicJsonDocument json(size);
        json["status"] = WiFi.status();
        serializeJson(json, response);

        AsyncWebServerResponse *web = request->beginResponse(HTTP_OK, CONTENT_TYPE_JSON, response.c_str());
        addCORS(web);
        request->send(web);
    });

    server.on(API_WIFI_NETWORKS, HTTP_GET, [](AsyncWebServerRequest *request) {
        int scanResult = WiFi.scanComplete();
        switch (scanResult)
        {
        case WIFI_SCAN_NOT_FINISHED:
        case WIFI_SCAN_NOT_TRIGGERED:
        {
            AsyncWebServerResponse *web = request->beginResponse(HTTP_NO_CONTENT);
            addCORS(web);
            request->send(web);
            break;
        }
        default:
        {
            uint16_t responseSize = MINIMUM_WIFI_NETWORK_RESPONSE_SIZE + (MAXIMUM_WIFI_NETWORK_OBJECT_SIZE + 1) * scanResult + 1;
            char *response = new char[responseSize];
            memset(response, 0, responseSize);

            strcat(response, "{\"status\":\"OK\",\"networks\":[");

            for (int i = 0; i < scanResult; i++)
            {
                char *object = new char[MAXIMUM_WIFI_NETWORK_OBJECT_SIZE];
                networkToJSONObject(i, object);
                strcat(response, object);

                // Nie można dodać przecinka do ostatniego, więc sprawdzam
                if (i + 1 < scanResult)
                {
                    strcat(response, ",");
                }

                delete[] object;
            }

            strcat(response, "]}");

            AsyncWebServerResponse *web = request->beginResponse(HTTP_OK, CONTENT_TYPE_JSON, response);
            addCORS(web);
            request->send(web);
            delete[] response;
            break;
        }
        }

        AsyncWebServerResponse *web = request->beginResponse(HTTP_NO_CONTENT);
        addCORS(web);
        request->send(web);
    });
}