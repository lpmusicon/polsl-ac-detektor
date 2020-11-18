#pragma once

#include <ESPAsyncWebServer.h>
#include "http/helpers.h"
#include "mqtt/mqtt-client.h"

#define API_MQTT_STATUS "/api/mqtt/status"
#define API_MQTT_CONNECT "/api/mqtt/connect"

void configureMqttEndpoints(AsyncWebServer &server)
{
    server.on(API_MQTT_STATUS, HTTP_GET, [](AsyncWebServerRequest *request) {
        std::string response;
        const size_t size = JSON_OBJECT_SIZE(1);
        DynamicJsonDocument json(size);
        json["status"] = MqttClient::GetInstance()->connected();
        serializeJson(json, response);

        AsyncWebServerResponse *web = request->beginResponse(HTTP_OK, CONTENT_TYPE_JSON, response.c_str());
        addCORS(web);
        request->send(web);
    });

    server.on(API_MQTT_CONNECT, HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("server", true) && request->hasParam("user", true) && request->hasParam("password", true) && request->hasParam("port", true))
        {
            AsyncWebParameter *paramServer = request->getParam("server", true);
            AsyncWebParameter *paramUser = request->getParam("user", true);
            AsyncWebParameter *paramPassword = request->getParam("password", true);
            AsyncWebParameter *paramPort = request->getParam("port", true);

            String server = paramServer->value();
            String user = paramUser->value();
            String password = paramPassword->value();
            String port = paramPort->value();

            Serial.printf("%s:%d\n [MQTT] Server: %s\n [MQTT] Password: %s\n [MQTT] User: %s\n [MQTT] Port: %s\n", __FILE__, __LINE__, server.c_str(), password.c_str(), user.c_str(), port.c_str());

            MqttClient::GetInstance()->tryConnect(server.c_str(), port.toInt(), user.c_str(), password.c_str());

            AsyncWebServerResponse *web = request->beginResponse(HTTP_NO_CONTENT);
            addCORS(web);
            request->send(web);
        }
        else
        {
            AsyncWebServerResponse *web = request->beginResponse(HTTP_BAD_REQUEST);
            addCORS(web);
            request->send(web);
        }
    });
}