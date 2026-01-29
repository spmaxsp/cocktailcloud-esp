
#include "udp_discovery.h"

#include <Arduino.h>
#include <WiFiUdp.h>

bool try_udp_brodcast_discovery(char* response_ip) {
    Serial.println(">Trying UDP Broadcast Discovery");

    WiFiUDP udp;

    int success = 0;
    
    // Send UDP broadcast to discover server
    udp.begin(5005);
    udp.beginPacket("255.255.255.255", 5005);
    udp.printf("DISCOVER_SERVER");
    udp.endPacket();

    // Wait for response
    char incomingPacket[255];

    Serial.println(">Waiting for response");

    for (int i = 0; i < 10; i++) {
        Serial.println(">    ...");
        int packetSize = udp.parsePacket();
        if (packetSize) {
            int len = udp.read(incomingPacket, 255);
            if (len > 0) {
                incomingPacket[len] = 0;
            }
            Serial.printf("Received packet: %s\n", incomingPacket);
            strcpy(response_ip, incomingPacket);
            success = 1;
            break;
        }
        delay(1000);
    }

    udp.stop();

    return success;
}