#ifndef GYRO_KINETIC_COLLISION_H
#define GYRO_KINETIC_COLLISION_H

#include <assert.h>
#include "gyro_kinetic_collider.h"
#include <libGyro/gyro_matrix_api.h>

#ifdef __cplusplus
extern "C" {
#endif



typedef GYVector4 GYPoint;

typedef struct GYLineSegment {
    GYPoint points[2];
} GYLineSegment;

typedef struct GYRay {
    GYPoint     point;
    GYVector3   dir;
} GYRay;

typedef struct GYSphere {
    GYPoint     point;
    float       radius;
} GYSphere;

typedef struct GYRectPrism {

} GYRectPrism;





struct _GYContact;
struct _GYStatic;
    
    
//price we gotta pay for inlinery...
//extern const GYCollider *_curCol;
//extern const struct _GYStatic *_curStat;

/* Specific collision algorithms for different collider combinations 
   HOW DO WE KNOW WHEN POC IS NECESSARY? NON AABBs/actual coefficient of restitution?
*/
    
void gyColliderCurrent(const GYCollider *const col, const struct _GYStatic *const stat);


// SAT - OBB v OBB && OBB v AABB
int gyCollisionOBBvOBB(const GYBoxCollider *boxA, const GYBoxCollider *boxB, struct _GYContact *con, const struct _GYStatic *const statA, const struct _GYStatic *const statB);

//SIMPLIFIED SAT - Only AABB v AABB
int gyCollisionAABBvAABB(const GYBoxCollider *boxA, const GYBoxCollider *boxB, struct _GYContact *con, const struct _GYStatic *const statA, const struct _GYStatic *const statB);

//SPECIALIZED SAT - Only OBB v Tri
int gyCollisionOBBvTri(const GYBoxCollider *boxA, const GYTriCollider *triB, struct _GYContact *con, const struct _GYStatic *const statA, const struct _GYStatic *const statB);

//OBB v Circle
int gyCollisionOBBvCircle(const GYBoxCollider* boxA, const GYCirCollider* cirB, struct _GYContact *con, const struct _GYStatic *const statA, const struct _GYStatic* const statB);

//SPECIALIZED SAT - Only Tri v Tri
int gyCollisionTrivTri(const GYTriCollider *triA, const GYTriCollider *triB, struct _GYContact *con, const struct _GYStatic *const statA, const struct _GYStatic *const statB);

//Circle v Circle
int gyCollisionCirclevCircle(const GYCirCollider *cirA, const GYCirCollider *cirB, struct _GYContact *con, const struct _GYStatic *const statA, const struct _GYStatic *const statB);


//Future Shit
//int gyCollisionCirclevShit();



/*
 Generic collision function that invokes the correct algorithm based on the
 collider types. 
*/

//Do we need a flag for swapping the order of the objects? I think yes.
//Fucking retarded, the only reason we're passing static attrib is so we don't have to pass to contact resolution later...
int gyCollision(GYCollider *bodB, const struct _GYStatic *statB, struct _GYContact *con, int *swapFlag);
    
void gyCalcBoxWorldVert(GYBoxCollider *const box);

void gyCalcTriWorldVert(GYTriCollider *const tri);

void gyCalcCirWorldVert(GYCirCollider* const cir);

static inline void gyCalcWorldVert(void *const obj) {
    GYCollider *const col = (GYCollider *)obj;
	switch(col->type) {
	case GY_COLLIDER_AABB:
	case GY_COLLIDER_OBB:
        gyCalcBoxWorldVert((GYBoxCollider *)obj);
		return;
	case GY_COLLIDER_RTRI:
        gyCalcTriWorldVert((GYTriCollider *)obj);
		return;
    case GY_COLLIDER_CIR:
        gyCalcCirWorldVert((GYCirCollider *)obj);
        return;
	default:
        assert(0);
	}
}



#ifdef __cplusplus
}
#endif


#endif
