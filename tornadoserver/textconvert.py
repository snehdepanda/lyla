import assemblyai as aai
import os
import glob

def find_latest_wav():
    import pdb; pdb.set_trace()
    list_of_files = glob.glob('./*.wav')
    if not list_of_files:
        return None
    return max(list_of_files, key=os.path.getmtime)

def convert_audio_to_text():
    aai.settings.api_key = "76330e0ff50e43da93f4f15b5dc57fbf" 
    # Initialize transcription service
    transcriber = aai.Transcriber()
    # Get most recent .wav file
    audio_url = find_latest_wav()
    # If no audio files are found, return
    if audio_url is None:
        print("No audio files found.")
        return
    # Transcribe the audio
    transcript = transcriber.transcribe(audio_url)
    if transcript.error:
        print(transcript.error)
    # Print the transcript
    print(transcript.text)

convert_audio_to_text()