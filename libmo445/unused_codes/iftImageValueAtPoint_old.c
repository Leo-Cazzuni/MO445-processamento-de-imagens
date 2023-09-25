int iftImageValueAtPoint(const iftImage *img, iftPoint P)
{
    iftVoxel u[8];
    int p[8], i, value;
    float dx,dy,dz, val[6];
    
    u[0].x = (int)P.x;      u[0].y = (int)P.y;       u[0].z = (int)P.z;
    u[1].x = u[0].x+1;      u[1].y = u[0].y;         u[1].z = u[0].z;
    u[2].x = u[0].x;        u[2].y = u[0].y + 1;     u[2].z = u[0].z;
    u[3].x = u[0].x+1;      u[3].y = u[0].y + 1;     u[3].z = u[0].z;
    u[4].x = u[0].x;        u[4].y = u[0].y;         u[4].z = u[0].z + 1;
    u[5].x = u[0].x+1;      u[5].y = u[0].y;         u[5].z = u[0].z + 1;
    u[6].x = u[0].x;        u[6].y = u[0].y + 1;     u[6].z = u[0].z + 1;
    u[7].x = u[0].x+1;      u[7].y = u[0].y + 1;     u[7].z = u[0].z + 1;
    
    for (i=0; i < 8; i++) {
        if (iftValidVoxel(img,u[i])){
            p[i] = iftGetVoxelIndex(img,u[i]);
        }else{
            p[0]   = iftGetVoxelIndex(img,u[0]);
            return(img->val[p[0]]);
        }
    }
    
    dx = dy = dz = 1.0;
    
    val[0] =(float)img->val[p[1]]*dx+(float)img->val[p[0]]*(1.0-dx);
    val[1] =(float)img->val[p[3]]*dx+(float)img->val[p[2]]*(1.0-dx);
    val[2] =(float)img->val[p[5]]*dx+(float)img->val[p[4]]*(1.0-dx);
    val[3] =(float)img->val[p[7]]*dx+(float)img->val[p[6]]*(1.0-dx);
    val[4] = val[1]*dy + val[0]*(1.0-dy);
    val[5] = val[3]*dy + val[2]*(1.0-dy);
    value  = (int)(val[5]*dz + val[4]*(1.0-dz));
    
    return(value);
}