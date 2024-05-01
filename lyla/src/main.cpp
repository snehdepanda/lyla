#include <Arduino.h>
#include <mic.h>
#include <WiFiManager.h>
#include <WiFi.h>
// #include <WebSocketsClient.h>

// WebSocketsClient webSocket;
// Button definitions
#define PUSH_BUTTON_PIN 15  // Change as per your pushbutton GPIO connection
volatile bool recording = false;
volatile bool buttonPressed = false;

// I2S buffer for mic initializations
int32_t mic_read_buffer[buf_len] = {0};

// Buffer counter global var
// int buf_counter = 0;

// // Setup Server URL
// const char* websocket_server = "ws://10.105.252.142:8888/path";

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

// void startAudioCollection() {
//     while (1) { 
//         size_t bytes_read = mic_i2s_to_buffer(mic_read_buffer, buf_len);

//         if (WiFi.isConnected()) {
//             webSocket.sendBIN((uint8_t*)mic_read_buffer, bytes_read);
//         }
//         buf_counter++;
//         if (buf_counter == 40) {
//             buf_counter = 0;
//             break;
//         }
//     }
    
//     webSocket.disconnect();
// }

// void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
//     switch(type) {
//         case WStype_DISCONNECTED:
//             Serial.printf("[WebSocket] Disconnected!\n");
//             break;
//         case WStype_CONNECTED:
//             Serial.printf("[WebSocket] Connected\n");
//             // Start collecting data
//             startAudioCollection();
//             break;
//         case WStype_TEXT:
//             Serial.printf("[WebSocket] Message received: %s\n", payload);
//             break;
//         case WStype_BIN:
//             Serial.printf("[WebSocket] Binary data received\n");
//             break;
//     }
// }

void setup() {
  Serial.begin(115200);
  setupI2Smic();

  WiFiManager wifiManager;
  wifiManager.autoConnect("AutoConnectAP"); 
  Serial.println("Connected to WiFi!");

  pinMode(PUSH_BUTTON_PIN, INPUT_PULLUP);  // Setup button pin
  attachInterrupt(PUSH_BUTTON_PIN, handleButtonPress, CHANGE);  // Attach interrupt to handle button press
  Serial.println("Button setup complete!");

//   webSocket.begin("10.105.252.142", 8888, "/path");
//   webSocket.onEvent(webSocketEvent);
}

void loop() {
    // webSocket.loop();
    static bool lastRecordingState = false;
    if (recording) {
        // // Function to read data from I2S and store in buffer
        // // Add your code here to do something with the data, e.g., saving to an SD card

        if (!lastRecordingState) {
            Serial.println("Started Recording...");
            lastRecordingState = true;
        }

        // sendControlSignal("Start");

        // while (1) {
        //     size_t bytes_read = mic_i2s_to_buffer(mic_read_buffer, buf_len);

        //     if (WiFi.isConnected()) {
        //         HTTPClient http;
        //         http.begin(SERVER_URL);
        //         http.addHeader("Content-Type", "application/octet-stream");
        //         http.POST((uint8_t*)mic_read_buffer, bytes_read);
        //         buf_counter++;
        //         http.end();
        //     }

        //     if (buf_counter == 4) {
        //         buf_counter = 0;
        //         break;
        //     }
        // }

        // // sendControlSignal("Stop");
        // Serial.printf("Started Recording");
        size_t bytes_read = mic_i2s_to_buffer(mic_read_buffer, buf_len);
        delay(1000);

        // recording = !recording; // Stop recording after one iteration
        // lastRecordingState = true;
    } else {
        // Perform any needed tasks while not recording
        if (lastRecordingState) {
            Serial.println("Stopped Recording...");
            lastRecordingState = false;
        }
        delay(100);
    }
}

