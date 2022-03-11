import numpy as np
import matplotlib.pyplot as plt
plt.close("all")

import os
import csv
import cv2

from utils.files import *
from utils.image import *
from feature_vectors import *

from sklearn.neighbors import KNeighborsClassifier
from sklearn.metrics import classification_report, confusion_matrix, ConfusionMatrixDisplay
#from sklearn.model_selection import GridSearchCV
from sklearn.model_selection import train_test_split

#reproducability!
random_seed = 42
import random
random.seed(random_seed)
np.random.seed(random_seed)

def image_to_feature_vector(image, size=(32, 32)):
	return cv2.resize(image, size).flatten()

def get_imagepaths_with_category(data_dir):
    image_paths = np.array([])
    labels = np.array([])
    
    
    for dirname in os.listdir(data_dir):
        dirpath = os.path.join(data_dir, dirname)
        if(os.path.isdir(dirpath)):
            #print(dirname)
            for filename in os.listdir(dirpath):
                filepath = os.path.join(dirpath, filename)
                if os.path.isfile(filepath):
                    #print(filepath)
                    image_paths = np.append(image_paths, filepath)
                    labels = np.append(labels, dirname)
    return (image_paths, labels)

def get_feature_vectors(image_paths, labels, window_size, norm):
    
    bins_size = 8

    feature_vectors = {}
    feature_vectors["raw"] = []
    feature_vectors["hsv"] = []
    feature_vectors["ehd"] = []
    feature_vectors["sobel"] = []
    feature_vectors["raw+hsv+edh"] = []
    feature_vectors["raw+hsv+sobel"] = []
    feature_vectors["hsv+edh"] = []
    feature_vectors["hsv+sobel"] = []
    feature_vectors["raw+hsv+edh+sobel"] = []

    num = len(labels)

    for (i, image_path) in enumerate(image_paths):
        image = read_image(image_path)
        image = resize(image, 256)

        # baseline
        raw_pixels = image_to_feature_vector(image)
        
        hsv_hist = get_hsv_features(image, window_size, norm).flatten()
        
        #contrast_image = increase_contrast(image)
        gray_image = grayscale_image(image)
        blurred_image = gaussian_blurred_image(gray_image)
        ehd_hist = get_edge_histogram_descriptor(blurred_image, window_size, norm).flatten()
        sobel_hist = get_sobel_features(blurred_image, window_size, bins_size, norm).flatten()
        
        feature_vectors["raw"].append(raw_pixels)
        feature_vectors["hsv"].append(hsv_hist)
        feature_vectors["ehd"].append(ehd_hist)
        feature_vectors["sobel"].append(sobel_hist)
        feature_vectors["raw+hsv+edh"].append(np.concatenate((raw_pixels, hsv_hist, ehd_hist, sobel_hist)))
        feature_vectors["raw+hsv+sobel"].append(np.concatenate((raw_pixels, hsv_hist, ehd_hist, sobel_hist)))
        feature_vectors["hsv+edh"].append(np.concatenate((hsv_hist, ehd_hist)))
        feature_vectors["hsv+sobel"].append(np.concatenate((hsv_hist, sobel_hist)))
        feature_vectors["raw+hsv+edh+sobel"].append(np.concatenate((raw_pixels, hsv_hist, ehd_hist, sobel_hist)))

        if i > 0 and i % 5 == 0:
            print("[INFO] processed {}/{}".format(i, num))
    return feature_vectors

def export_to_csv(cvs_filename, header, data):

    with open(cvs_filename, 'w', encoding='UTF8', newline='') as f:
        writer = csv.writer(f)

        # write the header
        writer.writerow(header)

        # write multiple rows
        writer.writerows(data)

def extract_classifaction_report(report):
    data = {}
    lines = report.split('\n')
    for line in lines[2:-3]:
        if(line == ''):
            break
        
        row_data = line.split('      ')
        row_data = list(filter(None, row_data))
        data[row_data[0].strip()] = float(row_data[3])

        #row = {}
        #row['class'] = row_data[0]
        #row['precision'] = float(row_data[1])
        #row['recall'] = float(row_data[2])
        #row['f1_score'] = float()
        #row['support'] = float(row_data[4])
        
    return data

def main():
    
    results = []
    
    normalizing = ["l1", "l2", "minmax", "standardize", "no normalization"]
    kNN_metrics = ['manhattan', 'euclidean', 'minkowski']
    p_metrics = [1, 2, 1.4]

    # the name of the data dir with the different categories in subfolders with images
    data_dir = "data" 
    (image_paths, labels) = get_imagepaths_with_category(data_dir)

    for norm in normalizing: # norm
        print("Normalizing by ", norm)
        for m in [4, 8, 16]: # m - mxm subregion size

            print("mxm block: ", m)          
            feature_vectors = get_feature_vectors(image_paths, labels, m, norm)

            for feature_name, feature_vector in feature_vectors.items():

                feature_vector = np.array(feature_vector)
                print("[INFO] {} feature vector: {:.2f}MB".format(feature_name, feature_vector.nbytes / (1024 * 1000.0)))


                for k in [3, 5, 7]: # k-NN - k nearest neighbours
                    print("k-NN: ", k)
                    i = 0
                    for kNN_metric in kNN_metrics:
                        print("kNN metric: ", kNN_metric)   
                        # 20 images per category - 15 for train + 5 for test
                        (train, test, train_labels, test_labels) = train_test_split(feature_vector, labels, test_size=0.25, 
                            stratify=labels, random_state=random_seed)

                        # default metric is actually euclidean with p=2, metric='minkowski' => euclidean_distance (l2) 
                        #L1 vs. L2. It is interesting to consider differences between the two metrics. In particular, the L2 distance is much more unforgiving than the L1 distance when it comes to differences between two vectors. That is, the L2 distance prefers many medium disagreements to one big one. L1 and L2 distances (or equivalently the L1/L2 norms of the differences between a pair of images) are the most commonly used special cases of a p-norm.
                        kNN = KNeighborsClassifier(n_neighbors=k, n_jobs=3, metric = kNN_metric, p=p_metrics[i])
                        kNN.fit(train, train_labels)

                        #the mean accuracy on the given test data and labels.
                        train_accuracy = kNN.score(train, train_labels)
                        test_accuracy = kNN.score(test, test_labels)
                        print("[INFO] Train: ", train_accuracy)
                        print("[INFO] Test: ", test_accuracy) 

                        predicted = kNN.predict(test)

                        report_str = classification_report(test_labels, predicted)
                        report = extract_classifaction_report(report_str)
                        print(f"[INFO] Classification report for classifier:\n {report_str}")

                        #cm = confusion_matrix(test_labels, predicted, labels=kNN.classes_)
                        #disp = ConfusionMatrixDisplay(confusion_matrix=cm, display_labels=kNN.classes_)
                        #disp.plot()
                        #plt.show()

                        results.append([k, m, norm, feature_name, kNN_metric, train_accuracy, test_accuracy, report['beach'], report['city'], report['mountain']])
                        i+=1

    header = ["k-nn", "mxm subregion", "normalize", "feature vector name", "k-nn distance metric", "train accuracy", "test accuracy", "f1-score beaches", "f1-score cities", "f1-score mountains"]
    export_to_csv("results.csv", header, results)
    
if __name__ == '__main__':
    main()