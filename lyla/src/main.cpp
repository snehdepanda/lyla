/*
Pseudocode (Assume device always on, so no wake word)
Initialize I2S
Initialize Camera
Initialize Wifi
Set threshold for confidence
initialize character array
Sample continuously at a specified interval:
    capture image
    if image classification is over threshold:
        turn on led signalling letter is recognized
        add classified character to character array
        turn off led
        delay 0.5s
break after specified condition
send character array to amazon aws
*/



/* Includes ---------------------------------------------------------------- */
#include <Arduino.h>
#include <WebSocketsClient.h>
#include <sign-language_inferencing.h>
#include "edge-impulse-sdk/dsp/image/image.hpp"
#include <WiFi.h>
#include <Wire.h>

#include "ei_utils.h"
#include "website.h"
#include "globals.h"


/* Private variables ------------------------------------------------------- */
const char* url = "10.105.252.142";  // sneh eduroam
// const char* url = "10.105.100.183";  // eduroam
// const char* url = "192.168.4.82";  // Replace with your WebSocket server URL
const uint16_t port = 8888;
const char* endpoint = "/text2speech";
static bool debug_nn = false; // Set this to true to see e.g. features generated from the raw signal
bool is_initialised = false;
uint8_t *snapshot_buf = nullptr; // points to the output of the capture


/* Function definitions ------------------------------------------------------- */
// bool ei_camera_init(void);
void ei_camera_deinit(void);
bool ei_camera_capture(uint32_t img_width, uint32_t img_height, uint8_t *out_buf) ;
static int ei_camera_get_data(size_t offset, size_t length, float *out_ptr);
void send_image_to_server(uint8_t *img);
// void flipLED();
void flipInfer();
// void flipSleep();
void classify_image();

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

#if defined(CAMERA_ESP32_S3)
#define S3_IND          12
#define SIGN            11
#define CAM_INFER       35
#define INFER_LED        45
#define WAKE_UP_PIN     14
#define MISC_BUTTON     2
#define LCD_SDA         41
#define LCD_SCL         42

#elif defined(CAMERA_ESP32_CAM)
#define S3_IND          16
#define SIGN            13
#define CAM_INFER       15
#define INFER_LED       -1
#define WAKE_UP_PIN     12
#define MISC_BUTTON     4
#define LCD_SDA         2
#define LCD_SCL         14
#endif

RTC_DATA_ATTR struct {
    bool shouldSleep = true;
} sleepState;

volatile bool infer = true;

const uint8_t num_chars = 10;
char tokens[num_chars];
static uint8_t ind = 0;
static uint8_t lcdColumns = 16;
static uint8_t lcdRows = 2;


WebSocketsClient client;
Adafruit_SSD1306 display(128, 32, &Wire, -1);

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


/**
* @brief      Arduino setup function
*/
void setup()
{
    Serial.begin(115200);
    Serial.println("Hello World!"); 

    pinMode(S3_IND, OUTPUT); // LED: S3 is woken up
    digitalWrite(S3_IND, HIGH);
    pinMode(SIGN, OUTPUT); // LED: sign recognized
    digitalWrite(SIGN, LOW);
    pinMode(INFER_LED, OUTPUT);
    digitalWrite(INFER_LED, LOW);

    pinMode(CAM_INFER, INPUT_PULLUP); // Button: start camera inference
    pinMode(WAKE_UP_PIN, INPUT_PULLUP); // Button: wake up from sleep
    pinMode(MISC_BUTTON, INPUT_PULLUP); // Button: miscellaneous

    attachInterrupt(digitalPinToInterrupt(CAM_INFER), flipInfer, FALLING);
    // attachInterrupt(digitalPinToInterrupt(WAKE_UP_PIN), flipSleep, FALLING);
    attachInterrupt(digitalPinToInterrupt(MISC_BUTTON), flipInfer, FALLING);


    // Enable wakeup by external pin
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


    delay(2000);
    while (ei_camera_init() == false) {
        Serial.println("Camera Init Failed!");
        delay(500);
    }
    is_initialised = true;
    Serial.println("Camera initialized\r\n");

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    Wire.setPins(41,42); // ESP32-S3
    // Wire.setPins(2,14); // ESP32-S
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
    }
    Serial.println("LCD screen initialized!");

    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(1);
    display.setTextWrap(true);
    display.setTextColor(WHITE);
    display.print("Waiting... ");
    display.display();

    // Connect to WiFi
    WiFi.mode(WIFI_STA);
	WiFi.begin(SSID);
	
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Connect to WebSocket server
    client.begin(url, port, endpoint);
    client.onEvent(webSocketEvent);
    client.setReconnectInterval(2000);


    ei_printf("\nStarting continious inference in 2 seconds...\n");
    // Serial.println(eloq::viz::collectionServer.address());
    ei_sleep(1000);


}

