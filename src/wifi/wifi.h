#include <WiFi.h>

#include "../config.h"

/**
 * Infrastructure SSID
 */
char SSID[33];

/**
 * Infrastructure Password
 */
char PASSWORD[64];

/**
 * AP SSID
 */
char AP_SSID[33];

/**
 * AP Password
 */
char AP_PASSWORD[64];

void configureWifi()
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