#ifndef GYRO_KINETIC_STATIC_H
#define GYRO_KINETIC_STATIC_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GYStatic {
	float inverseMass;
	float coeffOfRest;
	float damping;
	float invMomOfInertia;
	float angDamping;
} GYStatic;

#ifdef __cplusplus
}
#endif

#endif