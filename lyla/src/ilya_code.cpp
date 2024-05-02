// // #include <ilya-project-1_inferencing.h> // Change this to your library
// #include <sign-language_inferencing.h>
// #include <driver/i2s.h>
// #include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// // Connections to INMP441 I2S microphone
// #define I2S_WS 32
// #define I2S_SD 35
// #define I2S_SCK 33

// // connections to LED matrix
// #define R1_PIN 25
// #define G1_PIN 26
// #define B1_PIN 27
// #define R2_PIN 14
// #define G2_PIN 21
// #define B2_PIN 13
// #define A_PIN 23
// #define B_PIN 19
// #define C_PIN 5
// #define D_PIN 17
// #define E_PIN 18 // required for 1/32 scan panels, like 64x64. Any available pin would do, i.e. IO32
// #define LAT_PIN 4
// #define OE_PIN 15
// #define CLK_PIN 16

// #define PANEL_RES_X 64      // Number of pixels wide of each INDIVIDUAL panel module. 
// #define PANEL_RES_Y 32     // Number of pixels tall of each INDIVIDUAL panel module.
// #define PANEL_CHAIN 1      // Total number of panels chained one to another

// const bool debug = false;

// MatrixPanel_I2S_DMA *display = nullptr;

// const i2s_port_t i2s_port = I2S_NUM_1;
// const uint16_t sample_rate = 16000; // Unit: Hz
// const uint16_t buf_len = 1000;

// const i2s_bits_per_sample_t i2s_bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
// const uint8_t i2s_bytes_per_sample = i2s_bits_per_sample / 8;
// const uint16_t i2s_read_size_bytes = buf_len * i2s_bytes_per_sample;
// const uint16_t i2s_buffer_size_samples = buf_len;
// const uint16_t i2s_buffer_size_bytes = i2s_buffer_size_samples * i2s_bytes_per_sample;
// const uint16_t i2s_buffer_count = 16;
// const int i2s_queue_len = 16;

// int16_t mic_read_buffer[buf_len] = {0};
// int16_t mic_read_buffer_full[sample_rate] = {0};
// float features[sample_rate];
// QueueHandle_t i2s_queue = nullptr;

// QueueHandle_t q_led = xQueueCreate(10, sizeof(bool));

// const float classification_thresh = 0.8;

// void control_led_matrix_task(void * params) {
//     bool keyword_received = false;
//     uint32_t counter = 1;
//     while (1) {
//         // wait for queue
//         xQueueReceive(q_led, &keyword_received, portMAX_DELAY);
//         display->setTextSize(1);
//         display->setCursor(0,0);
//         display->setTextColor(display->color565(0,255,0));
//         display->printf("HI %u", counter++);
//         delay(2000);
//         display->clearScreen();
//     }
// }

// bool setupI2Smic()
// {
//     esp_err_t i2s_error;

//     // i2s configuration for sampling 16 bit mono audio data
//     //
//     // Notes related to i2s.c:
//     // - 'dma_buf_len', i.e. the number of samples in each DMA buffer, is limited to 1024
//     // - 'dma_buf_len' * 'bytes_per_sample' is limted to 4092
//     // - 'I2S_CHANNEL_FMT_ONLY_RIGHT' means "mono", i.e. only one channel to be received via i2s (must use RIGHT!!!)

//     i2s_config_t i2s_config = {
//         .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
//         .sample_rate = sample_rate,
//         .bits_per_sample = i2s_bits_per_sample,
//         .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
//         .communication_format = I2S_COMM_FORMAT_STAND_I2S,
//         .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
//         .dma_buf_count = i2s_buffer_count,
//         .dma_buf_len = i2s_buffer_size_samples,
//         .use_apll = false
//     };

//     i2s_error = i2s_driver_install(i2s_port, &i2s_config, i2s_queue_len, &i2s_queue);

//     if (i2s_error)
//     {
//         log_e("Failed to start i2s driver. ESP error: %s (%x)", esp_err_to_name(i2s_error), i2s_error);
//         return false;
//     }

//     if (i2s_queue == nullptr)
//     {
//         log_e("Failed to setup i2s event queue.");
//         return false;
//     }

//     i2s_pin_config_t i2sPinConfig = {
//         .bck_io_num = I2S_SCK,
//         .ws_io_num = I2S_WS,
//         .data_out_num = I2S_PIN_NO_CHANGE,
//         .data_in_num = I2S_SD
//     };

//     i2s_error = i2s_set_pin(i2s_port, &i2sPinConfig);

//     if (i2s_error)
//     {
//         log_e("Failed to set i2s pins. ESP error: %s (%x)", esp_err_to_name(i2s_error), i2s_error);
//         return false;
//     }

//     return true;
// }

// int raw_feature_get_data(size_t offset, size_t length, float *out_ptr) {
//     memcpy(out_ptr, features + offset, length * sizeof(float));
//     return 0;
// }

