// #include <Arduino.h>
// #include <sign-language_inferencing.h>
// #include "edge-impulse-sdk/dsp/image/image.hpp"

#define CAMERA_ESP32_S3


#if defined(CAMERA_ESP32_CAM)
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SDA_GPIO_NUM     26
#define SCK_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

#elif defined(CAMERA_ESP32_S3)
#define PWDN_GPIO_NUM     10
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      3
#define SDA_GPIO_NUM      46
#define SCK_GPIO_NUM       9

#define Y9_GPIO_NUM       18
#define Y8_GPIO_NUM       17
#define Y7_GPIO_NUM       16
#define Y6_GPIO_NUM       15
#define Y5_GPIO_NUM        7
#define Y4_GPIO_NUM        6
#define Y3_GPIO_NUM        5
#define Y2_GPIO_NUM        4
#define VSYNC_GPIO_NUM     8
#define HREF_GPIO_NUM     19
#define PCLK_GPIO_NUM     20

#else
#error "Camera model not selected"
#endif


/* Constant defines -------------------------------------------------------- */
#define EI_CAMERA_RAW_FRAME_BUFFER_COLS           240
#define EI_CAMERA_RAW_FRAME_BUFFER_ROWS           240
#define EI_CAMERA_FRAME_BYTE_SIZE                 3
#define EI_CAMERA_IMAGE_SIZE               172800



bool ei_camera_init(void);
// void ei_camera_deinit(void);
// bool ei_camera_capture(uint32_t img_width, uint32_t img_height, uint8_t *out_buf);
// static int ei_camera_get_data(size_t offset, size_t length, float *out_ptr);