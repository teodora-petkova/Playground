import os
from PIL import Image
from PIL import ImageCms

def convert_from_RGB_to_CMYK(input_filepath, output_filepath):
    image = Image.open(input_filepath)

    if image.mode == 'RGB':
        img = ImageCms.profileToProfile(
            image,  
            '.\\icc_profiles\\sRGB Color Space Profile.icm',
            '.\\icc_profiles\\USWebCoatedSWOP.icc',
            renderingIntent=0, outputMode='CMYK')

        img.save(output_filepath, quality=100)

def main():

    input_filepath = '.\\input\\in.jpg'
    output_folder = '.\\output'

    if not os.path.exists(output_folder):
        os.mkdir(output_folder)
        
    if not os.path.exists(input_filepath):
        print("No input file.")
    else:
        output_filepath = os.path.join(output_folder, os.path.splitext(os.path.basename(input_filepath))[0] + ".tiff")
        convert_from_RGB_to_CMYK(input_filepath, output_filepath)
        

if __name__ == '__main__':
    main()