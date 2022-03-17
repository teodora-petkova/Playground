#import sys, getopt
import os
from PIL import Image
from pdf2image import convert_from_path

def get_image_paths(dirpath):
    image_paths = []
    
    for filename in os.listdir(dirpath):
         if filename.endswith(".jpg"):
            filepath = os.path.join(dirpath, filename)
            if os.path.isfile(filepath):
               image_paths.append(filepath)
    return image_paths

def images_to_pdf(input_folder, pdf_filepath):
   image_paths = get_image_paths(input_folder)

   image_list = []
   for image_path in image_paths:
      image = Image.open(image_path) # .resize((2970, 2100), Image.ANTIALIAS)
      image_list.append(image)

   image_list[0].save(pdf_filepath, "PDF", resolution=100.0,
      save_all=True, append_images=image_list[1:])

""" # TODO: to investigate about poppler
def pdf_to_images(pdffile, output_folder):

   images = convert_from_path(pdffile)
   for i in range(len(images)):

        # Save pages as images in the pdf
        images[i].save('page' + str(i) + '.jpg', 'JPEG')
"""

def main():

   output_folder = '.\\output'
   if not os.path.exists(output_folder):
       os.mkdir(output_folder)
       
   images_to_pdf('.\\input_imgs', os.path.join(output_folder, 'out.pdf'))
   #pdf_to_images('.\\output\\out.pdf', output_folder)

if __name__ == '__main__':
   main()