#include <FS.h>
#include <SPIFFS.h>

#define WIFI_CONFIG "/wifi.config"
#define MQTT_CONFIG "/mqtt.config"
#define GSM_CONFIG "/gsm.config"

#define SOFT_AP_WIFI_SSID "AC-DETECTOR-00"
#define SOFT_AP_WIFI_PASSWORD "12345678"

#define NOTIFICATION_DATA "/notification.data"

#define MINIMUM_WIFI_NETWORK_RESPONSE_SIZE 30
#define MAXIMUM_WIFI_NETWORK_OBJECT_SIZE 72

#define WIFI_SCAN_NOT_TRIGGERED -2
#define WIFI_SCAN_NOT_FINISHED -1

enum class NOTIFICATION_TYPE {
    POWER_LOSS,
    POWER_CONNECT,
    LOW_BATTERY
};

uint8_t isConfigured();
bool saveWiFiconfig(const char * const SSID, const char * const PASSWORD);
bool loadWiFiconfig(char * SSID, char * PASSWORD);
void loadDefaultWiFiconfig(char * SSID, char * PASSWORD);
void deleteWiFiconfig();
bool saveGSMconfig(const char * const NAME, const char * const NUMBER);
bool loadGSMconfig(char * NAME, char * NUMBER);
void deleteGSMconfig();
bool writeNotification(NOTIFICATION_TYPE type, String date);
void deleteNotifications();