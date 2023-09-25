import pyift as ift                                                   
import numpy as np

# feature matrix (nsamples,nfeats) - it must be float32
X = np.array([[0.5, 1.5, 2.5], [3.5, 4.5, 5.5], [6.5, 7.5, 8.5], [9.5, 10.5, 11.5]], dtype=np.float32)

# truelabel array (nsamples,) - it must be int32
y = np.array([1,2,3,4], dtype=np.int32)

# Creating an iftDataSet from numpy arrays
# y could be None
Z = ift.CreateDataSetFromNumPy(X, y)
