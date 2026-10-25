/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef GYRO_VIDEO_COMMON_H
#define GYRO_VIDEO_COMMON_H

#include <libGyro/gyro_matrix_api.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GYRO_VIDEO_QUAD_VERTEX_COUNT    4

typedef enum GY_FRUSTUM_PLANE {
    GY_FRUSTUM_PLANE_RIGHT,
    GY_FRUSTUM_PLANE_LEFT,
    GY_FRUSTUM_PLANE_BOTTOM,
    GY_FRUSTUM_PLANE_TOP,
    GY_FRUSTUM_PLANE_FAR,
    GY_FRUSTUM_PLANE_NEAR,
    GY_FRUSTUM_PLANE_COUNT
} GY_FRUSTUM_PLANE;

typedef struct GYVector4 GYPlane;

typedef enum GY_PROJECTION_TYPE {
    GY_PROJECTION_ORTHO,
    GY_PROJECTION_PERSPECTIVE
} GY_PROJECTION_TYPE;

//figure out how to make this represent ORTHO too
typedef struct GYViewVolume {
    //static enum {OUTSIDE, INTERSECT, INSIDE};
    GYPlane planes[GY_FRUSTUM_PLANE_COUNT];
    GYVector3 ntl,ntr,nbl,nbr,ftl,ftr,fbl,fbr;
    float nearD, farD, ratio, angle, tang;
    float nw, nh, fw, fh;
} GYViewVolume;

typedef struct GYCamera {
    GYViewVolume		viewProjVolume;
    GYMatrix4			projMat;
    GYMatrix4			viewMat;
    GYMatrix4			projViewMat;
    GY_PROJECTION_TYPE	projType;
} GYCamera;


static inline void gyVidFrustumCalculatePlanes(GYPlane planes[], GYMatrix4 mat) {
    const float* clip = (const float*)mat;
    float(*frustumPlanes)[4] = (float(*)[4])planes;
#if 0
    const GYPlane localPlanes[GY_FRUSTUM_PLANE_COUNT] = {
        { clip[ 3] - clip[ 0], clip[ 7] - clip[ 4], clip[11] - clip[ 8], clip[15] - clip[12] }, //RIGHT
        { clip[ 3] + clip[ 0], clip[ 7] + clip[ 4], clip[11] + clip[ 8], clip[15] + clip[12] }, //LEFT
        { clip[ 3] + clip[ 1], clip[ 7] + clip[ 5], clip[11] + clip[ 9], clip[15] + clip[13] }, //BOTTOM
        { clip[ 3] - clip[ 1], clip[ 7] - clip[ 5], clip[11] - clip[ 9], clip[15] - clip[13] }, //TOP
        { clip[ 3] - clip[ 2], clip[ 7] - clip[ 6], clip[11] - clip[10], clip[15] - clip[14] }, //FAR
        { clip[ 3] + clip[ 2], clip[ 7] + clip[ 6], clip[11] + clip[10], clip[15] + clip[14] }  //NEAR
    };
#else
    frustumPlanes[0][0] = clip[ 3] - clip[ 0];
           frustumPlanes[0][1] = clip[ 7] - clip[ 4];
           frustumPlanes[0][2] = clip[11] - clip[ 8];
           frustumPlanes[0][3] = clip[15] - clip[12];

            //left
            frustumPlanes[1][0] = clip[ 3] + clip[ 0];
            frustumPlanes[1][1] = clip[ 7] + clip[ 4];
            frustumPlanes[1][2] = clip[11] + clip[ 8];
            frustumPlanes[1][3] = clip[15] + clip[12];

            //bottom
            frustumPlanes[2][0] = clip[ 3] + clip[ 1];
            frustumPlanes[2][1] = clip[ 7] + clip[ 5];
            frustumPlanes[2][2] = clip[11] + clip[ 9];
            frustumPlanes[2][3] = clip[15] + clip[13];

            //top
            frustumPlanes[3][0] = clip[ 3] - clip[ 1];
            frustumPlanes[3][1] = clip[ 7] - clip[ 5];
            frustumPlanes[3][2] = clip[11] - clip[ 9];
            frustumPlanes[3][3] = clip[15] - clip[13];

            //far
            frustumPlanes[4][0] = clip[ 3] - clip[ 2];
            frustumPlanes[4][1] = clip[ 7] - clip[ 6];
            frustumPlanes[4][2] = clip[11] - clip[10];
            frustumPlanes[4][3] = clip[15] - clip[14];

            //near
            frustumPlanes[5][0] = clip[ 3] + clip[ 2];
            frustumPlanes[5][1] = clip[ 7] + clip[ 6];
            frustumPlanes[5][2] = clip[11] + clip[10];
            frustumPlanes[5][3] = clip[15] + clip[14];


#endif

    for(unsigned i = 0; i < GY_FRUSTUM_PLANE_COUNT; ++i) {
        //Normalize the normal vectors of the plane, does not include D distance,
        //although the D factor is normalized by the same factor afterwards.
        const float len = gyVec3Magnitude(frustumPlanes[i]);
        gyVec4ScaleAccum(frustumPlanes[i], 1.0f/len);
    }

    memcpy(planes, frustumPlanes, sizeof(float)*4*6);
}

