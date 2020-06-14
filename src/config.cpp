#include "config.h"

/*
 * Wymagania sieci WiFi
 * Długość SSID: max 32 znaki! minimum 1
 * Password: WPA2 max 63 znaki, minimum 8 lub 0
*/

/**
 * Zwraca czy urzadzenie jest skonfigurowane
 */
uint8_t isConfigured()
{
    uint8_t result = 0;

    if (SPIFFS.exists(WIFI_CONFIG))
    {
        result += 2;
    }

    if (SPIFFS.exists(GSM_CONFIG))
    {
        result += 4;
    }

    if (result == 6)
    {
        result = 1;
    }

    return result;
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
 * Usuwa konfiguracje WiFi
 */
void deleteWiFiconfig()
{
    SPIFFS.remove(WIFI_CONFIG);
}

/**
 * Zapisuje konfigurację GSM do pliku
 */
bool saveGSMconfig(const char *const NAME, const char *const NUMBER)
{
    File config = SPIFFS.open(GSM_CONFIG, "w");

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
    if (!SPIFFS.exists(GSM_CONFIG))
    {
        return false;
    }

    File config = SPIFFS.open(GSM_CONFIG, "r");
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

/**
 * Usuwa config GSM
 */
void deleteGSMconfig()
{
    SPIFFS.remove(GSM_CONFIG);
}

/**
 * Zwraca ilość zdarzeń
 */
int notificationLength()
{
    int counter = 0;
    File file = SPIFFS.open(NOTIFICATION_DATA, "r");

    while (file.available())
    {
        file.readStringUntil('\n');
        ++counter;
    }
    return counter;
}

/**
 * Zapisuje zdarzenie do pamięci
 */
bool writeNotification(NOTIFICATION_TYPE type, String date)
{
    File file = SPIFFS.open(NOTIFICATION_DATA, "a");
    if (!file)
    {
        return false;
    }

    file.printf("%d,%s\n", (int)type, date.c_str());
    file.close();

    return true;
}


/**
 * Usuwa wszystkie zdarzenia
 */
void deleteNotifications()
{
    SPIFFS.remove(NOTIFICATION_DATA);
}