/**
* @brief      Get data and run inferencing
*
* @param[in]  debug  Get debug info if true
*/
void loop()
{
    if (isButtonPressed()) {
      Serial.println("Button pressed, toggling sleep state.");
      sleepState.shouldSleep = !sleepState.shouldSleep;
      if (sleepState.shouldSleep) {
        goToSleep();
      }
    }

    delay(200);
    if (infer) {
        client.loop();

        // instead of wait_ms, we'll wait on the signal, this allows threads to cancel us...
        if (ei_sleep(5) != EI_IMPULSE_OK) {
            return;
        }

        snapshot_buf = (uint8_t*)malloc(EI_CAMERA_IMAGE_SIZE);

        // check if allocation was successful
        if(snapshot_buf == nullptr) {
            ei_printf("ERR: Failed to allocate snapshot buffer!\n");
            return;
        }

        client.loop();
        if (ei_camera_capture((size_t)EI_CLASSIFIER_INPUT_WIDTH, (size_t)EI_CLASSIFIER_INPUT_HEIGHT, snapshot_buf) == false) {
            ei_printf("Failed to capture image\r\n");
            free(snapshot_buf);
            return;
        }

        classify_image();
        
        free(snapshot_buf);
        // ei_sleep(5);
    }
    else {
        client.loop();
        delay(100);
    }

}


void flipInfer() {
    infer = !infer;
    Serial.print("Infer Flipped to ");
    Serial.println(infer);
    digitalWrite(INFER_LED, infer);
}


void send_image_to_server(camera_fb_t *fb) {
    // send jpeg image to server
    // client.sendBIN(img, sizeof(img));
    client.sendTXT((const char*)fb->buf, fb->len);
}

/**
 * @brief   Setup image sensor & start streaming
 *
 * @retval  false if initialisation failed
 */
// bool ei_camera_init(void) {

//     if (is_initialised) return true;

//     //initialize the camera
//     while (true) {
//         esp_err_t err = esp_camera_init(&camera_config);
//         if (err != ESP_OK) Serial.printf("Camera init failed with error 0x%x\n", err);
//         else break;
//         delay(1000);
//     }
//     // esp_err_t err = esp_camera_init(&camera_config);
//     // if (err != ESP_OK) {
//     //   Serial.printf("Camera init failed with error 0x%x\n", err);
//     //   return false;
//     // }

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

/**
 * @brief      Stop streaming of sensor data
 */
