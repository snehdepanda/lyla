#include <Arduino.h>
// // #include <driver/i2s.h>

// // // you shouldn't need to change these settings

// // #define SAMPLE_RATE 44100
// // // most microphones will probably default to left channel but you may need to tie the L/R pin low
// // #define I2S_MIC_CHANNEL I2S_CHANNEL_FMT_ONLY_LEFT
// // // either wire your microphone to the same pins or change these to match your wiring
// // #define I2S_MIC_SERIAL_CLOCK 2
// // #define I2S_MIC_LEFT_RIGHT_CLOCK 15
// // #define I2S_MIC_SERIAL_DATA 13

// // // don't mess around with this
// // i2s_config_t i2s_config = {
// //   .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
// //   .sample_rate = SAMPLE_RATE,
// //   .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
// //   .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
// //   .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB), 
// //   .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
// //   .dma_buf_count = 4,
// //   .dma_buf_len = 1024,
// //   .use_apll = false};

// // // and don't mess around with this
// // i2s_pin_config_t i2s_mic_pins = {
// //     .bck_io_num = I2S_MIC_SERIAL_CLOCK,
// //     .ws_io_num = I2S_MIC_LEFT_RIGHT_CLOCK,
// //     .data_out_num = -1,
// //     .data_in_num = I2S_MIC_SERIAL_DATA};

// // void setup()
// // {
// //   // we need serial output for the plotter
// //   Serial.begin(115200);
// //   // start up the I2S peripheral
// //   auto res = i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
// //   if (res != ESP_OK)
// //   {
// //     while (1) {
// //       Serial.printf("Failed to install I2S driver: %d\n", res);
// //       delay(1000);
// //     }
// //   }
// //   res = i2s_set_pin(I2S_NUM_0, &i2s_mic_pins);
// //   if (res != ESP_OK)
// //   {
// //     while (1) {
// //       Serial.printf("Failed to set I2S pin: %d\n", res);
// //       delay(1000);
// //     }
// //   }
// // }

// // int32_t raw_samples[SAMPLE_BUFFER_SIZE];
// // void loop()
// // {
// //   // read from the I2S device
// //   size_t bytes_read = 0;
// //   i2s_read(I2S_NUM_0, raw_samples, 4 * SAMPLE_BUFFER_SIZE, &bytes_read, portMAX_DELAY);
// //   int samples_read = bytes_read / 4;
// //   // dump the samples out to the serial channel.
// //   for (int i = 0; i < samples_read; i++)
// //   {
// //     Serial.printf("%ld\n", raw_samples[i]);
// //   }
// // }




// // // // put function declarations here:
// // // int myFunction(int, int);

// // // void setup() {
// // //   // put your setup code here, to run once:
// // //   int result = myFunction(2, 3);
// // // }

// // // void loop() {
// // //   // put your main code here, to run repeatedly:
// // // }

// // // // put function definitions here:
// // // int myFunction(int x, int y) {
// // //   return x + y;
// // // }

// // #include <driver/i2s.h>
// // #define I2S_WS 15
// // #define I2S_SD 13
// // #define I2S_SCK 2
// // #define SAMPLE_BUFFER_SIZE 512

// // #define I2S_PORT I2S_NUM_0

// // void i2s_install(){
// //   const i2s_config_t i2s_config = {
// //     .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
// //     .sample_rate = 44100,
// //     .bits_per_sample = i2s_bits_per_sample_t(16),
// //     .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
// //     .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
// //     .intr_alloc_flags = 0, // default interrupt priority
// //     .dma_buf_count = 8,
// //     .dma_buf_len = 64,
// //     .use_apll = false
// //   };

// //   i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
// // }

// // void i2s_setpin(){
// //   const i2s_pin_config_t pin_config = {
// //     .bck_io_num = I2S_SCK,
// //     .ws_io_num = I2S_WS,
// //     .data_out_num = -1,
// //     .data_in_num = I2S_SD
// //   };

// //   i2s_set_pin(I2S_PORT, &pin_config);
// // }

// // void setup() {
// //   Serial.begin(115200);
// //   Serial.println("Setup I2S ...");

// //   delay(1000);
// //   i2s_install();
// //   i2s_setpin();
// //   delay(500);
// // }

// // int32_t raw_samples[SAMPLE_BUFFER_SIZE];

// // void loop() {
// //   // read from the I2S device
// //     // size_t bytes_read = 0;
// //     // i2s_read(I2S_NUM_0, raw_samples, 4 * SAMPLE_BUFFER_SIZE, &bytes_read, portMAX_DELAY);
// //     // int samples_read = bytes_read / 4;
// //     // // dump the samples out to the serial channel.
// //     // for (int i = 0; i < samples_read; i++)
// //     // {
// //     //   Serial.printf("%ld\n", raw_samples[i]);

// //   int32_t sample = 0;
// //   size_t bytes_read = 0;
// //   i2s_read(I2S_PORT, (int32_t*)&sample, 4, &bytes_read, portMAX_DELAY);
// //   if (sample > 0) {
// //     Serial.println(sample);
// //   }
// // }


#include "driver/i2s.h"

#define SAMPLE_BUFFER_SIZE 512
#define I2S_MIC_SERIAL_CLOCK GPIO_NUM_26
#define I2S_MIC_WORD_SELECT GPIO_NUM_25
#define I2S_MIC_SERIAL_DATA GPIO_NUM_34

