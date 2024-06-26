from api_utils import *
import tornado.ioloop
import tornado.web
import tornado.websocket
import wave
import struct
import time
import os
import glob
import assemblyai as aai


mp3_ready = False

# Handler to send MP3 file to ESP32 with Speaker
class GetMP3Server(tornado.web.RequestHandler):
    def get(self):
        print("Sending MP3 file...")
        filepath = "elevenlabs_test.mp3"  # Specify the path to your MP3 file
        self.set_header('Content-Type', 'audio/mpeg')
        with open(filepath, "rb") as f:
            self.write(f.read())
        self.finish()

# Handler to check if MP3 is Ready
class CheckMP3Ready(tornado.web.RequestHandler):
    def get(self):
        global mp3_ready
        if mp3_ready:
            self.write("mp3 ready")
            mp3_ready = False
        else:
            self.write("mp3 not ready")

# Handler to convert text to speech
class Text2SpeechHandler(tornado.websocket.WebSocketHandler):
    clients = set()

    @classmethod
    def send_to_all(cls, message):
        for client in cls.clients:
            client.write_message(message)
    
    def check_origin(self, origin):
        return True

    def open(self):
        self.clients.add(self)
        print("ESP32 WebSocket opened")
        self.api_caller = Caller()

    def on_message(self, message):
        print("Received message: {}".format(message))
        self.api_caller.query(message)
        self.update_mp3_ready()

    def update_mp3_ready(self):
        global mp3_ready
        mp3_ready = True  # Set mp3_ready to True once processing is done

    def on_close(self):
        print("ESP32 WebSocket closed")
        self.clients.remove(self)

# Handler to convert Audio Data to Text 
class AudioWebSocketHandler(tornado.websocket.WebSocketHandler):
    latest_transcript = "No transcript available yet."
    audio_sample_received = False

    def open(self):
        self.start_time = int(time.time())
        print(f"WebSocket opened at {self.start_time}")
        self.audio_samples = []

    def on_message(self, message):
        # Place audio samples into buffer for processing
        samples = struct.unpack('<' + 'i' * (len(message) // 4), message)
        self.audio_samples.extend(samples)
        self.audio_sample_received = True

    def on_close(self):
        print("WebSocket closed")
        if self.audio_sample_received:
            filename = f"output_{self.start_time}.wav"
            # Convert audio samples to WAV file
            self.save_to_wav(self.audio_samples, filename)
            print(f"Saved WAV file as {filename}")
            # Convert WAV file to text
            transcript = self.convert_audio_to_text()  # Call to process the latest file
            print(f"Transcript: {transcript}")
            # Update the latest transcript
            AudioWebSocketHandler.latest_transcript = transcript
            # Send the latest transcript to all clients of Text2SpeechHandler
            Text2SpeechHandler.send_to_all("Message: " + transcript)
            self.audio_sample_received = False

    # Save audio samples to WAV file
    def save_to_wav(self, audio_samples, filename):
        with wave.open(filename, 'w') as wav_file:
            wav_file.setnchannels(1)
            wav_file.setsampwidth(4)  # Adjust as per the actual bit depth
            wav_file.setframerate(16000)  # Ensure this matches the actual sample rate
            for sample in audio_samples:
                wav_file.writeframes(struct.pack('<i', sample))

    # Get latest timestamp to name WAV file
    def find_latest_wav(self):
        list_of_files = glob.glob('./*.wav')
        if not list_of_files:
            return None
        return max(list_of_files, key=os.path.getmtime)

    # Convert WAV file to text
    def convert_audio_to_text(self):
        aai.settings.api_key = "76330e0ff50e43da93f4f15b5dc57fbf" 
        # Initialize transcription service
        transcriber = aai.Transcriber()
        # Get most recent .wav file
        audio_url = self.find_latest_wav()
        # If no audio files are found, return
        if audio_url is None:
            print("No audio files found.")
            return
        # Transcribe the audio
        transcript = transcriber.transcribe(audio_url)
        if transcript.error:
            return transcript.error
        # Print the transcript
        return transcript.text

    def check_origin(self, origin):
        return True  # Allow all origins for WebSocket connections

# Handler for make app
def make_app():
    return tornado.web.Application([
        (r"/mp3", GetMP3Server),
        (r"/checkmp3", CheckMP3Ready),
        (r"/text2speech", Text2SpeechHandler),
        (r"/audio", AudioWebSocketHandler),
    ])

if __name__ == "__main__":
    app = make_app()
    app.listen(8888)
    print("Server is running on http://localhost:8888/mp3")
    tornado.ioloop.IOLoop.current().start()
