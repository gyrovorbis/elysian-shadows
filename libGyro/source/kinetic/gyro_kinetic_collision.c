#include "kinetic/gyro_kinetic_collision.h"
#include "kinetic/gyro_kinetic_contact.h"
#include "kinetic/gyro_kinetic_static.h"
#include <gyro_matrix_api.h>

const GYVector4 gyBoxLocalVerts[4] = {
	{ -0.5f, -0.5f, 0.0f, 1.0f },
	{ 0.5f, -0.5f, 0.0f, 1.0f },
	{ 0.5f, 0.5f, 0.0f, 1.0f },
	{ -0.5f, 0.5f, 0.0f, 1.0f }
};

const GYVector4 gyRTriLocalVerts[3] = {
	{ 0.5f, -0.5f, 0.0f, 1.0f },
	{ -0.5f, 0.5f, 0.0f, 1.0f },
	{ 0.5f, 0.5f, 0.0f, 1.0f }
};

const GYCollider *_curCol = 0;
const struct _GYStatic *_curStat = 0;

void gyColliderCurrent(const GYCollider *const col, const struct _GYStatic *const stat) {
	_curCol = col;
	_curStat = stat;
}

//Should we push/pop from the stack here? Probably?
//No reason to normalize axes for AABBs!!!
//Doesn't push/pop matrices, so that you can parent the bastards
void gyCalcBoxWorldVert(GYBoxCollider *const box) {
	//gyMatPush();
	//gyMatLoadIdentity();
	//gyMatInstance(&box->collider.transform);
	//Calculate vertices in world coordinates
	gyMatMultVec(gyBoxLocalVerts, box->worldVert, 4);
	//Normalize axes for later collision checks
    gyVec2Sub((GYVector2*)&box->axis[0], (GYVector2*)&box->worldVert[1], (GYVector2*)&box->worldVert[0]);
    gyVec2Sub((GYVector2*)&box->axis[1], (GYVector2*)&box->worldVert[3], (GYVector2*)&box->worldVert[0]);
	gyVec2Normalize(&box->axis[0]);
	gyVec2Normalize(&box->axis[1]);
	//gyMatPop();
}

void gyCalcTriWorldVert(GYTriCollider *const tri) {
	//gyMatPush();
	//gyMatLoadIdentity();
	//gyMatInstance(&tri->collider.transform);
	//Calculate vertices in world coordinates
	gyMatMultVec(gyRTriLocalVerts, tri->worldVert, 3);
	//Normalize axes for later collision checks
    gyVec2Sub((GYVector2*)&tri->axis[0], (GYVector2*)&tri->worldVert[1], (GYVector2*)&tri->worldVert[0]);
    gyVec2Sub((GYVector2*)&tri->axis[1], (GYVector2*)&tri->worldVert[2], (GYVector2*)&tri->worldVert[0]);
    gyVec2Sub((GYVector2*)&tri->axis[2], (GYVector2*)&tri->worldVert[2], (GYVector2*)&tri->worldVert[1]);
	gyVec2Normalize(&tri->axis[0]);
	gyVec2Normalize(&tri->axis[1]);
	gyVec2Normalize(&tri->axis[2]);
	//gyMatPop();
}

