#pragma once
#include <PubSubClient.h>
#include <WiFi.h>
#include <FS.h>
#include "config/config-manager.h"
#include "event/event-type.h"
#include "event/event-manager.h"
#include "mqtt/mqtt-request-type.h"

#define MQTT_CLIENT_ID "DETECTOR_000"

const std::string MQTT_EVENT_REQUEST_TOPIC = MQTT_CLIENT_ID + std::string("/EVENT/REQUEST");
const std::string MQTT_EVENT_RESPONSE_TOPIC = MQTT_CLIENT_ID + std::string("/EVENT/RESPONSE");

class MqttClient
{
protected:
    static MqttClient *_client;
    PubSubClient mqtt;
    std::vector<std::pair<std::string, std::string>> queue;
    const int KEEP_ALIVE_TIME_MS = 500;
    unsigned long timeKeepAlive = 0;

    bool tryNewConnection = false;
    std::string domain = "";
    uint16_t port = 1883;
    std::string user = "";
    std::string password = "";

    void subscribeTopics()
    {
        mqtt.subscribe(MQTT_EVENT_REQUEST_TOPIC.c_str());
        Serial.printf("%s:%d - subscribing to:  %s\n", __FILE__, __LINE__, MQTT_EVENT_REQUEST_TOPIC.c_str());
    }

    bool isRequestTopic(const char *topic)
    {
        return strcmp(topic, MQTT_EVENT_REQUEST_TOPIC.c_str()) == 0;
    }

    void handleJsonOk(const JsonVariant &type)
    {
        if (!type.isNull() && type.is<uint8_t>())
        {
            MqttRequestType requestType = (MqttRequestType)type.as<uint8_t>();
            switch (requestType)
            {
            case MqttRequestType::EVENT_ALL:
                handleEventAll();
                break;
            case MqttRequestType::EVENT_CLEAR:
                handleEventClear();
                break;
            default:
                handleEventUnknown();
                break;
            }
        }
        else
        {
            Serial.printf("%s:%d - Error, Type invalid\n", __FILE__, __LINE__);
            DynamicJsonDocument json(JSON_OBJECT_SIZE(1));
            json["result"] = "Invalid Type";
            std::string response;
            serializeJson(json, response);
            this->queue.emplace_back(MQTT_EVENT_RESPONSE_TOPIC, response);
        }
    }

    void handleJsonInvalidInput()
    {
        Serial.printf("%s:%d - Invalid Input\n", __FILE__, __LINE__);
        DynamicJsonDocument json(JSON_OBJECT_SIZE(1));
        json["result"] = "Invalid Input";
        std::string response;
        serializeJson(json, response);
        this->queue.emplace_back(MQTT_EVENT_RESPONSE_TOPIC, response);
    }

    void handleJsonNoMemory()
    {
        Serial.printf("%s:%d - Not Enough Memory\n", __FILE__, __LINE__);
        DynamicJsonDocument json(JSON_OBJECT_SIZE(1));
        json["result"] = "Not enough memory";
        std::string response;
        serializeJson(json, response);
        this->queue.emplace_back(MQTT_EVENT_RESPONSE_TOPIC, response);
    }

    void handleJsonIncompleteInput()
    {
        Serial.printf("%s:%d - Incomplete Json\n", __FILE__, __LINE__);
        DynamicJsonDocument json(JSON_OBJECT_SIZE(1));
        json["result"] = "Incomplete Input";
        std::string response;
        serializeJson(json, response);
        this->queue.emplace_back(MQTT_EVENT_RESPONSE_TOPIC, response);
    }

    void handleJsonUnknown()
    {
        Serial.printf("%s:%d - Serialization Error\n", __FILE__, __LINE__);
        DynamicJsonDocument json(JSON_OBJECT_SIZE(1));
        json["result"] = "Uknown Error";
        std::string response;
        serializeJson(json, response);
        this->queue.emplace_back(MQTT_EVENT_RESPONSE_TOPIC, response);
    }

    void handleEventClear()
    {
        auto eventMan = EventManager::GetInstance();
        eventMan->clear();
        DynamicJsonDocument json(JSON_OBJECT_SIZE(1));
        json["result"] = "Clearing Notifications";
        std::string response;
        serializeJson(json, response);
        this->queue.emplace_back(MQTT_EVENT_RESPONSE_TOPIC, response);
    }

    void handleEventAll()
    {
        auto eventMan = EventManager::GetInstance();
        if (eventMan->length() > 0)
        {
            eventMan->forEach([this](EVENT_TYPE type, std::string date, bool isLast) {
                std::string response;
                const size_t size = JSON_OBJECT_SIZE(2);
                DynamicJsonDocument json(size);
                json["type"] = (uint8_t)type;
                json["date"] = date;
                serializeJson(json, response);
                this->queue.emplace_back(MQTT_EVENT_RESPONSE_TOPIC, response);
            });
        }
        else
        {
            DynamicJsonDocument json(JSON_OBJECT_SIZE(1));
            json["result"] = "No Notifications";
            std::string response;
            serializeJson(json, response);
            this->queue.emplace_back(MQTT_EVENT_RESPONSE_TOPIC, response);
        }
    }

