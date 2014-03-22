#include "elysian_lua_wrappers.hpp"
#include "elysian_lua_manager.hpp"

#include "elysian_entity.hpp"

namespace elysian {

	void LuaManager::_registerWrappers(void) {

		//Register classes and structs
		_mainState->register_class<LuaBehaviorScriptAttrib>();
		_mainState->register_class<GYVector2>();
		_mainState->register_class<GYVector3>();
		_mainState->register_class<GYVector4>();
        _mainState->register_class<GYColor4>();
		_mainState->register_class<InstanceTransform>();
		OOLUA::register_class<Entity>(*_mainState);
		OOLUA::register_class<QuadSpriteAttrib>(*_mainState);
		_mainState->register_class<Component>();
		_mainState->register_class<Material>();
		_mainState->register_class<Renderable>();
		_mainState->register_class<AnimationSequence>();
		_mainState->register_class<AnimationAttrib>();
        _mainState->register_class<LightAttrib>();
		_mainState->register_class<Animation>();
		_mainState->register_class<CollisionFlags>();
		_mainState->register_class<GYStatic>();
		_mainState->register_class<CollidableAttrib>();
		_mainState->register_class<CollisionContact>();
		_mainState->register_class<Collidable>();
		_mainState->register_class<Character>();
		_mainState->register_class<Warp>();
		_mainState->register_class<DialogScreen>();
		_mainState->register_class<DialogText>();
		_mainState->register_class<DialogPrompt>();
		_mainState->register_class<Dialog>();
		_mainState->register_class<Examinable>();
		_mainState->register_class<ParticleEmitterAttrib>();
		_mainState->register_class<ParticleEmitter>();
		OOLUA::register_class<LuaBehavior>(*_mainState);
		_mainState->register_class<Tile>();
		_mainState->register_class<Level>();
		OOLUA::register_class<TileEntry>(*_mainState);
		_mainState->register_class<Area>();
		_mainState->register_class<GameInput>();
		_mainState->register_class<PartySystem>();
		_mainState->register_class<TimerSystem>();
		_mainState->register_class<Singletons>();
		_mainState->register_class<gyro>();
		_mainState->register_class<GYWav>();
		_mainState->register_class<AudioSource>();
		_mainState->register_class<LightSource>();
		_mainState->register_class<RigidBodyAttrib>();
		_mainState->register_class<RigidBody>();
		_mainState->register_class<AudioSystem>();
		_mainState->register_class<LightSystem>();
		_mainState->register_class<ParticleSystem>();
		_mainState->register_class<DaySystem>();
		_mainState->register_class<DialogSystem>();
		_mainState->register_class<RigidBodySystem>();
		_mainState->register_class<GuiSystem>();
		_mainState->register_class<LevelSystem>();
		_mainState->register_class<Direction>();
		OOLUA::register_class<GravityForceGenerator>(*_mainState);


		//Register C and static member functions
		lua_register(*_mainState, "print", &print);
		_mainState->register_class_static<Singletons>("getCurArea", &Singletons_getCurArea);
		_mainState->register_class_static<Singletons>("getCurLevel", &Singletons_getCurLevel);
		_mainState->register_class_static<Singletons>("getGameInput", &Singletons_getGameInput);
		_mainState->register_class_static<Singletons>("setProcessInput", &Singletons_setProcessInput);
		_mainState->register_class_static<Singletons>("getPartySys", &Singletons_getPartySystem);
		_mainState->register_class_static<Singletons>("getTimerSys", &Singletons_getTimerSystem);
		_mainState->register_class_static<Singletons>("getAudioSys", &Singletons_getAudioSystem);
		_mainState->register_class_static<Singletons>("getLightSys", &Singletons_getLightSystem);
		_mainState->register_class_static<Singletons>("getLevelSys", &Singletons_getLevelSystem);
		_mainState->register_class_static<Singletons>("getPartSys", &Singletons_getParticleSystem);
		_mainState->register_class_static<Singletons>("getDaySys", &Singletons_getDaySystem);
		_mainState->register_class_static<Singletons>("getRigidBodySys", &Singletons_getRigidBodySystem);
		_mainState->register_class_static<Singletons>("getDialogSys", &Singletons_getDialogSystem);
		_mainState->register_class_static<Singletons>("getGuiSys", &Singletons_getGuiSystem);

		//libGyro wrappers
		_mainState->register_class_static<gyro>("filePath", &gyroFilePath);
		_mainState->register_class_static<gyro>("createWav", &gyroWavCreate);
		_mainState->register_class_static<gyro>("destroyWav", &gyroWavDestroy);

		//Register constants and enumerations
		luaRegisterEnumsConstants(_mainState);
	}

}

