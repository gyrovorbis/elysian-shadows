/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef GYRO_PHYSICS_SIMULATION_H
#define GYRO_PHYSICS_SIMULATION_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GYRO_PHYSICS_SIMULATION_DEBUG 1

#define GYRO_PHYSICS_SIMULATION_MAJOR_VER   0
#define GYRO_PHYSICS_SIMULATION_MINOR_VER   1
#define GYRO_PHYSICS_SIMULATION_PATCH_VER   0
#define GYRO_PHYSICS_SIMULATION_VERSION_STR "BLAH"

typedef float GYReal_t;


typedef enum GY_PHYSICS_ERROR_CODE {
    GY_PHYSICS_SUCCESS,
    GY_PHYSICS_TYPE_MISMATCH,
    GY_PHYSICS_ERROR_CODE_MAX
} GY_PYSICS_ERROR_CODE;

const char* gyPhysicsErrorCodeStr(GY_PHYSICS_ERROR_CODE val);
GY_PHYSICS_ERROR_CODE gyPhysicsLastErrorCode(void);

typedef enum GY_COLLIDER_PRIMITIVE {
    GY_COLLIDER_CIRCLE,
    GY_COLLIDER_ELLIPSE,
    GY_COLLIDER_AABB,
    GY_COLLIDER_OBB,
    GY_COLLIDER_TRI,
    GY_COLLIDER_POLY,
    GY_COLLIDER_TYPE_MAX
} GY_COLLIDER_PRIMITIVE;

typedef struct GYPhysicsInitParams {
    int threadCount;
    int useGPGPU;

    int primPoolSizes[GY_COLLIDER_PRIMITIVE];
    int contactPoolSize;
    int particlePoolSize;
    int broadPhaseGroupPoolSize;
    int forcePoolSize;
    //Vulkan device #, etc

} GYPhysicsInitParams;


struct GYBody;
struct GYFile;
struct GYForce;

typedef struct GYSimulation {
    int bodyCount;
    int threadCount;

    GYBodyInternal*             bodyPool;
    GYColliderInternal*         colliderPool;
    GYColliderAttrInternal*     colliderAttrPool;
    GYRigidBodyInternal*        rigidBodyPool;
    GYRigidBodyAttrInternal*    rigidBodyAttrPool;
    GYContactInternal*          contactPool;
    GYBroadPhaseGroupInternal*  broadPhaseGroupPool;
    GYParticleInternal*         particlePool;
    GYParticleAttrInternal*     particleAttrPool;
    GYForce*                    persistentForces;
} GYSimulation;


typedef struct GYSimulationCreateParams {
    int maxBodyCount;
    int preallocCount;
    int bodyCount;
    GYBody* bodies;
    GYFile* simInputFile;
    int mapSize; //?
    int dynamicFragmentMax;
} GYSimulationInitParams;

int         gyPhysicsInit(GYPhysicsInitParams* initParams);
void        gyPhysicsRuntimeVersion(int* major, int* minor, int* patch);
const char* gyPhysicsRuntimeVersionStr(void);
int         gyPhysicsUninit(void);


GYSimulation* gySimulationCreate(GYSimulationCreateParams* initParams);
void gySimulationDestroy(GYSimulation* sim);
void gySimulationUpdate(GYSimulation* sim, GYReal_t deltaTime);
void gySimulationBodyAdd(GYSimulation* sim, GYBody* body);
void gySimulationBodyRemove(GYSimulation* sim, GYBody* body);
void gySimulationTileUpdate(int x, int y, int tileColliderType);
void gySimulationForceApply(GYSimulation* sim, GYForce* force);
void gySimulationForceAdd(GYSimulation*sim, GYForce* force);
void gySimulationForceRemove(GYSimulation*sim, GYForce* force);

struct GYBodyCreateParams;




typedef enum GY_BODY_FIELD {
    GY_BODY_ACTIVE,
    GY_BODY_TYPE_MASK,
    GY_BODY_TYPE_MASK_2,
    GY_BODY_TYPE_AGAINST_MASK,
    GY_BODY_TYPE_AGAINST_MASK_2,
    GY_BODY_COLLIDER_TYPE,
    GY_BODY_COLLIDER_DATA,
    GY_BODY_USERDATA,
    GY_BODY_ATTRIB_DATA,
    GY_BODY_FORCE_MASK,
    GY_BODY_TRIGGER_MASK,
    GY_BODY_VELOCITY,
    GY_BODY_ACCEL,
    GY_BODY_FORCE_ACCUM,
    GY_BODY_ROTATION,
    GY_BODY_TORQUE
} GY_BODY_FIELD;

typedef struct GYBodyInternal {
    int32_t index;
    int32_t colliderIndex;
    int32_t colliderAttrIndex;
    int32_t rigidBodyIndex;
    int32_t rigidBodyAttrIndex;
} GYBodyInternal;

typedef struct GYColliderInternal {
    int32_t     bodyIndex;
    uint32_t    primType;
    int32_t     primIndex;
    //uint32_t  broadPhasePrimType;
    int32_t     broadPhasePrimIndex;
    int32_t     broadPhaseGroupindex;
    uint32_t    contactCount;
} GYColliderInternal;