int gyCollision(GYCollider *bodB, const struct _GYStatic *statB, struct _GYContact *con, int *swapFlag) {
    const GYCollider *const bodA = (const GYCollider *)_curCol;
	const GYCollider *const colB = (const GYCollider *)bodB;
    const struct _GYStatic *statA = _curStat;
	*swapFlag = 0;
    
	switch(_curCol->type) {
            
        case GY_COLLIDER_AABB:
            switch(colB->type) {
                case GY_COLLIDER_AABB:
                    //return gyCollisionAABBvAABB((const GYBoxCollider *)bodA, (const GYBoxCollider *)bodB, con, statA, statB);
                case GY_COLLIDER_OBB:
                    return gyCollisionOBBvOBB((const GYBoxCollider *)bodA, (const GYBoxCollider *)bodB, con, statA, statB);
                case GY_COLLIDER_RTRI:
                    return gyCollisionOBBvTri((const GYBoxCollider *)bodA, (const GYTriCollider *)bodB, con, statA, statB);
                default:
                    assert(0);
            }
            break;
            
        case GY_COLLIDER_OBB:
            switch(colB->type) { 		
				case GY_COLLIDER_AABB:
                case GY_COLLIDER_OBB:
                    return gyCollisionOBBvOBB((const GYBoxCollider *)bodA, (const GYBoxCollider *)bodB, con, statA, statB);
                case GY_COLLIDER_RTRI:
                    return gyCollisionOBBvTri((const GYBoxCollider *)bodA, (const GYTriCollider *)bodB, con, statA, statB);
                default:
                    assert(0);
            }
            break;
            
        case GY_COLLIDER_RTRI:
            switch(colB->type) {
                case GY_COLLIDER_AABB:
                case GY_COLLIDER_OBB:
                    *swapFlag = 1; //Tri is the second argument, swapping order!
                    return gyCollisionOBBvTri((const GYBoxCollider *)bodB, (const GYTriCollider *)bodA, con, statB, statA);
                case GY_COLLIDER_RTRI:
                    return gyCollisionTrivTri((const GYTriCollider *)bodA, (const GYTriCollider *)bodB, con, statA, statB);
                default:
                    assert(0);
            }
            break;
            
        default:
            assert(0);
	}
	assert(0);
	return 0;
}



//SIMPLIFIED SAT - Only AABB v AABB
int gyCollisionAABBvAABB(const GYBoxCollider *boxA, const GYBoxCollider *boxB, struct _GYContact *con, const struct _GYStatic *const statA, const struct _GYStatic *const statB) {
	return gyCollisionOBBvOBB(boxA, boxB, con, statA, statB);
}


//SPECIALIZED SAT - Only OBB v Tri
int gyCollisionOBBvTri(const GYBoxCollider *boxA, const GYTriCollider *triB, struct _GYContact *con, const struct _GYStatic *const statA, const struct _GYStatic *const statB) {
	(void)boxA;
	(void)triB;
	(void)con;
	(void)statA;
	(void)statB;
	return 0;
}

//SPECIALIZED SAT - Only Tri v Tri
int gyCollisionTrivTri(const GYTriCollider *triA, const GYTriCollider *triB, struct _GYContact *con, const struct _GYStatic *const statA, const struct _GYStatic *const statB) {
	(void)triA;
	(void)triB;
	(void)con;
	(void)statA;
	(void)statB;
	return 0;
}



static inline void Swapf(float *const a, float *const b) { 
	float temp = *a;
	*a = *b;
	*b = temp;
}


static inline void ProjectPolygon(const GYVector2 *const axis, const GYVector4 vertex[], float *const min, float *const max, int *const minIndex, int *const maxIndex) {
	unsigned int i = 1;
    float dotProduct = gyVec2DotProd((GYVector2*)&vertex[0], axis);
	
	*max = *min = dotProduct;
	*minIndex = *maxIndex = 0;
	for(; i < 4; ++i) {
        dotProduct = gyVec2DotProd((GYVector2*)&vertex[i], axis);
        if(dotProduct < *min) {
			*min = dotProduct;
			*minIndex = i;
		}
        else if(dotProduct > *max) {
			*max = dotProduct;
			*maxIndex = i;
		}
    }
}

