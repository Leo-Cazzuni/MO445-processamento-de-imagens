The python scripts work as follows.

extract-features.py segments an image into superpixels and creates a
dataset of superpixels.

propagate-clusters.py computes groups in that dataset.

label-dataset-from-seeds.py adds seeds in that dataset, which do not
share groups, as supervised samples.

label-image-from-dataset.py shows the image overlay of samples with
true labels.

propagate-labels.py propagates true labels of samples to the remaining
training amples with no true labels.

*** usis can be used to create seeds.txt
*** ClassifierLearning can be used to create semi-supervised training sets


