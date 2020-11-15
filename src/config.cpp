#include "config.h"

/*
 * Wymagania sieci WiFi
 * Długość SSID: max 32 znaki! minimum 1
 * Password: WPA2 max 63 znaki, minimum 8 lub 0
*/

#define CONFIGURED_WIFI 1
#define CONFIGURED_MQTT 2
#define CONFIGURED_GSM 4

/**
 * Zwraca czy urzadzenie jest skonfigurowane
 */
uint8_t isConfigured()
{
    uint8_t result = 0;

    if (SPIFFS.exists(WIFI_CONFIG))
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
