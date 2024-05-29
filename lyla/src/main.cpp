#include <Arduino.h>
#include <wakeup.h>
#include <speaker.h>
#include <WebSocketsClient.h>
#include <wifi_reconnect.h>
#include <HTTPClient.h>
#include <mic.h>

// Tornado server URL to fetch MP3 stream
const char *mp3_url="http://10.105.252.142:8888/mp3";
const char *check_mp3_url = "http://10.105.252.142:8888/checkmp3";

// WebSocket Initialization
WebSocketsClient webSocket;
const char* websocket_server_host = "10.105.252.142";
const uint16_t websocket_server_port = 8888; 
const char* websocket_path = "/audio";


// Mic Recording Button definitions
#define PUSH_BUTTON_PIN 15  // Change as per your pushbutton GPIO connection
volatile bool recording = false;
volatile bool buttonPressed = false;

// I2S buffer for mic initializations
int32_t mic_read_buffer[buf_len] = {0};

// Buffer counter global var
uint8_t buf_counter = 0;

// Audio variables initialization
AudioGeneratorMP3 *mp3;
AudioFileSourceICYStream *file;
AudioFileSourceBuffer *buff;
AudioOutputI2S *out;

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
        digitalWrite(LED_PIN, HIGH);  // Turn on LED to indicate recording

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
        digitalWrite(LED_PIN, LOW);  // Turn off LED
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


void setup()
{
  Serial.begin(115200);
  delay(1000);
  pinMode(LED_PIN, OUTPUT);
  pinMode(WAKE_UP_PIN, INPUT_PULLUP);
  pinMode(WAKE_UP_LED_PIN, OUTPUT);

  // Enable wakeup by external pin
  esp_sleep_enable_ext0_wakeup((gpio_num_t)WAKE_UP_PIN, LOW);

  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0) {
    Serial.println("Woken up by external signal using RTC_IO.");
    sleepState.shouldSleep = !sleepState.shouldSleep;  // Toggle the sleep state
  } else {
    Serial.println("Power on or external reset.");
  }

  if (sleepState.shouldSleep) {
    Serial.println("Going to sleep now.");
    goToSleep();
  } else {
    digitalWrite(WAKE_UP_LED_PIN, HIGH); // Turn on LED to indicate wake up mode
  }

  wifi_setup();
  setupI2Smic();

  pinMode(PUSH_BUTTON_PIN, INPUT_PULLUP);  // Setup button pin
  attachInterrupt(PUSH_BUTTON_PIN, handleButtonPress, CHANGE);  // Attach interrupt to handle button press

  webSocket.begin(websocket_server_host, websocket_server_port, websocket_path);
  webSocket.onEvent(webSocketEvent);


// //   // Connect to WebSocket server
//   client.begin(url, port, endpoint);
//   client.onEvent(webSocketEvent);
//   client.setReconnectInterval(5000);




    
    // mp3ready_queue = xQueueCreate(mp3ready_queue_len, sizeof(int));

    // xTaskCreatePinnedToCore(checkMP3Status, "CheckMP3Status", 2048, NULL, 1, NULL, 0);
    // xTaskCreatePinnedToCore(MP3PlayTask, "MP3Playback", 5000, NULL, 1, NULL, 1);
}


void loop()
 {
//   while (!sign_buffer_received) {
//     client.loop();
//   }

    webSocket.loop();

    // Check if the button is pressed again to toggle the sleep state
    if (isButtonPressed()) {
      Serial.println("Button pressed, toggling sleep state.");
      sleepState.shouldSleep = !sleepState.shouldSleep;
      if (sleepState.shouldSleep) {
        goToSleep();
      } else {
        digitalWrite(WAKE_UP_LED_PIN, HIGH); // Turn on LED to indicate wake up mode
      
      }
    }
    if (webSocket.isConnected()) {
        startAudioCollection();
    } else {
        Serial.println("WebSocket not connected...");
        digitalWrite(LED_PIN, LOW);  // Turn off LED
    }

    check_mp3_ready(check_mp3_url, mp3_url, file, buff, out, mp3);
}




// void setup() {
//   Serial.begin(115200);
//   pinMode(WAKE_UP_PIN, INPUT_PULLUP);

//   // Enable wakeup by external pin
//   esp_sleep_enable_ext0_wakeup((gpio_num_t)WAKE_UP_PIN, LOW);

//   if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0) {
//     Serial.println("Woken up by external signal using RTC_IO.");
//     sleepState.shouldSleep = !sleepState.shouldSleep;  // Toggle the sleep state
//   } else {
//     Serial.println("Power on or external reset.");
//   }

//   if (sleepState.shouldSleep) {
//     Serial.println("Going to sleep now.");
//     goToSleep();
//   }
// }

// void loop() {
//   Serial.println("ESP32 is now awake and running. Press the button to go to sleep.");
//   delay(1000); // Simulate some activity

//   // Check if the button is pressed again to toggle the sleep state
//   if (isButtonPressed()) {
//     Serial.println("Button pressed, toggling sleep state.");
//     sleepState.shouldSleep = !sleepState.shouldSleep;
//     if (sleepState.shouldSleep) {
//       goToSleep();
//     }
//   }
// }

