from typing import List
from pydantic import BaseModel


class Task(BaseModel):
    """ Celery task representation """
    task_id: str
    status: str


class PredictionResult(BaseModel):
    """ Task result """
    task_id: str
    status: str
    prediction: str
