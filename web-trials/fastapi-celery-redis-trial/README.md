# An example for serving a ML model using FastAPI with Celery and Redis.

Based on:
- https://towardsdatascience.com/deploying-ml-models-in-production-with-fastapi-and-celery-7063e539a5db
- https://github.com/jonathanreadshaw/ServingMLFastCelery

## Usage

**Requirements:**
Uses:
- Celery  
- Redis  
- Flower 

**Set environment variables:**
* BROKER_URI: Message broker to be used by Celery e.g. Redis/RabbitMQ
* BACKEND_URI: Celery backend e.g. Redis
```bash
export BROKER_URI=...
export BACKEND_URI=...
```

**Start API:**
```bash
uvicorn api:app
```

**Start worker node:**
```bash
celery -A celery_task_app:worker worker -l info
```
OR
```bash
celery -A celery_task_app.tasks worker --loglevel=info -E
```

**Start Flower to monitor celery tasks**
```bash
celery --broker=redis://localhost:6379//0 --result-backend=redis://localhost:6379//1 flower
```
