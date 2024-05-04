// // if you define WIFI_SSID and WIFI_PASS before importing the library, 
// // you can call connect() instead of connect(ssid, pass)
// //

// #include <Arduino.h>
// #include <Wifi.h>
// #include <WiFiManager.h>

// #define WIFI_SSID "2146 Asbury"
// #define WIFI_PASS "NanoGold2146"
// #define HOSTNAME "esp32cam"
// #define IMG_SIZE 9216

// #include <eloquent_esp32cam.h>
// #include <eloquent_esp32cam/extra/esp32/wifi/sta.h>
// #include <eloquent_esp32cam/viz/image_collection.h>
// #include <sign-language_inferencing.h>
// #include "edge-impulse-sdk/dsp/image/image.hpp"

// #include "mic.h"
// #include "ei_utils.h"

// using eloq::camera;
// using eloq::wifi;
// using eloq::viz::collectionServer;

// /* Constant defines -------------------------------------------------------- */
// #define EI_CAMERA_RAW_FRAME_BUFFER_COLS           320
// #define EI_CAMERA_RAW_FRAME_BUFFER_ROWS           240
// #define EI_CAMERA_FRAME_BYTE_SIZE                 3

// /* Private variables ------------------------------------------------------- */
// static bool debug_nn = false; // Set this to true to see e.g. features generated from the raw signal
// static bool is_initialised = false;
// uint8_t *snapshot_buf; //points to the output of the capture


// // Image has 9216 pixels with rgb values for each pixel, so 24 bits per pixel
// const uint32_t image_buffer[2*IMG_SIZE] = {0};
// const uint32_t features[IMG_SIZE] = {0};

// void ei_camera_deinit(void);
// bool ei_camera_capture(uint32_t img_width, uint32_t img_height, uint8_t *out_buf);
// static int ei_camera_get_data(size_t offset, size_t length, float *out_ptr);

// // used in static_buffer.ino as one way to get data
// int raw_feature_get_data(size_t offset, size_t length, float *out_ptr) {
//     memcpy(out_ptr, features + offset, length * sizeof(float));
//     return 0;
// }

// void print_inference_result(ei_impulse_result_t result);

// void setup() {
//     delay(3000);
//     Serial.begin(115200);
//     Serial.println("EE 327 Project");

//     // connect to WiFi
//     // Use AutoConnect
//     // WiFiManager wifiManager;
//     // wifiManager.autoConnect("AutoConnectAP1"); 
//     // Serial.println("Connected to WiFi!");    

//     // camera settings
//     // replace with your own model!
//     camera.pinout.aithinker();
//     camera.brownout.disable();
//     // Edge Impulse models use square images 96x96
//     camera.resolution.square96();
//     camera.quality.high();

//     // init camera
//     while (!camera.begin().isOk())
//         Serial.println(camera.exception.toString());
//         delay(1000);
//     is_initialised = true;

//     // Using house wifi
//     // while (!wifi.connect().isOk())
//     //   Serial.println(wifi.exception.toString());

//     // // init face detection http server
//     // while (!collectionServer.begin().isOk())
//     //     Serial.println(collectionServer.exception.toString());

//     Serial.println("Camera OK");
//     Serial.println("WiFi OK");
//     Serial.println("Starting inference");
//     // Serial.println(collectionServer.address());
// }


// void loop() {
//     delay(2000);
//     // server runs in a separate thread, no need to do anything here

//     snapshot_buf = (uint8_t*)malloc(EI_CAMERA_RAW_FRAME_BUFFER_COLS * EI_CAMERA_RAW_FRAME_BUFFER_ROWS * EI_CAMERA_FRAME_BYTE_SIZE);
//     // check if allocation was successful
//     if(snapshot_buf == nullptr) {
//         ei_printf("ERR: Failed to allocate snapshot buffer!\n");
//         return;
//     }

//     signal_t signal;
//     signal.total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
//     signal.get_data = &ei_camera_get_data;

//     if (ei_camera_capture((size_t)EI_CLASSIFIER_INPUT_WIDTH, (size_t)EI_CLASSIFIER_INPUT_HEIGHT, snapshot_buf) == false) {
//         ei_printf("Failed to capture image\r\n");
//         free(snapshot_buf);
//         return;
//     }

//     // // check if correct size
//     // if (sizeof(features) / sizeof(uint32_t) != IMG_SIZE) {
//     //     ei_printf("The size of your 'features' array is not correct. Expected %lu items, but had %lu\n",
//     //         IMG_SIZE, sizeof(features) / sizeof(uint32_t));
//     //     delay(1000);
//     //     return;
//     // }

//     // run the classifier
//     ei_impulse_result_t result = {0};
    

