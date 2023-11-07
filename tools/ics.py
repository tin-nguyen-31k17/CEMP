import argparse
from PIL import Image
import io

def png_to_header(input_file, output_file, array_name):
    with open(input_file, 'rb') as f:
        img = Image.open(f)
        img.thumbnail((64, 64))  # Resize the image
        byte_arr = io.BytesIO()
        img.save(byte_arr, format='PNG')
        data = byte_arr.getvalue()

    with open(output_file, 'w') as f:
        f.write('#include <pgmspace.h>\n\n')
        f.write('PROGMEM const uint8_t {}[] = {{\n  '.format(array_name))

        line_length = 0
        for byte in data:
            f.write('0x{:02x}, '.format(byte))
            line_length += 1
            if line_length >= 12:
                f.write('\n  ')
                line_length = 0

        f.write('\n};\n')

def main():
    parser = argparse.ArgumentParser(description='Convert a PNG file to a C++ header file.')
    parser.add_argument('input_file', help='The input PNG file.')
    parser.add_argument('output_file', help='The output header file.')
    parser.add_argument('array_name', help='The name for the array in the header file.')
    args = parser.parse_args()

    png_to_header(args.input_file, args.output_file, args.array_name)

if __name__ == '__main__':
    main()