    void handleEventUnknown()
    {
        DynamicJsonDocument json(JSON_OBJECT_SIZE(1));
        json["result"] = "Unknown Event Type";
        std::string response;
        serializeJson(json, response);
        this->queue.emplace_back(MQTT_EVENT_RESPONSE_TOPIC, response);
    }

    void callback(char *topic, byte *payload, unsigned int length)
    {
        // DEBUG MSG
        char *message = new char[length + 1];
        memset(message, 0, length + 1);
        memcpy(message, payload, length);
        Serial.printf("%s:%d - [%s] of Length %d:  %s\n", __FILE__, __LINE__, topic, length, message);
        delete message;

        if (isRequestTopic(topic))
        {
            DynamicJsonDocument filter(JSON_OBJECT_SIZE(1));
            filter["type"] = true;

            size_t jsonSize = JSON_OBJECT_SIZE(1) + length;
            DynamicJsonDocument json(jsonSize);

            auto result = deserializeJson(json, payload, length, DeserializationOption::Filter(filter));
            switch (result.code())
            {
            case DeserializationError::Ok:
                handleJsonOk(json["type"]);
                break;
            case DeserializationError::InvalidInput:
                handleJsonInvalidInput();
                break;
            case DeserializationError::NoMemory:
                handleJsonNoMemory();
                break;
            case DeserializationError::IncompleteInput:
                handleJsonIncompleteInput();
                break;
            default:
                handleJsonUnknown();
                break;
            }
        }
    }

    void reconnect()
    {
        Serial.println("Attempting MQTT connection...");

        if (mqtt.connect(MQTT_CLIENT_ID, user.c_str(), password.c_str()))
        {
            Serial.println("connected");
            subscribeTopics();
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(mqtt.state());
            Serial.println(" try again in 5 seconds");
        }
    }

    void handleQueue()
    {
        if (!queue.empty())
            Serial.printf("%s:%d - Queue size: %d\n", __FILE__, __LINE__, queue.size());

        if (!mqtt.connected())
        {
            return;
        }

        while (!queue.empty())
        {
            auto element = queue.back();
            if (!mqtt.publish(element.first.c_str(), element.second.c_str()))
            {
                Serial.println("Cannot publish! Break loop to yield");
                break;
            }
            else
            {
                queue.pop_back();
            }
        }
    }

    MqttClient()
    {
        mqtt.setCallback([this](char *topic, byte *payload, unsigned int length) {
            this->callback(topic, payload, length);
        });
    }

public:
    MqttClient(MqttClient &o) = delete;
    MqttClient operator=(const MqttClient &) = delete;

    static MqttClient *GetInstance()
    {
        if (_client == nullptr)
        {
            _client = new MqttClient();
        }
        return _client;
    }

    void setClient(Client &client)
    {
        mqtt.setClient(client);
    }

    void keepAlive()
    {
        auto configManager = ConfigManager::GetInstance();

        if (configManager->mqttEnabled() && !mqtt.connected())
        {
            reconnect();
        }

        if (configManager->mqttEnabled() && (millis() - timeKeepAlive > KEEP_ALIVE_TIME_MS))
        {
            timeKeepAlive += KEEP_ALIVE_TIME_MS;
            mqtt.loop();
            handleQueue();
        }
    }

    void connect()
    {
        auto configManager = ConfigManager::GetInstance();
        if (configManager->mqttEnabled())
        {
            domain = configManager->getMqttServerIP();
            port = configManager->getMqttServerPort();
            user = configManager->getMqttServerUser();
            password = configManager->getMqttServerPassword();

            mqtt.setServer(domain.c_str(), port);
            mqtt.connect(MQTT_CLIENT_ID, user.c_str(), password.c_str());

            if (mqtt.connected())
            {
                Serial.println("connected to MQTT");
                subscribeTopics();
            }
            else
            {
                Serial.printf("%s:%d - Cannot connect MQTT, state %d\n", __FILE__, __LINE__, mqtt.state());
            }
        }
    }

    void checkNewConnection()
    {
        if (tryNewConnection)
        {
            mqtt.setServer(domain.c_str(), port);
            mqtt.connect(MQTT_CLIENT_ID, user.c_str(), password.c_str());
            if (mqtt.connected())
            {
                Serial.println("connected to MQTT");
                ConfigManager::GetInstance()->saveMQTT(domain, port, user, password);
                ConfigManager::GetInstance()->refresh();
            }
            tryNewConnection = false;
        }
    }

    void tryConnect(std::string domain, uint16_t port, std::string user, std::string password)
    {
        tryNewConnection = true;
        this->domain = domain;
        this->port = port;
        this->user = user;
        this->password = password;
    }

    bool connected()
    {
        return mqtt.connected();
    }

    void enqueue(std::pair<EVENT_TYPE, std::string> pair)
    {
        std::string response;
        const size_t size = JSON_OBJECT_SIZE(2);
        DynamicJsonDocument json(size);
        json["type"] = (uint8_t)pair.first;
        json["date"] = pair.second;
        serializeJson(json, response);
        queue.emplace_back(MQTT_EVENT_RESPONSE_TOPIC, response);
    }
};

MqttClient *MqttClient::_client = nullptr;
