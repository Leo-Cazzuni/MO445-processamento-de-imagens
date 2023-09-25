Brief:

Example of the pipeline for image segmentation using optimum connectivity proposed on [1].

* Functions DynTreeRoot and DynTreeClosestRoot performs an image segmentation using Dynamic 
Trees, parameters:
 + mimg          multi-band original image
 + A             adjacency relation
 + seeds         labeled seeds nodes
 + delta         (optional) plato penalization height
 + gamma		 (optional) neighbor distance scaling parameter (w_{dyn}(p, q) + \gamma ||I(p) - I(q)||)         
 + objmap        (optional) saliency image
 + alpha         (optional) saliency weight, is set to 1.0 if objmap is not given


Requirements:

* PyIFT

Example:

python3 superpixel -i <input img> -n <number of superpixels> -o <output img>
python3 objmap.py  -i <input img> -m <markers img> -s <superpixel img> -k <number of neighbors>	-o <output img> 
python3 segment.py -i <input img> -m <markers img> -s <saliency img> -o <output img>  

References:

[1] Falcão, A.X. and Bragantini, J.: The Role of Optimum Connectivity in Image Segmentation: 
Can the algorithm learn object information during the process? In: Discrete Geometry for 
Computer Imagery, 21th IAPR International COnference (2019), to appear

[2] Galvão, F. L., Falcão, A. X., & Chowdhury, A. S. (2018, October). 
RISF: Recursive Iterative Spanning Forest for Superpixel Segmentation.
In 2018 31st SIBGRAPI Conference on Graphics, Patterns and Images
(SIBGRAPI) (pp. 408-415). IEEE.