typedef struct GYPrimitiveInternal {
    int32_t colliderIndex;
} GYPrimitiveInternal;

typedef struct GYCirclePrimitiveInternal {
    GYPrimitiveInternal primitive;
    //MINIMAL REPRESENTATION
} GYCirclePrimitiveInternal;

typedef struct GYEllipsoidPrimitiveInternal {
    GYPrimitiveInternal primitive;
    //MINIMAL REPRESENTATION
} GYEllipsoidPrimitiveInternal;

typedef struct GYAABBPrimitiveInternal {
    GYPrimitiveInternal primitive;
    //MINIMAL REPRESENTATION
} GYAABBPrimitiveInternal;

typedef struct GYOBBPrimitiveInternal {
    GYPrimitiveInternal primitive;
    //MINIMAL REPRESENTATION
} GYOBBPrimitiveInternal;

typedef struct GYTriPrimitiveInternal {
    GYPrimitiveInternal primitive;
    //MINIMAL REPRESENTATION
} GYTriPrimitiveInternal;

typedef struct GYPolyPrimitiveInternal {
    GYPrimitiveInternal primitive;
    //MINIMAL REPRESENTATION
} GYPolyPrimitiveInternal;

typedef struct GYRigidBodyInternal {
    int32_t     bodyIndex;
    int32_t     rigidBodyAttrIndex;
    GYReal_t    velocity[3];
    GYReal_t    accel[3];
    GYReal_t    forceAccum[3];
    GYReal_t    rotation;
    GYReal_t    torqueAccum;
    GYReal_t    damping[3];
    int32_t     forceMask;
    int32_t     durability;
} GYRigidBodyInternal;

typedef struct GYContactInternal {
    int32_t     bodyAIndex;
    int32_t     bodyBIndex;
    GYReal_t    pointOfContact[3];
    GYReal_t    normalVector[3];
    GYReal_t    closingVelocity[3];
    GYReal_t    depth;
    GYReal_t    totalInertia;//?
} GYContactInternal;

typedef struct GYParticleInternal {
    GYReal_t    pos[3];
    GYReal_t    velocity[3];
    GYReal_t    accel;
    GYReal_t    size;
    int32_t     attrIndex;
    uint32_t    lifeTime;    
} GYParticle;

typedef struct GYParticleAttrInternal {
    uint32_t lifeTime;
    uint32_t startSize;
    uint32_t endSize;
    uint32_t forceMask;
} GYParticleAttrInternal;

typedef struct GYFragmentInternal {
    int32_t     bodyIndex;
    uint32_t    primType;
    int32_t     primIndex;
    uint32_t    lifeTime;
} GYFragmentInternal;




struct GYBody {
    int32_t index; //-1 if none
    void* userdata;


};

GYBody gyBodyCreate(GYBodyCreateParams* createParams);
void gyBodyDestroy(GYBody body);

int gyBodyFieldSetf(GYBody body, GY_BODY_FIELD field, GYReal_t value);
int gyBodyFieldSeti(GYBody body, GY_BODY_FIELD field, int value);
int gyBodyFieldSetPtr(GYBody body, GY_BODY_FIELD field, void* ptr);

int gyBodyFieldGetf(GYBody body, GY_BODY_FIELD field, GYReal_t* value);
int gyBodyFieldGeti(GYBody body, GY_BODY_FIELD field, int* value);
int gyBodyFieldGetPtr(GYBody body, GY_BODY_FIELD field, void** ptr);

//need a callback mechanism for telling entity to update its actual position...?


typedef enum GY_BODY_ATTR_FIELD {
    GY_BODY_ATTR_INV_MASS,
    GY_BODY_ATTR_COEFF_OF_REST,
    GY_BODY_ATTR_INV_MOM_OF_INERTIA,
    GY_BODY_ATTR_ANG_DAMPING,
    GY_BODY_ATTR_Z_COEFF_OF_REST,
    GY_BODY_ATTR_DAMPING
} GY_BODY_ATTRIB_FIELD;


class GYBodyAttrCreateParams;

GYBodyAttr gyBodyAttrCreate(GYBodyAttrCreateParams* createParams);
void gyBodyAttrDestroy(GYBodyAttr attr);

int gyBodyAttrFieldSetf(GYBody bodyAttr, GY_BODY_FIELD field, GYReal_t value);
int gyBodyAttrFieldSeti(GYBody bodyAttr, GY_BODY_FIELD field, int value);
int gyBodyAttrFieldSetPtr(GYBody bodyAttr, GY_BODY_FIELD field, void* ptr);

int gyBodyAttrFieldGetf(GYBody bodyAttr, GY_BODY_FIELD field, GYReal_t* value);
int gyBodyAttrFieldGeti(GYBody bodyAttr, GY_BODY_FIELD field, int* value);
int gyBodyAttrFieldGetPtr(GYBody bodyAttr, GY_BODY_FIELD field, void** ptr);










int gySimulationBroadPhaseQuery(GYSimulation* sim, GYBody* body, int* count, GYBody* bodyList);







#ifdef __cplusplus
}
#endif


#endif // GYRO_PHYSICS_SIMULATION_H
