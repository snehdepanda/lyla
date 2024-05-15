#include <Arduino.h>
#include <WiFiManager.h>
#include "AudioFileSourceICYStream.h"
#include "AudioFileSourceBuffer.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"
#include <WebSocketsClient.h>
// Called when a metadata event occurs (i.e. an ID3 tag, an ICY block, etc.
void MDCallback(void *cbData, const char *type, bool isUnicode, const char *string)
{
  const char *ptr = reinterpret_cast<const char *>(cbData);
  (void) isUnicode; // Punt this ball for now
  // Note that the type and string may be in PROGMEM, so copy them to RAM for printf
  char s1[32], s2[64];
  strncpy_P(s1, type, sizeof(s1));
  s1[sizeof(s1)-1]=0;
  strncpy_P(s2, string, sizeof(s2));
  s2[sizeof(s2)-1]=0;
  Serial.printf("METADATA(%s) '%s' = '%s'\n", ptr, s1, s2);
  Serial.flush();
}

// Called when there's a warning or error (like a buffer underflow or decode hiccup)
void StatusCallback(void *cbData, int code, const char *string)
{
  const char *ptr = reinterpret_cast<const char *>(cbData);
  // Note that the string may be in PROGMEM, so copy it to RAM for printf
  char s1[64];
  strncpy_P(s1, string, sizeof(s1));
  s1[sizeof(s1)-1]=0;
  Serial.printf("STATUS(%s) '%d' = '%s'\n", ptr, code, s1);
  Serial.flush();
}

// void setupSpeaker(const char *URL, AudioFileSourceICYStream *file, AudioFileSourceBuffer *buff, AudioOutputI2S *out, AudioGeneratorMP3 *mp3) {
    
// }

// Tornado server URL
const char *URL="http://10.105.252.142:8888/mp3";
// Websocket connection variables
// const char* url = "10.105.252.142";  // Replace with your WebSocket server URL
// const uint16_t port = 8888;
// const char* endpoint = "/websocket_esp32";

WebSocketsClient client;


AudioGeneratorMP3 *mp3;
AudioFileSourceICYStream *file;
AudioFileSourceBuffer *buff;
AudioOutputI2S *out;

// bool setup_done = false;

// // Event flags
// bool sign_buffer_received = false;
// bool speaker_setup = false; 

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
            // sign_buffer_received = true;
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
  WiFiManager wifiManager;
  wifiManager.autoConnect("AutoConnectAP"); 
  Serial.println("Connected to WiFi!");

// //   // Connect to WebSocket server
//   client.begin(url, port, endpoint);
//   client.onEvent(webSocketEvent);
//   client.setReconnectInterval(5000);
    audioLogger = &Serial;
    file = new AudioFileSourceICYStream(URL);
    file->RegisterMetadataCB(MDCallback, (void*)"ICY");
    buff = new AudioFileSourceBuffer(file, 2048);
    buff->RegisterStatusCB(StatusCallback, (void*)"buffer");
    out = new AudioOutputI2S();
    out -> SetGain(2);
    mp3 = new AudioGeneratorMP3();
    mp3->RegisterStatusCB(StatusCallback, (void*)"mp3");
    mp3->begin(buff, out);
  
}


void loop()
 {
//   while (!sign_buffer_received) {
//     client.loop();
//   }

  static int lastms = 0;
  if (mp3->isRunning()) {
    if (millis()-lastms > 1000) {
      lastms = millis();
      Serial.printf("Running for %d ms...\n", lastms);
      Serial.flush();
     }
    if (!mp3->loop()) mp3->stop();
  } else {
    Serial.printf("MP3 done\n");
    delay(1000);
  }
  }