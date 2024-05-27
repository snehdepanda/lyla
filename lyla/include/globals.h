#ifndef GLOBAL_H
#define GLOBAL_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "esp_camera.h"

extern Adafruit_SSD1306 display;
extern uint8_t *snapshot_buf;
extern bool is_initialised;
extern sensor_t *s;

#endif