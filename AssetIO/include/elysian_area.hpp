#ifndef ELYSIAN_AREA_HPP
#define ELYSIAN_AREA_HPP

#include <vector>
#include <stdint.h>
#include <gyro.h>
#include "elysian_asset.hpp"
#include "elysian_mem_pool.hpp"
#include "elysian_asset_typedefs.hpp"
#include ELYSIAN_ASSET_HEADER("elysian_level.hpp", "level.h")

#define ELYSIAN_MAP_BOUNDARY_CHECKS false //PLEASE, GOD, TAKE ME OUT BEFORE RELEASE!!!!!!
#define ELYSIAN_AREA_USE_INIT_LUA	true

#define ELYSIAN_AREA_MAP_LAYERS		3
#define ELYSIAN_AREA_MAP_WIDTH		200
#define ELYSIAN_AREA_MAP_HEIGHT		200
#define ELYSIAN_AREA_NAME_SIZE		50
#define ELYSIAN_AREA_STRING_SIZE	50
#define ELYSIAN_AREA_PATH_SIZE		200
#define ELYSIAN_AREA_MAP_PATH		"/map.txt"
#define ELYSIAN_AREA_ENTITY_PATH	"/entity.txt"
#define ELYSIAN_AREA_INIT_LUA_PATH	"/area_init.lua"
#define ELYSIAN_MAP_ENTRY_ELEV_MAX	31

//Common MapEntry properties
#define ELYSIAN_MAP_ENTRY_ORIENT_BIT_POS	9
#define ELYSIAN_MAP_ENTRY_ORIENT_BIT_MASK	0x0007
#define ELYSIAN_MAP_ENTRY_ELEV_BIT_POS		12
#define ELYSIAN_MAP_ENTRY_ELEV_BIT_MASK		0x001f

//TileEntry properties
#define ELYSIAN_TILE_ENTRY_INDEX_BIT_POS	0
#define ELYSIAN_TILE_ENTRY_INDEX_BIT_MASK	0x01ff
#define ELYSIAN_TILE_ENTRY_ZTRAN_BIT_POS	31
#define ELYSIAN_TILE_ENTRY_ZTRAN_BIT_MASK	0x0001
#define ELYSIAN_TILE_ENTRY_PLANE_BIT_POS	17
#define ELYSIAN_TILE_ENTRY_PLANE_BIT_MASK	0x000f

