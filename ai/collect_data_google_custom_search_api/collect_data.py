from google_images_search import GoogleImagesSearch
import os
from decouple import config

# configuration
API_KEY = config('API_KEY') # https://console.developers.google.com/apis/credentials -> API KEY
SEARCH_ENGINE_CX = config('SEARCH_ENGINE_CX') # the search engine id/cx https://cse.google.com/cse/all

"""
# TODO: still to check for the api library - google-images-search, TO TEST!!
image_url = "https://upload.wikimedia.org/wikipedia/commons/8/86/Man_o%27war_cove_near_lulworth_dorset_arp.jpg"
path_to_image = "test.jpg" 

with requests.get(image_url, stream=True) as response:
    if not response.ok:
        print(response) # what should we do here? 
    else:
        for chunk in response.iter_content(chunk_size=8192):
            if not chunk:
                break
            with open(path_to_image, 'wb') as f:
                f.write(chunk)

if os.path.exists(path_to_image): # if no check for the response => an exception: cannot identify image file 'test.jpg'
    img = Image.open(path_to_image)
    img.save(path_to_image, 'jpeg')

"""
"""
def get_raw_data(url):
    with requests.get(url, stream=True) as response:
        if not response.ok:
            print(response)
        else:
            for chunk in response.iter_content(chunk_size=8192):
                if not chunk:
                    break
                yield chunk

def save_file(path_to_image, url):
    
    for chunk in get_raw_data(url):
        with open(path_to_image, 'wb') as f:
            f.write(chunk)

    if os.path.exists(path_to_image):
        img = Image.open(path_to_image)
        img.save(path_to_image, 'jpeg')
        return True
    else:
        return False
    #except UnidentifiedImageError as err:
    #    print(f"Unexpected {err}, {type(err)}")
"""

def download_data():              
    
    categories = ['beach' ,'mountain', 'city']
    
    gis = GoogleImagesSearch(API_KEY, SEARCH_ENGINE_CX)

    for category in categories :
        search_parameters = {
            'q': category,
            'num': 20,
            'searchType': 'image',
            'fileType': 'jpg', #'jpg|gif|png',
            'rights': 'cc_publicdomain', #'cc_publicdomain|cc_attribute|cc_sharealike|cc_noncommercial|cc_nonderived',
            'safe': 'high', #active|high|medium|off|safeUndefined', ##
            'imgType': 'photo', #'clipart|face|lineart|stock|photo|animated|imgTypeUndefined', ##
            'imgSize': 'medium',#'huge|icon|large|medium|small|xlarge|xxlarge|imgSizeUndefined', ##
            #'imgDominantColor': 'black|blue|brown|gray|green|orange|pink|purple|red|teal|white|yellow|imgDominantColorUndefined', ##
            #'imgColorType': 'color|gray|mono|trans|imgColorTypeUndefined' ##
        }

        gis.search(search_params=search_parameters)
        i = 0
        for image in gis.results():
            print(str(i) + ": " + image.url)
            
            dirname = os.path.join("categories2", category)
            
            path_to_image =  os.path.join(dirname)
            if(not(os.path.exists(dirname))):
                os.makedirs(dirname)

            gis.download(image.url, path_to_image)
            i += 1
        
download_data()