#define I2S_SPEAKER_SERIAL_DATA GPIO_NUM_22

void setupI2S() {
  i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_TX),
    .sample_rate = 44100,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,  // mono channel input (left channel)
    .communication_format = (i2s_comm_format_t)I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = 0,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0};

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_MIC_SERIAL_CLOCK,
    .ws_io_num = I2S_MIC_WORD_SELECT,
    .data_out_num = I2S_SPEAKER_SERIAL_DATA,
    .data_in_num = I2S_MIC_SERIAL_DATA
  };

  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);
}

void setup() {
  Serial.begin(115200);
  setupI2S();
}


void loop() {
  static int32_t raw_samples[SAMPLE_BUFFER_SIZE];
  size_t bytes_read;

  // Read from microphone
  esp_err_t read_status = i2s_read(I2S_NUM_0, raw_samples, 2048, &bytes_read, portMAX_DELAY);
  if (read_status != ESP_OK) {
    Serial.println("Error reading from I2S...");
    return; // Handle read error
  }
  // }
  // int samples_read = bytes_read / sizeof(int32_t);
  // printf("Samples Read: %d\n", samples_read);
  // dump the samples out to the serial channel.
  // for (int i = 0; i < samples_read; i++)
  // {
  //   Serial.printf("%ld\n", raw_samples[i]);
  // }

  // Write to DAC
  esp_err_t write_status = i2s_write(I2S_NUM_0, raw_samples, 2048, NULL, portMAX_DELAY);
  printf("Write Status: %d\n", write_status);
  if (write_status != ESP_OK) {
    Serial.println("Error writing to I2S...");
  }
}

  // // Read from microphone
  // i2s_read(I2S_NUM_0, raw_samples, sizeof(raw_samples), &bytes_read, portMAX_DELAY);

  // int samples_read = bytes_read / sizeof(int32_t);
  // printf("Samples Read: %d\n", samples_read);
  // // dump the samples out to the serial channel.
  // // for (int i = 0; i < samples_read; i++)
  // // {
  // //   Serial.printf("%ld\n", raw_samples[i]);
  // // }
  // // Convert 32-bit to 16-bit
  // for (int i = 0; i < samples_read; i++) {
  //   // Assuming the meaningful data is in the lower 24 bits
  //   samples_to_dac[i] = (int16_t)(raw_samples[i] >> 8);  // Adjust shift based on where data is located
  // }

  // // Write to DAC
  // i2s_write(I2S_NUM_0, samples_to_dac, samples_read * sizeof(int16_t), NULL, portMAX_DELAY);

// int32_t raw_samples[SAMPLE_BUFFER_SIZE];
// void loop()
// {
//   // read from the I2S device
//   size_t bytes_read = 0;
//   i2s_read(I2S_NUM_0, raw_samples, 4 * SAMPLE_BUFFER_SIZE, &bytes_read, portMAX_DELAY);
//   int samples_read = bytes_read / 4;
//   // dump the samples out to the serial channel.
//   for (int i = 0; i < samples_read; i++)
//   {
//     Serial.printf("%ld\n", raw_samples[i]);
//   }
// }

// #include <driver/i2s.h>
    
//   // you shouldn't need to change these settings
//   #define SAMPLE_BUFFER_SIZE 512
//   #define SAMPLE_RATE 8000
//   // most microphones will probably default to left channel but you may need to tie the L/R pin low
//   #define I2S_MIC_CHANNEL I2S_CHANNEL_FMT_ONLY_LEFT
//   // either wire your microphone to the same pins or change these to match your wiring
//   #define I2S_MIC_SERIAL_CLOCK 26
//   #define I2S_MIC_LEFT_RIGHT_CLOCK 25
//   #define I2S_MIC_SERIAL_DATA 34
  
//   // don't mess around with this
//   i2s_config_t i2s_config = {
//       .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
//       .sample_rate = SAMPLE_RATE,
//       .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
//       .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
//       .communication_format = I2S_COMM_FORMAT_I2S,
//       .intr_alloc_flags = 0,
//       .dma_buf_count = 8,
//       .dma_buf_len = 64,
//       .use_apll = false,
//       .tx_desc_auto_clear = false,
//       .fixed_mclk = 0};
  
//   // and don't mess around with this
//   i2s_pin_config_t i2s_mic_pins = {
//       .bck_io_num = I2S_MIC_SERIAL_CLOCK,
//       .ws_io_num = I2S_MIC_LEFT_RIGHT_CLOCK,
//       .data_out_num = I2S_PIN_NO_CHANGE,
//       .data_in_num = I2S_MIC_SERIAL_DATA};
  
//   void setup()
//   {
//     // we need serial output for the plotter
//     Serial.begin(115200);
//     // start up the I2S peripheral
//     i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
//     i2s_set_pin(I2S_NUM_0, &i2s_mic_pins);
//   }
  
//   int32_t raw_samples[SAMPLE_BUFFER_SIZE];
//   void loop()
//   {
//     // read from the I2S device
//     size_t bytes_read = 0;
//     i2s_read(I2S_NUM_0, raw_samples, sizeof(int32_t) * SAMPLE_BUFFER_SIZE, &bytes_read, portMAX_DELAY);
//     int samples_read = bytes_read / sizeof(int32_t);
//     // dump the samples out to the serial channel.
//     for (int i = 0; i < samples_read; i++)
//     {
//       Serial.printf("%ld\n", raw_samples[i]);
//     }
//   }
