#include <Arduino.h>
#include <esp_sleep.h>

#define WAKE_UP_PIN 14
#define WAKE_UP_LED_PIN 27

// Define a structure to hold the sleep state
RTC_DATA_ATTR struct {
    bool shouldSleep = true;
} sleepState;


void goToSleep();

bool isButtonPressed();