OOLUA_EXPORT_FUNCTIONS(LuaBehaviorScriptAttrib,
					   inheritFromBehavior,
					   addSupportedTrigger)
OOLUA_EXPORT_FUNCTIONS_CONST(LuaBehaviorScriptAttrib)

EXPORT_OOLUA_FUNCTIONS_2_CONST(GYVector2, getX, getY)
EXPORT_OOLUA_FUNCTIONS_2_NON_CONST(GYVector2, setX, setY)


OOLUA_EXPORT_FUNCTIONS(GYVector3, setX, setY, setZ)
OOLUA_EXPORT_FUNCTIONS_CONST(GYVector3, getX, getY, getZ)

OOLUA_EXPORT_FUNCTIONS(GYVector4, setX, setY, setZ, setW)
OOLUA_EXPORT_FUNCTIONS_CONST(GYVector4, getX, getY, getZ, getW)


OOLUA_EXPORT_FUNCTIONS(GYColor4, setA, setR, setG, setB)
OOLUA_EXPORT_FUNCTIONS_CONST(GYColor4, getA, getR, getG, getB)

EXPORT_OOLUA_FUNCTIONS_0_CONST(GYWav)
EXPORT_OOLUA_FUNCTIONS_0_NON_CONST(GYWav)

EXPORT_OOLUA_FUNCTIONS_9_NON_CONST(InstanceTransform,
									setPos2f,
									setPos3f,
									translate2f,
									translate3f,
									setSize2f,
									setScale2f,
									setOrientZ,
									rotate,
									scale2f)
EXPORT_OOLUA_FUNCTIONS_6_CONST(InstanceTransform,
								apply,
								getPos2f,
								getPos3f,
								getSize2f,
								getScale2f,
								getOrientZ)

EXPORT_OOLUA_FUNCTIONS_1_NON_CONST(RenderableObject,
									getTransform)
EXPORT_OOLUA_FUNCTIONS_0_CONST(RenderableObject)

EXPORT_OOLUA_FUNCTIONS_0_NON_CONST(Component)
EXPORT_OOLUA_FUNCTIONS_1_CONST(Component, getEntity)

OOLUA_EXPORT_FUNCTIONS(Entity,
						move,
						setName,
						setElev,
						addRenderable,
						addAnimation,
						addCollidable,
						addCharacter,
						addWarp,
						addAudioSource,
						addLightSource,
						addExaminable,
						addRigidBody,
						addBehavior,
						addLuaBehavior,
						removeRenderable,
						removeAnimation,
						removeCollidable,
						removeCharacter,
						removeWarp,
						removeAudioSource,
						removeLightSource,
						removeExaminable,
						removeRigidBody,
						removeBehavior,
						removeLuaBehavior,
						setDepthType,
						setSelfTable,
						getSelfTable)
OOLUA_EXPORT_FUNCTIONS_CONST(Entity,
								getName,
								getElev,
								getRenderable,
								getAnimation,
								getCollidable,
								getCharacter,
								getWarp,
								getAudioSource,
								getLightSource,
								getExaminable,
								getRigidBody,
								getBehavior,
								getLuaBehavior,
								getDepthType)

EXPORT_OOLUA_FUNCTIONS_0_NON_CONST(PrimAttrib)
EXPORT_OOLUA_FUNCTIONS_0_CONST(PrimAttrib)

EXPORT_OOLUA_FUNCTIONS_6_NON_CONST(SpriteAttrib,
								   setFrame,
								   setColor,
								   setAlpha,
								   loadTexture,
								   loadSheet,
								   setTexCoord)
EXPORT_OOLUA_FUNCTIONS_2_CONST(SpriteAttrib,
							   getColor,
							   getAlpha)

EXPORT_OOLUA_FUNCTIONS_1_CONST(QuadSpriteAttrib,
							   getVertexColor)
EXPORT_OOLUA_FUNCTIONS_2_NON_CONST(QuadSpriteAttrib,
								   setVertexColor,
								   setVerticesAlpha)
