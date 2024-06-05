/*
task 1: camera sign inference
*/


/* Includes ---------------------------------------------------------------- */
#include <Arduino.h>
// #include <WebSocketsClient.h>
// #include <sign-language_inferencing.h>
// #include "edge-impulse-sdk/dsp/image/image.hpp"
#include <WiFi.h>
#include "esp_camera.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "ei_utils.h"
#include "website.h"
#include "globals.h"
// #include "deepsleep.h"

void toggleLED(void *parameter);
void flipInfer();
void goToSleep();
bool isButtonPressed();
void displayTask();

// #if CONFIG_FREERTOS_UNICORE
// static const BaseType_t app_cpu = 0;
// #else
// static const BaseType_t app_cpu = 1;
// #endif

// /*Wifi definitions*/
#define CAMPUS
#if defined(CAMPUS)
#define SSID        "Device-Northwestern"
#elif defined(ASBURY)
#define SSID        "2146 Asbury"
#define PASSWORD    "NanoGold2146"
#else
#error "WiFi not selected"
#endif



Adafruit_SSD1306 display(128, 32, &Wire, -1);

RTC_DATA_ATTR struct {
    bool shouldSleep = true;
} sleepState;

volatile bool infer = true;
bool is_initialised = false;

void setup() {
    Serial.begin(115200);
    Serial.println("Device on");


    pinMode(S3_IND, OUTPUT);
    digitalWrite(S3_IND, HIGH);
    pinMode(WAKE_UP_PIN, INPUT_PULLUP);

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
    }

    vTaskDelay(2000/portTICK_PERIOD_MS);

    pinMode(SIGN, OUTPUT);
    digitalWrite(SIGN, LOW);
    pinMode(INFER_LED, OUTPUT);
    digitalWrite(INFER_LED, LOW);

    pinMode(CAM_INFER, INPUT_PULLUP);
    // pinMode(MISC_BUTTON, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(CAM_INFER), flipInfer, FALLING);

    while (ei_camera_init() == false) {
        Serial.println("Failed to initialize Camera!\r\n");
        delay(500);
    }
    is_initialised = true;
    Serial.println("Camera Initialized");
    Wire.setPins(LCD_SDA, LCD_SCL);

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
    }
    Serial.println("LCD screen initialized!");

    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    wsInitialize();

    // xTaskCreate(
    //     toggleLED,
    //     "Toggle Infer LED",
    //     1024,
    //     NULL,
    //     2,
    //     NULL
    // );
    // Serial.println("Toggling Task created");

    xTaskCreatePinnedToCore(
        signInference,
        "Signing Inference",
        12000,
        NULL,
        3,
        NULL,
        (BaseType_t)0
    );
    Serial.println("Inference Task created");

    xTaskCreatePinnedToCore(
        wsConnect,
        "Websocket Connection",
        2048,
        NULL,
        1,
        NULL,
        wifi_core
    );
    Serial.println("Websockets Connection Task created");
}

void loop() {
    if (isButtonPressed()) {
      Serial.println("Button pressed, toggling sleep state.");
      sleepState.shouldSleep = !sleepState.shouldSleep;
      if (sleepState.shouldSleep) {
        goToSleep();
      }
    }
}

void toggleLED(void *parameter) {
    while(1) {
        if (infer) {
            digitalWrite(INFER_LED, HIGH);
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
        else {
            digitalWrite(INFER_LED, LOW);
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
    }
}


void flipInfer() {
    infer = !infer;
    digitalWrite(INFER_LED, infer);
}


void goToSleep() {
  Serial.println("Entering deep sleep...");
  delay(1000); // Delay to allow serial messages to complete
  esp_deep_sleep_start();
}

bool isButtonPressed() {
  if (digitalRead(WAKE_UP_PIN) == LOW) {
    delay(100); // Debounce delay
    if (digitalRead(WAKE_UP_PIN) == LOW) {
      while (digitalRead(WAKE_UP_PIN) == LOW); // Wait for button release to avoid multiple toggles
      return true;
    }
  }
  return false;
}