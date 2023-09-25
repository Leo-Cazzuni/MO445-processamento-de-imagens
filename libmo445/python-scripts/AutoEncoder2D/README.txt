Preparing your environment:

You must use Python 3.
The easiest way is to use a virtual env created with python 3, so that the
command python will be python3 by default, and all packages will be installed
into your environment.

0) Creating the virtual env with python 3:
# On your home diretory:
# myenv is the name of your environment
virtualenv -p python3 myenv

# Put the following command in your .bashrc to automatically activate the
environment myenv:
source $HOME/myenv/bin/activate

# To deactivate the enviroment myenv:
deactivate

1) Installing python packages:
pip install numpy
pip install tensorflow # or install the gpu version
pip install tensorflow-gpu
pip install keras

2) Installing PyIFT
cd $NEWIFT_DIR
make # compile libIFT
cd PyIFT
python parser/parser.py
python setup.py install --force
# pyift will be installed into your myenv environment in the folder:
# ~/myenv/lib/python3.XX/site-packages


