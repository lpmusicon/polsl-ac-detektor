#pragma once
#include <FS.h>
#include <ESPAsyncWebServer.h>
#define TINY_GSM_MODEM_SIM800
#include <TinyGsmClient.h>
#include <ArduinoJson.h>

#include "config.h"
#include "wifi/wifi-manager.h"
#include "gsm/gsm-manager.h"

extern bool acOn;

extern TinyGsm modem;

void configureWifkiiEndpoints(AsyncWebServer &server)
{
    /**
   * Zwraca status modułu WiFi
   * Zgodny z enum ESP32 WiFi Status
   */
    server.on("/api/wifi/status", HTTP_GET, [](AsyncWebServerRequest *request) {
        uint8_t wifiStatus = WiFi.status();
        char *response = new char[18];
        sprintf(response, "{\"status\":%d}", wifiStatus);
        request->send(200, "application/json", response);
        delete[] response;
    });

    /**
   * Zwraca obecnie podłączone SSID
   */
    server.on("/api/wifi/current", HTTP_GET, [](AsyncWebServerRequest *request) {
        String response = "{\"ssid\":\"";
        response += WiFi.SSID();
        response += "\"}";
        request->send(200, "application/json", response);
    });

    /**
   * Zapisuje SSID/Hasło WiFi w pamięci Flash modułu
   */
    server.on("/api/wifi/save", HTTP_POST, [](AsyncWebServerRequest *request) {
        String ssid, password;

        if (request->hasParam("ssid", true) && request->hasParam("password", true))
        {
            AsyncWebParameter *pssid = request->getParam("ssid", true);
            AsyncWebParameter *ppassword = request->getParam("password", true);
            ssid = pssid->value();
            password = ppassword->value();

            Serial.printf("%s:%d - SSID: %s, PASSWORD: %s\n", __FILE__, __LINE__, ssid.c_str(), password.c_str());

            if (!WifiManager::GetInstance()->saveWiFiconfig(ssid.c_str(), password.c_str()))
            {
                Serial.println("DID NOT SAVE WIFI CONFIG");
            }
        }

        request->send(200, "application/json", "{ \"status\": \"OK\" }");
    });
}

void configureEndpoints(AsyncWebServer &server)
{
    /**
   * Zapisuje kontakt GSM w pamięci Flash
   */
    server.on("/api/gsm/contact", HTTP_POST, [](AsyncWebServerRequest *request) {
        String name, number;

        if (request->hasParam("name", true) && request->hasParam("number", true))
        {
            AsyncWebParameter *pname = request->getParam("name", true);
            AsyncWebParameter *pnumber = request->getParam("number", true);
            name = pname->value();
            number = pnumber->value();

            Serial.println(name);
            Serial.println(number);

            if (!GsmManager::GetInstance()->saveGSMconfig(name.c_str(), number.c_str()))
            {
                Serial.printf("%s:%d - %s\n", __FILE__, __LINE__, "did not save gsm config");
            }
        }

        request->send(200, "application/json", "{ \"status\": \"ok\" }");
    });

    server.on("/api/gsm/signal", HTTP_GET, [](AsyncWebServerRequest *request) {
        String response = "{ \"status\": \"";
        response += modem.getSignalQuality();
        response += "\" }";
        request->send(200, "application/json", response);
    });
}