# Coclailcloud - ESP (Hardware)

This is the Arduino code for the Cocktailcloud Project. It is used to control the hardware and to communicate with the Cocktailcloud-API.

In the following sections ill give a short overview of the project. More information will be added soon.

## Software
The Machine is controlled by the React Frontend but can also load required Data from the API itself. 

The Project is based on the ESP32 and PlatformIO (should work with Arduino IDE as well).
The 

## Hardware
The ESP32 controls the 10 Stepper Motors. The Steppers all share the same Step/Direction-Pin and are selected by an Shift-Register which controls the EN pins.

The PCB for this project will be added soon.