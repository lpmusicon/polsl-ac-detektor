#include "config.h"

/*
 * Długość SSID: max 32 znaki! minimum 1
 * Password: 63 znaki dla WPA2 <3, minimum 8
*/

uint8_t isConfigured() {
    uint8_t result = 0;

    if(SPIFFS.exists(WIFI_CONFIG)) {
        result += 2;
    }

    if(SPIFFS.exists(GSM_CONFIG)) {
        result += 4;
    }

    /* ALE WEŹ TO POPRAW ZANIM TO ODDASZ PLS */
    if(result == 6) {
        result = 1;
    }

    return result;
}

bool saveWiFiconfig(const char * const SSID, const char * const PASSWORD) {
    File config = SPIFFS.open(WIFI_CONFIG, "w");
    
    // Error inside SPIFFS
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
    if(!SPIFFS.exists(WIFI_CONFIG)) {
        return false;
    }

    File config = SPIFFS.open(WIFI_CONFIG, "r");
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
    SPIFFS.remove(WIFI_CONFIG);
}

bool saveGSMconfig(const char * const NAME, const char * const NUMBER) {
    File config = SPIFFS.open(GSM_CONFIG, "w");
    
    // Error inside SPIFFS
    if(!config) {
        Serial.println("config.cpp, saveGSM failed");
        return false;
    }

    Serial.println(__LINE__);
    config.print(NAME); config.print('\n');
    config.print(NUMBER); config.print('\n');

    config.close();

    return true;
}

bool loadGSMconfig(char * NAME, char * NUMBER) {
    if(!SPIFFS.exists(GSM_CONFIG)) {
        return false;
    }

    File config = SPIFFS.open(GSM_CONFIG, "r");
    if(!config) {
        return false;
    }

    String localName = config.readStringUntil('\n');
    String localNumber = config.readStringUntil('\n');

    strcpy(NAME, localName.c_str());
    strcpy(NUMBER, localNumber.c_str());

    config.close();

    Serial.print("lwc NAME: "); Serial.println(NAME);
    Serial.print("lwc NUMBER: "); Serial.println(NUMBER);

    return true;
}

void deleteGSMconfig() {
    SPIFFS.remove(GSM_CONFIG);
}