
#include "config_server.h"
#include "config_server_html.h"

#include "wifi_helper.h"
#include "eeprom-config.h"

#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include <Arduino.h>

AsyncWebServer server(80);


void on_get_ssids(AsyncWebServerRequest *request);
void on_get_status(AsyncWebServerRequest *request);
void on_get_config(AsyncWebServerRequest *request);
void on_set_wifi(AsyncWebServerRequest *request);
void on_set_socketio(AsyncWebServerRequest *request);
void on_reboot(AsyncWebServerRequest *request);

void setup_config_webserver() {
    Serial.println(F(">Setting up Config-Webserver... "));

    // Setup Webserver
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", PAGE_NetworkConfiguration);
    });

    //server.on("/get_ssids", HTTP_GET, on_get_ssids);
    server.on("/get_status", HTTP_GET, on_get_status);
    server.on("/get_config", HTTP_GET, on_get_config);

    server.on("/set_wifi", HTTP_POST, on_set_wifi);
    server.on("/set_socketio", HTTP_POST, on_set_socketio);

    server.on("/reboot", HTTP_POST, on_reboot);

    server.begin();
    Serial.println(F(">Config-Webserver started!"));
}


void on_get_ssids(AsyncWebServerRequest *request) {
    Serial.println(F(">WEBSERVER: /get_ssids"));

    // Get SSIDs from Helper
    int networks_count = 0;
    WifiNetwork* networks;

    get_wifi_networks(&networks, &networks_count);

    // Create JSON
    AsyncJsonResponse *response = new AsyncJsonResponse();
    JsonArray root = response->getRoot();

    for (int i = 0; i < networks_count; i++) {
        JsonObject network = root.createNestedObject();
        network["ssid"] = networks[i].ssid;
        network["rssi"] = networks[i].rssi;
        network["encryption_type"] = networks[i].encryption_type;
    }

    free(networks);

    response->setLength();
    request->send(response);
}


void on_get_status(AsyncWebServerRequest *request) {
    Serial.println(F(">WEBSERVER: /get_status"));

    // Get IP-Address
    IPAddress ip = get_ip_address();
    int status = get_wifi_status(); 

    // Create JSON
    AsyncJsonResponse *response = new AsyncJsonResponse();
    JsonObject root = response->getRoot();

    root["ip"] = ip.toString();
    
    if (status == WIFI_STATUS_CONNECTED) {
        root["status"] = "Connected";
    }
    else if (status == WIFI_STATUS_AP_MODE) {
        root["status"] = "AP-Mode";
    }
    else {
        root["status"] = "Disconnected";
    }

    response->setLength();
    request->send(response);
}


void on_get_config(AsyncWebServerRequest *request) {
    Serial.println(F(">WEBSERVER: /get_config"));

    // Create JSON
    AsyncJsonResponse *response = new AsyncJsonResponse();
    JsonObject root = response->getRoot();

    root["ssid"] = get_config_ssid();
    root["password"] = get_config_password();
    root["socketio_server"] = get_config_socketio_server();

    response->setLength();
    request->send(response);
}


void on_set_wifi(AsyncWebServerRequest *request) {
    Serial.println(F(">WEBSERVER: /set_wifi"));

    if (request->hasArg("ssid") && request->hasArg("password")) {
        // Get Params
        String ssid = request->arg("ssid");
        String password = request->arg("password");

        // Set Config
        set_config_ssid((char*)ssid.c_str());
        set_config_password((char*)password.c_str());
    }

    // Send Response
    request->send(200, "text/plain", "OK");
}


void on_set_socketio(AsyncWebServerRequest *request) {
    Serial.println(F(">WEBSERVER: /set_socketio"));

    if (request->hasArg("socketio_server")) {
        // Get Params
        String socketio_server = request->arg("socketio_server");

        // Set Config
        set_config_socketio_server((char*)socketio_server.c_str());
    }

    // Send Response
    request->send(200, "text/plain", "OK");
}


void on_reboot(AsyncWebServerRequest *request) {
    Serial.println(F(">WEBSERVER: /reboot"));

    // Send Response
    request->send(200, "text/plain", "OK");

    // Reboot
    Serial.println(F(">Rebooting..."));
    ESP.restart();
}