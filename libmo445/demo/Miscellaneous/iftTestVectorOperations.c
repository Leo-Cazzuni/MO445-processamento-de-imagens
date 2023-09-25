#include "ift.h"


int main(int argc, const char **argv) {
    iftVoxel vox1 = {1, 2, 3}, vox2 = {3, 2, 1}, vox3;
    iftVector vec1 = {3.5, 2, 3}, vec2 = {3, 2, 1}, vec3;

    /*--------------------------------------------------------*/

    int MemDinInicial, MemDinFinal;
    MemDinInicial = iftMemoryUsed();
    /*--------------------------------------------------------*/

    vox3 = (iftVoxel)iftVectorSub(vox1, vox2);
    vec3 = (iftVector)iftVectorSub(vox1, vox2);

    printf("vox3 %d %d %d\n", vox3.x, vox3.y, vox3.z);
    printf("vec3 %f %f %f\n", vec3.x, vec3.y, vec3.z);

    vec3 = (iftVector)iftVectorSub(vec1, vox2);

    printf("vec3 %f %f %f\n", vec3.x, vec3.y, vec3.z);

    vec3 = (iftVector)iftVectorScalarProd(vec3, 2);

    printf("vec3 %f %f %f\n", vec3.x, vec3.y, vec3.z);

    vec3 = iftVectorScalarDiv(vec3, 4);

    printf("vec3 %f %f %f\n", vec3.x, vec3.y, vec3.z);

    vec3 = (iftVector)iftVectorSum(vec3, vox1);

    printf("vec3 %f %f %f\n", vec3.x, vec3.y, vec3.z);

    vec3 = (iftVector)iftVectorRound(vec3);

    printf("vec3 %f %f %f\n", vec3.x, vec3.y, vec3.z);


    /*--------------------------------------------------------*/

    MemDinFinal = iftMemoryUsed();
    iftVerifyMemory(MemDinInicial, MemDinFinal);
    /*--------------------------------------------------------*/



    return 0;
}