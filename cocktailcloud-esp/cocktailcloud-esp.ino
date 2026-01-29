// #####################################################################################################################
// #                                                                                
// #  ██████╗ ██████╗  ██████╗██╗  ██╗████████╗ █████╗ ██╗██╗      ██████╗██╗      ██████╗ ██╗   ██╗██████╗           
// # ██╔════╝██╔═══██╗██╔════╝██║ ██╔╝╚══██╔══╝██╔══██╗██║██║     ██╔════╝██║     ██╔═══██╗██║   ██║██╔══██╗          
// # ██║     ██║   ██║██║     █████╔╝    ██║   ███████║██║██║     ██║     ██║     ██║   ██║██║   ██║██║  ██║    █████╗
// # ██║     ██║   ██║██║     ██╔═██╗    ██║   ██╔══██║██║██║     ██║     ██║     ██║   ██║██║   ██║██║  ██║    ╚════╝
// # ╚██████╗╚██████╔╝╚██████╗██║  ██╗   ██║   ██║  ██║██║███████╗╚██████╗███████╗╚██████╔╝╚██████╔╝██████╔╝          
// #  ╚═════╝ ╚═════╝  ╚═════╝╚═╝  ╚═╝   ╚═╝   ╚═╝  ╚═╝╚═╝╚══════╝ ╚═════╝╚══════╝ ╚═════╝  ╚═════╝ ╚═════╝           
// #                                                                                                                
// # ███████╗███████╗██████╗                                                                                          
// # ██╔════╝██╔════╝██╔══██╗                                                                                         
// # █████╗  ███████╗██████╔╝                                                                                         
// # ██╔══╝  ╚════██║██╔═══╝                                                                                          
// # ███████╗███████║██║                                                                                              
// # ╚══════╝╚══════╝╚═╝                                                                                              
// #
// #####################################################################################################################    

#include "hw_define.h"
#include "shift_reg.h"
#include "wifi_helper.h"
#include "config_server.h"
#include "eeprom-config.h"
#include "udp_discovery.h"
#include "socketio.h"

#include <Arduino.h>

#include <ESP_FlexyStepper.h>


// Speed settings
const int SPEED_IN_STEPS_PER_SECOND = 3000;
const int ACCELERATION_IN_STEPS_PER_SECOND = 1000;
const int DECELERATION_IN_STEPS_PER_SECOND = 1000;
const int STEPS_PER_ONE_REVOLUTION = 200;


ESP_FlexyStepper stepper;

SocketIOclient socketIO;

bool busy_flag = false;

/* -------------------------------------------------------------------------- */
/*                                    HELPER                                  */
/* -------------------------------------------------------------------------- */

bool is_number(String str) {
    for (int i = 0; i < str.length(); i++) {
        if ((str[i] < '0' or str[i] > '9') and str[i] != '-'){
            return false;
        }
    }
    return true;
}

/* -------------------------------------------------------------------------- */
/*                                    SETUP                                   */
/* -------------------------------------------------------------------------- */

void setup() {
    Serial.begin(9600);
    Serial.println(F(""));
    Serial.println(F(">#######################################################"));
    Serial.println(F(">Starting up..."));

    pinMode(LED_PIN, OUTPUT);

    pinMode(BUTTON_PIN_1, INPUT_PULLUP);
    pinMode(BUTTON_PIN_2, INPUT_PULLUP);

    delay(500);

    stepper.connectToPins(STEPPER_COMMON_STEP_PIN);

    stepper.setSpeedInStepsPerSecond(SPEED_IN_STEPS_PER_SECOND);
    stepper.setAccelerationInStepsPerSecondPerSecond(ACCELERATION_IN_STEPS_PER_SECOND);
    stepper.setDecelerationInStepsPerSecondPerSecond(DECELERATION_IN_STEPS_PER_SECOND);
    stepper.setStepsPerRevolution(STEPS_PER_ONE_REVOLUTION);

    setup_shiftregister();

    init_config();
    setup_wifi();
    setup_config_webserver();

    char server[100];
    bool res = try_udp_brodcast_discovery(server);

    if (res) {
        Serial.println(">UDP Discovery Success");
        Serial.println(">Server: " + String(server));
        setup_socketio(server);
    }
    else {
        Serial.println(">UDP Discovery Failed");
        setup_socketio(get_config_socketio_server());
    }
}


/* -------------------------------------------------------------------------- */
/*                                    STEP_INFO                               */
/* -------------------------------------------------------------------------- */

void phrase_step_info(String payload) {
    Serial.println("    Got Payload: " + payload);

    // divide payload at | 
    int pos = payload.indexOf('|');

    if (pos == -1) {
        Serial.println("    Payload Invalid (cannot divide at second '|')");
        return;
    }

    // get the first and second part of the second part
        String amount_str = payload.substring(0, pos);
        String pump_str = payload.substring(pos + 1);

    Serial.println("    Pumps: " + pump_str);
    Serial.println("    Amount: " + amount_str);

    // check if pump is a binary number greater than 0 and less than 2^16
    if (pump_str.length() > 16 or pump_str.length() == 0) {
        Serial.println("    Payload Invalid (too many/little pumps)");
        return;
    }

    // check if pump is a binary number
    for (int i = 0; i < pump_str.length(); i++) {
        if (pump_str[i] != '0' and pump_str[i] != '1') {
            Serial.println("    Payload Invalid (pumps not in binary format)");
            Serial.println("        Unexpected char: " + String(pump_str[i]));
            return;
        }
    }

    // check if amount is a number
    if (!is_number(amount_str)) {
        Serial.println("    HTTP request failed (amount not a number)");
        return;
    }

    // set the move array
    bool pump_array[16] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};

    for (int i = 0; i < 16; i++) {
        if (pump_str[i] == '1') {
            pump_array[i] = true;
        }
        else {
            pump_array[i] = false;
        }
    }

    int amount = amount_str.toInt();

    stepper_make_move(pump_array, amount);
}

/* -------------------------------------------------------------------------- */
/*                              STEPPER FUNCTIONS                             */
/* -------------------------------------------------------------------------- */

void stepper_cancel_move() {
    Serial.println(">Canceling Steppers");

    shiftreg_clear();
    
    stepper.setTargetPositionToStop();

    while (stepper.getCurrentVelocityInStepsPerSecond() != 0.0){
        delay(10);
    }

    busy_flag = false;

    Serial.println(">    ...OK");
}

void stepper_make_move(bool pumps[16], int amount) {

    if (busy_flag) {
        Serial.println(">Canceling previous move");
        stepper_cancel_move();
    }

    busy_flag = true;

    Serial.println(">Moving Steppers");

    if (amount < 0) {
        amount = -amount;
        pumps[SHIFTREG_DIRECTION_BIT] = true;
    }
    else {
        pumps[SHIFTREG_DIRECTION_BIT] = false;
    }
    

    shiftreg_write(pumps);

    //stepper.setTargetPositionRelativeInSteps(amount);
    stepper.setTargetPositionRelativeInRevolutions(amount);

    Serial.println(">    ...OK");
}

/* -------------------------------------------------------------------------- */
/*                                    LOOP                                    */
/* -------------------------------------------------------------------------- */

void loop() {
    // put your main code here, to run repeatedly:
    stepper.processMovement();
    socketIO.loop();

    if (busy_flag and stepper.motionComplete()) {
        busy_flag = false;
        shiftreg_clear();

        Serial.println(">Move Complete");
        socketio_emit_complete();
    }
}

