#ifndef ELYSIAN_GUI_SYSTEM_HPP
#define ELYSIAN_GUI_SYSTEM_HPP

#include "elysian_system.hpp"
#include "elysian_font.hpp"
#include "elysian_adventure_log.hpp"
#include "elysian_mem_pool.hpp"
#include "elysian_asset_typedefs.hpp"
#include "elysian_entity.hpp"

#include <gyro.h>

#define ASSETS_VROOT                                "/assets"

#define ELYSIAN_FONT_IMAGE_PATH_VERDANA             ASSETS_VROOT "/font/verdana.png"
#define ELYSIAN_FONT_WIDTH_PATH_VERDANA             ASSETS_VROOT "/font/verdana_width.txt"
#define ELYSIAN_FONT_IMAGE_PATH_TWCEN               ASSETS_VROOT "/font/twcen.png"
#define ELYSIAN_FONT_WIDTH_PATH_TWCEN               ASSETS_VROOT "/font/twcen_width.txt"
#define ELYSIAN_FONT_IMAGE_PATH_PRAGMATA18          ASSETS_VROOT "/font/pragmata18.png"
#define ELYSIAN_FONT_WIDTH_PATH_PRAGMATA18          ASSETS_VROOT "/font/pragmata18_width.txt"
#define ELYSIAN_FONT_IMAGE_PATH_PRAGMATA22          ASSETS_VROOT "/font/pragmata22.png"
#define ELYSIAN_FONT_WIDTH_PATH_PRAGMATA22          ASSETS_VROOT "/font/pragmata22_width.txt"
#define ELYSIAN_FONT_IMAGE_PATH_PRAGMATA26          ASSETS_VROOT "/font/pragmata26.png"
#define ELYSIAN_FONT_WIDTH_PATH_PRAGMATA26          ASSETS_VROOT "/font/pragmata26_width.txt"

#define ELYSIAN_GUI_SYSTEM_COMBAT_TEXT_MAX_CHARS    50
#define ELYSIAN_GUI_SYSTEM_COMBAT_TEXT_POOL_SIZE    30
#define ELYSIAN_GUI_SYSTEM_COMBAT_TEXT_LIFE         1.0f
#define ELYSIAN_GUI_SYSTEM_COMBAT_TEXT_GRAVITY		GYVector2 { 0.0f, 0.4f }
#define ELYSIAN_GUI_SYSTEM_EXAMINE_TEXT_OFFSET      0.0f

ELYSIAN_ASSET_TYPEDEF(Entity)

namespace elysian {

	class GuiSystem;

	class MenuWidget {
	public:
		virtual			~MenuWidget(void)		= default;
		virtual void	recursiveRender(void)	= 0;
	};

	class MenuText: public MenuWidget {
		friend class GuiSystem;
	private:
        static Font*	_fonts[5];
		char			_text[100];
		GYVector2		_relPos		= { 0.0f, 0.0f };
		unsigned		_type		= 0;
		GYColor4		_color		= { 255, 255, 255, 255 };
		Font::ALIGNMENT _align		= Font::L_ALIGN;
	public:
        virtual         ~MenuText(void)	override = default;

        void            setRelPos(const float x, const float y);
        void            setFontType(const unsigned type);
        void            setColor(const GYColor4 color);
        void            setAlignment(const Font::ALIGNMENT align);
        void            setText(const char*const text);

        virtual void    recursiveRender(void) override;
	};

	class MenuEntity: public MenuWidget, public Entity {
	public:
		enum SIZE_TYPE { SIZE_RELATIVE, SIZE_ABSOLUTE };
	private:
        Pool<MenuWidget*> _childWidgets = Pool<MenuWidget*>(45);
        //Pool<MenuWidget*> _childWidgets = Pool<MenuWidget*>(10);
        //SIZE_TYPE _sizeType				= SIZE_ABSOLUTE;
        SIZE_TYPE _sizeType				= SIZE_RELATIVE;

