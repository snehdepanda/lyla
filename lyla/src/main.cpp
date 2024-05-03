#include <Arduino.h>
#include <WiFiManager.h>
#include <speaker.h>

// Tornado server URL
const char *URL="http://10.105.252.142:8888/mp3";

AudioGeneratorMP3 *mp3;
AudioFileSourceICYStream *file;
AudioFileSourceBuffer *buff;
AudioOutputI2S *out;


void setup()
{
  Serial.begin(115200);
  delay(1000);
  WiFiManager wifiManager;
  wifiManager.autoConnect("AutoConnectAP"); 
  Serial.println("Connected to WiFi!");
  

  setupSpeaker(URL, file, buff, out, mp3);
}


void loop()
{
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