OOLUA_EXPORT_FUNCTIONS(Material,
					   setShininess,
					   setAmbientColor,
					   setDiffuseColor,
					   setSpecularColor,
					   setEmissiveColor)
OOLUA_EXPORT_FUNCTIONS_CONST(Material)

OOLUA_EXPORT_FUNCTIONS(Renderable,
					   setFrame,
					   setOffset,
					   setSpriteAttrib,
					   loadNormalMap,
					   loadSpecularMap,
					   setMaterial)
OOLUA_EXPORT_FUNCTIONS_CONST(Renderable,
						   getFrame,
						   getOffset,
						   getSpriteAttrib,
						   getQuadSpriteAttrib,
						   getMaterial)

EXPORT_OOLUA_FUNCTIONS_1_NON_CONST(AnimationSequence,
								   addFrame)
EXPORT_OOLUA_FUNCTIONS_0_CONST(AnimationSequence)

EXPORT_OOLUA_FUNCTIONS_1_NON_CONST(AnimationAttrib,
								   addSequence)
EXPORT_OOLUA_FUNCTIONS_0_CONST(AnimationAttrib)

EXPORT_OOLUA_FUNCTIONS_5_NON_CONST(Animation,
								   setAttrib,
								   playAnimation,
								   stopAnimation,
								   setLooping,
								   setCurrentFrame)
EXPORT_OOLUA_FUNCTIONS_3_CONST(Animation,
							   getAttrib,
							   isLooping,
							   getCurrentFrame)

EXPORT_OOLUA_FUNCTIONS_4_NON_CONST(CollisionFlags,
								   resetType,
								   resetCheckAgainst,
								   addType,
								   addCheckAgainst)
EXPORT_OOLUA_FUNCTIONS_0_CONST(CollisionFlags)


OOLUA_EXPORT_FUNCTIONS_CONST(GYStatic,
							getInverseMass,
							getCoeffOfRest,
							getDamping,
							getInvMomOfInertia,
							getAngDamping)
OOLUA_EXPORT_FUNCTIONS(GYStatic,
						setInverseMass,
						setCoeffOfRest,
						setDamping,
						setInvMomOfInertia,
						setAngDamping)


EXPORT_OOLUA_FUNCTIONS_5_NON_CONST(CollidableAttrib,
								   setColliderType,
								   setRelPos,
								   setRelSize,
								   getFlags,
								   getStaticAttrib)
EXPORT_OOLUA_FUNCTIONS_1_CONST(CollidableAttrib,
							   getColliderType)

OOLUA_EXPORT_FUNCTIONS_CONST(CollisionContact,
							 getCollidable,
							 getWorldPos,
							 getNormal,
							 getObjPos,
							 getSeparatingVel,
							 getClosingVel,
							 getDeltaVel,
							 getDepth)
OOLUA_EXPORT_FUNCTIONS(CollisionContact)

EXPORT_OOLUA_FUNCTIONS_2_NON_CONST(Collidable,
								   setAttrib,
								   setResolvePenetration)
EXPORT_OOLUA_FUNCTIONS_2_CONST(Collidable,
							   getAttrib,
							   getContactCount)

OOLUA_EXPORT_FUNCTIONS_CONST(RigidBodyAttrib,
							 getForceMask)
OOLUA_EXPORT_FUNCTIONS(RigidBodyAttrib,
					   setForceMask)

OOLUA_EXPORT_FUNCTIONS_CONST(RigidBody)
OOLUA_EXPORT_FUNCTIONS(RigidBody,
					   applyForce,
					   applyForceToPoint,
					   setAttrib)

OOLUA_EXPORT_FUNCTIONS_CONST(ForceGenerator)
OOLUA_EXPORT_FUNCTIONS(ForceGenerator,
					   setForceMask)

OOLUA_EXPORT_FUNCTIONS_CONST(GravityForceGenerator)
OOLUA_EXPORT_FUNCTIONS(GravityForceGenerator,
					   setAccel)

EXPORT_OOLUA_FUNCTIONS_0_NON_CONST(Direction)
EXPORT_OOLUA_FUNCTIONS_0_CONST(Direction)

EXPORT_OOLUA_FUNCTIONS_3_NON_CONST(Character,
								   faceEntity,
								   faceDir,
								   move)
EXPORT_OOLUA_FUNCTIONS_2_CONST(Character,
							   getFaceDir,
							   isFacing)

