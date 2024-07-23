import numpy as np
import wave

def wav_to_numpy_array(filename):
    # Open the WAV file
    with wave.open(filename, 'rb') as wav_file:
        # Extract audio parameters
        sample_width = wav_file.getsampwidth()
        n_channels = wav_file.getnchannels()
        n_frames = wav_file.getnframes()
        sample_rate = wav_file.getframerate()
        
        # Read the audio frames
        audio_data = wav_file.readframes(n_frames)
    
    # Convert the audio data to a numpy array
    dtype = np.uint8 if sample_width == 1 else np.int16
    audio_array = np.frombuffer(audio_data, dtype=dtype)
    
    # If stereo, split the channels
    if n_channels == 2:
        audio_array = audio_array.reshape(-1, 2)  # Reshape for stereo audio
    
    return audio_array

# Example usage
filename = '../Resources/processed/DARK_ARIA.wav'
audio_array = wav_to_numpy_array(filename)

# Print the numpy array or process it further
print(filename)
print("type: " + str(type(audio_array[0])))
print("length: " + str(len(audio_array)))
print(audio_array)

cnt = 0
num = []
value = []
last = audio_array[0]
curr = audio_array[0]

for index in range(len(audio_array)):
    curr = audio_array[index]
    if curr == last:
        cnt += 1
    else:
        num.append(cnt)
        value.append(last)
        cnt = 1
    last = curr

num.append(cnt)
value.append(curr)

print("len(num): " + str(len(num)))
print("len(value): " + str(len(value)))
print("require size: " + str(len(num)/1024/1024 * 2) + "MB")
print("compress ratio: " + str(len(audio_array)/len(num)))