//Signed Distance Between Plane and Point (Ax + By + Cz + D)
//If the distance < 0, we're on the other side of the plane.
static inline float gyPlanePointDistance(const GYPlane* plane, const GYVector4* point) {
    return
#if 1
    gyVec3DotProd(point, plane) + plane->w;
#else
    point[v].x * plane[j].x +
    point[v].y * plane[j].y +
    point[v].z * plane[j].z +
    plane[j].w;
#endif
}


//Basically the separating axis fucking theorem.
static inline int gyVidFrustumCullVerts(const GYPlane* planes, const GYVector4* locs, size_t vertCount) {
    int culled = 0;
    for (unsigned j = 0; j < GY_FRUSTUM_PLANE_COUNT; j++) {
        unsigned visibleVerts = 0;
        for(unsigned v = 0; v < vertCount; v++) {

            // If one of the tests fails, then there is no intersection
            if (gyPlanePointDistance(&planes[j], &locs[v]) > 0.0f) {
                ++visibleVerts;
            }
        }
        if(!visibleVerts) {
            culled = 1;
            break;
        }
    }
    return culled;
}





static inline void gyVidOrthoProjMat(GYMatrix4 mat, const float pleft, const float pright,
                         const float pbottom, const float ptop,
                         const float pnear, const float pfar) {
    const GYMatrix4 orthoProjMat = {
        { 2.0f/(pright-pleft), 0.0f, 0.0f, -(pright+pleft)/(pright-pleft) },
        { 0.0f, 2.0f/(ptop-pbottom), 0.0f, -(ptop+pbottom)/(ptop-pbottom) },
        { 0.0f, 0.0f, -2.0f/(pfar-pnear), -(pfar+pnear)/(pfar-pnear) },
        { 0.0f, 0.0f, 0.0f, 1.0f }
    };
unsigned i = 0;
 unsigned j = 0;
    for(i = 0; i < 4; ++i)
        for(j = 0; j < 4; ++j)
            mat[i][j] = orthoProjMat[j][i];
//for(unsigned i = 0; i < 4; ++i)
//    for(unsigned j = 0; j < 4; ++j)
//        mat[i][j] = orthoProjMat[j][i];

}

//Camera setup
static inline void gyVidCamOrthoProj(GYCamera* cam, const float pleft, const float pright,
                    const float pbottom, const float ptop,
                    const float pnear, const float pfar)
{
    gyVidOrthoProjMat(cam->projMat, pleft, pright, pbottom, ptop, pnear, pfar);
    gyMatMultMat(cam->projViewMat, (const float (*)[4])cam->projMat, (const float (*)[4])cam->viewMat);
    cam->projType = GY_PROJECTION_ORTHO;

    cam->viewProjVolume.nw = pright-pleft;
    cam->viewProjVolume.fw = pright-pleft;
    cam->viewProjVolume.nh = pbottom-ptop;
    cam->viewProjVolume.fh = pbottom-ptop;
/*
    cam->viewVolume.ftr.x = pright;
    cam->viewVolume.ftr.y = ptop;
    cam->viewVolume.ftl.x = pleft;
    cam->viewVolume.ftl.y = ptop;
    cam->viewVolume.fbl.x = pleft;
    cam->viewVolume.fbl.y = pbottom;
    cam->viewVolume.fbr.x = pright;
    cam->viewVolume.fbr.y = pbottom;
*/
}


static inline void glalTranspose(GYMatrix4 mat) {
    GYMatrix4 temp;
    memcpy(temp, mat, sizeof(GYMatrix4));

    for(unsigned i = 0; i < 4; ++i)
        for(unsigned j = 0; j < 4; ++j)
            mat[i][j] = temp[j][i];
}


