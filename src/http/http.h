#pragma once
#include <ESPAsyncWebServer.h>
#include "entries.h"
#include "mqtt.h"
#include "setup.h"
#include "wifi.h"

void configureHttpServer(AsyncWebServer &server)
{
    configureEntriesEndpoints(server);
    configureMqttEndpoints(server);
    configureSetupEndpoints(server);
    configureWifiEndpoints(server);

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
        AsyncWebServerResponse *response = request->beginResponse(HTTP_OK);
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