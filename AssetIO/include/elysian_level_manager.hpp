#ifndef ELYSIAN_LEVEL_MANAGER_HPP
#define ELYSIAN_LEVEL_MANAGER_HPP

namespace elysian {

    struct Area;
	struct Level;

	class LevelManager { 
	protected:
		static Level *_curLevel;
		static Area  *_curArea;
	public:
		static Area* getCurArea(void) { return _curArea; }
		static Level* getCurLevel(void) { return _curLevel; }
	}; 
	
}

#endif

