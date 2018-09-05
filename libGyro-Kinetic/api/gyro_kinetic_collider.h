#ifndef GYRO_KINETIC_COLLIDER_H
#define GYRO_KINETIC_COLLIDER_H

#include <libGyro/gyro_matrix_api.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _GY_COLLIDER_TYPE { GY_COLLIDER_NONE, GY_COLLIDER_AABB, GY_COLLIDER_OBB, GY_COLLIDER_RTRI, GY_COLLIDER_OTHER_TRIS, GY_COLLIDER_CIR, GY_COLLIDER_MAX} GY_COLLIDER_TYPE;

//base "class"
typedef struct _GYCollider {
	GY_COLLIDER_TYPE type;
	GYInstMat transform; 
	GYVector3 worldPos;
} GYCollider;

//Subclassed specific colliders
typedef struct _GYBoxCollider {
	GYCollider collider;
	GYVector4 worldVert[4];
	GYVector2 axis[2];
} GYBoxCollider;

typedef struct _GYTriCollider {
	GYCollider collider;
	GYVector4 worldVert[3];
	GYVector2 axis[3];
} GYTriCollider; 

typedef struct _GYCirCollider {
	GYCollider collider;
	GYVector4 worldVert;
} GYCirCollider; 

//Might be useful to the application programs
extern const GYVector4 gyBoxLocalVerts[4];
extern const GYVector4 gyRTriLocalVerts[3];
extern const GYVector4 gyCirLocalVert;


#ifdef __cplusplus
}
#endif

#endif
