import tornado.ioloop
import tornado.websocket
import wave
import struct
import time
import os
import glob
import assemblyai as aai

# class MainHandler(tornado.web.RequestHandler):
#     def get(self):
#         self.render("index.html")

# class TranscriptWebSocketHandler(tornado.websocket.WebSocketHandler):
#     clients = set()  # Keep track of connected clients

#     def open(self):
#         self.clients.add(self)
#         print("WebSocket opened")

#     def on_message(self, message):
#         if message == 'refresh':
#             self.write_message(AudioWebSocketHandler.latest_transcript)

#     def on_close(self):
#         self.clients.remove(self)
#         print("WebSocket closed")

#     @classmethod
#     def send_updates(cls, transcript):
#         for client in cls.clients:
#             client.write_message(transcript)

class AudioWebSocketHandler(tornado.websocket.WebSocketHandler):
    # latest_transcript = "No transcript available yet."

    def open(self):
        self.start_time = int(time.time())
        print(f"WebSocket opened at {self.start_time}")
        self.audio_samples = []

    def on_message(self, message):
        samples = struct.unpack('<' + 'i' * (len(message) // 4), message)
        self.audio_samples.extend(samples)

    def on_close(self):
        print("WebSocket closed")
        filename = f"output_{self.start_time}.wav"
        self.save_to_wav(self.audio_samples, filename)
        print(f"Saved WAV file as {filename}")
        # transcript = self.convert_audio_to_text()  # Call to process the latest file
        # print(f"Transcript: {transcript}")
        # AudioWebSocketHandler.latest_transcript = transcript
        # TranscriptWebSocketHandler.send_updates(transcript)


    def save_to_wav(self, audio_samples, filename):
        with wave.open(filename, 'w') as wav_file:
            wav_file.setnchannels(1)
            wav_file.setsampwidth(4)  # Adjust as per the actual bit depth
            wav_file.setframerate(16000)  # Ensure this matches the actual sample rate
            for sample in audio_samples:
                wav_file.writeframes(struct.pack('<i', sample))

    # def find_latest_wav(self):
    #     list_of_files = glob.glob('./*.wav')
    #     if not list_of_files:
    #         return None
    #     return max(list_of_files, key=os.path.getmtime)

    # def convert_audio_to_text(self):
    #     aai.settings.api_key = "76330e0ff50e43da93f4f15b5dc57fbf" 
    #     # Initialize transcription service
    #     transcriber = aai.Transcriber()
    #     # Get most recent .wav file
    #     audio_url = self.find_latest_wav()
    #     # If no audio files are found, return
    #     if audio_url is None:
    #         print("No audio files found.")
    #         return
    #     # Transcribe the audio
    #     transcript = transcriber.transcribe(audio_url)
    #     if transcript.error:
    #         return transcript.error
    #     # Print the transcript
    #     return transcript.text

    def check_origin(self, origin):
        return True  # Allow all origins for WebSocket connections

def make_app():
    return tornado.web.Application([
    #     (r"/", MainHandler),
    #     (r"/transcript", TranscriptWebSocketHandler),
        (r"/audio", AudioWebSocketHandler),
    ])

if __name__ == "__main__":
    app = make_app()
    app.listen(8888)
    tornado.ioloop.IOLoop.current().start()