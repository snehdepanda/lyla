#include <Arduino.h>
#include "AudioFileSourceICYStream.h"
#include "AudioFileSourceBuffer.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"

void MDCallback(void *cbData, const char *type, bool isUnicode, const char *string);

void StatusCallback(void *cbData, int code, const char *string);

void setupSpeaker(const char *URL, AudioFileSourceICYStream *file, AudioFileSourceBuffer *buff, AudioOutputI2S *out, AudioGeneratorMP3 *mp3) 