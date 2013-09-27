#ifndef ELYSIAN_AREA_HPP
#define ELYSIAN_AREA_HPP

#include <vector>
#include <stdint.h>
#include <gyro.h>
#include "elysian_asset.hpp"

#define ELYSIAN_MAP_BOUNDARY_CHECKS true //am I ever even using this? NO, LULZ!!!
#define ELYSIAN_AREA_USE_INIT_LUA	true

#define ELYSIAN_AREA_MAP_LAYERS		3
#define ELYSIAN_AREA_MAP_WIDTH		200
#define ELYSIAN_AREA_MAP_HEIGHT		200
#define ELYSIAN_AREA_NAME_SIZE		50
#define ELYSIAN_AREA_STRING_SIZE	50
#define ELYSIAN_AREA_PATH_SIZE		200
#define ELYSIAN_AREA_MAP_PATH		"/map.txt"
#define ELYSIAN_AREA_ENTITY_PATH	"/entity.txt"
#define ELYSIAN_AREA_INIT_LUA_PATH	"/init.lua"

//Common MapEntry properties
#define ELYSIAN_MAP_ENTRY_ORIENT_BIT_POS	9
#define ELYSIAN_MAP_ENTRY_ORIENT_BIT_MASK	0x0007
#define ELYSIAN_MAP_ENTRY_ELEV_BIT_POS		12
#define ELYSIAN_MAP_ENTRY_ELEV_BIT_MASK		0x000f

//TileEntry properties
#define ELYSIAN_TILE_ENTRY_INDEX_BIT_POS	0
#define ELYSIAN_TILE_ENTRY_INDEX_BIT_MASK	0x01ff
#define ELYSIAN_TILE_ENTRY_ZTRAN_BIT_POS	31
#define ELYSIAN_TILE_ENTRY_ZTRAN_BIT_MASK	0x0001

//LightEntry properties
#define ELYSIAN_LIGHT_ENTRY_INDEX_BIT_POS	0
#define ELYSIAN_LIGHT_ENTRY_INDEX_BIT_MASK	0x00ff
#define ELYSIAN_LIGHT_ENTRY_PAL_BIT_POS		28
#define ELYSIAN_LIGHT_ENTRY_PAL_BIT_MASK	0x0007


