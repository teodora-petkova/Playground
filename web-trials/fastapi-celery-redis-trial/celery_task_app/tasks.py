import os
os.environ['BROKER_URI']= 'redis://localhost:6379/0' #'127.0.0.1:6379' # redis uri
os.environ['BACKEND_URI']= 'redis://localhost:6379/1' #'127.0.0.1:6379' # redis uri

import importlib
import logging
from typing import List
from celery import Task

from celery_task_app.worker import celery_app


class ClassifierTask(Task):
    """
    Abstraction of Celery's Task class to support loading ML model.

    """
    abstract = True

    def __init__(self):
        super().__init__()
        self.model = None 

    def __call__(self, *args, **kwargs):
        """
        Load model on first call (i.e. first task processed)
        Avoids the need to load model on each task request
        """
        if not self.model:
            logging.info('Loading Model...')
            module_import = importlib.import_module(self.path[0])
            model_obj = getattr(module_import, self.path[1])
            self.model = model_obj()
            logging.info('Model loaded')
        return self.run(*args, **kwargs)


@celery_app.task(ignore_result=False,
           bind=True,
           base=ClassifierTask,
           path=('celery_task_app.ml.model', 'ClassificationModel'))
def predict_task(self, img_filepath : str):

    features = self.model.predict(img_filepath)
    return features
