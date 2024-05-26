#include <speaker.h>

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


void check_mp3_ready(const char* check_mp3_url, const char* URL, AudioFileSourceICYStream *file, AudioFileSourceBuffer *buff, AudioOutputI2S *out, AudioGeneratorMP3 *mp3) {

    HTTPClient http;

    http.begin(check_mp3_url);
    Serial.println("HTTP Server Started");
    int httpCode = http.GET();
    Serial.println("HTTP GET Request Sent");

    if (httpCode == 200) {
        Serial.println("HTTP Code 200");
        String payload = http.getString();
        Serial.println(payload);
        if (payload == "mp3 ready") {
            Serial.println("MP3 is ready to be played.");
            audioLogger = &Serial;
            file = new AudioFileSourceICYStream(URL);
            file->RegisterMetadataCB(MDCallback, (void*)"ICY");
            buff = new AudioFileSourceBuffer(file, 2048);
            buff->RegisterStatusCB(StatusCallback, (void*)"buffer");
            out = new AudioOutputI2S(1);
            out -> SetGain(2);
            mp3 = new AudioGeneratorMP3();
            mp3->RegisterStatusCB(StatusCallback, (void*)"mp3");
            mp3->begin(buff, out);
            while (1) {
         if (mp3->isRunning()) {
            if (!mp3->loop()) mp3->stop();
        } else {
            Serial.printf("MP3 done\n");
            delay(1000);
            break;
        }
    } 
        }
    }

    http.end();
}


// void mp3playback(AudioGeneratorMP3 *mp3) {
    
// }


























// // Settings
// static const uint8_t mp3ready_queue_len = 5;

// // Globals
// static QueueHandle_t mp3ready_queue;

// // Websocket connection variables
// // const char* url = "10.105.252.142";  // Replace with your WebSocket server URL
// // const uint16_t port = 8888;
// // const char* endpoint = "/websocket_esp32";

// // WebSocketsClient client;

// // void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
// //     switch(type) {
// //         case WStype_DISCONNECTED:
// //             Serial.printf("[WebSocket] Disconnected!\n");
// //             break;
// //         case WStype_CONNECTED:
// //             Serial.printf("[WebSocket] Connected\n");
// //             break;
// //         case WStype_TEXT:
// //             Serial.printf("[WebSocket] Message received: %s\n", payload);
// //             // sign_buffer_received = true;
// //             break;
// //         case WStype_BIN:
// //             Serial.printf("[WebSocket] Binary data received\n");
// //             break;
// //     }
// // }

// void checkMP3Status(void *parameter) {

//     static int num  = 0;

//     String check_mp3_url = "http://10.105.252.142:8888/checkmp3";

//     HTTPClient http;

//     while (true) {
//         http.begin(check_mp3_url);
//         int httpCode = http.GET();

//         if (httpCode == 200) {
//             String payload = http.getString();
//             if (payload == "mp3 ready") {
//                 Serial.println("MP3 is ready to be played.");
//                 // Try to add item to queue for 10 ticks, fail if queue is full
//                 if (xQueueSend(mp3ready_queue, (void *)&num, 10) != pdTRUE) {
//                     Serial.println("Queue full");
//                 }
//                 num++;
//             } else {
//                 Serial.println("No MP3 ready yet.");
//             }
//         } else {
//             Serial.println("Failed to connect.");
//         }

//         http.end();
//         vTaskDelay(pdMS_TO_TICKS(2000)); // Check every 5 seconds
//     }
// }

// void MP3PlayTask(void *parameter) {
//     // Audio variables initialization
//     AudioGeneratorMP3 *mp3;
//     AudioFileSourceICYStream *file;
//     AudioFileSourceBuffer *buff;
//     AudioOutputI2S *out;

//     int item;

//     while (true) {
//         // See if there's a message in the queue (do not block)
//         if (xQueueReceive(mp3ready_queue, (void *)&item, 0) == pdTRUE) {
//             audioLogger = &Serial;
//             file = new AudioFileSourceICYStream(URL);
//             buff = new AudioFileSourceBuffer(file, 2048);
//             out = new AudioOutputI2S();
//             out->SetGain(2);
//             mp3 = new AudioGeneratorMP3();
//             mp3->begin(buff, out);

//             while (mp3->isRunning()) {
//                 if (!mp3->loop()) mp3->stop();
//             }

//             delete mp3;
//             delete out;
//             delete buff;
//             delete file;
//         }
            
//     }
// }