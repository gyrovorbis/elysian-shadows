#ifndef ELYSIAN_GUI_SYSTEM_HPP
#define ELYSIAN_GUI_SYSTEM_HPP

#include "elysian_system.hpp"
#include "elysian_font.hpp"
#include "elysian_adventure_log.hpp"
#include "elysian_mem_pool.hpp"
#include "elysian_asset_typedefs.hpp"
#include "elysian_entity.hpp"

#include <gyro_video_api.h>

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

#define ELYSIAN_GUI_SYSTEM_COMBAT_TEXT_MAX_CHARS    20
#define ELYSIAN_GUI_SYSTEM_COMBAT_TEXT_POOL_SIZE    30
#define ELYSIAN_GUI_SYSTEM_COMBAT_TEXT_LIFE         1.0f
#define ELYSIAN_GUI_SYSTEM_COMBAT_TEXT_GRAVITY		GYVector2 { 0.0f, 0.1f }

#define ELYSIAN_GUI_SYSTEM_VICTORY_TEXT_MAX_CHARS   50

#define ELYSIAN_GUI_SYSTEM_EXAMINE_TEXT_OFFSET      0.0f

#define ELYSIAN_GUI_SYSTEM_EXP_TEXT_BOUNCE_TIME     0.3f
#define ELYSIAN_GUI_SYSTEM_EXP_TEXT_STICK_TIME      3.0f
#define ELYSIAN_GUI_SYSTEM_EXP_TEXT_FADE_TIME       0.3f

ELYSIAN_ASSET_TYPEDEF(Entity)

namespace elysian {

	class GuiSystem;

	class MenuWidget {
	public:
		virtual			~MenuWidget(void)		= default;
        virtual void	recursiveRender(const float alpha)	= 0;
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

        virtual void    recursiveRender(const float alpha) override;
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
        virtual void    recursiveRender(const float alpha) override;

	};


    class GuiSystem: public System {
	private:

        class CombatStatusIndicator {
        private:
            Sprite          _healthMeter[2];
            const Entity*   _entity = nullptr;
        public:
                            CombatStatusIndicator(void);
            const Entity*   getEntity(void) const;
            void            setEntity(const Entity*const entity);
            void            update(void);
            void            render(void);
        };

        struct CombatText {
            char        text[ELYSIAN_GUI_SYSTEM_COMBAT_TEXT_MAX_CHARS];
            GYColor4    color;
            GYVector2   pos;
            GYVector2   vel;
            float       timer;
		};

        struct VictoryText {
            enum STATE { STATE_INACTIVE, STATE_BOUNCE, STATE_STICK, STATE_FADE, STATE_MAX };
            constexpr const static float stateTime[STATE_MAX] = {
                0.0f,
                ELYSIAN_GUI_SYSTEM_EXP_TEXT_BOUNCE_TIME,
                ELYSIAN_GUI_SYSTEM_EXP_TEXT_STICK_TIME,
                ELYSIAN_GUI_SYSTEM_EXP_TEXT_FADE_TIME
            };

            char        string[ELYSIAN_GUI_SYSTEM_VICTORY_TEXT_MAX_CHARS];
            GYVector2   pos;
            float       timer     = 0.0f;
            STATE       state     = STATE_INACTIVE;
        };

        enum FADE_STATE { FADE_ENABLED, FADE_OUT, FADE_DISABLED, FADE_IN };

        Pool<CombatStatusIndicator> _combatStatIndicator;
        Pool<CombatText>            _combatText;
        Pool<VictoryText>           _victoryText;
        static const GYVector2      _combatTextGravity;
        bool                        _debugOverlayEnabled	= false;

        unsigned int                _projWidth;
        unsigned int                _projHeight;

        MenuWidget*                 _fgMenuWidget           = nullptr;
        MenuWidget*                 _bgMenuWidget           = nullptr;
        const Entity*               _examineEntity          = nullptr;