#define ELYSIAN_MAP_ENTRY_GET_ATTRIB(t ,a)		((value >> ELYSIAN_##t##_ENTRY_##a##_BIT_POS) & ELYSIAN_##t##_ENTRY_##a##_BIT_MASK)
#define ELYSIAN_MAP_ENTRY_SET_ATTRIB(t, a, v)	do { \
													value &= ~(ELYSIAN_##t##_ENTRY_##a##_BIT_MASK << ELYSIAN_##t##_ENTRY_##a##_BIT_POS); \
													(value) |= ((unsigned)(v) << ELYSIAN_##t##_ENTRY_##a##_BIT_POS); \
												} while(0)

#define ELYSIAN_MAP_ENTRY_TO_VBO_INDEX(l, x, y) ((l*height*width + y*width + x)*4)

#ifdef ELYSIAN_MAP_BOUNDARY_CHECKS
#	define ELYSIAN_AREA_PROTECTED_MAP_ACCESS(t) ((t < mapEntryCount)? tiles[t] : tiles[0])
#	define ELYSIAN_AREA_PROTECTED_MAP_LAYER_ACCESS(l, t) ((t < layerEntryCount)? l[t] : l[0])
#else
#	define ELYSIAN_AREA_PROTECTED_MAP_ACCESS(t) tiles[t]
#	define ELYSIAN_AREA_PROTECTED_MAP_LAYER_ACCESS(l, t) l[t]
#endif

ELYSIAN_ASSET_TYPEDEF(Area)
ELYSIAN_ASSET_TYPEDEF(Level)
ELYSIAN_ASSET_TYPEDEF(Entity)

namespace elysian {

	class Renderable;
    class Animation;
    class Collidable;
	class RigidBody;
    class Character;
	class Warp;
	class Examinable;
	class AudioSource;
	class LightSource;
	class ParticleEmitter;
	class Combat;
	class Behavior;

	struct MapEntry {
		uint32_t value;
		MapEntry(const uint32_t val);
		MapEntry(void);

		uint32_t			getValue(void) const;
		void				setValue(const uint32_t val);
		GY_RECT_TEX_ORIENT	getOrient(void) const;
		unsigned char		getElev(void) const;
		void				setOrient(const GY_RECT_TEX_ORIENT o);
		void				setElev(const unsigned char e);
	};

	struct TileEntry: public MapEntry {
		TileEntry(void);
		TileEntry(const uint32_t val);

		void					setIndex(const uint16_t i);
		uint16_t				getIndex(void) const;
		void					setZTran(const bool z);
		bool					getZTran(void) const;
		void					setPlane(const GY_RECT_PLANE_TYPE n);
		GY_RECT_PLANE_TYPE		getPlane(void) const;
		bool					getNormal(void) const;
		void					setNormal(const bool val);
	};

	struct VboEntry {
		GYVector4	vert;
		GYTexCoord2	texCoord;
		GYColor4	color;
	};

    struct AreaBase: public Asset {
		//Entity data
		std::vector<Entity*>			entities;

		//Component data
		std::vector<Renderable*>		renderables;
		std::vector<Animation*>			animations;
		std::vector<Collidable*>		collidables;
		std::vector<RigidBody*>			rigidBodies;
		std::vector<Character*>			characters;
		std::vector<Warp*>				warps;
		std::vector<Examinable*>		examinables;
		std::vector<AudioSource*>		audioSources;
		std::vector<LightSource*>		lightSources;
		std::vector<ParticleEmitter*>	particleEmitters;
		std::vector<Combat*>			combats;
		std::vector<Behavior*>			behaviors;
		
        char dir[ELYSIAN_AREA_NAME_SIZE];		//name of area directory
        char path[ELYSIAN_AREA_PATH_SIZE];
		char string[ELYSIAN_AREA_STRING_SIZE];	//in-game area name

		//map - tile layers
		//pointer to entire map (contiguous block of memory)
		TileEntry*			tiles			= nullptr;
		//pointer to individual tile layers (faster indexing)
		TileEntry*			tileLayer2		= nullptr;
		TileEntry*			tileLayer3		= nullptr;

        Level*              level			= nullptr;

		unsigned short int	width			= 0;
		unsigned short int	height			= 0;
		unsigned			mapEntryCount	= 0;
		unsigned			layerEntryCount	= 0;

        GLuint              vboBuff         = 0;
        bool                vboPopulated    = false;

        //AreaBase(void);
        ~AreaBase(void);

        unsigned short  getMapWidth(void) const { return width; }
        unsigned short  getMapHeight(void) const { return height; }
        const char*     getDirName(void) const { return dir; }
        const char*     getPath(void) const { return path; }

		//Slower than layer-specific accessors, but loop-friendly
		TileEntry	getTileEntry(const unsigned l, const unsigned x, const unsigned y) const;
		void		setTileEntry(const unsigned l, const unsigned x, const unsigned y, const TileEntry e);
        bool        isValidTileEntry(const unsigned x, const unsigned y) const;
		//Faster lookups
#if 0
		TileEntry	getL1TileEntry(const unsigned x, const unsigned y) const;
		void		setL1TileEntry(const unsigned x, const unsigned y, const TileEntry e);
		TileEntry	getL2TileEntry(const unsigned x, const unsigned y) const;
		void		setL2TileEntry(const unsigned x, const unsigned y, const TileEntry e);
		TileEntry	getL3TileEntry(const unsigned x, const unsigned y) const;
		void		setL3TileEntry(const unsigned x, const unsigned y, const TileEntry e);
#endif
		//entity management
		void		addEntity(Entity *entity);
		void		removeEntity(Entity *entity);
		Entity*		findEntity(const char* name);
		
		void		allocMap(void); //width + height better be set first!
		void		freeMap(void);
		void		resizeMap(const unsigned w, const unsigned h);

        void        setLevel(Level*const lev);
        Level*      getLevel(void) const;
        bool		load(const char *const dir);
		bool		save(const char *const dir);

		bool		loadMap(const char *const filename);
		bool		saveMap(const char *const filename);
		bool		loadEntities(const char *const filename);
		bool		saveEntities(const char *const filename);


		void		populateVbo(void);
        void        setVboEntry(const TileEntry*const tileEntry, const unsigned l, const unsigned x, const unsigned y) const;
        void        vboEntryFromTileEntry(VboEntry*const vboEntry, const TileEntry *const tileEntry, const unsigned x, const unsigned y) const;
        void        animateVboTileEntry(const unsigned l, const unsigned x, const unsigned y, const uint16_t index);

	};

	// ==== INLINEZ ====

	inline MapEntry::MapEntry(void): value(0) {}
	inline MapEntry::MapEntry(const uint32_t val): value(val) {}
	inline uint32_t MapEntry::getValue(void) const { return value; }
	inline void MapEntry::setValue(const uint32_t val) { value = val; }
	inline GY_RECT_TEX_ORIENT MapEntry::getOrient(void) const { return (GY_RECT_TEX_ORIENT)ELYSIAN_MAP_ENTRY_GET_ATTRIB(MAP, ORIENT); }
	inline void MapEntry::setOrient(const GY_RECT_TEX_ORIENT o) { ELYSIAN_MAP_ENTRY_SET_ATTRIB(MAP, ORIENT, o); }
	inline unsigned char MapEntry::getElev(void) const { return ELYSIAN_MAP_ENTRY_GET_ATTRIB(MAP, ELEV); }
	inline void MapEntry::setElev(const unsigned char e) { ELYSIAN_MAP_ENTRY_SET_ATTRIB(MAP, ELEV, e); }

	inline TileEntry::TileEntry(void) {}
	inline TileEntry::TileEntry(const uint32_t val): MapEntry(val) {}
	inline uint16_t TileEntry::getIndex(void) const { return ELYSIAN_MAP_ENTRY_GET_ATTRIB(TILE, INDEX); }
	inline void TileEntry::setIndex(const uint16_t i) { ELYSIAN_MAP_ENTRY_SET_ATTRIB(TILE, INDEX, i); }
	inline bool TileEntry::getZTran(void) const { return ELYSIAN_MAP_ENTRY_GET_ATTRIB(TILE, ZTRAN); }
	inline void TileEntry::setZTran(const bool z) { ELYSIAN_MAP_ENTRY_SET_ATTRIB(TILE, ZTRAN, z); }
	inline GY_RECT_PLANE_TYPE TileEntry::getPlane(void) const { return (GY_RECT_PLANE_TYPE)ELYSIAN_MAP_ENTRY_GET_ATTRIB(TILE, PLANE); }
	inline void TileEntry::setPlane(const GY_RECT_PLANE_TYPE n) { ELYSIAN_MAP_ENTRY_SET_ATTRIB(TILE, PLANE, n); }
    inline bool TileEntry::getNormal(void) const {
        GY_RECT_PLANE_TYPE plane = (GY_RECT_PLANE_TYPE)ELYSIAN_MAP_ENTRY_GET_ATTRIB(TILE, PLANE);

        switch(plane) {
            case GY_RECT_PLANE_FRONT: return true;
            default: return false;
        }

    }
	inline void TileEntry::setNormal(const bool v) { ELYSIAN_MAP_ENTRY_SET_ATTRIB(TILE, PLANE, v); }

    inline TileEntry AreaBase::getTileEntry(const unsigned l, const unsigned x, const unsigned y) const {
		return ELYSIAN_AREA_PROTECTED_MAP_ACCESS(l*height*width + y*width + x);
	}


    inline void AreaBase::vboEntryFromTileEntry(VboEntry * const vboEntry, const TileEntry * const tileEntry, const unsigned x, const unsigned y) const {

        const GYTexCoord2*const coords = gyVidTexCoordArrayFromFrame(&level->tileSheet->_coord->texCoord[tileEntry->getIndex()],
                                                                           tileEntry->getOrient());

        switch(tileEntry->getPlane()) {

        case GY_RECT_PLANE_FRONT:
            vboEntry[0] = {
                GYVector4  { x*32.0f-16.0f, y*32.0f + 16.0f +tileEntry->getElev()*32.0f, tileEntry->getElev()*32.0f + 32.0f, 1.0f },
                GYTexCoord2 { coords[0].u, coords[0].v },
                GYColor4 { 255, 255, 255, 255 }
            };
            vboEntry[1] = {
                GYVector4  { x*32.0f+16.0f, y*32.0f + 16.0f + tileEntry->getElev()*32.0f, tileEntry->getElev()*32.0f + 32.0f, 1.0f },
                GYTexCoord2 { coords[1].u, coords[1].v },
                GYColor4 { 255, 255, 255, 255 }
            };
            vboEntry[2] = {
                GYVector4 { x*32.0f-16.0f, y*32.0f+ 16.0f + tileEntry->getElev()*32.0f, tileEntry->getElev()*32.0f, 1.0f },
                GYTexCoord2 { coords[2].u, coords[2].v },
                GYColor4 { 255, 255, 255, 255 }
            };
            vboEntry[3] = {
                GYVector4 { x*32.0f+16.0f, y*32.0f+ 16.0f +  tileEntry->getElev()*32.0f, tileEntry->getElev()*32.0f, 1.0f },
                GYTexCoord2 { coords[3].u, coords[3].v },
                GYColor4 { 255, 255, 255, 255 }
            };


            break;

        case GY_RECT_PLANE_LEFT:
            vboEntry[0] = {
                GYVector4  { x*32.0f-16.0f, y*32.0f - 16.0f +tileEntry->getElev()*32.0f, tileEntry->getElev()*32.0f + 32.0f, 1.0f },
                GYTexCoord2 { coords[0].u, coords[0].v },
                GYColor4 { 255, 255, 255, 255 }
            };
            vboEntry[1] = {
                GYVector4  { x*32.0f-16.0f, y*32.0f + 16.0f + tileEntry->getElev()*32.0f, tileEntry->getElev()*32.0f + 32.0f, 1.0f },
                GYTexCoord2 { coords[1].u, coords[1].v },
                GYColor4 { 255, 255, 255, 255 }
            };
            vboEntry[2] = {
                GYVector4 { x*32.0f-16.0f, y*32.0f- 16.0f + tileEntry->getElev()*32.0f, tileEntry->getElev()*32.0f, 1.0f },
                GYTexCoord2 { coords[2].u, coords[2].v },
                GYColor4 { 255, 255, 255, 255 }
            };
            vboEntry[3] = {
                GYVector4 { x*32.0f-16.0f, y*32.0f+ 16.0f +  tileEntry->getElev()*32.0f, tileEntry->getElev()*32.0f, 1.0f },
                GYTexCoord2 { coords[3].u, coords[3].v },
                GYColor4 { 255, 255, 255, 255 }
            };


            break;

        case GY_RECT_PLANE_RIGHT:

            vboEntry[0] = {
                GYVector4  { x*32.0f+16.0f, y*32.0f - 16.0f +tileEntry->getElev()*32.0f, tileEntry->getElev()*32.0f + 32.0f, 1.0f },
                GYTexCoord2 { coords[0].u, coords[0].v },
                GYColor4 { 255, 255, 255, 255 }
            };
            vboEntry[1] = {
                GYVector4  { x*32.0f+16.0f, y*32.0f + 16.0f + tileEntry->getElev()*32.0f, tileEntry->getElev()*32.0f + 32.0f, 1.0f },
                GYTexCoord2 { coords[1].u, coords[1].v },
                GYColor4 { 255, 255, 255, 255 }
            };
            vboEntry[2] = {
                GYVector4 { x*32.0f+16.0f, y*32.0f- 16.0f + tileEntry->getElev()*32.0f, tileEntry->getElev()*32.0f, 1.0f },
                GYTexCoord2 { coords[2].u, coords[2].v },
                GYColor4 { 255, 255, 255, 255 }
            };
            vboEntry[3] = {
                GYVector4 { x*32.0f+16.0f, y*32.0f+ 16.0f +  tileEntry->getElev()*32.0f, tileEntry->getElev()*32.0f, 1.0f },
                GYTexCoord2 { coords[3].u, coords[3].v },
                GYColor4 { 255, 255, 255, 255 }
            };


            break;

        case GY_RECT_PLANE_FRONT_SLOPE_45:
            vboEntry[0] = {
				GYVector4  { x*32.0f-16.0f, (y)*32.0f - 16.0f, tileEntry->getElev()*32.0f + 32.0f, 1.0f },
                GYTexCoord2 { coords[0].u, coords[0].v },
                GYColor4 { 255, 255, 255, 255 }
            };
            vboEntry[1] = {
				GYVector4  { x*32.0f+16.0f, y*32.0f - 16.0f , tileEntry->getElev()*32.0f + 32.0f, 1.0f },
                GYTexCoord2 { coords[1].u, coords[1].v },
                GYColor4 { 255, 255, 255, 255 }
            };
            vboEntry[2] = {
				GYVector4 { x*32.0f-16.0f, y*32.0f+ 16.0f, tileEntry->getElev()*32.0f, 1.0f },
                GYTexCoord2 { coords[2].u, coords[2].v },
                GYColor4 { 255, 255, 255, 255 }
            };
            vboEntry[3] = {
				GYVector4 { x*32.0f+16.0f, y*32.0f+ 16.0f, tileEntry->getElev()*32.0f, 1.0f },
                GYTexCoord2 { coords[3].u, coords[3].v },
                GYColor4 { 255, 255, 255, 255 }
            };


            break;

        case GY_RECT_PLANE_BOTTOM:
        default:

            vboEntry[0] = {
                GYVector4  { x*32.0f-16.0f, y*32.0f - 16.0f +tileEntry->getElev()*32.0f, tileEntry->getElev()*32.0f, 1.0f },
                GYTexCoord2 { coords[0].u, coords[0].v },
                GYColor4 { 255, 255, 255, 255 }
            };
            vboEntry[1] = {
                GYVector4  { x*32.0f+16.0f, y*32.0f - 16.0f + tileEntry->getElev()*32.0f, tileEntry->getElev()*32.0f, 1.0f },
                GYTexCoord2 { coords[1].u, coords[1].v },
                GYColor4 { 255, 255, 255, 255 }
            };
            vboEntry[2] = {
                GYVector4 { x*32.0f-16.0f, y*32.0f+ 16.0f + tileEntry->getElev()*32.0f, tileEntry->getElev()*32.0f, 1.0f },
                GYTexCoord2 { coords[2].u, coords[2].v },
                GYColor4 { 255, 255, 255, 255 }
            };
            vboEntry[3] = {
                GYVector4 { x*32.0f+16.0f, y*32.0f+ 16.0f +  tileEntry->getElev()*32.0f, tileEntry->getElev()*32.0f, 1.0f },
                GYTexCoord2 { coords[3].u, coords[3].v },
                GYColor4 { 255, 255, 255, 255 }
            };

            break;

        }
    }

    inline void AreaBase::animateVboTileEntry(const unsigned l, const unsigned x, const unsigned y, const uint16_t index) {
        TileEntry e = getTileEntry(l, x, y);
        const uint16_t oldIndex = e.getIndex();
        e.setIndex(index);
        setTileEntry(l, x, y, e);
        ELYSIAN_AREA_PROTECTED_MAP_ACCESS(l*height*width + y*width + x).setIndex(oldIndex);
    }

    inline void AreaBase::setTileEntry(const unsigned l, const unsigned x, const unsigned y, const TileEntry e) {
        VboEntry entry[4];

        vboEntryFromTileEntry(entry, &e, x, y);

		ELYSIAN_AREA_PROTECTED_MAP_ACCESS(l*height*width + y*width + x) = e;

		glBindBuffer(GL_ARRAY_BUFFER, vboBuff);
		glBufferSubData(GL_ARRAY_BUFFER, ELYSIAN_MAP_ENTRY_TO_VBO_INDEX(l, x, y)*sizeof(VboEntry), sizeof(VboEntry)*4, (void*)entry);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}

#if 0
    inline TileEntry AreaBase::getL1TileEntry(const unsigned x, const unsigned y) const {
		return ELYSIAN_AREA_PROTECTED_MAP_LAYER_ACCESS(tiles, y*width + x);
	}
    inline void AreaBase::setL1TileEntry(const unsigned x, const unsigned y, const TileEntry e) {
		ELYSIAN_AREA_PROTECTED_MAP_LAYER_ACCESS(tiles, y*width + x) = e;
	}
    inline TileEntry AreaBase::getL2TileEntry(const unsigned x, const unsigned y) const {
		return ELYSIAN_AREA_PROTECTED_MAP_LAYER_ACCESS(tileLayer2, y*width + x);
	}
    inline void AreaBase::setL2TileEntry(const unsigned x, const unsigned y, const TileEntry e) {
		ELYSIAN_AREA_PROTECTED_MAP_LAYER_ACCESS(tileLayer2, y*width + x) = e;
	}
    inline TileEntry AreaBase::getL3TileEntry(const unsigned x, const unsigned y) const {
		return ELYSIAN_AREA_PROTECTED_MAP_LAYER_ACCESS(tileLayer3, y*width + x);
	}
    inline void AreaBase::setL3TileEntry(const unsigned x, const unsigned y, const TileEntry e) {
		ELYSIAN_AREA_PROTECTED_MAP_LAYER_ACCESS(tileLayer3, y*width + x) = e;
	}
#endif

    inline void AreaBase::setLevel(Level * const lev) { level = lev; }

    inline Level* AreaBase::getLevel(void) const { return level; }

    inline void AreaBase::setVboEntry(const TileEntry*const tileEntry, const unsigned l, const unsigned x, const unsigned y) const {
        VboEntry entry[4];

        vboEntryFromTileEntry(entry, tileEntry, x, y);

        //ELYSIAN_AREA_PROTECTED_MAP_ACCESS(l*height*width + y*width + x) = e;

        glBindBuffer(GL_ARRAY_BUFFER, vboBuff);
        glBufferSubData(GL_ARRAY_BUFFER, ELYSIAN_MAP_ENTRY_TO_VBO_INDEX(l, x, y)*sizeof(VboEntry), sizeof(VboEntry)*4, (void*)entry);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

    }

     inline bool AreaBase::isValidTileEntry(const unsigned x, const unsigned y) const { return (x < width && y < height); }

}

#endif
