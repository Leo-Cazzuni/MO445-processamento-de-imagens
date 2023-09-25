//
// Created by adan on 17/11/17.
//

#ifndef IFT_IFTSEGMENTATION_H
#define IFT_IFTSEGMENTATION_H

#endif //IFT_IFTSEGMENTATION_H

/**
 * @brief Perform a fast smooth on the objects without mixture objects in different blocks
 * @author Adan Echemendia
 * @date 19/06/2017
 * @ingroup Segmentation
 *
 * @param label1 Input Label Image
 * @param weight Weight Map
 * @param block_img An image in wich each pixel has as value the block where it is contained it.
 * @param ninter Number of Iterations
 *
 * @return An image.
 */
iftImage *iftFastSmoothObjectsInsideBB(iftImage *label1, iftFImage *weight, iftImage *block_img,int niters);

/**
 * @brief Smoothes Regions by Diffusion without mixing objects between different blocks
 * @author
 * @date
 * @ingroup Segmentation
 *
 * @param label Label Image
 * @param orig Original Image
 * @param smooth_factor Smooth Factor
 * @pparam niters Number of Iterations
 *
 * @return Image with Smoothed Regions.
 */
iftImage *iftSmoothRegionsByDiffusionInsideBB(iftImage *label, iftImage *orig, iftImage *block_img,float smooth_factor, int niters);