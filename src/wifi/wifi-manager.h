#pragma once

#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include "config.h"

class WifiManager
{
protected:
    char SSID[33];
    char PASSWORD[64];
    char AP_SSID[33];
    char AP_PASSWORD[64];

    WifiManager()
    {
        WiFi.persistent(false);
        WiFi.setAutoReconnect(false);
        WiFi.mode(WIFI_AP_STA);
        WiFi.softAPdisconnect();
        WiFi.disconnect();

        if (loadWiFiconfig(SSID, PASSWORD))
        {
            WiFi.begin(SSID, PASSWORD);
            Serial.printf("%s:%d - SSID %s, PASSWORD %s\n", __FILE__, __LINE__, SSID, PASSWORD);
            while (WiFi.status() != WL_CONNECTED)
            {
                Serial.println("Connecting");
                delay(500);
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
        SPIFFS.remove(WIFI_CONFIG);
        WiFi.disconnect();
        WiFi.softAPdisconnect();
    }

    /**
 * Odczytuje SSID i hasło z pliku do zmiennych
 */
    bool loadWiFiconfig(char *SSID, char *PASSWORD)
    {
        if (!SPIFFS.exists(WIFI_CONFIG))
        {
            return false;
        }

        File config = SPIFFS.open(WIFI_CONFIG, "r");
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

    /**
 * Zapisuje SSID i Hasło do pliku
 */
    bool saveWiFiconfig(const char *const SSID, const char *const PASSWORD)
    {
        File config = SPIFFS.open(WIFI_CONFIG, "w");

        // Error inside SPIFFS
        if (!config)
        {
            Serial.println("config.cpp, saveWiFi failed");
            return false;
        }

        config.print(SSID);
        config.print('\n');
        config.print(PASSWORD);
        config.print('\n');

        config.close();

        return true;
    }
};

WifiManager *WifiManager::_manager = nullptr;
