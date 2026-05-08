import sys
import os

def embed_resource(input_path, output_path, symbol_name):
    with open(input_path, 'rb') as f:
        data = f.read()
    
    with open(output_path, 'w') as out:
        out.write(f'extern "C" __declspec(align(1)) unsigned char _binary_{symbol_name}_start[] = {{')
        for i, byte in enumerate(data):
            if i % 20 == 0:
                out.write('\n    ')
            out.write(f'0x{byte:02x},')
        out.write('\n};\n')
        out.write(f'extern "C" const unsigned int _binary_{symbol_name}_size = {len(data)};\n')

if __name__ == '__main__':
    if len(sys.argv) != 4:
        print(f"Usage: {sys.argv[0]} <input_file> <output_file> <symbol_name>")
        sys.exit(1)
    embed_resource(sys.argv[1], sys.argv[2], sys.argv[3])
