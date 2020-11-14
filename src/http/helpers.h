#pragma once

#define CONTENT_TYPE_JSON "application/json"

#define HTTP_OK 200
#define HTTP_CREATED 201
#define HTTP_ACCEPTED 202
#define HTTP_NO_CONTENT 204
#define HTTP_BAD_REQUEST 400
#define HTTP_SERVER_ERROR 500

/**
 * Formats network to JSON per ID
 * Outputs to string
 */
void networkToJSONObject(int id, char *string)
{
    snprintf(string, 72, "{\"SSID\":\"%s\",\"dBm\":\"%d\",\"encType\":\"%d\"}", WiFi.SSID(id).c_str(), WiFi.RSSI(id), WiFi.encryptionType(id));
}