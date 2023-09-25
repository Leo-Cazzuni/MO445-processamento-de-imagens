
Required python packages:
1) SimpleITK
    pip install SimpleITK
2) NiLabel
    pip install nibabel

To convert DICOM to nii (do not try to convert to scn), you better use
iftConvertDicomByITK, which requires the compilation of
iftNormalizeImage (in Miscellaneous/RadiometricTransforms), iftInterp (in Miscellaneous), and SimpleITK. It is prepared to
normalize within 0-4095 and interpolate the image to the minimum voxel
size.



