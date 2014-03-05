#ifndef GYRO_VIDEO_COMMON_H
#define GYRO_VIDEO_COMMON_H

#include "../gyro_matrix_api.h"

typedef enum _GY_FRUSTUM_PLANE {
	GY_FRUSTUM_TOP,
	GY_FRUSTUM_BOTTOM,
	GY_FRUSTUM_LEFT,
	GY_FRUSTUM_RIGHT,
	GY_FRUSTUM_NEARP,
	GY_FRUSTUM_FARP
} GY_FRUSTUM_PLANE;

typedef struct _GYPlane {
	GYVector3 p[3];
} GYPlane;

typedef enum _GY_PROJECTION_TYPE {
	GY_PROJECTION_ORTHO,
	GY_PROJECTION_PERSPECTIVE
} GY_PROJECTION_TYPE;

//figure out how to make this represent ORTHO too
typedef struct _GYViewVolume {
	//static enum {OUTSIDE, INTERSECT, INSIDE};
	GYPlane pl[6];
	GYVector3 ntl,ntr,nbl,nbr,ftl,ftr,fbl,fbr;
	float nearD, farD, ratio, angle, tang;
	float nw, nh, fw, fh;
} GYViewVolume;

typedef struct _GYCamera {
	GYViewVolume		viewVolume;
	GYMatrix4			projMat;
	GYMatrix4			viewMat;
	GYMatrix4			projViewMat;
	GY_PROJECTION_TYPE	projType;
} GYCamera;

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
	gyMatMultMat(cam->projViewMat, cam->projMat, cam->viewMat);
	cam->projType = GY_PROJECTION_ORTHO;

	cam->viewVolume.nw = pright-pleft;
	cam->viewVolume.fw = pright-pleft;
	cam->viewVolume.nh = pbottom-ptop;
	cam->viewVolume.fh = pbottom-ptop;
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

static inline void gyVidCamOrthoLookAt(GYCamera *cam, const GYVector3* pos, const float scale, const float rotZ, const float rotX) {
	unsigned scrWidth, scrHeight;
	GYViewVolume*const volume = &cam->viewVolume;
	//rotX = -0.785398163f;
	GYVector2 botRight;
	float diagonalDist;

	float rectWidth = volume->nw/scale;
	float rectHeight = volume->nh/scale;

	if(rotZ != 0.0f) {
		botRight.x = rectWidth;
		botRight.y = rectHeight;
		diagonalDist = gyVec2Magnitude(&botRight);
		rectWidth = rectHeight = diagonalDist;
	}

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
//	gyMatRotate(rotX, 1.0f, 0.0f, 0..0f);
	gyMatScale(scale, scale, 1.0f);
	gyMatRotateZ(rotZ);

	gyMatTranslate(-pos->x, -pos->y, 0.0f);


	gyMatStore(&cam->viewMat);
	gyMatPop();

	gyMatMultMat(cam->projViewMat, cam->projMat, cam->viewMat);

}

static inline void gyVidCamFrustum(GYCamera* const cam, const float fov, const float aspect, const float znear, const float zfar) {
	GYViewVolume* volume = &cam->viewVolume;

	gyVidViewFrustumMat(cam->projMat, fov, aspect, znear, zfar);
	cam->projType = GY_PROJECTION_PERSPECTIVE;

	gyMatMultMat(cam->projViewMat, cam->projMat, cam->viewMat);

	volume->ratio = aspect;
	volume->nearD = znear;
	volume->farD = zfar;
	volume->angle = fov;

	volume->tang = (float)tan(GY_ANG2RAD * volume->angle * 0.5);
	volume->nh = volume->nearD * volume->tang;
	volume->nw = volume->nh * volume->ratio;
	volume->fh = volume->farD * volume->tang;
	volume->fw = volume->fh * volume->ratio;

}

static inline void gyVidCamFrustumLookAt(GYCamera* const cam, const GYVector3* const eyePos,
												const GYVector3* const lookDir,
												const GYVector3* const upDir) {

	gyVidLookAtMat(cam->viewMat, eyePos->x, eyePos->y, eyePos->z,
				   lookDir->x, lookDir->y, lookDir->z,
				   upDir->x, upDir->y, upDir->z);
	gyMatMultMat(cam->projViewMat, cam->projMat, cam->viewMat);

	GYViewVolume* const volume = &cam->viewVolume;
	GYVector3 dir, nc, fc, X, Y, Z;

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
}

//Camera usage
//void gyVidCamPush(GYCamera* cam) needs to waste a stack slot to be orthogonal with DC?

//Camera culling
//int gyVidCamBoxWithinViewVolume(GYCamera* cam);
//int gyVidCamSphereWithinViewVolume(GYCamera* cam);




#endif // GYRO_VIDEO_COMMON_H
