/*
Purpose: Collects Images
*/


/* Includes ---------------------------------------------------------------- */
#include <Arduino.h>
#include <WebSocketsClient.h>
#include <WiFi.h>
// #include <sign-language_inferencing.h>
// #include "esp_camera.h"
// #include "edge-impulse-sdk/dsp/image/image.hpp"

#include "ei_utils.h"
#include "website.h"


/* Private variables ------------------------------------------------------- */
// const char* url = "10.105.252.142";  // sneh eduroam
const char* url = "10.105.100.183";  // eduroam
// const char* url = "192.168.4.82";  // Replace with your WebSocket server URL
const uint16_t port = 8888;
const char* endpoint = "/websocket_esp32";
static bool debug_nn = false; // Set this to true to see e.g. features generated from the raw signal
static bool is_initialised = false;
uint8_t *snapshot_buf; //points to the output of the capture


/* Function definitions ------------------------------------------------------- */
bool ei_camera_init(void);
bool camera_capture(uint8_t *out_buf);
static int ei_camera_get_data(size_t offset, size_t length, float *out_ptr);
void send_image_to_server(uint8_t *img);
// void classify_image();
uint8_t count = 0;

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



WebSocketsClient client;

/**
* @brief      Arduino setup function
*/
void setup()
{
    Serial.begin(115200);

    Serial.println("Edge Impulse Inferencing Demo");
    delay(2000);
    while (ei_camera_init() == false) {
        Serial.println("Failed to initialize Camera!\r\n");
        delay(500);
    }
    Serial.println("Camera initialized\r\n");

    // Connect to WiFi
    WiFi.mode(WIFI_STA);
	WiFi.begin(SSID);
	
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    // Serial.print("Classifier input width: ");
    // Serial.println(EI_CLASSIFIER_INPUT_WIDTH);
    // Serial.print("Classifier input height: ");
    // Serial.println(EI_CLASSIFIER_INPUT_HEIGHT);
    // Serial.print("Classifier raw frame buffer cols: ");
    // Serial.println(EI_CAMERA_RAW_FRAME_BUFFER_COLS);
    // Serial.print("Classifier raw frame buffer rows: ");
    // Serial.println(EI_CAMERA_RAW_FRAME_BUFFER_ROWS);

    // Connect to WebSocket server
    client.begin(url, port, endpoint);
    client.onEvent(webSocketEvent);
    client.setReconnectInterval(1000);


    delay(2000);


}

/**
* @brief      Get data and run inferencing
*
* @param[in]  debug  Get debug info if true
*/
void loop()
{
    
    // client.sendTXT("hello");
    client.loop();


    snapshot_buf = (uint8_t*)malloc(EI_CAMERA_IMAGE_SIZE);

    // check if allocation was successful
    if(snapshot_buf == nullptr) {
        Serial.println("ERR: Failed to allocate snapshot buffer!\n");
        return;
    }

    client.loop();
    if (camera_capture(snapshot_buf) == false) {
        Serial.println("Failed to capture image\r\n");
        free(snapshot_buf);
    }

    // classify_image();

    free(snapshot_buf);


}

void send_image_to_server(camera_fb_t *fb) {
    // send jpeg image to server
    // client.sendBIN(img, sizeof(img));
    client.sendBIN((const uint8_t*)fb->buf, fb->len);
    // Serial.print("Sent image ");
    // Serial.println(count);
}

/**
 * @brief   Setup image sensor & start streaming
 *
 * @retval  false if initialisation failed
 */
bool ei_camera_init(void) {

    if (is_initialised) return true;

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

    //initialize the camera
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK) {
      Serial.printf("Camera init failed with error 0x%x\n", err);
      return false;
    }

    sensor_t * s = esp_camera_sensor_get();
    // initial sensors are flipped vertically and colors are a bit saturated
    if (s->id.PID == OV3660_PID) {
      s->set_vflip(s, 1); // flip it back
      s->set_brightness(s, 1); // up the brightness just a bit
      s->set_saturation(s, 0); // lower the saturation
    }

    is_initialised = true;
    return true;
}


/**
 * @brief      Capture, rescale and crop image
 *
 * @param[in]  img_width     width of output image
 * @param[in]  img_height    height of output image
 * @param[in]  out_buf       pointer to store output image, NULL may be used
 *                           if ei_camera_frame_buffer is to be used for capture and resize/cropping.
 *
 * @retval     false if not initialised, image captured, rescaled or cropped failed
 *
 */
bool camera_capture(uint8_t *out_buf) {
    bool do_resize = false;

    if (!is_initialised) {
        Serial.println("ERR: Camera is not initialized\r\n");
        return false;
    }

    camera_fb_t *fb = esp_camera_fb_get();

    if (!fb) {
        Serial.println("Camera capture failed\n");
        return false;
    }
    client.loop();
    delay(40);
    send_image_to_server(fb);
    // client.loop();
    esp_camera_fb_return(fb);

    return true;
}

static int ei_camera_get_data(size_t offset, size_t length, float *out_ptr)
{
    // we already have a RGB888 buffer, so recalculate offset into pixel index
    size_t pixel_ix = offset * 3;
    size_t pixels_left = length;
    size_t out_ptr_ix = 0;

    while (pixels_left != 0) {
        // Swap BGR to RGB here
        // due to https://github.com/espressif/esp32-camera/issues/379
        out_ptr[out_ptr_ix] = (snapshot_buf[pixel_ix + 2] << 16) + (snapshot_buf[pixel_ix + 1] << 8) + snapshot_buf[pixel_ix];

        // go to the next pixel
        out_ptr_ix++;
        pixel_ix+=3;
        pixels_left--;
    }
    // and done!
    return 0;
}

// void classify_image() {
//     ei::signal_t signal;
//     signal.total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
//     signal.get_data = &ei_camera_get_data;

//     // Run the classifier
//     ei_impulse_result_t result = { 0 };
//     client.loop();

//     EI_IMPULSE_ERROR err = run_classifier(&signal, &result, debug_nn);
//     if (err != EI_IMPULSE_OK) {
//         ei_printf("ERR: Failed to run classifier (%d)\n", err);
//         return;
//     }

//     // print the predictions
//     ei_printf("Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms.): \n",
//                 result.timing.dsp, result.timing.classification, result.timing.anomaly);

// #if EI_CLASSIFIER_OBJECT_DETECTION == 1
//     bool bb_found = result.bounding_boxes[0].value > 0;
//     for (size_t ix = 0; ix < result.bounding_boxes_count; ix++) {
//         auto bb = result.bounding_boxes[ix];
//         if (bb.value == 0) {
//             continue;
//         }
//         ei_printf("    %s (%f) [ x: %u, y: %u, width: %u, height: %u ]\n", bb.label, bb.value, bb.x, bb.y, bb.width, bb.height);
//     }
//     if (!bb_found) {
//         ei_printf("    No objects found\n");
//     }
// #else
//     for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
//         ei_printf("    %s: %.5f\n", result.classification[ix].label,
//                                     result.classification[ix].value);
//     }
// #endif

// #if EI_CLASSIFIER_HAS_ANOMALY == 1
//         ei_printf("    anomaly score: %.3f\n", result.anomaly);
// #endif
// }