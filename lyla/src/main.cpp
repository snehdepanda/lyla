#include <Arduino.h>
#include <mic.h>
#include <WiFiManager.h>
#include <WiFi.h>
#include <WebSocketsClient.h>

WebSocketsClient webSocket;
// Button definitions
#define PUSH_BUTTON_PIN 15  // Change as per your pushbutton GPIO connection
volatile bool recording = false;
volatile bool buttonPressed = false;

// I2S buffer for mic initializations
int32_t mic_read_buffer[buf_len] = {0};

// Buffer counter global var
uint8_t buf_counter = 0;

// Setup Server URL
const char* websocket_server_host = "10.105.252.142";
const uint16_t websocket_server_port = 8888; 
const char* websocket_path = "/audio";


// // Button handler
void IRAM_ATTR handleButtonPress() {
    // Toggle recording state only if button release is detected
    if (digitalRead(PUSH_BUTTON_PIN) == LOW) {
        buttonPressed = !buttonPressed;
        if (!buttonPressed) {  // Trigger on button release to avoid multiple toggles
            recording = !recording;
        }
    }
}

void startAudioCollection() {
    if (recording) {
        Serial.println("Started Recording...");

        while (1) { 
            size_t bytes_read = mic_i2s_to_buffer(mic_read_buffer, buf_len);

            if (WiFi.isConnected()) {
                webSocket.sendBIN((uint8_t*)mic_read_buffer, bytes_read);
            }
            buf_counter++;
            if (buf_counter == 80) {
                buf_counter = 0;
                webSocket.disconnect();
                break;
            }
        }
        
        recording = !recording; // Stop recording after one iteration
        Serial.println("Stopped Recording...");
    } else {
        // Perform any needed tasks while not recording
        Serial.println("Not Recording...");
        delay(100);
    }
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
            break;
        case WStype_BIN:
            Serial.printf("[WebSocket] Binary data received\n");
            break;
    }
}

void setup() {
  Serial.begin(115200);
  setupI2Smic();

  WiFiManager wifiManager;
  wifiManager.autoConnect("AutoConnectAP"); 
  Serial.println("Connected to WiFi!");

  pinMode(PUSH_BUTTON_PIN, INPUT_PULLUP);  // Setup button pin
  attachInterrupt(PUSH_BUTTON_PIN, handleButtonPress, CHANGE);  // Attach interrupt to handle button press

  webSocket.begin(websocket_server_host, websocket_server_port, websocket_path);
  webSocket.onEvent(webSocketEvent);
}

void loop() {
    webSocket.loop();
    if (webSocket.isConnected()) {
        startAudioCollection();
    } else {
        Serial.println("WebSocket not connected...");
    }
    // static bool lastRecordingState = false;
    // if (recording) {
    //     // // Function to read data from I2S and store in buffer
    //     // // Add your code here to do something with the data, e.g., saving to an SD card

    //     if (!lastRecordingState) {
    //         Serial.println("Started Recording...");
    //         lastRecordingState = true;
    //     }

    //     // sendControlSignal("Start");

    //     // while (1) {
    //     //     size_t bytes_read = mic_i2s_to_buffer(mic_read_buffer, buf_len);

    //     //     if (WiFi.isConnected()) {
    //     //         HTTPClient http;
    //     //         http.begin(SERVER_URL);
    //     //         http.addHeader("Content-Type", "application/octet-stream");
    //     //         http.POST((uint8_t*)mic_read_buffer, bytes_read);
    //     //         buf_counter++;
    //     //         http.end();
    //     //     }

    //     //     if (buf_counter == 4) {
    //     //         buf_counter = 0;
    //     //         break;
    //     //     }
    //     // }

    //     // // sendControlSignal("Stop");
    //     // Serial.printf("Started Recording");
    //     // size_t bytes_read = mic_i2s_to_buffer(mic_read_buffer, buf_len);
    //     // delay(1000);

    //     // recording = !recording; // Stop recording after one iteration
    //     // lastRecordingState = true;
    // } else {
    //     // Perform any needed tasks while not recording
    //     if (lastRecordingState) {
    //         Serial.println("Stopped Recording...");
    //         lastRecordingState = false;
    //     }
    //     delay(100);
    // }
}

