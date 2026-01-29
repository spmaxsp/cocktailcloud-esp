
#ifndef WIFI_HELPER_H
#define WIFI_HELPER_H

/* -------------------------------------------------------------------------- */
/*                               WIFI MANAGER                                 */
/* -------------------------------------------------------------------------- */

#include <Arduino.h>
#include <WiFi.h>

#define WIFI_STATUS_DISCONNECTED 0
#define WIFI_STATUS_CONNECTED 1
#define WIFI_STATUS_AP_MODE 2

typedef struct {
    char ssid[32];
    int rssi;
    int encryption_type;
} WifiNetwork;

void setup_wifi(void);
void get_wifi_networks(WifiNetwork** networks, int* networks_count);
int get_wifi_status(void);
IPAddress get_ip_address(void);


#endif // WIFI_HELPER_H