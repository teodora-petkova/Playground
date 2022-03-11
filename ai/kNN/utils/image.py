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

def gaussian_blurred_image(image):
    blurred_image = cv2.GaussianBlur(image, (5, 5), 0)
    return blurred_image

def increase_contrast(img):
    lab = cv2.cvtColor(img, cv2.COLOR_RGB2LAB)
    l, a, b = cv2.split(lab)
    
    clahe = cv2.createCLAHE(clipLimit = 2.0, tileGridSize = (8,8))
    l2 = clahe.apply(l)

    lab = cv2.merge((l2,a,b))
    img2 = cv2.cvtColor(lab, cv2.COLOR_LAB2RGB) 
    
    return img2

def get_canny_edges(image):

    smoothen_image = gaussian_blurred_image(image)

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

def minmax_normalize(A):
    min_intensity = np.min(A)
    max_intensity = np.max(A)
    if(max_intensity - min_intensity != 0):
        # normalize to [0, 1]
        A = (A - min_intensity) / (max_intensity - min_intensity)
    
    return A

def l1_normalize(A):
    norm = np.sum(np.abs(A))
    if(norm != 0):
        A = A / norm
    return A

def l2_normalize(A):
    norm = np.sqrt(np.sum(np.power(A, 2)))
    if(norm != 0):
        A = A / norm
    return A

def standardize(A):
    std = np.std(A)
    if(std != 0):
        A = (A - np.mean(A)) / np.std(A)
    return A
