#pragma once
#include <FS.h>
#include <SPIFFS.h>

/**
 * Nazwy plikow konfiguracyjnych
 */
#define WIFI_CONFIG "/wifi.config"
#define MQTT_CONFIG "/mqtt.config"
#define GSM_CONFIG "/gsm.config"

/**
 * Domyslna sieci WiFi softAP
 */
#define SOFT_AP_WIFI_SSID "DETECTOR_SETUP"
#define SOFT_AP_WIFI_PASSWORD "12345678"

#define MINIMUM_WIFI_NETWORK_RESPONSE_SIZE 30
#define MAXIMUM_WIFI_NETWORK_OBJECT_SIZE 72

#define WIFI_SCAN_NOT_TRIGGERED -2
#define WIFI_SCAN_NOT_FINISHED -1

uint8_t isConfigured();