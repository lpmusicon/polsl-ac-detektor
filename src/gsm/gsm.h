#pragma once

#define TINY_GSM_MODEM_SIM800
#include <TinyGsmClient.h>

#define SERIAL_BAUD 115200

TinyGsm modem(Serial2);
/**
 * Device SIM number
 */
String gsmNumber;
TinyGsmClient client(modem);

/**
 * This function gets SIM number and sets it to local variable
 * TONS of side effect coding
 * but it works
 */
void getSIMNumber()
{
    modem.sendAT("+CNUM");
    modem.stream.readStringUntil('\n');
    modem.stream.readStringUntil('"');
    modem.stream.readStringUntil('"');
    modem.stream.readStringUntil('"');
    String data = modem.stream.readStringUntil('"');
    modem.streamClear();
    Serial.printf("%s:%d - DATA %s\n", __FILE__, __LINE__, data.c_str());

    gsmNumber = data;
}

/**
 * This function gets formatted time and date from GSM module
 * @returns current time/date string HH:mm dd/MM/YYYY
 */
String getTimeString()
{
    /**
   * Have to get every part of time/date string
   */
    int year = 0, month = 0, day = 0, hour = 0, min = 0, sec = 0;
    float timezone = 0.0;
    if (!modem.getNetworkTime(&year, &month, &day, &hour, &min, &sec, &timezone))
    {
        Serial.printf("%s:%d - cannot get network time\n", __FILE__, __LINE__);
    }

    char buf[20];
    sprintf(buf, "%02d:%02d %02d/%02d/%d", hour, min, day, month, year);

    return String(buf);
}

void configureGsmModem()
{
    Serial2.begin(SERIAL_BAUD, SERIAL_8N1);
    modem.sendAT("+IPR=115200");

    if (!modem.restart())
    {
        Serial.printf("%s:%d - %s\n", __FILE__, __LINE__, "couldn't restart modem");
    }
    Serial.printf("%s:%d - %s\n", __FILE__, __LINE__, "modem restarted");

    // if (!modem.waitForNetwork(300000L))
    // {
    //   Serial.printf("%s:%d - %s\n", __FILE__, __LINE__, "couldn't connect modem");
    // }
    // Serial.printf("%s:%d - %s\n", __FILE__, __LINE__, "modem connected to network");
    // Serial.printf("%s:%d - Current Time: %s\n", __FILE__, __LINE__, getTimeString().c_str());

    // getSIMNumber();
}