//void gyVidCamFrustum(GYCamera* const cam, const float fov, const float aspect, const float znear, const float zfar);

// to perform cross product between 2 vectors in lookAtMat
static inline void _lookAtCrossProd(float x1, float y1, float z1, float x2, float y2, float z2, float res[3])
{
    res[0] = y1*z2 - y2*z1;
    res[1] = x2*z1 - x1*z2;
    res[2] = x1*y2 - x2*y1;
}

// my own implementation
static inline void gyVidLookAtMat(GYMatrix4 mat, float eyeX, float eyeY, float eyeZ, float lookAtX, float lookAtY, float lookAtZ, float upX, float upY, float upZ)
{

#if 0

    glMatrixMode(GL_MODELVIEW);

    //Cache cube shadow map lookat matrices
    glLoadIdentity();
    gluLookAt(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, upX, upY, upZ);
    glGetFloatv(GL_MODELVIEW_MATRIX, (float*)mat);
    glalTranspose(mat);
    glLoadIdentity();
/*
    gyMatPush();
    gyMatLoad(mat);
    gyMatScale(-1.0f, -1.0f, -1.0f);
    gyMatTranslate(ex, ey, ez);
    gyMatStore(mat);
    gyMatPop();*/
#else


    // i am not using here proper implementation for vectors.
    // if you want, you can replace the arrays with your own
    // vector types
    float f[3];

    // calculating the viewing vector
    f[0] = lookAtX - eyeX;
    f[1] = lookAtY - eyeY;
    f[2] = lookAtZ - eyeZ;


    float fMag = gySqrt(f[0]*f[0] + f[1]*f[1] + f[2]*f[2]);
    float upMag = gySqrt(upX*upX + upY*upY + upZ*upZ);

    // normalizing the viewing vector
    if(fMag != 0) {
        f[0] = f[0]/fMag;
        f[1] = f[1]/fMag;
        f[2] = f[2]/fMag;
    }

    // normalising the up vector. no need for this here if you have your
    // up vector already normalised, which is mostly the case.
    if(upMag != 0) {
        upX = upX/upMag;
        upY = upY/upMag;
        upZ = upZ/upMag;
    }

    float s[3], u[3];

    _lookAtCrossProd(f[0], f[1], f[2], upX, upY, upZ, s);
    _lookAtCrossProd(s[0], s[1], s[2], f[0], f[1], f[2], u);

    float M[]=
    {
        s[0], u[0], -f[0], 0,
        s[1], u[1], -f[1], 0,
        s[2], u[2], -f[2], 0,
        0, 0, 0, 1
    };

    for(unsigned i = 0; i < 4; ++i)
        for(unsigned j = 0; j < 4; ++j)
            mat[i][j] = M[j*4+i];

    gyMatPush();
    gyMatLoad((GYMatrix4*)mat);
    gyMatTranslate(-eyeX, -eyeY, -eyeZ);
    gyMatStore((GYMatrix4*)mat);
    gyMatPop();

#endif

}


static inline void gyVidCamOrthoLookAt(GYCamera *cam, const GYVector3* pos, const float scale, const float angle, const GYVector3* dir) {

    unsigned scrWidth, scrHeight;
    GYViewVolume*const volume = &cam->viewProjVolume;


    float rectWidth = volume->nw/scale;
    float rectHeight = volume->nh/scale;

    //Have to calculate the geometry differently when rotated about the X axis...
    //fuck it for now...

    if(angle != 0.0f) {
        rectWidth = 9999.0f;
        rectHeight = 99999.0f;
    }
#if 0
    GYVector2 botRight;
    float diagonalDist;
    if(rotZ != 0.0f) {
        botRight.x = rectWidth;
        botRight.y = rectHeight;
        diagonalDist = gyVec2Magnitude(&botRight);
        rectWidth = rectHeight = diagonalDist;
    }
#endif

    const float halfW = (rectWidth)*0.5f;
    const float halfH = (rectHeight)*0.5f;

    volume->ntl.x = pos->x - halfW; volume->ntl.y = pos->y - halfH;
    volume->ntr.x = pos->x + halfW; volume->ntr.y = pos->y - halfH;
    volume->nbl.x = pos->x - halfW; volume->nbl.y = pos->y + halfH;
    volume->nbr.x = pos->x + halfW; volume->nbr.y = pos->y + halfH;

    volume->ftl = volume->ntl;
    volume->ftr = volume->ntr;
    volume->fbl = volume->nbl;
    volume->fbr = volume->nbr;

    //Calculate view matrix
    gyMatPush();
    gyMatLoadIdentity();
    gyVidResolution(&scrWidth, &scrHeight);
    gyMatTranslate((float)scrWidth*0.5f, (float)scrHeight*0.5f, 0.0f);
    gyMatScale(scale, scale, 1.0f);
#if 0
    gyMatRotate(rotX, 1.0f, 0.0f, 0.0f);
    gyMatRotateZ(rotZ);
#else
    gyMatRotate(angle, dir->x, dir->y, dir->z);
#endif
#if 0
    GYMatrix4 temp;

    gyVidLookAtMat(temp,
                   0, 0, 1.0f,
                   0.0f, 0.0f, 0.0f,
                   0.0f, -1.0f, 0.0f);
    gyMatApply(temp);

#endif
    gyMatTranslate(floor(-pos->x*scale)/scale, floor(-pos->y*scale)/scale,  0.0f);

    gyMatStore(&cam->viewMat);
    gyMatPop();

    gyMatMultMat(cam->projViewMat, (const float(*)[4])cam->projMat, (const float(*)[4])cam->viewMat);

    gyVidFrustumCalculatePlanes(cam->viewProjVolume.planes, cam->projViewMat);

}



