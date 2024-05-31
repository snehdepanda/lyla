#include "website.h"
#include "globals.h"


WebSocketsClient client;

// const char* url = "10.105.252.142";  // sneh eduroam
const char* url = "10.105.100.183";  // eduroam
// const char* url = "192.168.4.82";  // Replace with your WebSocket server URL
const uint16_t port = 8888;
// const char* endpoint = "/text2speech";  // text to speech
const char* endpoint = "/websocket_esp32";

void wsInitialize() {
    client.begin(url, port, endpoint);
    client.onEvent(webSocketEvent);
    client.setReconnectInterval(2000);
}

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

void wsConnect(void *parameter) {
    while (1) {
        client.loop();
        vTaskDelay(3);
    }
}