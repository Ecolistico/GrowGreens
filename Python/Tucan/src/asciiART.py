#!/usr/bin/env python3

# Import Directories
from PIL import Image

class asciiArt:
    def __init__(self):
        self.ASCII_CHARS = [ '#', '?', '%', '.', 'S', '+', '.', '*', ':', ',', '@']

    def scale_image(self, image, new_width=75):
        """Resizes an image preserving the aspect ratio.
        """
        (original_width, original_height) = image.size
        aspect_ratio = original_height/float(original_width)
        new_height = int(aspect_ratio * new_width)

        new_image = image.resize((new_width, new_height))
        return new_image

    def convert_to_grayscale(self, image):
        return image.convert('L')

    def map_pixels_to_ascii_chars(self, image, range_width=25):
        """Maps each pixel to an ascii char based on the range
        in which it lies.

        0-255 is divided into 11 ranges of 25 pixels each.
        """

        pixels_in_image = list(image.getdata())
        pixels_to_chars = [self.ASCII_CHARS[int(pixel_value/range_width)] for pixel_value in
                pixels_in_image]

        return "".join(pixels_to_chars)

    def convert_image_to_ascii(self, image, new_width=75):
        image = self.scale_image(image)
        image = self.convert_to_grayscale(image)

        pixels_to_chars = self.map_pixels_to_ascii_chars(image)
        len_pixels_to_chars = len(pixels_to_chars)

        image_ascii = [pixels_to_chars[index: index + new_width] for index in
                range(0, len_pixels_to_chars, new_width)]

        return "\n".join(image_ascii)

    def handle_image_conversion(self, image_filepath):
        image = None
        try:
            image = Image.open(image_filepath)
        except Exception as e:
            print ("Unable to open image file {image_filepath}.".format(image_filepath=image_filepath))
            print (e)
            return

        image_ascii = self.convert_image_to_ascii(image)
        return image_ascii
    
    def img_print(self, image_filepath):
        img = self.handle_image_conversion(image_filepath)
        print (img)