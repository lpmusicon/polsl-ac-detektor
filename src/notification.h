#pragma once
#include <Arduino.h>

class Notification
{
    uint8_t type;
    String date;
    public:
        Notification(uint8_t type, const String & date) {
            this->type = type;
            this->date = date;
        }

        /**
         * Zwraca reprezentacje w JSON String
         */
        String toJSON() {
            String json = "{\"type\":\"";
            json += type;
            json += "\",\"date\":\"";
            json += date;
            json += "\"}";
            return json;
        } ;
};
