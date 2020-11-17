#pragma once

#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>

#define CONFIGURED_WIFI 1
#define CONFIGURED_MQTT 2
#define CONFIGURED_GSM 4

#define WIFI_STA_CONFIG "/wifi.sta.config"
#define WIFI_AP_CONFIG "/wifi.ap.config"
#define MQTT_CONFIG "/mqtt.config"
#define GSM_CONFIG "/gsm.config"

class ConfigManager
{
protected:
    static ConfigManager *_manager;
    std::string mqttServer = "";
    std::string mqttUser = "";
    uint16_t mqttPort = 1883;
    std::string mqttPassword = "";

    std::string wifiSsid = "";
    std::string wifiPassword = "";
    std::string wifiMac = "";

    ConfigManager()
    {
        SPIFFS.begin();
        refresh();
    }

public:
    ConfigManager(ConfigManager &o) = delete;
    ConfigManager operator=(const ConfigManager &) = delete;
    static ConfigManager *GetInstance()
    {
        if (_manager == nullptr)
        {
            _manager = new ConfigManager();
        }
        return _manager;
    }

    void refresh()
    {
        if (SPIFFS.exists(WIFI_STA_CONFIG))
        {
            File f = SPIFFS.open(WIFI_STA_CONFIG);

            if (f.available())
            {
                wifiSsid = f.readStringUntil('\n').c_str();
                if (f.available())
                {
                    wifiPassword = f.readStringUntil('\n').c_str();
                }
            }

            f.close();
        }

        if (SPIFFS.exists(MQTT_CONFIG))
        {
            File f = SPIFFS.open(MQTT_CONFIG);

            if (f.available())
            {
                mqttServer = f.readStringUntil('\n').c_str();
                if (f.available())
                {
                    mqttUser = f.readStringUntil('\n').c_str();
                    if (f.available())
                    {
                        mqttPassword = f.readStringUntil('\n').c_str();
                        if (f.available())
                        {
                            mqttPort = f.readStringUntil('\n').toInt();
                        }
                    }
                }
            }

            f.close();
        }

        wifiMac = WiFi.macAddress().c_str();
    }

    void clear()
    {
        if (SPIFFS.exists(WIFI_STA_CONFIG))
        {
            SPIFFS.remove(WIFI_STA_CONFIG);
        }

        if (SPIFFS.exists(MQTT_CONFIG))
        {
            SPIFFS.remove(MQTT_CONFIG);
        }

        if (SPIFFS.exists(GSM_CONFIG))
        {
            SPIFFS.remove(GSM_CONFIG);
        }
    }

    uint8_t isConfigured()
    {
        uint8_t result = 0;

        if (SPIFFS.exists(WIFI_STA_CONFIG))
        {
            result |= CONFIGURED_WIFI;
        }

        if (SPIFFS.exists(MQTT_CONFIG))
        {
            result |= CONFIGURED_MQTT;
        }

        if (SPIFFS.exists(GSM_CONFIG))
        {
            result |= CONFIGURED_GSM;
        }

        return result;
    }

    static std::string getWifiConfigPath()
    {
        return WIFI_STA_CONFIG;
    }

    static std::string getGsmConfigPath()
    {
        return GSM_CONFIG;
    }

    static std::string getMqttConfigPath()
    {
        return MQTT_CONFIG;
    }

    std::string getMqttServerIP()
    {
        return mqttServer;
    }

    uint16_t getMqttServerPort()
    {
        return mqttPort;
    }

    std::string getMqttServerUser()
    {
        return mqttUser;
    }

    std::string getMqttServerPassword()
    {
        return mqttPassword;
    }

    bool mqttEnabled()
    {
        return (isConfigured() & CONFIGURED_MQTT) == CONFIGURED_MQTT;
    }

    bool saveSTA(const String &ssid, const String &password)
    {
        File config = SPIFFS.open(WIFI_STA_CONFIG, "w");
        if (!config)
        {
            return false;
        }
        config.print(ssid);
        config.print('\n');
        config.print(password);
        config.print('\n');
        config.close();
        return true;
    }

    bool saveAP(const String &ssid, const String &password)
    {
        File config = SPIFFS.open(WIFI_AP_CONFIG, "w");
        if (!config)
        {
            return false;
        }
        config.print(ssid);
        config.print('\n');
        config.print(password);
        config.print('\n');
        config.close();
        return true;
    }

    std::string getWifiMac()
    {
        return wifiMac;
    }

    std::string getWifiSsid()
    {
        return wifiSsid;
    }
};

ConfigManager *ConfigManager::_manager = nullptr;
