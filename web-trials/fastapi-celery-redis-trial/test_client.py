
import requests
import urllib
from time import sleep

def dummy_task(image_filepath, poll_interval=5, max_attempts=10):
    base_uri = r'http://127.0.0.1:8066'
    predict_task_uri = base_uri + '/classifier/predict'

    file = {'image_file': open(image_filepath, 'rb')}

    task = requests.post(predict_task_uri, 
        files=file)
    task_id = task.json()['task_id']
    predict_result_uri = base_uri + '/classifier/result/' + task_id
    attempts = 0
    result = None
    while attempts < max_attempts:
        attempts += 1
        result_response = requests.get(predict_result_uri)
        if result_response.status_code == 200:
            result = result_response.json()['prediction']
            break
        sleep(poll_interval)
    return result


if __name__ == '__main__':
    url, filename = ("https://github.com/pytorch/hub/raw/master/images/dog.jpg", "dog.jpg")
    try: urllib.URLopener().retrieve(url, filename)
    except: urllib.request.urlretrieve(url, filename)

    prediction = dummy_task(filename)
    print(prediction)
