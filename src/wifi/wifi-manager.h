#pragma once

#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include <WiFi.h>

#include "config/config-manager.h"

#define SOFT_AP_WIFI_SSID "DETECTOR_SETUP"
#define SOFT_AP_WIFI_PASSWORD "12345678"

class WifiManager
{
protected:
    char SSID[33];
    char PASSWORD[64];
    char AP_SSID[33];
    char AP_PASSWORD[64];

    wifi_event_id_t eventId;

    WifiManager()
    {
        WiFi.persistent(false);
        WiFi.setAutoReconnect(false);
        WiFi.mode(WIFI_MODE_APSTA);
        WiFi.softAPdisconnect();
        WiFi.disconnect();

        if (loadWiFiconfig(SSID, PASSWORD))
        {
            WiFi.begin(SSID, PASSWORD);
            Serial.printf("%s:%d - SSID %s, PASSWORD %s\n", __FILE__, __LINE__, SSID, PASSWORD);
            int retries = 0;
            while (WiFi.status() != WL_CONNECTED)
            {
                Serial.println("Connecting");
                delay(500);
                if (retries++ > 10)
                {
                    break;
                }
            }
            Serial.printf("%s:%d - Connected WiFi IP: %s\n", __FILE__, __LINE__, WiFi.localIP().toString().c_str());
        }

        loadDefaultWiFiconfig(AP_SSID, AP_PASSWORD);
        Serial.printf("%s:%d - AP SSID %s, AP PASSWORD %s\n", __FILE__, __LINE__, AP_SSID, AP_PASSWORD);
        WiFi.softAP(AP_SSID, AP_PASSWORD);
    }
    static WifiManager *_manager;

public:
    WifiManager(WifiManager &o) = delete;
    WifiManager operator=(const WifiManager &) = delete;
    static WifiManager *GetInstance()
    {
        if (_manager == nullptr)
        {
            _manager = new WifiManager();
        }
        return _manager;
    }
    void clear()
    {
        WiFi.disconnect();
        WiFi.softAPdisconnect();
    }

    void tryConnect(String ssid, String password)
    {
        WiFi.removeEvent(eventId);

        WiFi.persistent(false);

        if (WiFi.isConnected())
        {
            WiFi.disconnect();
        }

        WiFi.begin(ssid.c_str(), password.c_str());

        auto onWiFiConnect = [ssid, password](WiFiEvent_t event, WiFiEventInfo_t info) {
            Serial.printf("%s:%d - [WiFi] SSID: %s PASS: %s\n", __FILE__, __LINE__, ssid.c_str(), password.c_str());
            Serial.printf("%s:%d - [WiFi] IP: %s\n", __FILE__, __LINE__, WiFi.localIP().toString().c_str());
            auto config = ConfigManager::GetInstance();
            if (!config->saveSTA(ssid, password))
            {
                Serial.printf("%s:%d - [WiFi] Cannot save STA\n", __FILE__, __LINE__);
            }
            else
            {
                Serial.printf("%s:%d - [WiFi] Saved STA\n", __FILE__, __LINE__);
            }
        };

        eventId = WiFi.onEvent(onWiFiConnect, WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);
    }

    /**
 * Odczytuje SSID i hasło z pliku do zmiennych
 */
    bool loadWiFiconfig(char *SSID, char *PASSWORD)
    {
        if (!SPIFFS.exists(ConfigManager::getWifiConfigPath().c_str()))
        {
            return false;
        }

        File config = SPIFFS.open(ConfigManager::getWifiConfigPath().c_str(), "r");
        if (!config)
        {
            return false;
        }

        String localSSID = config.readStringUntil('\n');
        String localPassword = config.readStringUntil('\n');

        strcpy(SSID, localSSID.c_str());
        strcpy(PASSWORD, localPassword.c_str());

        config.close();

        Serial.print("lwc SSID: ");
        Serial.println(SSID);
        Serial.print("lwc Password: ");
        Serial.println(PASSWORD);

        return true;
    }

    /**
 * Kopiuje domyslne zmienne do SSID i HASLA
 */
    void loadDefaultWiFiconfig(char *SSID, char *PASSWORD)
    {
        strcpy(SSID, SOFT_AP_WIFI_SSID);
        strcpy(PASSWORD, SOFT_AP_WIFI_PASSWORD);
    }
};

WifiManager *WifiManager::_manager = nullptr;
