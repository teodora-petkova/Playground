from cmath import pi
import numpy as np
import math 

# visualize the detected features
import cv2
import matplotlib.pyplot as plt
plt.close("all")

from utils.image import l1_normalize, l2_normalize, minmax_normalize, standardize

def normalize(A:np.array, norm:str) -> np.array:
    
    if norm == "l1":
        return l1_normalize(A)
    elif norm == "l2":
        return l2_normalize(A)
    elif norm == "minmax":
        return minmax_normalize(A)
    elif norm == "standardize":
        return standardize(A)
    else: # no normalization
        return A

# sources:
#
# EHD - edge histograms descriptor
# - https://ijcsit.com/docs/Volume%207/vol7issue1/ijcsit2016070190.pdf
#
# HOG - histogram of gradients => Sobel histograms
# source: https://docs.opencv.org/master/dd/d3b/tutorial_py_svm_opencv.html
#

def apply_transform_over_each_window(image, window_size, stride, transform_func):
    
    results = []
    
    image_size = np.shape(image)[0]
    for i in np.arange(0, image_size - window_size + 1, stride):
        for j in np.arange(0, image_size - window_size + 1, stride):
    
            window = image[i:i+window_size, j:j+window_size]
            
            processed_window = transform_func(window)
            
            results.append(processed_window)
    
    return np.array(results)

def get_hsv_features(image, window_size, norm):
    # HSV is more robust towards external lighting changes
    # i.e. in the minor changes in lighting, hue values vary relatively lesser than RGB values
    image = image.copy()
    def extract_color_histogram(img, binsize = 8):
    	
        hsv = cv2.cvtColor(img, cv2.COLOR_RGB2HSV)
        #hist = cv2.calcHist([hsv], [0, 1, 2], None, binsize,
    	#	[0, 180, 0, 256, 0, 256])
        # hue range is [0,179], saturation range is [0,255], and value range is [0,255].
        h, s, v = hsv[:,:,0], hsv[:,:,1], hsv[:,:,2]
        hist_h = cv2.calcHist([h], [0], None, [binsize], [0,180])
        hist_s = cv2.calcHist([s], [0], None, [binsize], [0,256])
        hist_v = cv2.calcHist([v], [0], None, [binsize], [0,256])

        hist_h = normalize(hist_h, norm)
        hist_s = normalize(hist_s, norm)
        hist_v = normalize(hist_v, norm)

        window_feature_vector =  np.array([hist_h, hist_s, hist_v]).flatten()

        return window_feature_vector

    stride = window_size # non-overlapping
    
    features = apply_transform_over_each_window(image, window_size, stride, extract_color_histogram)
  
    return features

def show_hsv_histograms_image(image, features,
    features_wsize, window_size, binsize=8):
    
    clone = image.copy()
    thickness = cv2.FILLED
    
    rectsize = int(window_size/3)
    
    for i in np.arange(0, features_wsize):
        for j in np.arange(0, features_wsize):

            x = j*window_size
            y = i*window_size

            subregion = features[j + i*features_wsize]
            
            hi = np.argmax(subregion[0:binsize])
            si = np.argmax(subregion[binsize:2*binsize])
            vi = np.argmax(subregion[2*binsize:])
            
            #[0, 1] => [0, window_size]
            change_range = lambda x : math.floor(x * window_size)
            
            h = change_range(subregion[hi])
            s = change_range(subregion[binsize + si])
            v = change_range(subregion[2*binsize + vi])

            drawRectangle = lambda p1, p2, color : cv2.rectangle(clone, p1, p2, color, thickness)

            epsilon = 1
            drawRectangle((x+epsilon, y+epsilon), (x+rectsize, y+h), (255, 0, 0))
            drawRectangle((x+rectsize+epsilon, y+epsilon), (x+2*rectsize, y+s), (0, 255, 0))
            drawRectangle((x+2*rectsize+epsilon, y+epsilon), (x+window_size, y+v), (0, 0, 255))
        
    plt.imshow(clone)
    plt.axis('off')
    plt.show()

