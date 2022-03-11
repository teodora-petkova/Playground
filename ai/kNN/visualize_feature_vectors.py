import numpy as np

import matplotlib.pyplot as plt
plt.close("all")

from utils.files import *
from utils.image import *
from feature_vectors import *

#image_path = 'D:\\Repositories\\Git-mine\\playground\\ai\\kNN\\data\\train\\beach\\image2.jpg'
image_path = '.\\samples\\star.jpg'

image = read_image(image_path)
image = resize(image, 256)

# window_size = 8, image_size = 256x256
window_size = 8
norm = "l1"

# 32x32 cells -> each  8x8 window => (8 bins for h, s, v each - 24 values per window) => (1024, 24) feature vector
hsvimage = hsv_image(image)
hsv_features = get_hsv_features(hsvimage, window_size, norm)
hsv_features_size = int(image.shape[0]/window_size)
show_hsv_histograms_image(image, hsv_features, hsv_features_size, window_size)
print(np.shape(hsv_features))

# 32x32 cells -> each  8x8 window => (5 values for each edge kernel) => (1024, 5) feature vector
grayimage = grayscale_image(image)
ehd_features = get_edge_histogram_descriptor(grayimage, window_size, norm)
ehd_features_size = int(image.shape[0]/window_size)
show_edge_histogram_image(image, ehd_features, ehd_features_size, window_size)
print(np.shape(ehd_features))

# 32x32 cells -> each  8x8 window => (8 bins/ranges for the angles accumalating the magnitude for each range in the window) => (1024, 8) feature vector
bins_number = 8 # bins for the angles distribution in the histograms
sobel_features = get_sobel_features(grayimage, window_size, bins_number, norm)
sobel_features_size = int(image.shape[0]/window_size)
show_sobel_edges_image(image, sobel_features, sobel_features_size, window_size, bins_number)
print(np.shape(sobel_features))