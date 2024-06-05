#ifndef GLOBAL_H
#define GLOBAL_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "esp_camera.h"
#include <WebSocketsClient.h>

extern Adafruit_SSD1306 display;
extern uint8_t *snapshot_buf;
extern bool is_initialised;
extern sensor_t *s;
extern volatile bool infer;
extern WebSocketsClient client;

#define CAMERA_ESP32_S3
// defines
#if defined(CAMERA_ESP32_S3)
#define S3_IND          14          // LED: ESP is on
#define SIGN            11          // LED: sign recognized
#define CAM_INFER       2           // Button: toggle inference
#define INFER_LED       45          // LED: inference is on
#define WAKE_UP_PIN     12          // Button: toggle sleep
#define MISC_BUTTON     2           // Button: random button
#define LCD_SDA         41          // LCD SDA
#define LCD_SCL         42          // LCD SCL

#elif defined(CAMERA_ESP32_CAM)
// #define S3_IND          16          // bruh can't use io16
#define SIGN            13
#define CAM_INFER       15
#define WAKE_UP_PIN     12
#define MISC_BUTTON     4
#define LCD_SDA         2
#define LCD_SCL         14
#endif

#endif