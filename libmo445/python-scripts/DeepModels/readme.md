Firstly, open requirements.txt file to see the list of packages required to run the scripts properly. After, edit setup.json file to run your own dataset.


Usage: To show information about the index or layer name:
python Show_model_info.py model name





For tensorflow: 

- To finetune DNN in your own dataset:
python Finetune_tensorflow.py <1 csv file/ input directory> <name of model> <output file (*.h5)> <pretrained weights (*.h5) optional> 

1 - csv file or input directory containing list of images or images, respectively.
2 - name of model to load, such as Resnet50, Resnet101, Densenet161, Densenet169.

The output is a trained model saved in file.h5.


- To predict a list of images or a single image:

python Data_Visualization_Extraction_tensorflow.py <1 csv file/ input directory> <name of model> <input file (*.h5) > <target layer index> <type of visualization>

For instance, for multiple visualization use: 1,2,3,4 or all

- To extract data visualization:

python Predict_tensorflow.py <1 csv file/ input directory> <name of model> <input file (*.h5) >


For theano:

- To finetune DNN in your own dataset:
python Finetune_theano.py <1 csv file/ input directory> <name of model> <output file (*.h5)> <pretrained weights (*.h5) optional> 

1 - csv file or input directory containing list of images or images, respectively.
2 - name of model to load, such as Resnet50, Resnet101, Densenet161, Densenet169.

The output is a trained model saved in file.h5.


- To predict a list of images or a single image:

python Predict_theano.py <1 csv file/ input directory> <name of model> <input file (*.h5) >


python Data_Visualization_Extraction_theano.py <1 csv file/ input directory> <name of model> <input file (*.h5) > <target layer index> <type of visualization>

For instance, for multiple visualization use: 1,2,3,4 or all

