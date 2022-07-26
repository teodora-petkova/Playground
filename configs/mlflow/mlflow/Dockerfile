FROM continuumio/miniconda3
RUN apt-get update && apt-get install -y git

RUN pip install mlflow

RUN cd /home && git clone https://github.com/mlflow/mlflow-example.git