#ifndef ELYSIAN_GUI_SYSTEM_HPP
#define ELYSIAN_GUI_SYSTEM_HPP

#include "elysian_system.hpp"
#include "elysian_font.hpp"
#include "elysian_adventure_log.hpp"

#include <gyro.h>

#define ELYSIAN_FONT_IMAGE_PATH_VERDANA		"font/verdana.png"
#define ELYSIAN_FONT_WIDTH_PATH_VERDANA		"font/verdana_width.txt"
#define ELYSIAN_FONT_IMAGE_PATH_TWCEN		"font/twcen.png"
#define ELYSIAN_FONT_WIDTH_PATH_TWCEN		"font/twcen_width.txt"

namespace elysian {

	class GuiSystem: public System { //tolua_export
	private:

		unsigned int _projWidth, _projHeight;
	public:

		GuiSystem(void);
		bool loadAssets(void);
		void update(void);
		//void Render(void);
		void renderOverlay(void);
		inline void getResolution(unsigned int &width, unsigned int &height) const { width = _projWidth; height = _projHeight; }
//tolua_begin
		AdventureLog adventureLog;
		Font verdana;
		Font twcen;

	};
//tolua_end
	
}

#endif