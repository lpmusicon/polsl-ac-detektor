#include <LittleFS.h>

#define WIFI_CONFIG "/wifi.config"
#define MQTT_CONFIG "/mqtt.config"

#define DEFAULT_WIFI_SSID "defaultWIFI"
#define DEFAULT_WIFI_PASSWORD ""

bool isConfigured();
bool saveWiFiconfig(const char * const SSID, const char * const PASSWORD);
bool loadWiFiconfig(char * SSID, char * PASSWORD);
void loadDefaultWiFiconfig(char * SSID, char * PASSWORD);
void deleteWiFiconfig();