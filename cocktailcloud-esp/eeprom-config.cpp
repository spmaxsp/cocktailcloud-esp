
#include "eeprom-config.h"

#include <EEPROM.h>

Configuration config;

int initialised = 0;

void set_default_config() {
    config.test_data_a = CONFIG_TEST_DATA;
    config.test_data_b = CONFIG_TEST_DATA;
    
    strcpy(config.ssid, CONFIG_DEFAULT_SSID);
    strcpy(config.password, CONFIG_DEFAULT_PASSWORD);
    strcpy(config.socketio_server, CONFIG_DEFAULT_SOCKETIO_SERVER);
}

void read_config() {
    EEPROM.begin(sizeof(Configuration));
    EEPROM.get(CONFIG_EEPROM_ADDR, config);
    EEPROM.end();
}

void write_config() {
    EEPROM.begin(sizeof(Configuration));
    EEPROM.put(CONFIG_EEPROM_ADDR, config);

    if (EEPROM.commit()) {
        Serial.println(">EEPROM successfully committed !!!");
    } else {
        Serial.println(">ERROR! EEPROM commit failed");
    }

    EEPROM.end();
}

void init_config() {
    Serial.println(F(">Initiating Config..."));
    read_config();
    
    if (config.test_data_a != CONFIG_TEST_DATA || config.test_data_b != CONFIG_TEST_DATA) {
        Serial.println(F(">Config not found, setting default config"));
        set_default_config();
        write_config();
    }

    Serial.println(F(">Config initiated!"));

    Serial.print(F("    SSID: "));
    Serial.println(config.ssid);
    Serial.print(F("    Password: "));
    Serial.println(config.password);
    Serial.print(F("    SocketIO Server: "));
    Serial.println(config.socketio_server);

    initialised = 1;
}

void set_config_ssid(char* ssid) {
    if (initialised) {
        Serial.println(F(">Setting SSID to: "));
        Serial.println(ssid);
        strcpy(config.ssid, ssid);
        write_config();
    }
}

void set_config_password(char* password) {
    if (initialised) {
        Serial.println(F(">Setting Password to: "));
        Serial.println(password);
        strcpy(config.password, password);
        write_config();
    }
}

void set_config_socketio_server(char* socketio_server) {
    if (initialised) {
        Serial.println(F(">Setting SocketIO Server to: "));
        Serial.println(socketio_server);
        strcpy(config.socketio_server, socketio_server);
        write_config();
    }
}

char* get_config_ssid() {
    if (initialised) {
        return config.ssid;
    }
    else {
        return CONFIG_DEFAULT_SSID;
    }
}

char* get_config_password() {
    if (initialised) {
        return config.password;
    }
    else {
        return CONFIG_DEFAULT_PASSWORD;
    }
}

char* get_config_socketio_server() {
    if (initialised) {
        return config.socketio_server;
    }
    else {
        return CONFIG_DEFAULT_SOCKETIO_SERVER;
    }
}