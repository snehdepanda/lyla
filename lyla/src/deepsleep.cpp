// #include <Arduino.h>
// #include "deepsleep.h"


// RTC_DATA_ATTR struct {
//     bool shouldSleep = true;
// } sleepState;

// void goToSleep() {
//   Serial.println("Entering deep sleep...");
//   delay(1000); // Delay to allow serial messages to complete
//   esp_deep_sleep_start();
// }

// bool isButtonPressed() {
//   if (digitalRead(WAKE_UP_PIN) == LOW) {
//     delay(100); // Debounce delay
//     if (digitalRead(WAKE_UP_PIN) == LOW) {
//       while (digitalRead(WAKE_UP_PIN) == LOW); // Wait for button release to avoid multiple toggles
//       return true;
//     }
//   }
//   return false;
// }