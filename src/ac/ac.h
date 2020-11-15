#pragma once
#include <Arduino.h>
#define AC_PIN 34

class AC
{
protected:
    bool previousState;
    bool currentState;

    static AC *_ac;
    AC()
    {
        pinMode(AC_PIN, INPUT);
        previousState = digitalRead(AC_PIN);
        currentState = previousState;
    }

public:
    AC(AC &other) = delete;
    AC operator=(const AC &) = delete;
    static AC *GetInstance()
    {
        if (_ac == nullptr)
        {
            _ac = new AC();
        }

        return _ac;
    }

    void check()
    {
        previousState = currentState;
        currentState = digitalRead(AC_PIN);
    }

    bool justConnected()
    {
        return previousState && !currentState;
    }

    bool justDisconnected()
    {
        return !previousState && currentState;
    }

    bool connected()
    {
        return currentState;
    }
};

AC *AC::_ac = nullptr;