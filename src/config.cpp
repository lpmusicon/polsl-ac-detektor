#include "config.h"

/*
 * Długość SSID: max 32 znaki! minimum 1
 * Password: 63 znaki dla WPA2 <3, minimum 8
*/

bool isConfigured() {
    return LittleFS.exists(WIFI_CONFIG);
}

bool saveWiFiconfig(const char * const SSID, const char * const PASSWORD) {
    File config = LittleFS.open(WIFI_CONFIG, "w");
    
    // Error inside LittleFS
    if(!config) {
        Serial.println("config.cpp, saveWiFi failed");
        return false;
    }

    config.print(SSID); config.print('\n');
    config.print(PASSWORD); config.print('\n');

    config.close();

    return true;
}

bool loadWiFiconfig(char * SSID, char * PASSWORD) {
    if(!LittleFS.exists(WIFI_CONFIG)) {
        return false;
    }

    File config = LittleFS.open(WIFI_CONFIG, "r");
    if(!config) {
        return false;
    }

    String localSSID = config.readStringUntil('\n');
    String localPassword = config.readStringUntil('\n');

    strcpy(SSID, localSSID.c_str());
    strcpy(PASSWORD, localPassword.c_str());

    config.close();

    Serial.print("lwc SSID: "); Serial.println(SSID);
    Serial.print("lwc Password: "); Serial.println(PASSWORD);

    return true;
}

void loadDefaultWiFiconfig(char * SSID, char * PASSWORD) {
    Serial.println("default wifi config");
    strcpy(SSID, DEFAULT_WIFI_SSID);
    strcpy(PASSWORD, DEFAULT_WIFI_PASSWORD);

    Serial.print("ldwc SSID: "); Serial.println(SSID);
    Serial.print("ldwc Password: "); Serial.println(PASSWORD);
}

void deleteWiFiconfig() {
    LittleFS.remove(WIFI_CONFIG);
}