void ei_camera_deinit(void) {

    //deinitialize the camera
    esp_err_t err = esp_camera_deinit();

    if (err != ESP_OK)
    {
        ei_printf("Camera deinit failed\n");
        return;
    }

    is_initialised = false;
    return;
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
bool ei_camera_capture(uint32_t img_width, uint32_t img_height, uint8_t *out_buf) {
    bool do_resize = false;

    if (!is_initialised) {
        ei_printf("ERR: Camera is not initialized\r\n");
        return false;
    }

    camera_fb_t *fb = esp_camera_fb_get();

    if (!fb) {
        ei_printf("Camera capture failed\n");
        return false;
    }

    client.loop();

    bool converted = fmt2rgb888(fb->buf, fb->len, PIXFORMAT_JPEG, out_buf);

    esp_camera_fb_return(fb);

    if(!converted){
        ei_printf("Conversion failed\n");
        return false;
    }

    if ((img_width != EI_CAMERA_RAW_FRAME_BUFFER_COLS)
        || (img_height != EI_CAMERA_RAW_FRAME_BUFFER_ROWS)) {
        do_resize = true;
    }

    if (do_resize) {
        ei::image::processing::crop_and_interpolate_rgb888(
        out_buf,
        EI_CAMERA_RAW_FRAME_BUFFER_COLS,
        EI_CAMERA_RAW_FRAME_BUFFER_ROWS,
        out_buf,
        img_width,
        img_height);
    }


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

void classify_image() {
    ei::signal_t signal;
    signal.total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
    signal.get_data = &ei_camera_get_data;
    ei_printf("input width: %i, input height: %i\n", EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT);

    client.loop();

    ei_impulse_result_t result = { 0 };

    EI_IMPULSE_ERROR err = run_classifier(&signal, &result, debug_nn);
        if (err != EI_IMPULSE_OK) {
            ei_printf("ERR: Failed to run classifier (%d)\n", err);
            return;
        }

    // print the predictions
    ei_printf("Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms.): \n",
                result.timing.dsp, result.timing.classification, result.timing.anomaly);

    client.loop();
    #if EI_CLASSIFIER_OBJECT_DETECTION == 1
        bool bb_found = result.bounding_boxes[0].value > 0;
        for (size_t ix = 0; ix < result.bounding_boxes_count; ix++) {
            auto bb = result.bounding_boxes[ix];
            if (bb.value == 0) {
                continue;
            }
            digitalWrite(SIGN, HIGH);
            ei_printf("    %s (%f) [ x: %u, y: %u, width: %u, height: %u ]\n", bb.label, bb.value, bb.x, bb.y, bb.width, bb.height);
            client.loop();
            delay(1000);
            client.loop();
            digitalWrite(SIGN, LOW);
            if (ind < num_chars) {
                tokens[ind] = bb.label[0];
                ind++;
                ei_printf("%s added to symbol array, currently %i elements in the array", bb.label, ind);
                break;
            }
            
        }
        if (!bb_found) {
            ei_printf("    No objects found\n");
        }
    #else
        for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
            ei_printf("    %s: %.5f\n", result.classification[ix].label,
                                        result.classification[ix].value);
        }
    #endif

    #if EI_CLASSIFIER_HAS_ANOMALY == 1
            ei_printf("    anomaly score: %.3f\n", result.anomaly);
    #endif
        client.loop();
        if (ind == num_chars) {
            ind = 0;
            // display.clearDisplay();
            // display.setCursor(0,0);
            // display.setTextSize(1);
            // display.print(tokens);
            Serial.printf("Sent %s\n", tokens);
            client.sendTXT(tokens, num_chars);
        }
    
}

#if !defined(EI_CLASSIFIER_SENSOR) || EI_CLASSIFIER_SENSOR != EI_CLASSIFIER_SENSOR_CAMERA
#error "Invalid model for current sensor"
#endif



// #include <Arduino.h>
// /**************************************************************************
//  This is an example for our Monochrome OLEDs based on SSD1306 drivers

//  Pick one up today in the adafruit shop!
//  ------> http://www.adafruit.com/category/63_98

//  This example is for a 128x32 pixel display using I2C to communicate
//  3 pins are required to interface (two I2C and one reset).

//  Adafruit invests time and resources providing this open
//  source code, please support Adafruit and open-source
//  hardware by purchasing products from Adafruit!

//  Written by Limor Fried/Ladyada for Adafruit Industries,
//  with contributions from the open source community.
//  BSD license, check license.txt for more information
//  All text above, and the splash screen below must be
//  included in any redistribution.
//  **************************************************************************/

// #include <Wire.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>

// #define SCREEN_WIDTH 128 // OLED display width, in pixels
// #define SCREEN_HEIGHT 32 // OLED display height, in pixels

// // Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// // The pins for I2C are defined by the Wire-library. 
// // On an arduino UNO:       A4(SDA), A5(SCL)
// // On an arduino MEGA 2560: 20(SDA), 21(SCL)
// // On an arduino LEONARDO:   2(SDA),  3(SCL), ...
// #define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
// #define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// char message[]="297M LstWyPt, 345M StPt, rec#89";
// int x, minX;

// void setup() {
//   Serial.begin(115200);
//   Wire.setPins(41,42);

//   // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
//   if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
//     Serial.println(F("SSD1306 allocation failed"));
//     for(;;); // Don't proceed, loop forever
//   }

//   // Show initial display buffer contents on the screen --
//   // the library initializes this with an Adafruit splash screen.
//   display.setTextColor(WHITE);
//   display.setTextWrap(false);
//   x = display.width();
//   minX = -6 * strlen(message) * 2;  // 12 = 6 pixels/character * text size * additional
// }

// void loop() {
//     display.clearDisplay();
//     display.setCursor(0,0);
//     display.setTextSize(1);
//     display.print("Sat:13  03:56:32  67%");// GPS # Satellites, Time, % Batt chg
//     display.setTextSize(1);
//     display.setCursor(x,10);
//     display.print(message);
//     display.setCursor(x,20);
//     display.setTextSize(1);
//     display.print("Press #1 New StPt, 2 RecWayPt, Cur 32.567, -102.456");
//     display.display();
//     x=x-1; // scroll speed, make more positive to slow down the scroll
//     if(x < minX) x= display.width();
// }
