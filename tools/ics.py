#   Project: CEMP Mobility Water Monitoring Station
#   Author: Nguyen Trong Tin
#   Email:tin.nguyen.31k17@hcmut.edu.vn
#   Description: This code is part of the CEMP Mobility Water Monitoring Station project.

import argparse
import os
from PIL import Image
import io
from cairosvg import svg2png

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

def convert_svg_to_png(input_file):
    if os.path.splitext(input_file)[1].lower() == '.svg':
        output_file = os.path.splitext(input_file)[0] + '.png'
        svg2png(url=input_file, write_to=output_file)
        return output_file
    return input_file

def main():
    parser = argparse.ArgumentParser(description='Convert a PNG file to a C++ header file.')
    parser.add_argument('input_file', help='The input PNG file.')
    parser.add_argument('output_file', help='The output header file.')
    parser.add_argument('array_name', help='The name for the array in the header file.')
    args = parser.parse_args()

    input_file = convert_svg_to_png(args.input_file)
    png_to_header(input_file, args.output_file, args.array_name)

if __name__ == '__main__':
    main()
