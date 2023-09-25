import numpy as np
from sklearn import datasets
from sklearn.manifold import MDS,TSNE
from scipy.stats.mstats import zscore
import matplotlib.pyplot as plt

from lamp import Lamp

X = np.loadtxt("gaussians_5_20_with_label.dat")

number_of_control_points = 100

# randomly chosing the control points
control_points_id = np.random.randint(0, high=X.shape[0], size=(number_of_control_points,))   

##### projecting control points with MDS 
##### this should be replaced by the coordinates of 
##### points projected using the persistency 
#ctp_mds = MDS(n_components=2)
#ctp_proj = ctp_mds.fit_transform(X[control_points_id,0:-1])

ctp_tsne = TSNE(n_components=2)
ctp_proj = ctp_tsne.fit_transform(X[control_points_id,0:-1])

# including ids of the control points as
# the last column of the project coordinates 
ctp_proj = np.hstack((ctp_proj,control_points_id.reshape(number_of_control_points,1)))             

lamp_proj = Lamp(Xdata = X, control_points = ctp_proj, label=True, scale=True)
X_proj = lamp_proj.fit()

plt.scatter(X_proj[:,0],X_proj[:,1],c=X_proj[:,-1])
plt.show()
