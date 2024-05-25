// /*
// Pseudocode (Assume device always on, so no wake word)
// Initialize I2S
// Initialize Camera
// Initialize Wifi
// Set threshold for confidence
// initialize character array
// Sample continuously at a specified interval:
//     capture image
//     if image classification is over threshold:
//         turn on led signalling letter is recognized
//         add classified character to character array
//         turn off led
//         delay 0.5s
// break after specified condition
// send character array to amazon aws
// */



/* Includes ---------------------------------------------------------------- */
#include <Arduino.h>
#include <WebSocketsClient.h>
#include <sign-language_inferencing.h>
#include "edge-impulse-sdk/dsp/image/image.hpp"
#include <WiFi.h>
#include "esp_camera.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "ei_utils.h"
#include "website.h"

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Pins

static const int led_pin = 11;

void toggleLED(void *parameter) {
    while(1) {
        digitalWrite(led_pin, HIGH);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        digitalWrite(led_pin, LOW);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void setup() {
    pinMode(led_pin, OUTPUT);

    xTaskCreatePinnedToCore(
        toggleLED,
        "Toggle LED",
        1024,
        NULL,
        1,
        NULL,
        app_cpu);
}

void loop() {
    
}