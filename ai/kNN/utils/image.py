import numpy as np
import cv2
import os

import matplotlib.pyplot as plt
plt.close("all")

def grayscale_image(image):
    gray_image = cv2.cvtColor(image, cv2.COLOR_RGB2GRAY)
    return gray_image

def hsv_image(image):
    hsv_image = cv2.cvtColor(image, cv2.COLOR_RGB2HSV)
    return hsv_image

def normalize(image):
    image = image
    min_intensity = np.min(image)
    max_intensity = np.max(image)
    if(max_intensity - min_intensity != 0):
        image = (image - min_intensity) / (max_intensity - min_intensity)
    
    return image

def get_canny_edges(image):
    smoothen_image = cv2.GaussianBlur(image, (5, 5), 0)
    otsu_threshold, otsu_image = cv2.threshold(smoothen_image, 0, 255,
                                               cv2.THRESH_BINARY_INV + cv2.THRESH_OTSU)
    structuring_element = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (3, 3))
    image_closed_otsu = cv2.morphologyEx(otsu_image, cv2.MORPH_CLOSE, 
                                         structuring_element, iterations = 3)
    canny_image = cv2.Canny(image_closed_otsu, otsu_threshold, 0.5*otsu_threshold)
    return canny_image

def resize(image, size=256):
    image = cv2.resize(image, (size, size), interpolation = cv2.INTER_NEAREST)
    return image

def show_image(image, title, colour_map = "gray"):
    plt.imshow(image, cmap = colour_map)
    plt.title(title)
    plt.axis("off")
    plt.show()