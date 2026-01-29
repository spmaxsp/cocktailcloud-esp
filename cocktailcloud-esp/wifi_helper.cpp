
#include "wifi_helper.h"
#include "eeprom-config.h"
#include "hw_define.h"

#include <WiFiManager.h>
#include <Arduino.h>

void setup_ap_mode();
void setup_sta_mode();

void setup_wifi() {
    Serial.println(F(">Setting up WiFi... "));

    WiFi.setHostname("CocktailCloud-ESP");
    
    if (digitalRead(BUTTON_PIN_2) == LOW) {
        Serial.println(F(">Jumper Set: Starting in AP-Mode... "));
        setup_ap_mode();

    } else {
        Serial.println(F(">Jumper Not Set: Starting in STA-Mode... "));
        setup_sta_mode();
    }
}


void setup_ap_mode() {
    Serial.println(F(">Setting up AP-Mode... "));

    WiFi.mode(WIFI_AP);
    WiFi.softAP("CocktailCloud-ESP");

    Serial.print(F(">Device IP-Address: "));
    Serial.println(WiFi.softAPIP());
}

void setup_sta_mode() {
    Serial.println(F(">Setting up STA-Mode... "));

    WiFi.mode(WIFI_STA);
    WiFi.begin(get_config_ssid(), get_config_password());

    // Try to connect to WiFi
    for (int i = 0; i < 10; i++) {
        if (WiFi.status() == WL_CONNECTED) {
            Serial.print(F("\n"));
            Serial.println(F(">Connected to WiFi!"));
            Serial.print(F(">Device IP-Address: "));
            Serial.println(WiFi.localIP());
            break;
        }
        Serial.print(F("."));
        delay(1000);
    }

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println(F("\n>Failed to connect to WiFi. Starting AP-Mode..."));
        setup_ap_mode();
    }
}

void get_wifi_networks(WifiNetwork** networks, int* networks_count) {
    Serial.println(F(">Getting WiFi Networks..."));

    int n = WiFi.scanNetworks(true);
    Serial.println(F(">Scan done"));

    *networks_count = n;

    if (n == 0) {
        return;
    }
    
    *networks = (WifiNetwork*)malloc(n * sizeof(WifiNetwork));

    for (int i = 0; i < n; ++i) {
        (*networks)[i].rssi = WiFi.RSSI(i);
        (*networks)[i].encryption_type = WiFi.encryptionType(i);
        strcpy((*networks)[i].ssid, WiFi.SSID(i).c_str());
    }

    return;
}

int get_wifi_status() {
    if (WiFi.getMode() == WIFI_AP) {
        return WIFI_STATUS_AP_MODE;
    }
    else if (WiFi.status() == WL_CONNECTED) {
        return WIFI_STATUS_CONNECTED;
    }
    else {
        return WIFI_STATUS_DISCONNECTED;
    }
}

IPAddress get_ip_address() {
    if (WiFi.getMode() == WIFI_AP) {
        return WiFi.softAPIP();
    }
    else {
        return WiFi.localIP();
    }
}




