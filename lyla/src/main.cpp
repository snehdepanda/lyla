#include <Arduino.h>
#include "mic.h"

void setup() {
  Serial.begin(115200);
  setupI2S();
}


void loop() {
  static int32_t raw_samples[SAMPLE_BUFFER_SIZE];
  size_t bytes_read;
  mic_i2s_to_buffer(raw_samples, SAMPLE_BUFFER_SIZE);
}
