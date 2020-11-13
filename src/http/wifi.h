#pragma once

#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#include "helpers.h"
#include "../config.h"

#define API_WIFI_SCAN "/api/wifi/scan"
#define API_WIFI_RESCAN "/api/wifi/rescan"
#define API_WIFI_CONNECT "/api/wifi/connect"
#define API_WIFI_STATUS "/api/wifi/status"
#define API_WIFI_NETWORKS "/api/wifi/networks"

volatile int8_t networks = 0;

void configureWifiEndpoints(AsyncWebServer &server)
{
    server.on(API_WIFI_SCAN, HTTP_POST, [](AsyncWebServerRequest *request) {
        int scanResult = WiFi.scanComplete();
        switch (scanResult)
        {
        case WIFI_SCAN_NOT_TRIGGERED:
            WiFi.scanDelete();
            networks = WiFi.scanNetworks(true);
            request->send(HTTP_OK);
            break;
        case WIFI_SCAN_NOT_FINISHED:
            request->send(HTTP_ACCEPTED);
            break;
        default:
            request->send(HTTP_NO_CONTENT);
            break;
        }
    });

    server.on(API_WIFI_RESCAN, HTTP_POST, [](AsyncWebServerRequest *request) {
        WiFi.scanDelete();
        networks = WiFi.scanNetworks(true);
        request->send(HTTP_OK);
    });

    server.on(API_WIFI_CONNECT, HTTP_POST, [](AsyncWebServerRequest *request) {
        if (WiFi.isConnected())
            WiFi.disconnect();

        String ssid;
        String password;

        if (request->hasParam("ssid", true) && request->hasParam("password", true))
        {
            AsyncWebParameter *pssid = request->getParam("ssid", true);
            AsyncWebParameter *ppassword = request->getParam("password", true);

            ssid = pssid->value();
            password = ppassword->value();

            Serial.printf("%s:%d - SSID: %s, PASSWORD: %s\n", __FILE__, __LINE__, ssid.c_str(), password.c_str());

            WiFi.persistent(false);
            WiFi.begin(ssid.c_str(), password.c_str());

            String response;
            const size_t size = JSON_OBJECT_SIZE(1);
            DynamicJsonDocument json(size);
            json["status"] = WiFi.status();
            serializeJson(json, response);
            request->send(HTTP_OK, CONTENT_TYPE_JSON, response);
        }
        else
        {
            request->send(HTTP_BAD_REQUEST);
        }
    });

    server.on(API_WIFI_STATUS, HTTP_GET, [](AsyncWebServerRequest *request) {
        String response;
        const size_t size = JSON_OBJECT_SIZE(1);
        DynamicJsonDocument json(size);
        json["status"] = WiFi.status();
        serializeJson(json, response);
        request->send(HTTP_OK, CONTENT_TYPE_JSON, response);
    });

    server.on(API_WIFI_NETWORKS, HTTP_GET, [](AsyncWebServerRequest *request) {
        int scanResult = WiFi.scanComplete();
        switch (scanResult)
        {
        case WIFI_SCAN_NOT_FINISHED:
        case WIFI_SCAN_NOT_TRIGGERED:
            request->send(HTTP_NO_CONTENT);
            break;
        default:
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

            request->send(200, "application/json", response);
            delete[] response;
            break;
        }

        request->send(HTTP_OK, CONTENT_TYPE_JSON);
    });
}