#define ELYSIAN_MAP_ENTRY_GET_ATTRIB(t ,a)		((value >> ELYSIAN_##t##_ENTRY_##a##_BIT_POS) & ELYSIAN_##t##_ENTRY_##a##_BIT_MASK)
#define ELYSIAN_MAP_ENTRY_SET_ATTRIB(t, a, v)	do { \
													value &= ~(ELYSIAN_##t##_ENTRY_##a##_BIT_MASK << ELYSIAN_##t##_ENTRY_##a##_BIT_POS); \
													(value) |= ((unsigned)(v) << ELYSIAN_##t##_ENTRY_##a##_BIT_POS); \
												} while(0)


namespace elysian {

    struct Level;
    class Entity;
	class Renderable;
    class Animation;
    class Collidable;
    class Character;
	class Warp;
	class Item;
	class Friendly;
	class Enemy;
	class Examinable;
	class AudioSource;
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

		void		setIndex(const uint16_t i);
		uint16_t	getIndex(void) const;
		void		setZTran(const bool z);
		bool		getZTran(void) const;
	};

	struct LightEntry: public MapEntry {
		LightEntry(void);
		LightEntry(const uint32_t val);

		void			setIndex(const unsigned char i);
		unsigned char	getIndex(void) const;
		void			setPal(const unsigned char p);
		unsigned char	getPal(void) const;
	};

    struct Area: public Asset {
		//Entity data
		std::vector<Entity*>		entities;

		//Component data
		std::vector<Renderable*>	renderables;
		std::vector<Animation*>		animations;
		std::vector<Collidable*>	collidables;
		std::vector<Character*>		characters;
		std::vector<Warp*>			warps;
		std::vector<Item*>			items;
		std::vector<Friendly*>		friendlies;
		std::vector<Enemy*>			enemies;
		std::vector<Examinable*>	examinables;
		std::vector<AudioSource*>	audioSources;
		std::vector<Behavior*>		behaviors;
		
		char name[ELYSIAN_AREA_NAME_SIZE];		//name of area directory
		char path[ELYSIAN_AREA_PATH_SIZE];
		char string[ELYSIAN_AREA_STRING_SIZE];	//in-game area name

		//map - tile layers
		//pointer to entire map (contiguous block of memory)
		TileEntry *tiles;
		//pointer to individual tile layers (faster indexing)
		TileEntry *tileLayer2;
		TileEntry *tileLayer3;
		//map - light layer
		LightEntry *lights;

        const Level* level;

		unsigned short int width, height;

		Area(void);
        ~Area(void);

		unsigned short getMapWidth(void) const { return width; }
		unsigned short getMapHeight(void) const { return height; }
		const char* getName(void) const { return name; }
		const char* getPath(void) const { return path; }

		//Slower than layer-specific accessors, but loop-friendly
		TileEntry	getTileEntry(const unsigned l, const unsigned x, const unsigned y) const;
		void		setTileEntry(const unsigned l, const unsigned x, const unsigned y, const TileEntry e);
		//Faster lookups
		TileEntry	getL1TileEntry(const unsigned x, const unsigned y) const;
		void		setL1TileEntry(const unsigned x, const unsigned y, const TileEntry e);
		TileEntry	getL2TileEntry(const unsigned x, const unsigned y) const;
		void		setL2TileEntry(const unsigned x, const unsigned y, const TileEntry e);
		TileEntry	getL3TileEntry(const unsigned x, const unsigned y) const;
		void		setL3TileEntry(const unsigned x, const unsigned y, const TileEntry e);

		LightEntry	getLightEntry(const unsigned x, const unsigned y) const;
		void		setLightEntry(const unsigned x, const unsigned y, const LightEntry l);

		//entity management
		void		addEntity(Entity *entity);
		void		removeEntity(Entity *entity);
		Entity*		findEntity(const char* name);
		
		void		allocMap(void); //width + height better be set first!
		void		freeMap(void);
		void		resizeMap(const unsigned w, const unsigned h);

		bool		load(const char *const dir, const Level *lev=NULL);
		bool		save(const char *const dir);

		bool		loadMap(const char *const filename);
		bool		saveMap(const char *const filename);
		bool		loadEntities(const char *const filename);
		bool		saveEntities(const char *const filename);

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
	inline void TileEntry::setIndex(const uint16_t i) {	ELYSIAN_MAP_ENTRY_SET_ATTRIB(TILE, INDEX, i); }
	inline bool TileEntry::getZTran(void) const { return ELYSIAN_MAP_ENTRY_GET_ATTRIB(TILE, ZTRAN); }
	inline void TileEntry::setZTran(const bool z) { ELYSIAN_MAP_ENTRY_SET_ATTRIB(TILE, ZTRAN, z); }

	inline LightEntry::LightEntry(void) {}
	inline LightEntry::LightEntry(const uint32_t v): MapEntry(v) {}
	inline unsigned char LightEntry::getIndex(void) const { return ELYSIAN_MAP_ENTRY_GET_ATTRIB(LIGHT, INDEX); }
	inline void LightEntry::setIndex(const unsigned char i) { ELYSIAN_MAP_ENTRY_SET_ATTRIB(LIGHT, INDEX, i); }
	inline unsigned char LightEntry::getPal(void) const { return ELYSIAN_MAP_ENTRY_GET_ATTRIB(LIGHT, PAL); }
	inline void LightEntry::setPal(const unsigned char p) { ELYSIAN_MAP_ENTRY_SET_ATTRIB(LIGHT, PAL, p); }


	inline TileEntry Area::getTileEntry(const unsigned l, const unsigned x, const unsigned y) const {
		return tiles[l*height*width + y*width + x];
	}
	inline void Area::setTileEntry(const unsigned l, const unsigned x, const unsigned y, const TileEntry e) {
		tiles[l*height*width + y*width + x] = e;
	}
	inline TileEntry Area::getL1TileEntry(const unsigned x, const unsigned y) const {
		return tiles[y*width + x];
	}
	inline void Area::setL1TileEntry(const unsigned x, const unsigned y, const TileEntry e) {
		tiles[y*width + x] = e;
	}
	inline TileEntry Area::getL2TileEntry(const unsigned x, const unsigned y) const {
		return tileLayer2[y*width + x];
	}
	inline void Area::setL2TileEntry(const unsigned x, const unsigned y, const TileEntry e) {
		tileLayer2[y*width + x] = e;
	}
	inline TileEntry Area::getL3TileEntry(const unsigned x, const unsigned y) const {
		return tileLayer3[y*width + x];
	}
	inline void Area::setL3TileEntry(const unsigned x, const unsigned y, const TileEntry e) {
		tileLayer3[y*width + x] = e;
	}
	inline LightEntry Area::getLightEntry(const unsigned x, const unsigned y) const {
		return lights[y*width + x];
	}
	inline void Area::setLightEntry(const unsigned x, const unsigned y, const LightEntry l) {
		lights[y*width + x] = l;
	}

}

#endif
