#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

/*
 * Długość SSID: max 32 znaki! minimum 1
 * Password: 63 znaki dla WPA2 <3, minimum 8
*/
const char ssid[] = "UPC0444679";
const char password[] = "x58nzmvfTzrf";

const uint8_t PIN_LED = 2;
const uint8_t PIN2_LED = 16;

/* 
 * Setup
 * 
 * Trzeba:
 *  > Sprawdzić czy jest konfig
 *  > > Jak jest to wczytać i witaj świecie
 *  > > W innym wypadku trzeba załadować konfigurację podstawową i dać użytkownikowi się wykazać
 *  > Odpalić WiFi dla użytkownika
 *  > Połączyć się z modułem GSM
 *  > Muszę sprawdzać poziom zasilania (dzielnik napięcia na rezystorze wystarczy 1/3 - 2/3)
 *  > Pilnować pinu zasilania (też dzielnik napięcia 5V - 1/3 2/3)
*/

void setup() {
    // checkConfig()
    // if(exists)
    // configure
    // else setup
    // wifiSetup
    // gsmSetup
    // pinCheck (battery + ac)
    // serverSetup
}


void loop() {

}