def get_edge_histogram_descriptor(image, window_size, norm):

    sq2 = math.sqrt(2)
    vertical_kernel       = np.array([[ 1, -1],
                                      [ 1, -1]])
    horizontal_kernel     = np.array([[ 1,  1],
                                      [-1, -1]])
    left_diagonal_kernel  = np.array([[sq2, 0],
                                     [0,-sq2]])
    right_diagonal_kernel = np.array([[0, sq2],
                                      [-sq2, 0]])
    no_directional_kernel = np.array([[ 2, -2],
                                      [-2,  2]])
    kernels = [no_directional_kernel,
               vertical_kernel,
               horizontal_kernel,
               left_diagonal_kernel,
               right_diagonal_kernel]

    def get_max_edge_kernel_index(subregion):
        maxi = -1
        maxv = -1

        ind = 0
        for kernel in kernels:
            v = np.sum((subregion*kernel).flatten())

            if(maxv < v):
                maxv = v
                maxi = ind
            ind += 1

        return maxi

    def get_ehd_for_window(window): 

        bins = np.zeros(np.shape(kernels)[0]) # 5 predifined 2x2 kernels
        kernel_size = kernels[0].shape[0]
        stride = kernel_size # non-overlapping

        window_size = np.shape(window)[0]
        for i in np.arange(0, window_size - kernel_size + 1, stride):
            for j in np.arange(0, window_size - kernel_size + 1, stride):
            
                inner_window = window[i:i+kernel_size, j:j+kernel_size]
                max_edge_for_window_index = get_max_edge_kernel_index(inner_window)

                bins[max_edge_for_window_index] += 1 

        bins = normalize(bins, norm)

        return bins

    stride = window_size # non-overlapping

    features = apply_transform_over_each_window(image, window_size, stride, get_ehd_for_window)
  
    
    return features

def show_edge_histogram_image(image, features, features_wsize, window_size):
    
    clone = image.copy()
    color = (0, 255, 0)
    thickness = 1

    half = int(window_size/2)
    for i in np.arange(0, features_wsize):
        for j in np.arange(0, features_wsize):

            x = j*window_size
            y = i*window_size

            epsilon = 1 # in order for the arrows not to reach exactly the boundaries of the subregion
            # (0, 0) in the top left corner - coordinates in opencv
            # x-values go left-to-right (column number) and y-values go top-to-bottom
            drawArrow = lambda p1, p2 : cv2.arrowedLine(clone, 
                p1, p2, color, thickness)  
            
            max_ind = np.argmax(features[j + i*features_wsize])
            if (max_ind == 1):
                clone = drawArrow((x+epsilon, y+half), (x+window_size-epsilon, y+half))
            elif (max_ind == 2):
                clone = drawArrow((x+half, y+epsilon), (x+half, y+window_size-epsilon))
            elif (max_ind == 3):
                clone = drawArrow((x+window_size-epsilon, y+window_size-epsilon), (x+epsilon, y+epsilon))  
            elif (max_ind == 4):
                clone = drawArrow((x+window_size-epsilon, y+epsilon), (x+epsilon, y+window_size-epsilon))
            #elif (max_ind == 0):
            #    clone = drawArrow((x+window_size-epsilon, y+window_size-epsilon), (x+epsilon, y+epsilon))
            #    clone = drawArrow((x+window_size-epsilon, y+epsilon), (x+epsilon, y+window_size-epsilon))

    plt.imshow(clone)
    plt.axis('off')
    plt.show()

def get_sobel_features(image, window_size, bins_size, norm):
    
    stride = window_size # non-overlapping (remark: in HOG - the stride is 1 and we have overlapping!)

    gx = cv2.Sobel(image, cv2.CV_32F, 1, 0,	ksize = 1)
    gy = cv2.Sobel(image, cv2.CV_32F, 0, 1,	ksize = 1)
    
    magnitudes, angles = cv2.cartToPolar(gx, gy)
   
    # the angle range = [0, 2*pi] => (angle / (2*pi)) => new range [0, 1] * binsize => [0, binsize]
    bins = np.int32(bins_size * (angles  / (2*np.pi)))

    features = []
    image_size = image.shape[0]
    
    for i in np.arange(0, image_size - window_size + 1, stride):
        for j in np.arange(0, image_size - window_size + 1, stride):
    
            magnitudes_window = magnitudes[i:i+window_size, j:j+window_size]
            angles_window = bins[i:i+window_size, j:j+window_size]

            hists = np.array([np.bincount(angles_window.flatten(), magnitudes_window.flatten(), bins_size)]).flatten()
            hists = normalize(hists, norm)

            features.append(hists.tolist())
            
    features = np.array(features)    

    return features

def show_sobel_edges_image(image, features, features_wsize, window_size, bins_number=8):
    
    clone = image.copy()
    color = (0, 255, 0)
    thickness = 1

    def draw_arrow(p1, angle, magnitude):

        length = int(magnitude * window_size) # scaling as magnitude is now normalized from 0 to 1
        p2 = (int(round(p1[0] + length * math.cos(angle * math.pi / 180.0))),
              int(round(p1[1] + length * math.sin(angle * math.pi / 180.0))))

        cv2.arrowedLine(clone, p1, p2, color, thickness)      

    for i in np.arange(0, features_wsize):
        for j in np.arange(0, features_wsize):

            x = j*window_size
            y = i*window_size

            subregion = features[j + i*features_wsize]

            max_angle_ind = np.argmax(subregion) 
            max_angle = max_angle_ind / bins_number * 2*np.pi
            magnitude = subregion[max_angle_ind]

            draw_arrow((x, y), max_angle, magnitude)

    plt.imshow(clone)
    plt.axis('off')
    plt.show()