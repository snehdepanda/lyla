/*
WiFi and WebSockets Connections
*/

#include "globals.h"

const BaseType_t wifi_core = 1;


void wsInitialize();
void webSocketEvent(WStype_t type, uint8_t* payload, size_t length);
void wsConnect(void* parameter);
