#pragma once
#include <FS.h>
#include <ESPAsyncWebServer.h>
#define TINY_GSM_MODEM_SIM800
#include <TinyGsmClient.h>

#include "config.h"

extern bool acOn;
extern uint16_t batteryVoltage;
extern String gsmNumber;
extern TinyGsm modem;
volatile int8_t networks = 0;

/**
 * Formats network to JSON per ID
 * Outputs to string
 */
void networkToJSONObject(int id, char *string)
{
    snprintf(string, 72, "{\"SSID\":\"%s\",\"dBm\":\"%d\",\"encType\":\"%d\"}", WiFi.SSID(id).c_str(), WiFi.RSSI(id), WiFi.encryptionType(id));
}

void configureWifiEndpoints(AsyncWebServer &server)
{
    /**
   * Skanuje dostępne sieci WiFi
   */
    server.on("/api/wifi/scan", HTTP_GET, [](AsyncWebServerRequest *request) {
        int scanResult = WiFi.scanComplete();
        if (scanResult == WIFI_SCAN_NOT_TRIGGERED)
        {
            WiFi.scanDelete();
            networks = WiFi.scanNetworks(true);
            request->send(200);
        }
        else if (scanResult == WIFI_SCAN_NOT_FINISHED)
        {
            request->send(202);
        }
        else
        {
            request->send(201);
        }
    });

    /**
   * Usuwa obecnie zapisany stan zapisanych sieci
   */
    server.on("/api/wifi/rescan", HTTP_GET, [](AsyncWebServerRequest *request) {
        WiFi.scanDelete();
        networks = WiFi.scanNetworks(true);
        request->send(200);
    });

    /**
   * Zwraca listę sieci WiFi
   */
    server.on("/api/wifi/networks", HTTP_GET, [](AsyncWebServerRequest *request) {
        int scanResult = WiFi.scanComplete();
        if (scanResult >= 0)
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

            request->send(200, "application/json", response);
            delete[] response;
        }
        else if (scanResult == WIFI_SCAN_NOT_FINISHED)
        {
            request->send(204);
        }
        else if (scanResult == WIFI_SCAN_NOT_TRIGGERED)
        {
            request->send(204);
        }
    });

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
   * Łączy do danego STA podanego w formularzu
   */
    server.on("/api/wifi/connect", HTTP_POST, [](AsyncWebServerRequest *request) {
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
        }
        else
        {
            Serial.println("Mamma mia, it's me disconectario");
        }

        uint8_t wifiStatus = WiFi.status();
        char *response = new char[18];
        sprintf(response, "{\"status\":%d}", wifiStatus);
        request->send(200, "application/json", response);
    });

    /**
   * Rozłącza sieć WiFi
   */
    server.on("/api/wifi/disconnect", HTTP_DELETE, [](AsyncWebServerRequest *request) {
        WiFi.disconnect();
        request->send(200, "application/json", "{ \"status\": \"ok\" }");
    });

    /**
   * Zwraca MAC modułu WiFi
   */
    server.on("/api/wifi/mac", HTTP_GET, [](AsyncWebServerRequest *request) {
        String response = "{ \"mac\": \"";
        response += WiFi.macAddress();
        response += "\" }";

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

            if (!saveWiFiconfig(ssid.c_str(), password.c_str()))
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
   * Ogólnie to najchętniej bym te metody przeniósł do innego pliku i tam to wykonywał.
   * Tutaj znajdują się wszystkie endpointy wymagane do działania aplikacji
   */
    server.on("/api/setup/completed", HTTP_GET, [](AsyncWebServerRequest *request) {
        String response = "{ \"status\": \"";
        response += isConfigured();
        response += "\" }";

        request->send(200, "application/json", response);
    });

    /**
   * Zwraca stan baterii w mV
   */
    server.on("/api/battery/status", HTTP_GET, [](AsyncWebServerRequest *request) {
        String response = "{ \"status\": \"";
        response += batteryVoltage;
        response += "\"}";
        request->send(200, "application/json", response);
    });

    /**
   * Zwraca stan zasilania AC
   */
    server.on("/api/ac/status", HTTP_GET, [](AsyncWebServerRequest *request) {
        String response = "{ \"status\": \"";
        response += acOn;
        response += "\" }";
        request->send(200, "application/json", response);
    });

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

            if (!saveGSMconfig(name.c_str(), number.c_str()))
            {
                Serial.printf("%s:%d - %s\n", __FILE__, __LINE__, "did not save gsm config");
            }
        }

        request->send(200, "application/json", "{ \"status\": \"ok\" }");
    });

    /**
   * Zwraca nazwę, nr telefonu zapisany w module
   */
    server.on("/api/gsm/contact", HTTP_GET, [](AsyncWebServerRequest *request) {
        char name[64];
        memset(name, 0, 64);
        char number[18];
        memset(number, 0, 18);

        if (!loadGSMconfig(name, number))
        {
            Serial.printf("%s:%d - %s\n", __FILE__, __LINE__, "couldn't load gsm config");
        }

        String response = "{ \"name\": \"";
        response += name;
        response += "\", \"number\" : \"";
        response += number;
        response += "\" }";

        request->send(200, "application/json", response);
    });

    server.on("/api/gsm/signal", HTTP_GET, [](AsyncWebServerRequest *request) {
        String response = "{ \"status\": \"";
        response += modem.getSignalQuality();
        response += "\" }";
        request->send(200, "application/json", response);
    });

    server.on("/api/gsm/number", HTTP_GET, [](AsyncWebServerRequest *request) {
        String response = "{ \"number\": \"";
        response += gsmNumber;
        response += "\"}";
        request->send(200, "application/json", response);
    });

    /**
   * Zwraca listę powiadomień z modułu
   */
    server.on("/api/notification", HTTP_GET, [](AsyncWebServerRequest *request) {
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
        request->send(200, "application/json", response);
    });

    /**
   * Zwraca frontEnd - czyt. aplikację
   */
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/index.html");
    });

    /**
   * Powinno naprawiać problemy z CORS na niektórych konfiguracjach
   */
    server.on("/api/*", HTTP_OPTIONS, [](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "OK");
        response->addHeader("Access-Control-Allow-Origin", "*");
        response->addHeader("Access-Control-Expose-Headers", "*");
        response->addHeader("Access-Control-Allow-Credentials", "true");
        request->send(response);
    });

    /**
   * Pozwala na serwowanie potrzebnych assetów przygotowanych w pamięci urządzenia
   */
    server.serveStatic("/", SPIFFS, "/");

    server.begin();
}