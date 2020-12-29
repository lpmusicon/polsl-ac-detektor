#pragma once
#include <FS.h>
#include <ESPAsyncWebServer.h>
#define TINY_GSM_MODEM_SIM800
#include <TinyGsmClient.h>
#include <ArduinoJson.h>

#include "config.h"
#include "wifi/wifi-manager.h"
#include "gsm/gsm-manager.h"
#include "http/http.h"
