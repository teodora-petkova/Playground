import time

import torch
from torchvision import transforms
from torchvision.models import ResNet101_Weights

from PIL import Image

SECONDS_TIMEOUT = 5 # to simulate longer model loading

DEVICE = 'cuda' if torch.cuda.is_available() else 'cpu'
device = torch.device(DEVICE)

WEIGHTS = ResNet101_Weights.IMAGENET1K_V2

class ClassificationModel:
    
    def __init__(self):
        model, preprocess = self._load_model()
        self.model = model
        self.preprocess = preprocess

    @staticmethod
    def _load_model():

        model = torch.hub.load('pytorch/vision:v0.10.0', 'resnet101', weights=WEIGHTS)
        model.eval()

        preprocess = transforms.Compose([
            transforms.Resize(256),
            transforms.CenterCrop(224),
            transforms.ToTensor(),
            transforms.Normalize(mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225]),
        ])

        # to simulate longer model loading
        time.sleep(SECONDS_TIMEOUT)
        
        return model, preprocess


    def predict(self, img_filepath : str):

        image = Image.open(img_filepath)

        input_tensor = self.preprocess(image)
        input_batch = input_tensor.unsqueeze(0) # create a mini-batch as expected by the model

        # move the input and model to GPU for speed if available
        if torch.cuda.is_available():
            input_batch = input_batch.to(DEVICE)
            self.model.to(DEVICE)

        with torch.no_grad():
            output = self.model(input_batch)
        # Tensor of shape 1000, with confidence scores over Imagenet's 1000 classes
        #print(output[0])

        # The output has unnormalized scores. To get probabilities, you can run a softmax on it.
        probabilities = torch.nn.functional.softmax(output[0], dim=0)
        #print(probabilities)
        
        #prediction = output.squeeze(0).softmax(0)
        label = probabilities.argmax().item()
        score = probabilities[label].item()
   
        # Use meta to get the labels
        category_name = WEIGHTS.meta['categories'][label]
        result = f"{category_name}: {100 * score}%"

        return result