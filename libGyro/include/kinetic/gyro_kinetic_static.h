#ifndef GYRO_KINETIC_STATIC_H
#define GYRO_KINETIC_STATIC_H

#include <gyro_matrix_api.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GYStatic {
    GYVector3   damping;
    float       inverseMass;
    float       coeffOfRest;
    float       invMomOfInertia;
    float       angDamping;
    float       zCoeffOfRest;
} GYStatic;

#ifdef __cplusplus
}
#endif

#endif
