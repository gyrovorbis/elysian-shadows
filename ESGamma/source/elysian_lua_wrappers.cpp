#include "elysian_lua_wrappers.hpp"
#include "elysian_lua_manager.hpp"

#include "elysian_entity.hpp"

namespace elysian {

	void LuaManager::_registerWrappers(void) {
		
		//Register classes and structs
		_mainState->register_class<LuaBehaviorScriptAttrib>();
		_mainState->register_class<GYVector2>();
		_mainState->register_class<InstanceTransform>();
		OOLUA::register_class_and_bases<Entity>(*_mainState);
		OOLUA::register_class_and_bases<SpriteAttrib>(*_mainState);
		_mainState->register_class<Component>();
		_mainState->register_class<Renderable>();
		_mainState->register_class<AnimationSequence>();
		_mainState->register_class<AnimationAttrib>();
		_mainState->register_class<Animation>();
		_mainState->register_class<CollisionFlags>();
		_mainState->register_class<GYStatic>();
		_mainState->register_class<CollidableAttrib>();
		_mainState->register_class<Collidable>();
		_mainState->register_class<Character>();
		_mainState->register_class<Warp>();
		OOLUA::register_class_and_bases<LuaBehavior>(*_mainState);
		_mainState->register_class<Tile>();
		_mainState->register_class<Level>();
		OOLUA::register_class_and_bases<TileEntry>(*_mainState);
		_mainState->register_class<Area>();
		_mainState->register_class<GameInput>();
		_mainState->register_class<Singletons>();
		_mainState->register_class<gyro>();
		_mainState->register_class<GYWav>();
		_mainState->register_class<AudioSource>();
		_mainState->register_class<AudioSystem>();

		//Register C and static member functions
		lua_register(*_mainState, "print", &print);
		_mainState->register_class_static<Singletons>("getCurArea", &Singletons_getCurArea);
		_mainState->register_class_static<Singletons>("getCurLevel", &Singletons_getCurLevel);
		_mainState->register_class_static<Singletons>("getGameInput", &Singletons_getGameInput);
		_mainState->register_class_static<Singletons>("setProcessInput", &Singletons_setProcessInput);
		_mainState->register_class_static<Singletons>("getPartySys", &Singletons_getPartySystem);
		_mainState->register_class_static<Singletons>("getTimerSys", &Singletons_getTimerSystem);
		_mainState->register_class_static<Singletons>("getAudioSys", &Singletons_getAudioSystem);

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

EXPORT_OOLUA_FUNCTIONS_2_CONST(GYVector2, get_x, get_y)
EXPORT_OOLUA_FUNCTIONS_2_NON_CONST(GYVector2, set_x, set_y)

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
						activate,
						deactivate,
						move,
						setName,
						setElev,
						addRenderable,
						addAnimation,
						addCollidable,
						addCharacter,
						addWarp,
						addAudioSource,
						addBehavior,
						addLuaBehavior,
						removeRenderable,
						removeAnimation,
						removeCollidable,
						removeCharacter,
						removeWarp,
						removeAudioSource,
						removeBehavior,
						removeLuaBehavior);
OOLUA_EXPORT_FUNCTIONS_CONST(Entity,
								getName,
								getElev,
								getRenderable,
								getAnimation,
								getCollidable,
								getCharacter,
								getWarp,
								getAudioSource,
								getBehavior,
								getLuaBehavior)

EXPORT_OOLUA_FUNCTIONS_2_NON_CONST(RectAttrib,
								   setColor,
								   setAlpha)
EXPORT_OOLUA_FUNCTIONS_2_CONST(RectAttrib,
							   getColor,
							   getAlpha)

EXPORT_OOLUA_FUNCTIONS_2_NON_CONST(TexturedAttrib,
								   loadTexture,
								   loadSheet)
EXPORT_OOLUA_FUNCTIONS_0_CONST(TexturedAttrib)

EXPORT_OOLUA_FUNCTIONS_1_NON_CONST(SpriteAttrib,
								   setFrame)
EXPORT_OOLUA_FUNCTIONS_0_CONST(SpriteAttrib)

EXPORT_OOLUA_FUNCTIONS_3_NON_CONST(Renderable,
								   setFrame,
								   setOffset,
								   setSpriteAttrib)
EXPORT_OOLUA_FUNCTIONS_3_CONST(Renderable,
							   getFrame,
							   getOffset,
							   getSpriteAttrib)

EXPORT_OOLUA_FUNCTIONS_1_NON_CONST(AnimationSequence,
								   addFrame)
EXPORT_OOLUA_FUNCTIONS_0_CONST(AnimationSequence)

EXPORT_OOLUA_FUNCTIONS_1_NON_CONST(AnimationAttrib,
								   addSequence)
EXPORT_OOLUA_FUNCTIONS_0_CONST(AnimationAttrib)

EXPORT_OOLUA_FUNCTIONS_3_NON_CONST(Animation,
								   setAttrib,
								   playAnimation,
								   stopAnimation)
EXPORT_OOLUA_FUNCTIONS_1_CONST(Animation,
							   getAttrib)

EXPORT_OOLUA_FUNCTIONS_4_NON_CONST(CollisionFlags,
								   resetType,
								   resetCheckAgainst,
								   addType,
								   addCheckAgainst)
EXPORT_OOLUA_FUNCTIONS_0_CONST(CollisionFlags)


EXPORT_OOLUA_FUNCTIONS_5_CONST(GYStatic,
								   get_inverseMass,
								   get_coeffOfRest,
								   get_damping,
								   get_invMomOfInertia,
								   get_angDamping)
EXPORT_OOLUA_FUNCTIONS_5_NON_CONST(GYStatic,
							   set_inverseMass,
							   set_coeffOfRest,
							   set_damping,
							   set_invMomOfInertia,
							   set_angDamping)


EXPORT_OOLUA_FUNCTIONS_5_NON_CONST(CollidableAttrib,
								   setColliderType,
								   setRelPos,
								   setRelSize,
								   getFlags,
								   getStaticAttrib)
EXPORT_OOLUA_FUNCTIONS_1_CONST(CollidableAttrib,
							   getColliderType)

EXPORT_OOLUA_FUNCTIONS_2_NON_CONST(Collidable,
								   setAttrib,
								   setResolvePenetration)
EXPORT_OOLUA_FUNCTIONS_2_CONST(Collidable,
							   getAttrib,
							   getContactCount)

EXPORT_OOLUA_FUNCTIONS_2_NON_CONST(Character,
								   face,
								   move)
EXPORT_OOLUA_FUNCTIONS_0_CONST(Character)

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

EXPORT_OOLUA_FUNCTIONS_2_NON_CONST(Behavior,
								   addTrigger,
								   removeTrigger)
EXPORT_OOLUA_FUNCTIONS_1_CONST(Behavior,
								getBehavName)

EXPORT_OOLUA_FUNCTIONS_0_NON_CONST(LuaBehavior)//,
								  // getDataTable)
EXPORT_OOLUA_FUNCTIONS_0_CONST(LuaBehavior)

EXPORT_OOLUA_FUNCTIONS_1_NON_CONST(Tile,
									setColliderType)
EXPORT_OOLUA_FUNCTIONS_0_CONST(Tile)

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

EXPORT_OOLUA_FUNCTIONS_3_CONST(TimerSystem,
								getTotalTicks,
								getCurrentFrameTicks,
								getDeltaTime)
EXPORT_OOLUA_FUNCTIONS_0_NON_CONST(TimerSystem)


EXPORT_OOLUA_FUNCTIONS_2_NON_CONST(PartySystem,
									addCharacterToParty,
									removeCharacterFromParty)
EXPORT_OOLUA_FUNCTIONS_2_CONST(PartySystem,
								getPartyLeader,
								getCharacterPartyMember)

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

EXPORT_OOLUA_FUNCTIONS_0_NON_CONST(Singletons)
EXPORT_OOLUA_FUNCTIONS_0_CONST(Singletons)

EXPORT_OOLUA_FUNCTIONS_0_NON_CONST(gyro)
EXPORT_OOLUA_FUNCTIONS_0_CONST(gyro)
