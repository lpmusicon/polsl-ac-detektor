#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "event/event-manager.h"
#include "wifi/wifi-manager.h"
#include "gsm/gsm-manager.h"
#include "config/config-manager.h"

#define BUTTON_PIN 5
#define BUTTON_RESET_TIME_THRESHOLD 5000

class ResetManager
{
protected:
    uint16_t time = 0;
    static ResetManager *_manager;
    ResetManager()
    {
        pinMode(BUTTON_PIN, INPUT_PULLUP);
    }

public:
    ResetManager(ResetManager &o) = delete;
    ResetManager operator=(const ResetManager &) = delete;
    static ResetManager *GetInstance()
    {
        if (_manager == nullptr)
        {
            _manager = new ResetManager();
        }
        return _manager;
    }
    void check()
    {
        bool clicked = !digitalRead(BUTTON_PIN);
        if (clicked)
        {
            if (millis() - time > BUTTON_RESET_TIME_THRESHOLD)
            {
                ConfigManager::GetInstance()->clear();
                WifiManager::GetInstance()->clear();
                EventManager::GetInstance()->clear();
                ESP.restart();
            }
        }
        else
        {
            time = millis();
        }
    }
};

ResetManager *ResetManager::_manager = nullptr;