static inline void gyVidPerspectiveMat(GYMatrix4 mat, const float fov, const float aspect, const float znear, const float zfar) {
#if 0
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluPerspective(fov, aspect, znear, zfar);
    glGetFloatv(GL_MODELVIEW, (float*)mat);
    glalTranspose(mat);
    glLoadIdentity();
#else
    (void)zfar;

    float *m = (float*)mat;
    float ymax = znear * tan(fov * M_PI/360.0f);
     float ymin = -ymax;
     float xmax = ymax * aspect;
     float xmin = ymin * aspect;

     float width = xmax - xmin;
     float height = ymax - ymin;

    // float depth = zfar - znear;
     float q = -1.0f;//-(zfar + znear) / depth;
     float qn = -2.0f*znear;//-2 * (zfar * znear) / depth;

     float w = 2 * znear / width;
     w = w / aspect;
     float h = 2 * znear / height;

     m[0]  = w;
     m[1]  = 0;
     m[2]  = 0;
     m[3]  = 0;

     m[4]  = 0;
     m[5]  = h;
     m[6]  = 0;
     m[7]  = 0;

     m[8]  = 0;
     m[9]  = 0;
     m[10] = q;
     m[11] = -1;

     m[12] = 0;
     m[13] = 0;
     m[14] = qn;
     m[15] = 0;

     glalTranspose(mat);



#endif
}




