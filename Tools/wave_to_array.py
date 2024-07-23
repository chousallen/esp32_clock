# bin_to_array.py
import sys
import os

def bin_to_array(input_file, output_file):
    array_name = os.path.splitext(os.path.basename(output_file))[0]

    with open(input_file, 'rb') as f:
        data = f.read()

    with open(output_file, 'w') as f:
        f.write('#ifndef AUDIO_DATA_H\n')
        f.write('#define AUDIO_DATA_H\n\n')
        f.write(f'const unsigned char {array_name}[] = {{\n')

        # Write data in hexadecimal format
        for i in range(0, len(data), 16):
            chunk = data[i:i+16]
            if i==0:
                print(type(chunk))
                print('\n')
                print(chunk)
                print('\n')
            f.write('    ' + ', '.join(f'0x{byte:02X}' for byte in chunk) + ',\n')

        f.write('};\n\n')
        f.write('#endif // AUDIO_DATA_H\n')

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print(f'Usage: {sys.argv[0]} <input_bin_file> <output_header_file>')
        sys.exit(1)
    
    bin_to_array(sys.argv[1], sys.argv[2])
