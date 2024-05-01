#include "mic.h"
#include "driver/i2s.h"

void setupI2Smic() {
    esp_err_t i2s_error;

    i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 44100,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,  // mono channel input (left channel) - bug from library
    .communication_format = (i2s_comm_format_t)I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = 0,
    .dma_buf_count = 2,
    .dma_buf_len = buf_len,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0};

    i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_MIC_SERIAL_CLOCK,
    .ws_io_num = I2S_MIC_WORD_SELECT,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_MIC_SERIAL_DATA
    };

    i2s_error = i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);

    if (i2s_error) {
        log_e("Failed to start i2s driver. ESP error: %s (%x)", esp_err_to_name(i2s_error), i2s_error);
        return;
    }

    i2s_error = i2s_set_pin(I2S_NUM_0, &pin_config);

    if (i2s_error) {
        log_e("Failed to set i2s pins. ESP error: %s (%x)", esp_err_to_name(i2s_error), i2s_error);
        return;
    }
}


size_t mic_i2s_to_buffer(int32_t *buffer, int16_t buffer_size) {
  size_t bytes_read;
  esp_err_t read_status = i2s_read(I2S_NUM_0, buffer, buffer_size * sizeof(int32_t), &bytes_read, portMAX_DELAY);
  if (read_status != ESP_OK) {
    Serial.println("Error reading from I2S...");
    return 0; // Handle read error
  }
  // Serial.println("Read from I2S...");
  // int samples_read = bytes_read / sizeof(int16_t);
  // //dump the samples out to the serial channel.
  // for (int i = 0; i < samples_read; i++)
  // {
  //   if (buffer[i] > 0) {
  //     Serial.printf("%ld\n", buffer[i]);
  //   }
  // }

  return bytes_read;
}