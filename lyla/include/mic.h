#include <Arduino.h>

#define I2S_MIC_SERIAL_CLOCK GPIO_NUM_26
#define I2S_MIC_WORD_SELECT GPIO_NUM_25
#define I2S_MIC_SERIAL_DATA GPIO_NUM_34

#define I2S_SPEAKER_SERIAL_DATA GPIO_NUM_22
const int16_t buf_len = 1024;

void setupI2Smic();

size_t mic_i2s_to_buffer(int32_t *buffer, int16_t buffer_size);