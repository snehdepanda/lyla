from openai import OpenAI
import os
import requests  # Used for making HTTP requests
import json  # Used for working with JSON data

class Caller:
    def __init__(self):
        # CHAT_API_KEY = 'sk-proj-rUsEo9MJd2sZg0Hy5bGGT3BlbkFJizaob0ZJyPssZG8iFWeI' # IT IS SECRET!
        self.client = OpenAI(
            api_key='sk-proj-NPpCXNE8yseRY0g2O1XfT3BlbkFJPVA29cGH4x4lzVAJnxXq'
            )   # default calls os.environ.get("OPENAI_API_KEY")

        self.chunk_size = 1024
        self.xi_key = "a38175cb391b4c1e2a2ba89b12f9dfb9"
        self.xi_voice = "jGf6Nvwr7qkFPrcLThmD"
        self.xi_url = f"https://api.elevenlabs.io/v1/text-to-speech/{self.xi_voice}/stream" # URL for XI request
        self.output_path = "elevenlabs_test.mp3"

        
    
    def query(self, message):
        completion = self.client.chat.completions.create(
            model="gpt-3.5-turbo",
            messages=[
                # {"role": "system", "content": "Prompt: Interpret the stream of letters and numbers as a grammatically correct sentence. Take into account possible typos. If the input is just one number, treat each digit as an individual number. Respond only with the sentence."},
                {"role": "system", "content": "Prompt: Take the input number and return each digit as an individual number from 0-9. For example, '1234' should be '1 2 3 4'."},
                {"role": "user", "content": message}
            ]
            )
        chat_response = completion.choices[0].message.content

        headers = {
            "Accept": "application/json",
            "xi-api-key": self.xi_key
        }

        data = {
            "text": chat_response,
            "model_id": "eleven_multilingual_v2",
            "voice_settings": {
                "stability": 0.5,
                "similarity_boost": 0.8,
                "style": 0.0,
                "use_speaker_boost": True
            }
        }

        # Make the POST request to the TTS API with headers and data, enabling streaming response
        response = requests.post(self.xi_url, headers=headers, json=data, stream=True)

        # Check if the request was successful
        if response.ok:
            # Open the output file in write-binary mode
            with open(self.output_path, "wb") as f:
                # Read the response in chunks and write to the file
                count = 0
                for chunk in response.iter_content(chunk_size=self.chunk_size):
                    f.write(chunk)
                    print(len(chunk))
                    count += 1
            # Inform the user of success
            print("Audio stream saved successfully.")
            print(count)
        else:
            # Print the error message if the request was not successful
            print(response.text)



# # Use if need to convert audio file to other formats
# from pydub import AudioSegment
# sound = AudioSegment.from_mp3("ElevenLabs_2024-04-29T06_56_41_Rachel_pre_s50_sb75_se0_b_m2.mp3")
# sound.export("rachel_hello_world.wav", format= "wav")