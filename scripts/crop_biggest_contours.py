import os
import cv2

def get_cropped_biggest_contour(image_path):

    image = cv2.imread(image_path)
    grayscale_image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

    smoothen_image = cv2.GaussianBlur(grayscale_image, (3, 3), 0)
    otsu_threshold, otsu_image = cv2.threshold(smoothen_image, 0, 255, cv2.THRESH_BINARY_INV + cv2.THRESH_OTSU)

    contours, hierarchy = cv2.findContours(otsu_image, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

    cropped_area = None
    if len(contours) != 0:
        
        biggest_contour = max(contours, key = cv2.contourArea)
        x, y, w, h = cv2.boundingRect(biggest_contour) 

        cropped_area = image[y:y+h, x:x+w]

    return cropped_area

def get_image_paths(dirpath):
    image_paths = []
    
    for filename in os.listdir(dirpath):
        filepath = os.path.join(dirpath, filename)
        if os.path.isfile(filepath):
            image_paths.append(filepath)
    return image_paths

def main():

    input_folder = 'input'
    output_folder = 'output'
    
    if not os.path.exists(input_folder):
        print("No input folder with images.")

    else:
        image_paths = get_image_paths(input_folder)

        if not os.path.exists(output_folder):
            os.mkdir(output_folder)

        for image_path in image_paths:
            cropped_area = get_cropped_biggest_contour(image_path)
            if cropped_area is not None:
                output_image_filepath = os.path.join(".", output_folder, os.path.basename(image_path))
                cv2.imwrite(output_image_filepath, cropped_area)

if __name__ == '__main__':
    main()