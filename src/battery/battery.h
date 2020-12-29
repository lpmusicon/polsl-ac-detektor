#pragma once

#include <Arduino.h>

#define BATTERY_PIN 32
#define MAX_MAPPED_VOLTAGE 7020
#define MAX_ADC_VALUE 4096
#define MIN_VOLTAGE 0
#define BATTERY_LOW_CHARGE_VOLTAGE 3500

class Battery
{
protected:
    uint16_t voltage;
    static Battery *_battery;
    Battery()
    {
        pinMode(BATTERY_PIN, INPUT);
        voltage = map(analogRead(BATTERY_PIN), MIN_VOLTAGE, MAX_ADC_VALUE, MIN_VOLTAGE, MAX_MAPPED_VOLTAGE);
    }

public:
    Battery(Battery &other) = delete;
    Battery operator=(const Battery &) = delete;
    static Battery *GetInstance()
    {
        if (_battery == nullptr)
        {
            _battery = new Battery();
        }

        return _battery;
    }

    uint16_t getVoltage()
    {
        return voltage;
    }

    void measure()
    {
        /**
         * Filtr uśredniający z poprzedniej wartości ze względu na dość duży szum ADC w ESP32
        * Wartości są mapowane z 10 bitowego zakresu 3.3V do 5V (ze względu na dzielnik napięcia)
        */
        auto value = analogRead(BATTERY_PIN);
        voltage = 0.4 * voltage + 0.6 * map(value, MIN_VOLTAGE, MAX_ADC_VALUE, MIN_VOLTAGE, MAX_MAPPED_VOLTAGE);
    }

    bool isLow()
    {
        return voltage < BATTERY_LOW_CHARGE_VOLTAGE;
    }
};

Battery *Battery::_battery = nullptr;
