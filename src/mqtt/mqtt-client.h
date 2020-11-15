#pragma once
#include <PubSubClient.h>
#include <WiFi.h>

#define MQTT_CLIENT_ID "DETECTOR_000"

const std::string MQTT_EVENT_TOPIC = MQTT_CLIENT_ID + std::string("/EVENT");
const std::string MQTT_EVENT_REQUEST_TOPIC = MQTT_CLIENT_ID + std::string("/EVENT/REQUEST");
const std::string MQTT_EVENT_RESPONSE_TOPIC = MQTT_CLIENT_ID + std::string("/EVENT/RESPONSE");
const std::string MQTT_ALIVE_TOPIC = MQTT_CLIENT_ID + std::string("/ALIVE");

class MqttClient
{
    PubSubClient mqtt;

    void subscribeTopics()
    {
        mqtt.subscribe(MQTT_EVENT_REQUEST_TOPIC.c_str());
        Serial.printf("%s:%d - subscribing to:  %s\n", __FILE__, __LINE__, MQTT_EVENT_REQUEST_TOPIC.c_str());
    }

    void callback(char *topic, byte *payload, unsigned int length)
    {
        Serial.print("Message [");
        Serial.print(topic);
        Serial.print("] ");
        for (int i = 0; i < length; i++)
        {
            Serial.print((char)payload[i]);
        }
        Serial.println();
    }

    void reconnect()
    {
        Serial.println("Attempting MQTT connection...");
        if (mqtt.connect(MQTT_CLIENT_ID))
        {
            Serial.println("connected");
            mqtt.publish("outTopic", "Nodemcu connected to MQTT");
            subscribeTopics();
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(mqtt.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }

public:
    MqttClient(Client &client)
    {
        mqtt.setClient(client);
        mqtt.setCallback([this](char *topic, byte *payload, unsigned int length) {
            this->callback(topic, payload, length);
        });
    }

    void keepAlive()
    {
        mqtt.loop();
        if (!mqtt.connected())
        {
            reconnect();
        }
        else
        {
            mqtt.publish(MQTT_ALIVE_TOPIC.c_str(), "I am alive");
        }
    }

    void setServer(const char *domain, uint16_t port)
    {
        mqtt.setServer(domain, port);
    }

    void connect(const char *domain, uint16_t port)
    {
        mqtt.setServer(domain, port);
        mqtt.connect(MQTT_CLIENT_ID);

        if (mqtt.connected())
        {
            Serial.println("connected to MQTT");
            subscribeTopics();
            mqtt.publish(MQTT_EVENT_TOPIC.c_str(), "connected to MQTT");
        }
        else
        {
            Serial.printf("%s:%d - Cannot connect MQTT, state %d\n", __FILE__, __LINE__, mqtt.state());
        }
    }

    void printDiagnostic()
    {
        Serial.printf("%s:%d - MQTT state %d\n", __FILE__, __LINE__, mqtt.state());
        Serial.printf("%s:%d - MQTT connected %s\n", __FILE__, __LINE__, mqtt.connected() ? "YES" : "NO");
    }
};