// void setup()
// {
//     // put your setup code here, to run once:
//     Serial.begin(115200);
//     setupI2Smic();

//     HUB75_I2S_CFG::i2s_pins _pins={R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN, A_PIN, B_PIN, C_PIN, D_PIN, E_PIN, LAT_PIN, OE_PIN, CLK_PIN};
//     HUB75_I2S_CFG mxconfig(
//         PANEL_RES_X, // Module width
//         PANEL_RES_Y, // Module height
//         PANEL_CHAIN, // chain length
//         _pins // pin mapping
//     );
//     mxconfig.clkphase = false;

//     display = new MatrixPanel_I2S_DMA(mxconfig);
//     display->begin();
//     display->setBrightness8(60); //0-255
//     display->clearScreen();

//     xTaskCreate(
//         control_led_matrix_task,
//         "control_led_matrix",
//         8192,
//         NULL,
//         1,
//         NULL
//     );

//     log_d("Setup successfully completed.");
// }

// void loop()
// {
//     uint8_t buffer_counter = 0;
//     uint8_t process_delay_counter = 0;
//     uint8_t process_every_n_buffers = 7;
//     esp_err_t i2s_error = ESP_OK;
//     size_t i2s_bytes_read = 0;

//     unsigned long tic = micros();
//     while (process_delay_counter < process_every_n_buffers) {
//         i2s_error = i2s_read(i2s_port, mic_read_buffer, i2s_read_size_bytes, &i2s_bytes_read, portMAX_DELAY);

//         // Check i2s error state after reading
//         if (i2s_error)
//         {
//             log_e("i2s_read failure. ESP error: %s (%x)", esp_err_to_name(i2s_error), i2s_error);
//         }

//         // Check whether right number of bytes has been read
//         if (i2s_bytes_read != i2s_read_size_bytes)
//         {
//             log_w("i2s_read unexpected number of bytes: %d", i2s_bytes_read);
//         }

//         // Append new data to buffer (don't circle, as that will be problematic if a signal starts at the end of the buffer)
//         memmove(mic_read_buffer_full, mic_read_buffer_full+buf_len, (sample_rate-buf_len)*sizeof(*mic_read_buffer_full));
//         memcpy(mic_read_buffer_full+sample_rate-buf_len, mic_read_buffer, buf_len*sizeof(*mic_read_buffer_full));

//         process_delay_counter++;
//     }
//     process_delay_counter = 0;

//     unsigned long toc = micros();
//     unsigned long time_diff = toc - tic;
//     // ei_printf("Time to gather data for processing: %ld ms\r\n", time_diff/1000);

//     for (uint16_t ii=0;ii<sample_rate;ii++) {
//         features[ii] = (float)mic_read_buffer_full[ii];
//     }

//     if (sizeof(features) / sizeof(float) != EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
//         ei_printf("The size of your 'features' array is not correct. Expected %lu items, but had %lu\n",
//             EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, sizeof(features) / sizeof(float));
//         delay(1000);
//         return;
//     }

//     ei_impulse_result_t result = { 0 };

//     // the features are stored into flash, and we don't want to load everything into RAM
//     signal_t features_signal;
//     features_signal.total_length = sizeof(features) / sizeof(features[0]);
//     features_signal.get_data = &raw_feature_get_data;

//     // invoke the impulse
//     EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false /* debug */);
//     if (res != EI_IMPULSE_OK) {
//         ei_printf("run_classifier returned: %d\n", res);
//         delay(1000);
//     }

//     if (debug) {
//         // print the predictions
//         ei_printf("Predictions ");
//         ei_printf("(DSP: %d ms., Classification: %d ms., Anomaly: %d ms.)",
//             result.timing.dsp, result.timing.classification, result.timing.anomaly);
//         ei_printf(": \n");
//         ei_printf("[");
//         for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
//             ei_printf("%.5f", result.classification[ix].value);
// #if EI_CLASSIFIER_HAS_ANOMALY == 1
//             ei_printf(", ");
// #else
//             if (ix != EI_CLASSIFIER_LABEL_COUNT - 1) {
//                 ei_printf(", ");
//             }
// #endif
//         }
// #if EI_CLASSIFIER_HAS_ANOMALY == 1
//         ei_printf("%.3f", result.anomaly);
// #endif
//         ei_printf("]\n");

//         // human-readable predictions
//         for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
//             ei_printf("    %s: %.5f\n", result.classification[ix].label, result.classification[ix].value);
//         }
// #if EI_CLASSIFIER_HAS_ANOMALY == 1
//         ei_printf("    anomaly score: %.3f\n", result.anomaly);
// #endif
//     }

//     if (result.classification[0].value > classification_thresh) {
//         log_i("Keyword detected! (%.3f)\r\n", result.classification[0].value);
//         bool command_received = true;
//         xQueueSend(q_led, (void *) &command_received, (TickType_t) 0);
//     }
// }