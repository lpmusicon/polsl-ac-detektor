#pragma once

#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>

#include "event/event-type.h"

#define NOTIFICATION_DATA "/notification.data"

class EventManager
{
protected:
    EventManager() {}
    static EventManager *_manager;

public:
    EventManager(EventManager &o) = delete;
    EventManager operator=(const EventManager &) = delete;
    static EventManager *GetInstance()
    {
        if (_manager == nullptr)
        {
            _manager = new EventManager();
        }
        return _manager;
    }
    void clear() { SPIFFS.remove(NOTIFICATION_DATA); }
    bool add(EVENT_TYPE type, String date)
    {
        File file = SPIFFS.open(NOTIFICATION_DATA, "a");
        if (!file)
        {
            return false;
        }

        file.printf("%d,%s\n", (int)type, date.c_str());
        file.close();

        return true;
    }
    uint8_t length()
    {
        uint8_t counter = 0;
        File file = SPIFFS.open(NOTIFICATION_DATA, "r");

        while (file.available())
        {
            file.readStringUntil('\n');
            ++counter;
        }
        return counter;
    }
    void forEach(std::function<void(EVENT_TYPE, std::string, bool)> callback)
    {
        File f = SPIFFS.open(NOTIFICATION_DATA, "r");
        while (f.available())
        {
            EVENT_TYPE type = (EVENT_TYPE)f.readStringUntil(',').toInt();
            std::string date = f.readStringUntil('\n').c_str();
            callback(type, date, !f.available());
        }
    }

    void forEach(std::function<void(uint8_t, std::string, bool)> callback)
    {
        File f = SPIFFS.open(NOTIFICATION_DATA, "r");
        while (f.available())
        {
            uint8_t type = f.readStringUntil(',').toInt();
            std::string date = f.readStringUntil('\n').c_str();
            callback(type, date, !f.available());
        }
    }
};

EventManager *EventManager::_manager = nullptr;