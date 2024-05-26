#include <Arduino.h>
#include <wifi_reconnect.h>

TaskHandle_t wifi_reconnect_task_handle = NULL;

void wifi_setup() {
    WiFiManager wifiManager;
    wifiManager.autoConnect("AutoConnectAP"); 

    while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
    }

    Serial.println("Connected to WiFi!");

    // Start task to reconnect wifi on disconnect
    xTaskCreatePinnedToCore(
    wifi_reconnect_task,
    "wifi_reconnect",
    4096,
    NULL,
    1,
    &wifi_reconnect_task_handle,
    CONFIG_ARDUINO_RUNNING_CORE
    );
}

void wifi_reconnect_task(void * parameters) {
    while (1) {

        if (WiFi.status() == WL_CONNECTED) {
            vTaskDelay(10000 / portTICK_PERIOD_MS);
            continue;
        }

        Serial.println("WiFi disconnected...reconnecting");

        WiFiManager wifiManager;
        wifiManager.autoConnect("AutoConnectAP"); 

        uint32_t start_attempt = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - start_attempt < 10000);
    
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("WiFi reconnection failed!");
            vTaskDelay(20000 / portTICK_PERIOD_MS);
        }
        else {
            Serial.println("WiFi reconnected: " + WiFi.localIP());
        }
    }
}