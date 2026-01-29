
#include "shift_reg.h"
#include "hw_define.h"

#include <Arduino.h>


void setup_shiftregister() {
    Serial.println(F(">Setting up Shiftregister... "));

    pinMode(SHIFTREG_DATA_PIN, OUTPUT);
    pinMode(SHIFTREG_CLOCK_PIN, OUTPUT);
    pinMode(SHIFTREG_STORE_PIN, OUTPUT);
    pinMode(SHIFTREG_MR_PIN, OUTPUT);

    digitalWrite(SHIFTREG_MR_PIN, HIGH);
    digitalWrite(SHIFTREG_STORE_PIN, LOW);

    shiftreg_clear();
}


void shiftreg_clear() {
    Serial.println(">Clearing Shiftregister");

    bool shiftreg_data[16] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
    shiftreg_write(shiftreg_data);
}


void shiftreg_write(bool shiftreg_data[16]) {
    Serial.print(">Writing Shiftregister:");
    for (int i = 0; i < 16; i++) {
        Serial.print(" ");
        Serial.print(shiftreg_data[i]);
    }
    Serial.println("");

    digitalWrite(SHIFTREG_STORE_PIN, LOW);

    for (int i = 16; i >= 0; i--) {
        digitalWrite(SHIFTREG_DATA_PIN, shiftreg_data[i]);
        digitalWrite(SHIFTREG_CLOCK_PIN, HIGH);
        delayMicroseconds(1);
        digitalWrite(SHIFTREG_CLOCK_PIN, LOW);
        delayMicroseconds(1);
    }

    digitalWrite(SHIFTREG_STORE_PIN, HIGH);
}