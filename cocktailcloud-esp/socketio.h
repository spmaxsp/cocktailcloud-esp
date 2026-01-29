
#ifndef SOCKETIO_H
#define SOCKETIO_H

/* -------------------------------------------------------------------------- */
/*                                    SocketIO                                */
/* -------------------------------------------------------------------------- */

#include <SocketIOclient.h>

void setup_socketio(char* server);

void socketio_handle_event(socketIOmessageType_t type, uint8_t * payload, size_t length);

void socketio_handle_message(String topic, String payload);
void socketio_handle_connect(void);

void socketio_simple_emit(String topic, String payload);

void socketio_emit_complete(void);

#endif // SOCKETIO_H
