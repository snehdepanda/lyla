#include "website.h"
#include "globals.h"


void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[WebSocket] Disconnected!\n");
            break;
        case WStype_CONNECTED:
            Serial.printf("[WebSocket] Connected\n");
            break;
        case WStype_TEXT:
            Serial.printf("[WebSocket] Message received: %s\n", payload);
            display.clearDisplay();
            display.setCursor(0,0);
            display.setTextSize(1);
            display.setTextWrap(true);
            display.setTextColor(WHITE);
            display.print((char*)payload);
            display.display();
            break;
        case WStype_BIN:
            Serial.printf("[WebSocket] Binary data received\n");
            break;
    }
}