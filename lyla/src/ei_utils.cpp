// #include "ei_utils.h"

// #include <sign-language_inferencing.h>
// #include "edge-impulse-sdk/dsp/image/image.hpp"
// #include "esp_camera.h"

// // bool ei_camera_init(void) {

// //     if (is_initialised) return true;

// // #if defined(CAMERA_MODEL_ESP_EYE)
// //   pinMode(13, INPUT_PULLUP);
// //   pinMode(14, INPUT_PULLUP);
// // #endif

// //     //initialize the camera
// //     esp_err_t err = esp_camera_init(&camera_config);
// //     if (err != ESP_OK) {
// //       Serial.printf("Camera init failed with error 0x%x\n", err);
// //       return false;
// //     }

// //     sensor_t * s = esp_camera_sensor_get();
// //     // initial sensors are flipped vertically and colors are a bit saturated
// //     if (s->id.PID == OV3660_PID) {
// //       s->set_vflip(s, 1); // flip it back
// //       s->set_brightness(s, 1); // up the brightness just a bit
// //       s->set_saturation(s, 0); // lower the saturation
// //     }

// // #if defined(CAMERA_MODEL_M5STACK_WIDE)
// //     s->set_vflip(s, 1);
// //     s->set_hmirror(s, 1);
// // #elif defined(CAMERA_MODEL_ESP_EYE)
// //     s->set_vflip(s, 1);
// //     s->set_hmirror(s, 1);
// //     s->set_awb_gain(s, 1);
// // #endif

// //     is_initialised = true;
// //     return true;
// // }

// void ei_camera_deinit(bool &is_initialised) {

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


// bool ei_camera_capture(uint32_t img_width, uint32_t img_height, uint8_t *out_buf, bool &is_initialised) {
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

