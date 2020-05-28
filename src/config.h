#pragma once
#include <FS.h>
#include <SPIFFS.h>

#define WIFI_CONFIG "/wifi.config"
#define MQTT_CONFIG "/mqtt.config"
#define GSM_CONFIG "/gsm.config"

#define DEFAULT_WIFI_SSID "defaultWIFI"
#define DEFAULT_WIFI_PASSWORD "12345678"

uint8_t isConfigured();
bool saveWiFiconfig(const char * const SSID, const char * const PASSWORD);
bool loadWiFiconfig(char * SSID, char * PASSWORD);
void loadDefaultWiFiconfig(char * SSID, char * PASSWORD);
void deleteWiFiconfig();
bool saveGSMconfig(const char * const NAME, const char * const NUMBER);
bool loadGSMconfig(char * NAME, char * NUMBER);
void deleteGSMconfig();