// SAT - OBB v OBB && OBB v AABB
int gyCollisionOBBvOBB(const GYBoxCollider *boxA, const GYBoxCollider *boxB, struct _GYContact *con, const struct _GYStatic *const statA, const struct _GYStatic *const statB) {
	float min, max;
	unsigned int i = 0;
	//int minIndex, maxIndex;
	//gyroVector2 axis[4]; //move this shit
	//cache the bitches
		
	// evens are min projections, odds are max
	// scalarA[0-1] and scalarB[0-1] are scalar projections onto axis[0]
	// scalarA[2-3] and scalarB[2-3] are scalar projections onto axis[1]
	// scalarA[4-5] and scalarB[4-5] are scalar projections onto axis[2]
	// scalarA[6-7] and scalarB[6-7] are scalar projections onto axis[3]
	//Indeces hold the worldVertex index number of the largest/smallest projections onto the axes
	float scalarA[8], scalarB[8];
	int scalarAIndex[8], scalarBIndex[8];

	//=============
	float diff, minDiff;
	unsigned int axisNum = 0;
	char neg = 0;
		
	//MAKE FUCKING SURE COLLIDABLE::CALCULATEVERTICES() HAS BEEN CALLED!
		
	// Determine 4 axes (move this elsewhere later)
	/*gyroVector2Sub(&axis[0], &colA->worldVertex[1], &colA->worldVertex[0]);
	gyroVector2Sub(&axis[1], &colA->worldVertex[3], &colA->worldVertex[0]);
	gyroVector2Sub(&axis[2], &colB->worldVertex[1], &colB->worldVertex[0]);
	gyroVector2Sub(&axis[3], &colB->worldVertex[3], &colB->worldVertex[0]);
		
	for(; i < 4; ++i) gyroVector2Normalize(&axis[i]); //ugh*/
		
	// Calculate axis[0] projections.
    scalarA[0] = gyVec2DotProd((GYVector2*)&boxA->worldVert[0], (GYVector2*)&boxA->axis[0]);
    scalarA[1] = gyVec2DotProd((GYVector2*)&boxA->worldVert[1], (GYVector2*)&boxA->axis[0]);
	if(scalarA[0] > scalarA[1]) {
		Swapf(&scalarA[0], &scalarA[1]);
		scalarAIndex[0] = 0;
		scalarAIndex[1] = 1;
	}
	else {
		scalarAIndex[0] = 1;
		scalarAIndex[1] = 0;		
	}
		
	ProjectPolygon(&boxA->axis[0], boxB->worldVert, &min, &max, &scalarBIndex[0], &scalarBIndex[1]);
	scalarB[0] = min;
	scalarB[1] = max;
		
	if(!((scalarB[0] < scalarA[1]) && (scalarB[1] > scalarA[0]))) return 0; //De Morgan, dumbass
		
	// Calculate axis[1] projections.
    scalarA[2] = gyVec2DotProd((GYVector2*)&boxA->worldVert[0], (GYVector2*)&boxA->axis[1]);
    scalarA[3] = gyVec2DotProd((GYVector2*)&boxA->worldVert[3], (GYVector2*)&boxA->axis[1]);
	if(scalarA[2] > scalarA[3]) {
		Swapf(&scalarA[2], &scalarA[3]);
		scalarAIndex[2] = 0;
		scalarAIndex[3] = 3;
	}
	else {
		scalarAIndex[2] = 3;
		scalarAIndex[3] = 0;
	}
		
	ProjectPolygon(&boxA->axis[1], boxB->worldVert, &min, &max, &scalarBIndex[2], &scalarBIndex[3]);
	scalarB[2] = min;
	scalarB[3] = max;
		
	if(!((scalarB[2] < scalarA[3]) && (scalarB[3] > scalarA[2]))) return 0;
		
	// Calculate axis[2] projections.
    scalarB[4] = gyVec2DotProd((GYVector2*)&boxB->worldVert[0], (GYVector2*)&boxB->axis[0]);
    scalarB[5] = gyVec2DotProd((GYVector2*)&boxB->worldVert[1], (GYVector2*)&boxB->axis[0]);
	if(scalarB[4] > scalarB[5]) {
		Swapf(&scalarB[4], &scalarB[5]);
		scalarBIndex[4] = 0;
		scalarBIndex[5] = 1;
	}
	else {
		scalarBIndex[4] = 1;
		scalarBIndex[5] = 0;
	}
		
	ProjectPolygon(&boxB->axis[0], boxA->worldVert, &min, &max, &scalarAIndex[4], &scalarAIndex[5]);
	scalarA[4] = min;
	scalarA[5] = max;
		
	if(!((scalarB[4] < scalarA[5]) && (scalarB[5] > scalarA[4]))) return 0;
		
	// Calculate axis[3] projections.
    scalarB[6] = gyVec2DotProd((GYVector2*)&boxB->worldVert[0], (GYVector2*)&boxB->axis[1]);
    scalarB[7] = gyVec2DotProd((GYVector2*)&boxB->worldVert[3], (GYVector2*)&boxB->axis[1]);
	if(scalarB[6] > scalarB[7]) {
		Swapf(&scalarB[6], &scalarB[7]);
		scalarBIndex[6] = 0;
		scalarBIndex[7] = 3;
	}
	else {
		scalarBIndex[6] = 3;
		scalarBIndex[7] = 0;
	}
		
	ProjectPolygon(&boxB->axis[1], boxA->worldVert, &min, &max, &scalarAIndex[6], &scalarAIndex[7]);
	scalarA[6] = min;
	scalarA[7] = max;
		
	if(!((scalarB[6] < scalarA[7]) && (scalarB[7] > scalarA[6]))) return 0;
		
	// Otherwise, there is a collision!
		
	// The direction of the normal vector is in the same direction as the axis with the smallest area of overlap.
	// The magnitude of the normal vector is the magnitude of said overlap.
		
	// diff is a temporary difference that is recalculated and compared with minDiff for each of the eight overlaps.
	// minDiff stores the magnitude of the minimal overlap.
	// axisNum stores the axis number (0-3) that minDiff occurs on.
	// neg indicates whether or not the colliding rectangle is coming from a direction in which the normal vector
	// must be negated in order to push the rectangle being collided with in the right direction.

	con->staticAttrib[0] = statA;
	con->staticAttrib[1] = statB;
	con->collider[0] = (const GYCollider*)boxA;
	con->collider[1] = (const GYCollider*)boxB;

	minDiff = scalarB[1] - scalarA[0];
	for(i = 0; i < 8; i += 2) {
		//Use position to avoid these two conditionals?
		diff = scalarB[i+1] - scalarA[i];
		if(diff < minDiff) {
			minDiff = diff;
			axisNum = i>>1;
			neg = 0;
		}
		diff = scalarA[i+1] - scalarB[i];
		if(diff < minDiff) {
			minDiff = diff;
			axisNum = i>>1;
			neg = 1;
		}
	}
	
	// Calculate the normal vector.
	//normal = (neg)? (axis[axisNum] * minDiff * -1) : (axis[axisNum] * minDiff);
	//Contact generation
	//con->_colIndex[0] = colIndexA;
	//con->_colIndex[1] = colIndexB;
	//printf("axisnum - %d, <%f, %f>\n", axisNum, axis[axisNum].x, axis[axisNum].y);
	con->normal = (axisNum <= 1)? boxA->axis[axisNum] : boxB->axis[axisNum-2]; 
	//contact->_normal.y = axis[axisNum].y;
	/*contact->_depth = (neg)? minDiff * -1 : minDiff;*/
	if(neg) {
		con->normal.x = -con->normal.x;
		con->normal.y = -con->normal.y;
	}
	con->depth = minDiff;
    
    
    
	con->worldPos.x = (boxA->collider.transform._pos.x + boxB->collider.transform._pos.x) * 0.5f;
	con->worldPos.y = (boxA->collider.transform._pos.y + boxB->collider.transform._pos.y) * 0.5f;
    

	return 1;
	//contact->_type = GYRO_CONTACT_PARTICLE_PARTICLE;
	
}