        FADE_STATE                  _fgFadeState            = FADE_ENABLED;
        float                       _fgFadeElapsed          = 0.0f;
        float                       _fgFadeTime             = 0.0f;
        float                       _fgFadeAlpha            = 1.0f;
        float                       _fgFadeAlphaStart       = 1.0f;

        void            _printDebugLine(const char*title, const char*str, ...);
        void            _renderDebugOverlay(void);
        void            _renderCombatText(void);
        void            _renderExamineEntity(void);
        void            _renderMenuWidget(MenuWidget* widget);
	public:

                        GuiSystem(void);

        void            setClearColor(const GYColor4 color);

        bool            loadAssets(void);
        void            update(void);
        void            renderBackground(void);
        void            renderForeground(void);
        void            render(void);
        void            getResolution(unsigned int &width, unsigned int &height) const;
        void            addCombatText(const char*const text, const GYColor4 color, const GYVector2& pos, const GYVector2& vel);
        void            setDebugOverLayEnabled(const bool enabled);
        void            setFgMenuWidget(MenuWidget*const widget);
        void            clearFgMenuWidget(void);
        void            setBgMenuWidget(MenuWidget*const widget);
        void            clearBgMenuWidget(void);
        void            setAdvLogEnabled(const bool val);
        void            setExamineEntity(const Entity* const entity);
        const Entity*   getExamineEntity(void) const;

        void            addCombatStatsEntity(const Entity*const entity);
        void            removeCombatStatsEntity(const Entity*const entity);

        void            addVictoryText(const GYVector2& pos, const char*const string);

        GYColor4        getFontColorPaletteEntry(const unsigned index) const;
        void            setFontColorPaletteEntry(const unsigned index, const GYColor4 color);

        void            fadeFgLayer(const float time, const bool enabled=false); // doesn't affect AdventureLog!
        FADE_STATE      getFgFadeState(void) const;

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
        adventureLog.fade(2.0f, val);
    }
    inline void GuiSystem::getResolution(unsigned int &width, unsigned int &height) const { width = _projWidth; height = _projHeight; }

    inline const Entity* GuiSystem::CombatStatusIndicator::getEntity(void) const { return _entity; }
    inline void GuiSystem::CombatStatusIndicator::setEntity(const Entity * const entity) { _entity = entity; }

    inline void GuiSystem::addVictoryText(const GYVector2& pos, const char*const string) {
        auto& vic   = _victoryText.claimFreeEntry();
        vic.timer   = 0.0f;
        vic.pos     = pos;
        strcpy(vic.string, string);
        vic.state   = VictoryText::STATE_BOUNCE;
    }

    inline void GuiSystem::addCombatStatsEntity(const Entity * const entity) {
        auto& indicator = _combatStatIndicator.claimFreeEntry();
        indicator.setEntity(entity);
        indicator.update();
    }

    inline void GuiSystem::removeCombatStatsEntity(const Entity * const entity) {
       auto it = _combatStatIndicator.findByComparator([&](const CombatStatusIndicator& indicator) {
            return (indicator.getEntity() == entity);
        });

       if(it != _combatStatIndicator.end()) {
           (*it).setEntity(nullptr);
           _combatStatIndicator.remove(it);
       }
    }

    inline GYColor4 GuiSystem::getFontColorPaletteEntry(const unsigned index) const { return Font::getColorPaletteEntry(index); }
    inline void GuiSystem::setFontColorPaletteEntry(const unsigned index, const GYColor4 color) { Font::setColorPaletteEntry(index, color); }

    //Probably isn't graceful if you fade in/out while already fading... whatever for now.
    inline void GuiSystem::fadeFgLayer(const float time, const bool enabled) {
        _fgFadeState = enabled? FADE_IN : FADE_OUT;
        _fgFadeTime = time;
        _fgFadeElapsed = 0.0f;
        _fgFadeAlphaStart = _fgFadeAlpha;
    }

    inline auto GuiSystem::getFgFadeState(void) const -> FADE_STATE { return _fgFadeState; }


}

#endif
