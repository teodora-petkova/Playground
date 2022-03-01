import os
import requests
from bs4 import BeautifulSoup

def download_images_for_category(google_images_url, user_agent,
        category_folder, images_count):
    
    category = os.path.basename(os.path.normpath(category_folder))

    search_url = google_images_url + '&q=' + category
    response = requests.get(search_url, headers=user_agent)
    html = response.text

    # find all images with class='rg_i Q4LuWd'
    b_soup = BeautifulSoup(html, 'html.parser')
    results = b_soup.findAll('img', {'class': 'rg_i Q4LuWd'})

    # find the image links inside 'data-src' where this tag is present!
    count = 0
    imagelinks= []
    for res in results:
        if('data-src' in res.attrs):
            link = res['data-src']
            imagelinks.append(link)
            count = count + 1
            if (count >= images_count):
                break
    
    print(f'Exported {len(imagelinks)} images for {category}!')
    
    for i, imagelink in enumerate(imagelinks):
        response = requests.get(imagelink)
        
        imagename = os.path.join(category_folder, category + str(i) + '.jpg')

        with open(imagename, 'wb') as file:
            file.write(response.content)

def download_images(google_images_url, user_agent,
    output_folder, categories, number_of_images_per_category):

    for category in categories:

        category_folder = os.path.join(output_folder, category)
        if not os.path.exists(category_folder):
            os.mkdir(category_folder)

        download_images_for_category(google_images_url, user_agent,
            category_folder, number_of_images_per_category)

def main():

    google_filter = "&tbm=isch" # the filter for images -> to be matched = image search
    google_images_url = 'https://www.google.com/search?site=' + google_filter
    user_agent = {
        'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/98.0.4758.102 Safari/537.36',
        'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8',
        'Accept-Charset': 'ISO-8859-1,utf-8;q=0.7,*;q=0.3',
        'Accept-Encoding': 'none',
        'Accept-Language': 'en-US,en;q=0.8',
        'Connection': 'keep-alive',
    }

    output_folder = 'categories'
    if not os.path.exists(output_folder):
        os.mkdir(output_folder)

    categories = ['beach', 'mountain', 'city']
    number_of_images_per_category = 60

    download_images(google_images_url, user_agent,
        output_folder, categories, number_of_images_per_category)

if __name__ == '__main__':
    main()