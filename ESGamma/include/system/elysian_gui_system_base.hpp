#ifndef ELYSIAN_GUI_SYSTEM_BASE_HPP
#define ELYSIAN_GUI_SYSTEM_BASE_HPP

#include "elysian_system.hpp"
#include "elysian_font.hpp"
#include "elysian_adventure_log.hpp"
#include <utility/elysian_mem_pool.hpp>
#include <crtp/elysian_entity.hpp>
#include <memory>

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

#define ELYSIAN_GUI_SYSTEM_EXAMINE_TEXT_LOG_OFFSET  0.0f

#define ELYSIAN_GUI_SYSTEM_EXP_TEXT_BOUNCE_TIME     0.3f
#define ELYSIAN_GUI_SYSTEM_EXP_TEXT_STICK_TIME      3.0f
#define ELYSIAN_GUI_SYSTEM_EXP_TEXT_FADE_TIME       0.3f

#define ELYSIAN_GUI_SYSTEM_DEBUG_OVERLAY_PAGES      2

ELYSIAN_ASSET_TYPEDEF(Entity)

namespace elysian {

	class GuiSystem;
    class Widget;

    namespace game {
        class MenuWidget;
    }

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
        int                         _debugOverlayPage       = -1; //disabled

        unsigned int                _projWidth;
        unsigned int                _projHeight;

        Widget*                     _bgWidget               = nullptr;
        Widget*                     _fgWidget               = nullptr;
        const Entity*               _examineEntity          = nullptr;


        FADE_STATE                  _fgFadeState            = FADE_ENABLED;
        float                       _fgFadeElapsed          = 0.0f;
        float                       _fgFadeTime             = 0.0f;
        float                       _fgFadeAlpha            = 1.0f;
        float                       _fgFadeAlphaStart       = 1.0f;


        bool                        _menuActive         = false;
        game::MenuWidget*           _menuWidget         = nullptr;

        void            _printDebugLine(const char*title, const char*str, ...);
        void            _renderDebugOverlay(void);
        void            _renderCombatText(void);
        void            _renderExamineEntity(void);
	public:

                        GuiSystem(void);
                        ~GuiSystem(void);

        void            setClearColor(const GYColor4 color);

        bool            loadAssets(void);
        void            update(void);
        void            renderBackground(void);
        void            renderForeground(void);
        void            processInput(void);
        void            render(void);
        void            getResolution(unsigned int &width, unsigned int &height) const;
        void            addCombatText(const char*const text, const GYColor4 color, const GYVector2& pos, const GYVector2& vel);
        void            setDebugOverlayPage(const int page);
        int             getDebugOverlayPage(void) const;
        void            nextDebugOverlayPage(void);
        void            setFgWidget(Widget* widget);
        void            setBgWidget(Widget* widget);
        Widget*         getFgWidget(void) const;
        Widget*         getBgWidget(void) const;
        void            clearBgWidget(void);
        void            clearFgWidget(void);
        void            setExamineEntity(const Entity* const entity);
        const Entity*   getExamineEntity(void) const;
        AdventureLog&   getAdvLog(void);
        void            setAdvLogEnabled(const bool enabled=true);

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

    inline void GuiSystem::setExamineEntity(const Entity * const entity) { _examineEntity = entity; }
    inline const Entity* GuiSystem::getExamineEntity(void) const { return _examineEntity; }
    inline void GuiSystem::setDebugOverlayPage(const int page) { _debugOverlayPage = page; }
    inline int GuiSystem::getDebugOverlayPage(void) const { return _debugOverlayPage; }
    inline void GuiSystem::nextDebugOverlayPage(void) {
        if(++_debugOverlayPage >= ELYSIAN_GUI_SYSTEM_DEBUG_OVERLAY_PAGES) _debugOverlayPage = -1;
    }
    inline void GuiSystem::setFgWidget(Widget* widget) { _fgWidget = widget; }
    inline void GuiSystem::setBgWidget(Widget* widget) { _bgWidget = widget; }
    inline Widget* GuiSystem::getFgWidget(void) const { return _fgWidget; }
    inline Widget* GuiSystem::getBgWidget(void) const { return _bgWidget; }
    inline void GuiSystem::clearFgWidget(void) { _fgWidget = nullptr; }
    inline void GuiSystem::clearBgWidget(void) { _bgWidget = nullptr; }
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

    inline AdventureLog& GuiSystem::getAdvLog(void) { return adventureLog; }


}

#endif
