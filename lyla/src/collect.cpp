// // /*
// // Pseudocode (Assume device always on, so no wake word)
// // Initialize I2S
// // Initialize Camera
// // Initialize Microphone
// // Initialize Speaker
// // Initialize Wifi
// // Set threshold for confidence
// // initialize character array
// // Sample continuously at a specified interval:
// //     capture image
// //     if image classification is over threshold:
// //         turn on led signalling letter is recognized
// //         add classified character to character array
// //         turn off led
// //         delay 0.5s
// // break after specified condition
// // send character array to amazon aws
// // */



// /* Includes ---------------------------------------------------------------- */
// #include <Arduino.h>
// #include <WebSocketsClient.h>
// #include <WiFi.h>
// #include "esp_camera.h"

// #include "ei_utils.h"
// #include "website.h"


// /* Private variables ------------------------------------------------------- */
// // const char* url = "10.105.252.142";  // sneh eduroam
// const char* url = "10.105.100.183";  // eduroam
// // const char* url = "192.168.4.82";  // Replace with your WebSocket server URL
// const uint16_t port = 8888;
// const char* endpoint = "/websocket_esp32";
// static bool debug_nn = false; // Set this to true to see e.g. features generated from the raw signal
// static bool is_initialised = false;
// uint8_t *snapshot_buf; //points to the output of the capture


// /* Function definitions ------------------------------------------------------- */
// bool ei_camera_init(void);
// void ei_camera_deinit(void);
// bool ei_camera_capture(uint32_t img_width, uint32_t img_height, uint8_t *out_buf) ;
// static int ei_camera_get_data(size_t offset, size_t length, float *out_ptr);
// void send_image_to_server(uint8_t *img);

// // /*Wifi definitions*/
// #define CAMPUS
// #if defined(CAMPUS)
// #define SSID        "Device-Northwestern"
// #elif defined(ASBURY)
// #define SSID        "2146 Asbury"
// #define PASSWORD    "NanoGold2146"
// #else
// #error "WiFi not selected"
// #endif


// const uint8_t num_chars = 5;
// char tokens[num_chars];
// static uint8_t ind = 0;


// WebSocketsClient client;

// /**
// * @brief      Arduino setup function
// */
// void setup()
// {
//     Serial.begin(115200);

//     Serial.println("Edge Impulse Inferencing Demo");
//     delay(2000);
//     while (ei_camera_init() == false) {
//         Serial.println("Failed to initialize Camera!\r\n");
//         delay(500);
//     }
//     Serial.println("Camera initialized\r\n");

//     // Connect to WiFi
//     WiFi.mode(WIFI_STA);
// 	WiFi.begin(SSID);
	
//     while (WiFi.status() != WL_CONNECTED) {
//         delay(1000);
//         Serial.println("Connecting to WiFi...");
//     }
//     Serial.print("IP address: ");
//     Serial.println(WiFi.localIP());

//     // Connect to WebSocket server
//     client.begin(url, port, endpoint);
//     client.onEvent(webSocketEvent);
//     client.setReconnectInterval(5000);


//     Serial.println("\nStarting continious inference in 2 seconds...\n");
//     delay(2000);


// }

// /**
// * @brief      Get data and run inferencing
// *
// * @param[in]  debug  Get debug info if true
// */
// void loop()
// {
    
//     // client.sendTXT("hello");
//     client.loop();


//     snapshot_buf = (uint8_t*)malloc(EI_CAMERA_IMAGE_SIZE);

//     // check if allocation was successful
//     if(snapshot_buf == nullptr) {
//         Serial.println("ERR: Failed to allocate snapshot buffer!\n");
//         return;
//     }

//     client.loop();
//     if (ei_camera_capture((size_t)240, (size_t)240, snapshot_buf) == false) {
//         Serial.println("Failed to capture image\r\n");
//         free(snapshot_buf);
//         return;
//     }
//     client.loop();


// }

// void send_image_to_server(camera_fb_t *fb) {
//     // send jpeg image to server
//     // client.sendBIN(img, sizeof(img));
//     client.sendBIN((const uint8_t*)fb->buf, fb->len);
// }

// /**
//  * @brief   Setup image sensor & start streaming
//  *
//  * @retval  false if initialisation failed
//  */
// bool ei_camera_init(void) {

//     if (is_initialised) return true;

// #if defined(CAMERA_MODEL_ESP_EYE)
//   pinMode(13, INPUT_PULLUP);
//   pinMode(14, INPUT_PULLUP);
// #endif

//     //initialize the camera
//     esp_err_t err = esp_camera_init(&camera_config);
//     if (err != ESP_OK) {
//       Serial.printf("Camera init failed with error 0x%x\n", err);
//       return false;
//     }

//     sensor_t * s = esp_camera_sensor_get();
//     // initial sensors are flipped vertically and colors are a bit saturated
//     if (s->id.PID == OV3660_PID) {
//       s->set_vflip(s, 1); // flip it back
//       s->set_brightness(s, 1); // up the brightness just a bit
//       s->set_saturation(s, 0); // lower the saturation
//     }

//     is_initialised = true;
//     return true;
// }

// /**
//  * @brief      Stop streaming of sensor data
//  */
// void ei_camera_deinit(void) {

//     //deinitialize the camera
//     esp_err_t err = esp_camera_deinit();

//     if (err != ESP_OK)
//     {
//         Serial.println("Camera deinit failed\n");
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
//         Serial.println("ERR: Camera is not initialized\r\n");
//         return false;
//     }

//     camera_fb_t *fb = esp_camera_fb_get();

//     if (!fb) {
//         Serial.println("Camera capture failed\n");
//         return false;
//     }
//     client.loop();
//     send_image_to_server(fb);
//     client.loop();

//     esp_camera_fb_return(fb);

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

// #if !defined(EI_CLASSIFIER_SENSOR) || EI_CLASSIFIER_SENSOR != EI_CLASSIFIER_SENSOR_CAMERA
// #error "Invalid model for current sensor"
// #endif