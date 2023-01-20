import os
os.environ['BROKER_URI']= 'redis://localhost:6379/0' #'127.0.0.1:6379' # redis uri
os.environ['BACKEND_URI']= 'redis://localhost:6379/1' #'127.0.0.1:6379' # redis uri

# general
import numpy as np
from pathlib import Path
from typing import BinaryIO

# images
from PIL import Image
import cv2

# fastapi and uvicorn
from fastapi import FastAPI, UploadFile, File, Form, HTTPException
from fastapi.responses import JSONResponse
import uvicorn

# celery
from celery.result import AsyncResult

# custom
from celery_task_app.tasks import predict_task
from models import Task, PredictionResult


app = FastAPI()


ALLOWED_EXTENSION = {'jpg', 'png', 'jpeg', 'webp'}


def load_binary_image_file(image_file: bytes):
    nparr = np.frombuffer(image_file, np.uint8)
    img = cv2.imdecode(nparr, cv2.IMREAD_COLOR)
    return img


async def load_image_file(image_file: BinaryIO):
    file_extension = image_file.filename.split('.')[-1] \
        in ALLOWED_EXTENSION
    if not file_extension:
        return 'File type is invalid.'
    contents = await image_file.read()
    img = load_binary_image_file(contents)
    return img


@app.post('/classifier/predict', response_model=Task, status_code=202)
async def predict(image_file: UploadFile = File(...)):
    """Create celery prediction task. Return task_id to client in order to retrieve result"""
    img = []
    try:
        img = await load_image_file(image_file)
    except Exception as e:
        raise HTTPException(status_code=400, detail=f'Failed to open the image: {e}')

    image = Image.fromarray(img)
    downloads_dir = Path('./downloads').resolve()
    if not downloads_dir.exists():
        downloads_dir.mkdir()

    img_filepath = str(downloads_dir / image_file.filename)
    image.save(img_filepath)

    task_id = predict_task.delay(img_filepath)

    return {'task_id': str(task_id), 'status': 'Processing'}


@app.get('/classifier/result/{task_id}', response_model=PredictionResult, status_code=200,
         responses={202: {'model': Task, 'description': 'Accepted: Not Ready'}})
async def extract_result(task_id):
    """Fetch result for given task_id"""
    task = AsyncResult(task_id)
    if not task.ready():
        return JSONResponse(status_code=202, content={'task_id': str(task_id), 'status': 'Processing'})
    result = task.get()
    return {'task_id': task_id, 'status': 'Success', 'prediction': result}


if __name__ == '__main__':
    uvicorn.run('api:app', host='0.0.0.0', port=8066, reload=False)