	public:
                        MenuEntity(void);
        virtual         ~MenuEntity(void);

        void            addChildWidget(MenuWidget*const widget);
        void            removeChildWidget(MenuWidget*const widget);
        void            setSizeType(const SIZE_TYPE type);
        virtual void    recursiveRender(void) override;

	};


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
        bool					_debugOverlayEnabled	= false;

        unsigned int            _projWidth;
        unsigned int            _projHeight;

        MenuWidget*             _fgMenuWidget           = nullptr;
        MenuWidget*             _bgMenuWidget           = nullptr;
        const Entity*           _examineEntity          = nullptr;

		void _printDebugLine(const char*title, const char*str, ...);
		void _renderDebugOverlay(void);
        void _renderCombatText(void);
        void _renderExamineEntity(void);
		void _renderMenuWidget(MenuWidget* widget);
	public:

		GuiSystem(void);

		void setClearColor(const GYColor4 color);

		bool loadAssets(void);
		void update(void);
		//void Render(void);
		void renderBackground(void);
		void renderForeground(void);
		void render(void);
		inline void getResolution(unsigned int &width, unsigned int &height) const { width = _projWidth; height = _projHeight; }

		void addCombatText(const char*const text, const GYColor4 color, const GYVector2& pos, const GYVector2& vel);

		void setDebugOverLayEnabled(const bool enabled);

		void setFgMenuWidget(MenuWidget*const widget);
		void clearFgMenuWidget(void);

		void setBgMenuWidget(MenuWidget*const widget);
		void clearBgMenuWidget(void);

        void setAdvLogEnabled(const bool val);

        void            setExamineEntity(const Entity* const entity);
        const Entity*   getExamineEntity(void) const;

        Font			verdana;
        Font			twcen;
        Font            pragmata18;
        Font            pragmata22;
        Font            pragmata26;
		AdventureLog	adventureLog;

	};



	// ====== INLINEZ ======

	inline void MenuText::setRelPos(const float x, const float y) { _relPos.x = x; _relPos.y = y; }
	inline void MenuText::setFontType(const unsigned type) { _type = type; }
	inline void MenuText::setColor(const GYColor4 color) { _color = color; }
	inline void MenuText::setAlignment(const Font::ALIGNMENT align) { _align = align; }
	inline void MenuText::setText(const char * const text) { strcpy(_text, text); }

    inline MenuEntity::MenuEntity(void) { setPlaneType(GY_RECT_PLANE_BOTTOM); }
	inline void MenuEntity::addChildWidget(MenuWidget*const widget) { _childWidgets.claimFreeEntry() = widget; }
	inline void MenuEntity::setSizeType(const SIZE_TYPE type) { _sizeType = type; }
	inline void MenuEntity::removeChildWidget(MenuWidget*const widget) { _childWidgets.removeEntry(widget); }

    inline void GuiSystem::setExamineEntity(const Entity * const entity) { _examineEntity = entity; }
    inline const Entity* GuiSystem::getExamineEntity(void) const { return _examineEntity; }
	inline void GuiSystem::setDebugOverLayEnabled(const bool enabled) { _debugOverlayEnabled = enabled; }
	inline void GuiSystem::setFgMenuWidget(MenuWidget * const widget) { _fgMenuWidget = widget; }
	inline void GuiSystem::clearFgMenuWidget(void) { _fgMenuWidget = nullptr; }
	inline void GuiSystem::setBgMenuWidget(MenuWidget * const widget) { _bgMenuWidget = widget; }
	inline void GuiSystem::clearBgMenuWidget(void) { _bgMenuWidget = nullptr; }

	inline void GuiSystem::setClearColor(const GYColor4 color) { gyVidClearColor(color); }

    inline void GuiSystem::setAdvLogEnabled(const bool val) {
        adventureLog.setEnabled(val);
    }


}

#endif