EXPORT_OOLUA_FUNCTIONS_5_NON_CONST(Warp,
								   setToPos,
								   setAreaPath,
								   setLevelPath,
								   setChangeArea,
								   setChangeLevel)
EXPORT_OOLUA_FUNCTIONS_0_CONST(Warp)

 EXPORT_OOLUA_FUNCTIONS_7_NON_CONST(AudioSource,
									 setWav,
									 playWav,
									 play,
									 stop,
									 setGain,
									 setPitch,
									 setLooping)
EXPORT_OOLUA_FUNCTIONS_0_CONST(AudioSource)

OOLUA_EXPORT_FUNCTIONS(LightAttrib,
					   setAmbientColor,
					   setDiffuseColor,
					   setSpecularColor,
					   setAttenuationFactors,
					   setSpotExponent)
OOLUA_EXPORT_FUNCTIONS_CONST(LightAttrib)

OOLUA_EXPORT_FUNCTIONS(LightSource,
					   setLightType,
					   setDirection,
					   setSpotCutoffAngle,
					   setAttrib,
					   getAttrib,
					   setShadowCaster)
OOLUA_EXPORT_FUNCTIONS_CONST(LightSource)

OOLUA_EXPORT_FUNCTIONS(ParticleEmitterAttrib,
					   setAcceleration,
					   setMaxLife,
					   setSize,
					   setFrameRange,
					   loadSpecularMap,
					   setMaterial)
OOLUA_EXPORT_FUNCTIONS_CONST(ParticleEmitterAttrib);

OOLUA_EXPORT_FUNCTIONS(ParticleEmitter,
					   emitFromPoint,
					   emitFromEntity,
					   setAttrib)
OOLUA_EXPORT_FUNCTIONS_CONST(ParticleEmitter)

OOLUA_EXPORT_FUNCTIONS(DialogScreen,
					   addSourceEntity,
					   setTrigger)
OOLUA_EXPORT_FUNCTIONS_CONST(DialogScreen,
							 getText,
							 getIndex)

OOLUA_EXPORT_FUNCTIONS(DialogText,
					   setNextIndex)
OOLUA_EXPORT_FUNCTIONS_CONST(DialogText,
							 getNextIndex)

OOLUA_EXPORT_FUNCTIONS(DialogPrompt,
					   addOption)
OOLUA_EXPORT_FUNCTIONS_CONST(DialogPrompt)

OOLUA_EXPORT_FUNCTIONS(Dialog,
					   addTextScreen,
					   addPromptScreen,
					   setTriggerEntity)
OOLUA_EXPORT_FUNCTIONS_CONST(Dialog)

OOLUA_EXPORT_FUNCTIONS(Examinable,
					   setDialog,
					   setMaxDist)
OOLUA_EXPORT_FUNCTIONS_CONST(Examinable)

EXPORT_OOLUA_FUNCTIONS_2_NON_CONST(Behavior,
								   addTrigger,
								   removeTrigger)
EXPORT_OOLUA_FUNCTIONS_1_CONST(Behavior,
								getBehavName)

EXPORT_OOLUA_FUNCTIONS_1_NON_CONST(LuaBehavior,
								   getSelfTable)
EXPORT_OOLUA_FUNCTIONS_0_CONST(LuaBehavior)

OOLUA_EXPORT_FUNCTIONS(Tile,
						setColliderType,
						setDamping)
EXPORT_OOLUA_FUNCTIONS_1_CONST(Tile,
                               getColliderType)

EXPORT_OOLUA_FUNCTIONS_1_NON_CONST(Level,
									getTile)
EXPORT_OOLUA_FUNCTIONS_3_CONST(Level,
								getTileAmount,
								getName,
								getPath)

EXPORT_OOLUA_FUNCTIONS_3_NON_CONST(MapEntry,
									setValue,
									setOrient,
									setElev)
EXPORT_OOLUA_FUNCTIONS_3_CONST(MapEntry,
								getValue,
								getOrient,
								getElev)

EXPORT_OOLUA_FUNCTIONS_2_NON_CONST(TileEntry,
									setIndex,
									setZTran)
EXPORT_OOLUA_FUNCTIONS_2_CONST(TileEntry,
								getIndex,
								getZTran)

EXPORT_OOLUA_FUNCTIONS_5_CONST(Area,
									getName,
									getPath,
									getTileEntry,
									getMapWidth,
									getMapHeight)
