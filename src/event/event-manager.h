#pragma once

#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>

#include "event/event-type.h"
#include "ac/ac.h"
#include "battery/battery.h"
#include "config/config-manager.h"
#include "gsm/gsm-manager.h"

#define EVENT_DATA "/event.data"

class EventManager
{
protected:
    EventManager() {}
    static EventManager *_manager;
    unsigned long time = 0;
    const unsigned int INTERVAL = 1000;
    bool lowBatteryEventSent = false;

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
    bool clear()
    {
        return SPIFFS.exists(EVENT_DATA) && SPIFFS.remove(EVENT_DATA);
    }
    bool add(EVENT_TYPE type, String date)
    {
        File file = SPIFFS.open(EVENT_DATA, "a");
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
        File file = SPIFFS.open(EVENT_DATA, "r");

        while (file.available())
        {
            file.readStringUntil('\n');
            ++counter;
        }
        return counter;
    }
    void forEach(std::function<void(EVENT_TYPE, std::string, bool)> callback)
    {
        File f = SPIFFS.open(EVENT_DATA, "r");
        while (f.available())
        {
            EVENT_TYPE type = (EVENT_TYPE)f.readStringUntil(',').toInt();
            std::string date = f.readStringUntil('\n').c_str();
            callback(type, date, !f.available());
        }
    }

    void forEach(std::function<void(uint8_t, std::string, bool)> callback)
    {
        File f = SPIFFS.open(EVENT_DATA, "r");
        while (f.available())
        {
            uint8_t type = f.readStringUntil(',').toInt();
            std::string date = f.readStringUntil('\n').c_str();
            callback(type, date, !f.available());
        }
    }

    void loop()
    {
        if ((millis() - time > INTERVAL) && ConfigManager::GetInstance()->isConfigured() == (CONFIGURED_MQTT | CONFIGURED_WIFI | CONFIGURED_GSM))
        {
            time += INTERVAL;
            auto ac = AC::GetInstance();
            auto gsm = GsmManager::GetInstance();
            if (ac->justConnected())
            {
                Serial.printf("%s:%d - Power loss\n", __FILE__, __LINE__);
                add(EVENT_TYPE::POWER_LOSS, gsm->getTimeString());
            }
            else if (ac->justDisconnected())
            {
                Serial.printf("%s:%d - Power on\n", __FILE__, __LINE__);
                add(EVENT_TYPE::POWER_CONNECT, gsm->getTimeString());
            }

            if (!ac->connected() && Battery::GetInstance()->isLow() && !lowBatteryEventSent)
            {
                lowBatteryEventSent = true;
                add(EVENT_TYPE::LOW_BATTERY, gsm->getTimeString());
            }
        }
    }
};

EventManager *EventManager::_manager = nullptr;