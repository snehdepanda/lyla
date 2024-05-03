import wave
import struct
import math

def test_save_to_wav(filename):
    sample_rate = 44100  # Sample rate in Hz
    num_channels = 1  # Mono audio
    sample_width = 2  # 16-bit samples
    num_samples = 44100  # One second of audio

    # Generating a simple tone (440 Hz sine wave for simplicity)
    frequency = 440
    volume = 32767  # Max volume for 16-bit audio
    audio_samples = [int(volume * math.sin(2 * math.pi * frequency * x / sample_rate)) for x in range(num_samples)]

    with wave.open(filename, 'w') as wav_file:
        wav_file.setnchannels(num_channels)
        wav_file.setsampwidth(sample_width)
        wav_file.setframerate(sample_rate)

        for sample in audio_samples:
            wav_file.writeframes(struct.pack('<h', sample))

    print("WAV file has been written successfully.")

# Usage
test_save_to_wav("test_output_2.wav")