//     // invoke the impulse
//     EI_IMPULSE_ERROR err = run_classifier(&signal, &result, debug_nn);
//     if (err != EI_IMPULSE_OK) {
//         ei_printf("ERR: Failed to run classifier (%d)\n", err);
//         return;
//     }

//     // print inference return code
//     ei_printf("run_classifier returned: %d\r\n", err);
//     print_inference_result(result);

//     delay(1000);
// }




// void print_inference_result(ei_impulse_result_t result) {

//     // Print how long it took to perform inference
//     ei_printf("Timing: DSP %d ms, inference %d ms, anomaly %d ms\r\n",
//             result.timing.dsp,
//             result.timing.classification,
//             result.timing.anomaly);

//     // Print the prediction results (object detection)
// #if EI_CLASSIFIER_OBJECT_DETECTION == 1
//     ei_printf("Object detection bounding boxes:\r\n");
//     for (uint32_t i = 0; i < result.bounding_boxes_count; i++) {
//         ei_impulse_result_bounding_box_t bb = result.bounding_boxes[i];
//         if (bb.value == 0) {
//             continue;
//         }
//         ei_printf("  %s (%f) [ x: %u, y: %u, width: %u, height: %u ]\r\n",
//                 bb.label,
//                 bb.value,
//                 bb.x,
//                 bb.y,
//                 bb.width,
//                 bb.height);
//     }

//     // Print the prediction results (classification)
// #else
//     ei_printf("Predictions:\r\n");
//     for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
//         ei_printf("  %s: ", ei_classifier_inferencing_categories[i]);
//         ei_printf("%.5f\r\n", result.classification[i].value);
//     }
// #endif

//     // Print anomaly result (if it exists)
// #if EI_CLASSIFIER_HAS_ANOMALY == 1
//     ei_printf("Anomaly prediction: %.3f\r\n", result.anomaly);
// #endif

// }



// /**
//  * @brief      Stop streaming of sensor data
//  */
// void ei_camera_deinit(void) {

//     //deinitialize the camera
//     esp_err_t err = esp_camera_deinit();

//     if (err != ESP_OK)
//     {
//         ei_printf("Camera deinit failed\n");
//         return;
//     }

//     is_initialised = false;
//     return;
// }


// /**
//  * @brief      Capture, rescale and crop image
//  *
//  * @param[in]  img_width     width of output image
//  * @param[in]  img_height    height of output image
//  * @param[in]  out_buf       pointer to store output image, NULL may be used
//  *                           if ei_camera_frame_buffer is to be used for capture and resize/cropping.
//  *
//  * @retval     false if not initialised, image captured, rescaled or cropped failed
//  *
//  */
// bool ei_camera_capture(uint32_t img_width, uint32_t img_height, uint8_t *out_buf) {
//     bool do_resize = false;

//     if (!is_initialised) {
//         ei_printf("ERR: Camera is not initialized\r\n");
//         return false;
//     }

//     camera_fb_t *fb = esp_camera_fb_get();

//     if (!fb) {
//         ei_printf("Camera capture failed\n");
//         return false;
//     }

//    bool converted = fmt2rgb888(fb->buf, fb->len, PIXFORMAT_JPEG, snapshot_buf);

//    esp_camera_fb_return(fb);

//    if(!converted){
//        ei_printf("Conversion failed\n");
//        return false;
//    }

//     if ((img_width != EI_CAMERA_RAW_FRAME_BUFFER_COLS)
//         || (img_height != EI_CAMERA_RAW_FRAME_BUFFER_ROWS)) {
//         do_resize = true;
//     }

//     if (do_resize) {
//         ei::image::processing::crop_and_interpolate_rgb888(
//         out_buf,
//         EI_CAMERA_RAW_FRAME_BUFFER_COLS,
//         EI_CAMERA_RAW_FRAME_BUFFER_ROWS,
//         out_buf,
//         img_width,
//         img_height);
//     }


//     return true;
// }

// static int ei_camera_get_data(size_t offset, size_t length, float *out_ptr)
// {
//     // we already have a RGB888 buffer, so recalculate offset into pixel index
//     size_t pixel_ix = offset * 3;
//     size_t pixels_left = length;
//     size_t out_ptr_ix = 0;

//     while (pixels_left != 0) {
//         // Swap BGR to RGB here
//         // due to https://github.com/espressif/esp32-camera/issues/379
//         out_ptr[out_ptr_ix] = (snapshot_buf[pixel_ix + 2] << 16) + (snapshot_buf[pixel_ix + 1] << 8) + snapshot_buf[pixel_ix];

//         // go to the next pixel
//         out_ptr_ix++;
//         pixel_ix+=3;
//         pixels_left--;
//     }
//     // and done!
//     return 0;
// }