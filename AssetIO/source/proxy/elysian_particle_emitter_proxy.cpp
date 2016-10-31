#include <proxy/elysian_entity_proxy.hpp>

OOLUA_EXPORT_FUNCTIONS(ParticleEmitterAttrib,
                       setAcceleration,
                       setMaxLife,
                       setForceMask,
                       setIntensityFactor,
                       setStartSize,
                       setEndSize,
                       setEndColor,
                       setAnimGridSize,
                       setAnimGridIndex,
                       setAnimFrameDuration,
                       setStick,
                       setRandomStartFrame,
                       setShadowVolume,
                       getEndColor,
                       getForceMask)
OOLUA_EXPORT_FUNCTIONS_CONST(ParticleEmitterAttrib,
                       getShadowVolume,
                       getRandomStartFrame,
                       getStick,
                       getAnimFrameDuration,
                       getEndSize,
                       getStartSize,
                       getIntensityFactor,
                       getMaxLife)

OOLUA_EXPORT_FUNCTIONS(ParticleEmitter,
                       emitFromPoint,
                       emitFromPointInDir,
                       emitFromEntity,
                       setAttrib,
                       allocatePool)
OOLUA_EXPORT_FUNCTIONS_CONST(ParticleEmitter,
                             getAttrib)
