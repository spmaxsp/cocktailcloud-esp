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
//
//  This is the main file for the cocktailcloud-esp project.
//  It is based on the ESP32 microcontroller and is used to control the stepper motors of the cocktailmachine.
//  Information about the project can be found in the README.md file.
//
// #####################################################################################################################

/* -------------------------------------------------------------------------- */
/*                        INCLUDES AND PIN DEFINITIONS                        */
/* -------------------------------------------------------------------------- */

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FastAccelStepper.h>

#define COMMON_STEP_PIN 26
#define COMMON_DIR_PIN 25

/* -------------------------------------------------------------------------- */
/*                                    INIT                                    */
/* -------------------------------------------------------------------------- */

const bool _STA = true; // true = STA, false = AP

const char* ap_ssid = "cocktailcloud-1";
const char* ap_password = "1234567890";

const char* sta_ssid = "SSID";
const char* sta_password = "passwd";

struct State {
    int active_cocktail = -1;
    int step_curr = -1;
    int step_todo = -1;

    int err_state = -1;
};

State state;

AsyncWebServer server(80);

FastAccelStepperEngine engine = FastAccelStepperEngine();
FastAccelStepper *stepper = NULL;


/* -------------------------------------------------------------------------- */
/*                                    SETUP                                   */
/* -------------------------------------------------------------------------- */

void setup() {
    Serial.begin(9600);

    setup_wifi();

    setup_webserver();
    
    setup_stepper();
}


void setup_wifi() {
    if (_STA) {
        Serial.println(F(">Connecting to WiFi...: "));

        WiFi.mode(WIFI_STA);
        WiFi.begin(sta_ssid, sta_password);
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
        Serial.println("");
        Serial.print(">WiFi connected, IP address: ");
        Serial.println(WiFi.localIP());
    } 
    else {
        Serial.println(F(">Open AP...: "));
    
        WiFi.mode(WIFI_AP);
        WiFi.softAP(ap_ssid, ap_password);
        IPAddress myIP = WiFi.softAPIP();
        Serial.print(">AP IP address: ");
        Serial.println(myIP);
    }
}

void setup_webserver() {
    Serial.println(F(">Booting up Webserver...: "));

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", "");
    });

}

void setup_stepper() {
    engine.init();
    stepper = engine.stepperConnectToPin(COMMON_STEP_PIN);
    if (stepper == NULL) {
        Serial.println(F(">Error connecting to stepper"));
    }
    stepper->setDirectionPin(COMMON_DIR_PIN);
    stepper->setAutoEnable(true);

    stepper->setSpeedInHz(500);       // 500 steps/s
    stepper->setAcceleration(100);    // 100 steps/s²
}

void loop() {

}