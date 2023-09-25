import pyift as ift
import numpy as np

# reading the dataset abc.zip
Z = ift.ReadDataSet("abc.zip")

# Get the feature matrix as a numpy array (nsamples,nfeats)
data = Z.GetData()

# Get the projections as a numpy array
projections = Z.GetProjection()

# Get the samples' truel labels as numpy array (nsamples,)
truelabels = Z.GetTrueLabels()

# Get the samples' labels as numpy array (nsamples,)
labels = Z.GetLabels()

# Get the samples' ids as numpy array (nsamples,)
ids = Z.GetIds()

# Get the samples' groups as numpy array (nsamples,)
groups = Z.GetGroups()

# Get the samples' flag isSupervised as numpy array (nsamples,)
is_supervised_arr = Z.GetIsSupervised()

# Get the samples' flag isLabelPropagated as numpy array (nsamples,)
is_propagated_arr = Z.GetIsLabelPropagated()

# Get the samples' weights as numpy array (nsamples,)
weights = Z.GetWeights()

# Get the samples' status as numpy array (nsamples,)
status = Z.GetStatus()

# Get the features' alphas as numpy array (nfeats,)
alphas = Z.GetAlphas()