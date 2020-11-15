#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include "event/event-type.h"

class Notification
{
    EVENT_TYPE type;
    std::string date;

public:
    Notification(EVENT_TYPE type, const std::string &date)
    {
        this->type = type;
        this->date = date;
    }

    Notification(int type, const std::string &date)
    {
        this->type = (EVENT_TYPE)type;
        this->date = date;
    }

    /**
         * Zwraca reprezentacje w JSON String
         */
    std::string toJSON()
    {
        const size_t size = JSON_OBJECT_SIZE(2);
        DynamicJsonDocument json(size);
        json["type"] = (int)type;
        json["date"] = date;
        String response;
        serializeJson(json, response);
        return response.c_str();
    };
};
