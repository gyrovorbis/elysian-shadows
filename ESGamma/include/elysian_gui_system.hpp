#ifndef ELYSIAN_GUI_SYSTEM_HPP
#define ELYSIAN_GUI_SYSTEM_HPP

#include "elysian_system.hpp"
#include "elysian_font.hpp"
#include "elysian_adventure_log.hpp"
#include "elysian_mem_pool.hpp"

#include <gyro.h>

#define ELYSIAN_FONT_IMAGE_PATH_VERDANA             "font/verdana.png"
#define ELYSIAN_FONT_WIDTH_PATH_VERDANA             "font/verdana_width.txt"
#define ELYSIAN_FONT_IMAGE_PATH_TWCEN               "font/twcen.png"
#define ELYSIAN_FONT_WIDTH_PATH_TWCEN               "font/twcen_width.txt"

#define ELYSIAN_GUI_SYSTEM_COMBAT_TEXT_MAX_CHARS    50
#define ELYSIAN_GUI_SYSTEM_COMBAT_TEXT_POOL_SIZE    30
#define ELYSIAN_GUI_SYSTEM_COMBAT_TEXT_LIFE         1.0f
#define ELYSIAN_GUI_SYSTEM_COMBAT_TEXT_GRAVITY		GYVector2 { 0.0f, 0.4f }

namespace elysian {


    class GuiSystem: public System {
	private:

        struct CombatText {
            char        text[ELYSIAN_GUI_SYSTEM_COMBAT_TEXT_MAX_CHARS];
            GYColor4    color;
            GYVector2   pos;
            GYVector2   vel;
            float       timer;
		};

		Pool<CombatText>		_combatText;
		static const GYVector2	_combatTextGravity;
		bool					_debugOverlayEnabled			= false;

		unsigned int _projWidth, _projHeight;


		void _renderDebugOverlay(void);
	public:

		GuiSystem(void);

		bool loadAssets(void);
		void update(void);
		//void Render(void);
		void renderOverlay(void);
		void render(void);
		inline void getResolution(unsigned int &width, unsigned int &height) const { width = _projWidth; height = _projHeight; }

		void addCombatText(const char*const text, const GYColor4 color, const GYVector2& pos, const GYVector2& vel);

		void setDebugOverLayEnabled(const bool enabled);

		AdventureLog	adventureLog;
		Font			verdana;
		Font			twcen;

	};



	// ====== INLINEZ ======

	inline void GuiSystem::setDebugOverLayEnabled(const bool enabled) { _debugOverlayEnabled = enabled; }

	
}

#endif
