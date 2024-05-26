#include <Arduino.h>

#define I2S_MIC_SERIAL_CLOCK GPIO_NUM_32
#define I2S_MIC_WORD_SELECT GPIO_NUM_33
#define I2S_MIC_SERIAL_DATA GPIO_NUM_34

#define LED_PIN GPIO_NUM_19
const int16_t buf_len = 1024;

void setupI2Smic();

size_t mic_i2s_to_buffer(int32_t *buffer, int16_t buffer_size);