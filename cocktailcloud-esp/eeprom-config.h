
#ifndef EEPROM_CONFIG_H
#define EEPROM_CONFIG_H

/* -------------------------------------------------------------------------- */
/*                                CONFIGURATION                               */
/* -------------------------------------------------------------------------- */


typedef struct {
    int test_data_a;

    char ssid[50];
    char password[50];
    char socketio_server[20];

    int test_data_b;
} Configuration;


#define CONFIG_DEFAULT_SSID ""
#define CONFIG_DEFAULT_PASSWORD ""
#define CONFIG_DEFAULT_SOCKETIO_SERVER ""

#define CONFIG_EEPROM_ADDR 0
#define CONFIG_TEST_DATA 2735 //b0000101010101111  (used to validate the eeprom content)


void init_config();

void set_config_ssid(char* ssid);
void set_config_password(char* password);
void set_config_socketio_server(char* socketio_server);

char* get_config_ssid();
char* get_config_password();
char* get_config_socketio_server();


#endif // EEPROM_CONFIG_H