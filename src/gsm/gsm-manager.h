#pragma once

#include <Arduino.h>

#define TINY_GSM_MODEM_SIM800
#include <TinyGsmClient.h>

#include "config/config-manager.h"

#define GSM_RESET_PIN 13
#define SERIAL_BAUD 115200

class GsmManager
{
protected:
    TinyGsm modem = TinyGsm(Serial2);
    GsmManager()
    {
        pinMode(GSM_RESET_PIN, OUTPUT);
        digitalWrite(GSM_RESET_PIN, LOW);
        delay(2000);
        digitalWrite(GSM_RESET_PIN, HIGH);

        Serial2.begin(SERIAL_BAUD, SERIAL_8N1);
        modem.sendAT("+IPR=115200");

        if (!modem.restart())
        {
            Serial.printf("%s:%d - %s\n", __FILE__, __LINE__, "couldn't restart modem");
        }
        else
        {
            Serial.printf("%s:%d - %s\n", __FILE__, __LINE__, "modem restarted");
        }

        if (!modem.waitForNetwork(300000L))
        {
            Serial.printf("%s:%d - %s\n", __FILE__, __LINE__, "couldn't connect modem");
        }
        else
        {
            Serial.printf("%s:%d - %s\n", __FILE__, __LINE__, "modem connected to network");
            Serial.printf("%s:%d - Current Time: %s\n", __FILE__, __LINE__, getTimeString().c_str());

            getSIMNumber();
        }
    }
    static GsmManager *_manager;

public:
    GsmManager(GsmManager &o) = delete;
    GsmManager operator=(const GsmManager &) = delete;
    static GsmManager *GetInstance()
    {
        if (_manager == nullptr)
        {
            _manager = new GsmManager();
        }
        return _manager;
    }

    /**
 * This function gets formatted time and date from GSM module
 * @returns current time/date string HH:mm dd/MM/YYYY
 */
    String getTimeString()
    {
        /**
   * Have to get every part of time/date string
   */
        int year = 0, month = 0, day = 0, hour = 0, min = 0, sec = 0;
        float timezone = 0.0;
        if (!modem.getNetworkTime(&year, &month, &day, &hour, &min, &sec, &timezone))
        {
            Serial.printf("%s:%d - cannot get network time\n", __FILE__, __LINE__);
        }

        char buf[20];
        sprintf(buf, "%02d:%02d %02d/%02d/%d", hour, min, day, month, year);

        return String(buf);
    }

    String getSIMNumber()
    {
        modem.sendAT("+CNUM");
        modem.stream.readStringUntil('\n');
        modem.stream.readStringUntil('"');
        modem.stream.readStringUntil('"');
        modem.stream.readStringUntil('"');
        String data = modem.stream.readStringUntil('"');
        modem.streamClear();
        Serial.printf("%s:%d - DATA %s\n", __FILE__, __LINE__, data.c_str());
        return data;
    }

    /**
 * Zapisuje konfigurację GSM do pliku
 */
    bool saveGSMconfig(const char *const NAME, const char *const NUMBER)
    {
        File config = SPIFFS.open(ConfigManager::getGsmConfigPath().c_str(), "w");

        // Error inside SPIFFS
        if (!config)
        {
            Serial.println("config.cpp, saveGSM failed");
            return false;
        }

        Serial.println(__LINE__);
        config.print(NAME);
        config.print('\n');
        config.print(NUMBER);
        config.print('\n');
        config.close();

        return true;
    }

    /**
 * Odczytuje z pamięci konfigurację GSM
 */
    bool loadGSMconfig(char *NAME, char *NUMBER)
    {
        if (!SPIFFS.exists(ConfigManager::getGsmConfigPath().c_str()))
        {
            return false;
        }

        File config = SPIFFS.open(ConfigManager::getGsmConfigPath().c_str(), "r");
        if (!config)
        {
            return false;
        }

        String localName = config.readStringUntil('\n');
        String localNumber = config.readStringUntil('\n');

        strcpy(NAME, localName.c_str());
        strcpy(NUMBER, localNumber.c_str());

        config.close();
        return true;
    }
};

GsmManager *GsmManager::_manager = nullptr;