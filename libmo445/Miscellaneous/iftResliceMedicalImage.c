#include "ift.h"



/**
 * @brief Possible Voxel Orders for Volumetric Images.
 * @author Samuka Martins
 * @date Jun 22, 2017
 *
 * Image Coordinates
 *     z
 *    /
 *   /
 *  /___________ x
 * |
 * |
 * |
 * y
 *
 *
 * This impacts how the data (voxel values) are stored on struct and file.
 *
 * We can only have the following plan orientation references (abbreviation):
 *      Plan 1       |           Plan 2          |         Plan 3
 * Left to Right (R) | Anterior to Posterior (P) | Inferior to Superior (S)
 * Right to Left (L) | Posterior to Anterior (A) | Superior to Inferior (I)
 *
 * Then, each coordinate can have any orientation of a single plane.
 * To represent the plan orientation of the coordinates (x,y,z), we have the terminology:
 * RAS = Left to Right in x, Posterior to Anterior in y, Inferior to Superior in z
 * ILP = Superior to Inferior in x, Right to Left in y, Anterior to Posterior in z
 * ...
 *
 * The standard for the .scn images is LPS.
 */