static inline void gyVidCamFrustumLookAt(GYCamera* const cam, const GYVector3* const eyePos,
                                                const GYVector3* const lookDir,
                                                const GYVector3* const upDir) {

    gyVidLookAtMat(cam->viewMat, eyePos->x, eyePos->y, eyePos->z,
                   lookDir->x, lookDir->y, lookDir->z,
                   upDir->x, upDir->y, upDir->z);
/*
    gyMatPush();
    gyMatLoad(&cam->viewMat);
    gyMatScale(1.0f, -1.0f, -1.0f);
    gyMatStore(&cam->viewMat);
    gyMatPop();
    */

    gyMatMultMat(cam->projViewMat, (const float (*)[4])cam->projMat, (const float (*)[4])cam->viewMat);

    GYViewVolume* const volume = &cam->viewProjVolume;
#if 0
    GYVector3 dir, nc, fc, X, Y, Z;
    (void)dir;

    // compute the Z axis of camera
    // this axis points in the opposite direction from
    // the looking direction
    gyVec3Sub(&Z, eyePos, lookDir);
    //Z = p - l;
    gyVec3Normalize(&Z);
    //Z.normalize();

    // X axis of camera with given "up" vector and Z axis
    gyVec3CrossProd(&X, upDir, &Z);
    //X = u * Z;
    gyVec3Normalize(&X);
    //X.normalize();

    // the real "up" vector is the cross product of Z and X
    gyVec3CrossProd(&Y, &Z, &X);
    //Y = Z * X;

    // compute the centers of the near and far planes
    gyVec3AddScaled(&nc, 1.0f, eyePos, -volume->nearD, &Z);
    //nc = p - Z * nearD;
    gyVec3AddScaled(&fc, 1.0f, eyePos, -volume->farD, &Z);
    //fc = p - Z * farD;

    // compute the 4 corners of the frustum on the near plane
    gyVec3AddScaled(&volume->ntl, volume->nh, &Y, -volume->nw, &X);
    gyVec3AddAccum(&volume->ntl, &nc);
    //ntl = nc + Y * nh - X * nw;
    gyVec3AddScaled(&volume->ntr, volume->nh, &Y, volume->nw, &X);
    gyVec3AddAccum(&volume->ntr, &nc);
    //ntr = nc + Y * nh + X * nw;
    gyVec3AddScaled(&volume->nbl, -volume->nh, &Y, -volume->nw, &X);
    gyVec3AddAccum(&volume->nbl, &nc);
    //nbl = nc - Y * nh - X * nw;
    gyVec3AddScaled(&volume->ntl, -volume->nh, &Y, volume->nw, &X);
    gyVec3AddAccum(&volume->ntl, &nc);
    //nbr = nc - Y * nh + X * nw;

    // compute the 4 corners of the frustum on the far plane
    gyVec3AddScaled(&volume->ftl, volume->fh, &Y, -volume->fw, &X);
    gyVec3AddAccum(&volume->ftl, &fc);
    //ftl = fc + Y * fh - X * fw;
    gyVec3AddScaled(&volume->ftr, volume->fh, &Y, volume->fw, &X);
    gyVec3AddAccum(&volume->ftr, &fc);
    //ftr = fc + Y * fh + X * fw;
    gyVec3AddScaled(&volume->fbl, -volume->fh, &Y, -volume->fw, &X);
    gyVec3AddAccum(&volume->fbl, &fc);
    //fbl = fc - Y * fh - X * fw;
    gyVec3AddScaled(&volume->fbr, -volume->fh, &Y, volume->fw, &X);
    gyVec3AddAccum(&volume->fbr, &fc);
    //fbr = fc - Y * fh + X * fw;

    // compute the six planes
    // the function set3Points assumes that the points
    // are given in counter clockwise order
    /*pl[TOP].set3Points(ntr,ntl,ftl);
    pl[BOTTOM].set3Points(nbl,nbr,fbr);
    pl[LEFT].set3Points(ntl,nbl,fbl);
    pl[RIGHT].set3Points(nbr,ntr,fbr);
    pl[NEARP].set3Points(ntl,ntr,nbr);
    pl[FARP].set3Points(ftr,ftl,fbl);*/

#else

    //Fuck the calculations
    volume->ftl.x = volume->ntl.x = -99999.0f;
    volume->ftl.y = volume->ntl.y = -99999.0f;

    volume->ftr.x = volume->ntr.x = 99999.0f;
    volume->ftr.y = volume->ntr.y = -99999.0f;

    volume->fbr.x = volume->nbr.x = 99999.0f;
    volume->fbr.y = volume->nbr.y = 99999.0f;

    volume->fbl.x = volume->nbl.x = -99999.0f;
    volume->fbl.y = volume->nbl.y = 99999.0f;

#endif
}

//Camera usage
//void gyVidCamPush(GYCamera* cam) needs to waste a stack slot to be orthogonal with DC?

//Camera culling
//int gyVidCamBoxWithinViewVolume(GYCamera* cam);


static inline void gyVidCamFrustum(GYCamera* const cam, const float fov, const float aspect, const float znear, const float zfar) {
    GYViewVolume* volume = &cam->viewProjVolume;

    gyVidPerspectiveMat(cam->projMat, fov, aspect, znear, zfar);
    cam->projType = GY_PROJECTION_PERSPECTIVE;

    gyMatMultMat(cam->projViewMat, (const float (*)[4])cam->projMat, (const float (*)[4])cam->viewMat);

    volume->ratio = aspect;
    volume->nearD = znear;
    volume->farD = zfar;
    volume->angle = fov;

    volume->tang = (float)tan(GY_ANG2RAD * volume->angle * 0.5);
    volume->nh = volume->nearD * volume->tang;
    volume->nw = volume->nh * volume->ratio;
    volume->fh = volume->farD * volume->tang;
    volume->fw = volume->fh * volume->ratio;

    gyVidFrustumCalculatePlanes(cam->viewProjVolume.planes, cam->projViewMat);

}










//int gyVidCamSphereWithinViewVolume(GYCamera* cam);

#ifdef __cplusplus
}
#endif


#endif // GYRO_VIDEO_COMMON_H
