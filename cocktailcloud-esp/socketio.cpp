
#include "socketio.h"

#include <Arduino.h>
#include <SocketIOclient.h>
#include <ArduinoJson.h>

extern void phrase_step_info(String payload);
extern SocketIOclient socketIO;

void setup_socketio(char* server) {
    Serial.println(F(">Setting up SocketIO... "));

    // server address, port and URL
	socketIO.begin(server, 43560, "/socket.io/?EIO=4");

	// event handler
	socketIO.onEvent(socketio_handle_event);

	// try ever 5000 again if connection has failed
	socketIO.setReconnectInterval(5000);
}

void change_socketio_server(char* server) {
    Serial.println(F(">Changing SocketIO Server... "));

    socketIO.begin(server, 43560, "/socket.io/?EIO=4");
}


void socketio_handle_event(socketIOmessageType_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case sIOtype_DISCONNECT:
            Serial.printf("[IOc] Disconnected!\n");
            break;
        case sIOtype_CONNECT:
            Serial.printf("[IOc] Connected to url: %s\n", payload);

            // join default namespace (no auto join in Socket.IO V3)
            socketIO.send(sIOtype_CONNECT, "/");

            socketio_handle_connect();
            break;
        case sIOtype_EVENT:
        {
            char * sptr = NULL;
            int id = strtol((char *)payload, &sptr, 10);
            Serial.printf("[IOc] get event: %s id: %d\n", payload, id);
            if(id) {
                payload = (uint8_t *)sptr;
            }

            DynamicJsonDocument doc(1024);
            DeserializationError error = deserializeJson(doc, payload, length);
            if(error) {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.c_str());
                return;
            }

            String eventName = doc[0];
            String eventPayload = doc[1];

            // Message Includes a ID for a ACK (callback)
            if(id) {
                // creat JSON message for Socket.IO (ack)
                DynamicJsonDocument docOut(1024);
                JsonArray array = docOut.to<JsonArray>();

                // add payload (parameters) for the ack (callback function)
                JsonObject param1 = array.createNestedObject();
                param1["now"] = millis();

                // JSON to String (serializion)
                String output;
                output += id;
                serializeJson(docOut, output);

                // Send event
                socketIO.send(sIOtype_ACK, output);
            }

            // handle message
            socketio_handle_message(eventName, eventPayload);
        }
            break;
        case sIOtype_ACK:
            Serial.printf("[IOc] get ack: %u\n", length);
            break;
        case sIOtype_ERROR:
            Serial.printf("[IOc] get error: %u\n", length);
            break;
        case sIOtype_BINARY_EVENT:
            Serial.printf("[IOc] get binary: %u\n", length);
            break;
        case sIOtype_BINARY_ACK:
            Serial.printf("[IOc] get binary ack: %u\n", length);
            break;
    }
}


void socketio_handle_message(String topic, String payload) {
    Serial.println(">Handling SocketIO Message");

    if (topic == "machine_request_move") {
        Serial.println("    Machine Request Move");
        phrase_step_info(payload);
    }
}


void socketio_handle_connect() {
    Serial.println(">Handling SocketIO Connect");

    socketio_simple_emit("register_machine", "ESP32");
}


void socketio_emit_complete() {
    Serial.println(">Emitting Complete");

    socketio_simple_emit("machine_return_done", "OK");
}


void socketio_simple_emit(String topic, String payload) {
    Serial.println(">Emitting SocketIO Message");

    DynamicJsonDocument doc(1024);
    JsonArray array = doc.to<JsonArray>();

    array.add(topic);
    array.add(payload);

    String output;
    serializeJson(doc, output);

    socketIO.sendEVENT(output);
}