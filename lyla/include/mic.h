#include <Arduino.h>

#define SAMPLE_BUFFER_SIZE 512
#define I2S_MIC_SERIAL_CLOCK GPIO_NUM_26
#define I2S_MIC_WORD_SELECT GPIO_NUM_25
#define I2S_MIC_SERIAL_DATA GPIO_NUM_34

#define I2S_SPEAKER_SERIAL_DATA GPIO_NUM_22

void setupI2S();

void mic_i2s_to_buffer(int32_t *buffer, size_t buffer_size);