EXPORT_OOLUA_FUNCTIONS_4_NON_CONST(Area,
								setTileEntry,
								addEntity,
								removeEntity,
								findEntity)

EXPORT_OOLUA_FUNCTIONS_2_NON_CONST(GameInput,
									setUp,
									setDown)
EXPORT_OOLUA_FUNCTIONS_2_CONST(GameInput,
								getDown,
								getTapped)

OOLUA_EXPORT_FUNCTIONS_CONST(TimerSystem,
								getTotalTicks,
								getCurrentFrameTicks,
								getDeltaTime,
								getTotalTime)
OOLUA_EXPORT_FUNCTIONS(TimerSystem)


OOLUA_EXPORT_FUNCTIONS(PartySystem,
						addCharacterToParty,
						removeCharacterFromParty)
OOLUA_EXPORT_FUNCTIONS_CONST(PartySystem,
								getPartyLeader,
								getCharacterPartyMember)

OOLUA_EXPORT_FUNCTIONS(LevelSystem)
OOLUA_EXPORT_FUNCTIONS_CONST(LevelSystem,
								getPrevAreaPath,
								getPrevLevelPath)

OOLUA_EXPORT_FUNCTIONS(RigidBodySystem,
					   addForce,
					   removeForce,
					   clearForces)
OOLUA_EXPORT_FUNCTIONS_CONST(RigidBodySystem)

OOLUA_EXPORT_FUNCTIONS(LightSystem,
					   setGlobalAmbientColor)
OOLUA_EXPORT_FUNCTIONS_CONST(LightSystem)

OOLUA_EXPORT_FUNCTIONS(AudioSystem,
					   loadBgmTrack,
					   setBgmTrack,
					   fadeOutBgmTrack,
					   fadeInBgmTrack,
					   morphIntoBgmTrack,
					   setBgmTrackLooping,
					   setBgmTrackGain,
					   fadeIntoBgmTrack,
					   morphIntoNewBgmTrack,
					   fadeIntoNewBgmTrack,
					   loadAuxTrack,
					   fadeOutAuxTrack,
					   fadeInAuxTrack,
					   morphIntoNewAuxTrack,
					   fadeIntoNewAuxTrack,
					   setAuxTrackLooping,
					   setAuxTrackGain)


OOLUA_EXPORT_FUNCTIONS_CONST(AudioSystem,
							 playSfxChannel,
							 stopSfxChannel,
							 setSfxChannelGain,
							 setSfxChannelPitch,
							 setSfxChannelLooping,
							 setSfxChannelRolloffFactor,
							 setSfxChannelPos,
							 playBgmTrack,
							 stopBgmTrack,
							 pauseBgmTrack,
							 isBgmTrackPlaying,
							 getBgmTrack,
							 getBgmTrackPath,
							 setBgmTrackPitch,
							 setBgmTrackPos,
							 setBgmTrackRolloffFactor,
							 playAuxTrack,
							 stopAuxTrack,
							 pauseAuxTrack,
							 isAuxTrackPlaying,
							 getAuxTrackPath,
							 setAuxTrackPitch,
							 setAuxTrackPos,
							 setAuxTrackRolloffFactor,
							 getBgmTrackTime,
							 getAuxTrackTime)

OOLUA_EXPORT_FUNCTIONS(ParticleSystem,
					   emitFromPoint)
OOLUA_EXPORT_FUNCTIONS_CONST(ParticleSystem,
							 getAttrib)

OOLUA_EXPORT_FUNCTIONS(GuiSystem,
					   addCombatText)
OOLUA_EXPORT_FUNCTIONS_CONST(GuiSystem)

OOLUA_EXPORT_FUNCTIONS_CONST(DaySystem,
							 getDays,
							 getHours,
							 getMinutes,
							 getSunLightAttrib)
OOLUA_EXPORT_FUNCTIONS(DaySystem,
					   setTimeDilation,
					   setSunEnabled)

OOLUA_EXPORT_FUNCTIONS(DialogSystem,
					   startDialog)
OOLUA_EXPORT_FUNCTIONS_CONST(DialogSystem)

EXPORT_OOLUA_FUNCTIONS_0_NON_CONST(Singletons)
EXPORT_OOLUA_FUNCTIONS_0_CONST(Singletons)

EXPORT_OOLUA_FUNCTIONS_0_NON_CONST(gyro)
EXPORT_OOLUA_FUNCTIONS_0_CONST(gyro)
