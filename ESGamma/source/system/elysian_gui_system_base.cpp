#include <crtp/elysian_gui_system.hpp>
#include "elysian_adventure_log.hpp"
#include "elysian_font.hpp"
#include "elysian_debug.hpp"
#include "elysian_timer_system.hpp"
#include "elysian_party_system.hpp"
#include "elysian_day_system.hpp"
#include "elysian_collision_system.hpp"
#include "elysian_lua_manager.hpp"
#include "elysian_light_system.hpp"
#include "elysian_rigid_body_system.hpp"
#include <crtp/elysian_camera_system.hpp>
#include <crtp/elysian_rigid_body.hpp>
#include <crtp/elysian_examinable.hpp>
#include <crtp/elysian_combat.hpp>
#include <crtp/elysian_renderable.hpp>
#include "gui/elysian_label_widget.hpp"

namespace elysian {

constexpr const float GuiSystem::VictoryText::stateTime[GuiSystem::VictoryText::STATE_MAX];

GuiSystem::CombatStatusIndicator::CombatStatusIndicator(void) {
    _healthMeter[0].setColor(80, 50, 25, 150);
    _healthMeter[1].setColor(255, 170, 85, 255);

    _healthMeter[0]._transform.setSize(30.0f, 6.0f);
    _healthMeter[1]._transform.setSize(28.0f, 4.0f);
}

void GuiSystem::CombatStatusIndicator::update(void) {
        const CombatStatus stats = _entity->getCombat()->getStatus();
        const float hp      = stats.getHp();
        const float maxHp   = stats.getMaxHp();

        const static GYColor4 _colors[Combat::TARGET_MAX][2] = {
          { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },
          { { 25, 50, 80, 150 }, { 85, 170, 255, 255 } },
          { { 80, 50, 25, 150 }, { 255, 170, 85, 255 } },
          { { 25, 80, 50, 0 }, { 85, 255, 170, 255 } }
        };

        _healthMeter[0].setColor(_colors[_entity->getCombat()->getTargetType()][0]);
        _healthMeter[1].setColor(_colors[_entity->getCombat()->getTargetType()][1]);

        _healthMeter[0]._transform._pos.x = _healthMeter[1]._transform._pos.x = _entity->_transform._pos.x;
        _healthMeter[0]._transform._pos.y = _entity->_transform._pos.y - 0.75f*_entity->_transform._size.y;
        _healthMeter[1]._transform._pos.y = _healthMeter[0]._transform._pos.y;

        assert(stats.getMaxHp());
        _healthMeter[1]._transform._size.x = ( hp / maxHp ) * 28.0f;
        _healthMeter[1]._transform._pos.x = _healthMeter[0]._transform._pos.x - 0.5f* (28.0f - _healthMeter[1]._transform._size.x);
}

inline void GuiSystem::CombatStatusIndicator::render(void) {
    _healthMeter[0].render();
    _healthMeter[1].render();
}

const GYVector2 GuiSystem::_combatTextGravity = ELYSIAN_GUI_SYSTEM_COMBAT_TEXT_GRAVITY;

GuiSystem::GuiSystem(void):
    _projWidth(0),
    _projHeight(0)
{
    gyVidResolution(&_projWidth, &_projHeight);

}

bool GuiSystem::loadAssets(void) {
    bool success = true;

    Debug::printf(Debug::LOG_VERBOSE, "GuiSystem::LoadAssets()");
    Debug::push();

    success &= twcen.loadTextData(ELYSIAN_FONT_IMAGE_PATH_TWCEN, 256, 256, 16, 32,ELYSIAN_FONT_WIDTH_PATH_TWCEN);
    success &= verdana.loadTextData(ELYSIAN_FONT_IMAGE_PATH_VERDANA, 256, 256, 16, 32,ELYSIAN_FONT_WIDTH_PATH_VERDANA);
    success &= pragmata18.loadTextData(ELYSIAN_FONT_IMAGE_PATH_PRAGMATA18, 256, 256, 16, 32,ELYSIAN_FONT_WIDTH_PATH_PRAGMATA18);
    pragmata18.setCharacterHeight(16);
    success &= pragmata22.loadTextData(ELYSIAN_FONT_IMAGE_PATH_PRAGMATA22, 256, 256, 16, 32,ELYSIAN_FONT_WIDTH_PATH_PRAGMATA22);
    success &= pragmata26.loadTextData(ELYSIAN_FONT_IMAGE_PATH_PRAGMATA26, 512, 512, 32, 32,ELYSIAN_FONT_WIDTH_PATH_PRAGMATA26);

    LabelWidget::setDefaultFont(&pragmata18);

    gyVidResolution(&_projWidth, &_projHeight);

    adventureLog.setLabelFont(&pragmata22);
    adventureLog.setTextFont(&pragmata18);

    Debug::pop();
    return success;
}

void GuiSystem::update(void) {
    gyVidResolution(&_projWidth, &_projHeight);
    adventureLog.update();

    const float deltaTime = getTimerSys()->getDeltaTime();

    //update internal fade state if fading in or out
    if(_fgFadeTime != 0.0f) _fgFadeElapsed += deltaTime;
    switch(_fgFadeState) {
    case FADE_OUT:
        if(_fgFadeElapsed >= _fgFadeTime) {
            _fgFadeState = FADE_DISABLED;
            _fgFadeAlpha = 0.0f;
            _fgFadeTime = 0.0f;
        } else {
            _fgFadeAlpha = 1.0f - (_fgFadeElapsed/_fgFadeTime);
        }
        break;
    case FADE_IN:
        if(_fgFadeElapsed >= _fgFadeTime) {
            _fgFadeState = FADE_ENABLED;
            _fgFadeAlpha = 1.0f;
            _fgFadeTime = 0.0f;
        } else {
            _fgFadeAlpha = (_fgFadeElapsed/_fgFadeTime);
        }
    default: break;
    }

    //update CombatText
    for(auto it = _combatText.begin(); it < _combatText.end(); ++it) {

        gyVec2AddAccum(&(*it).pos, &(*it).vel);
        gyVec2AddAccum(&(*it).vel, &_combatTextGravity);
        (*it).timer += deltaTime;
        (*it).color.a = 255.0f - ((*it).timer/ELYSIAN_GUI_SYSTEM_COMBAT_TEXT_LIFE)*255.0f;

        if((*it).timer >= ELYSIAN_GUI_SYSTEM_COMBAT_TEXT_LIFE) {
            _combatText.remove(it);
        }
    }

    //update VictoryText
    for(auto it = _victoryText.begin(); it < _victoryText.end(); ++it) {
        (*it).timer += deltaTime;

        if((*it).timer >= GuiSystem::VictoryText::stateTime[(*it).state]) {
            if((*it).state == VictoryText::STATE_FADE) {
                (*it).state = VictoryText::STATE_INACTIVE;
                _victoryText.remove(it);
            }
            (*it).state = (VictoryText::STATE)((*it).state + 1);
            (*it).timer = 0.0f;
        }
    }

    for(auto it = _combatStatIndicator.begin(); it < _combatStatIndicator.end(); ++it) {
        if((*it).getEntity()) (*it).update();
    }
}

void GuiSystem::renderBackground(void) {
    //if(!_debugOverlayEnabled) return;
    if(_bgWidget) _bgWidget->render();
}

void GuiSystem::renderForeground(void) {

    gyVidAlphaLevel(GY_ALPHA_FULL);
    gyVidBlendMode(GY_BLEND_ONE_MINUS_SRC_ALPHA);
    gyVidDepthTest(false);
    _renderDebugOverlay();
    if(_fgFadeAlpha == 0.0f) return;
    //if(!_debugOverlayEnabled) return;
    if(_fgWidget) _fgWidget->render();
    adventureLog.render();
}

void GuiSystem::_printDebugLine(const char *title, const char *str, ...) {
    char buffer[100];
    va_list argptr;
    va_start(argptr,str);
    vsprintf(buffer,str,argptr);
    va_end(argptr);

    pragmata18._transform._pos.y += 25.0f;
    pragmata18._transform._pos.x = _projWidth-250.0f;
    pragmata18.drawStringf(Font::R_ALIGN, title);
    pragmata18._transform._pos.x = _projWidth-230.0f;
    pragmata18.drawStringf(Font::L_ALIGN, buffer);
}

void GuiSystem::_renderDebugOverlay(void) {
    int startX, startY, endX, endY;

    if(_debugOverlayPage == -1)
        return;

    pragmata18.setColor(0, 255, 100, 255);
    pragmata18._transform._pos.y = 75.0f;
    pragmata18._transform._pos.z = 50.0f;

    pragmata18.startBatchRender();

    switch(_debugOverlayPage) {
    case 0:
        _printDebugLine("FPS:", "%.2f", getTimerSys()->getFramerate());
        _printDebugLine("Verts:", "%u", gyVidDbgVertexRenderCount());
        _printDebugLine("Uniform Uploads:", "%u", gyVidDbgShaderUniformChangeCount());
        _printDebugLine("LightSys Uploads:", "%u", getLightSys()->getShaderUniformChangesLastFrame());
        _printDebugLine("Texture Changes:", "%u", gyVidDbgTextureChangeCount());
        _printDebugLine("Textures:", "%u", gyVidTexAmount());
        _printDebugLine("Texture MBytes:", "%f", gyVidTexBytes()/1024.0f/1024.0f);
        getCameraSys()->getVisibleTileRegion(startX, startY, endX, endY);
        _printDebugLine("Camera Rect:", "<%d, %d, %d, %d>", startX, startY, endX, endY);
        pragmata18._transform._pos.y += 25.0f;
        _printDebugLine("Onscreen Lights:", "%d", getLightSys()->getOnscreenLightCount());
        _printDebugLine("Entities:", "%d", getCurrentArea()->entities.size());
        pragmata18._transform._pos.y += 25.0f;
        _printDebugLine("Game Time:", "%d:%d:%d", getDaySys()->getDays(), getDaySys()->getHours(), getDaySys()->getMinutes());
         if(getPartySys()->getPartyLeader()) {
            _printDebugLine("player.pos:", "<%.2f, %.2f, %.2f>",
                            getPartySys()->getPartyLeader()->_transform._pos.x,
                            getPartySys()->getPartyLeader()->_transform._pos.y,
                            getPartySys()->getPartyLeader()->_transform._pos.z);

        }
        break;
    case 1:
        _printDebugLine("Collision Checks:", "%d", getCollisionSys()->getEntityEntityChecks());
        _printDebugLine("Collision Time:", "%.4lf", getCollisionSys()->getDetectionTime());
        _printDebugLine("Contacts:", "%d", getCollisionSys()->getContactCount());
        _printDebugLine("Pen Res Iter:", "%d", getCollisionSys()->getPenResIterUsed());
        _printDebugLine("Vel Res Iter:", "%d", getRigidBodySys()->getVelResIterUsed());
        pragmata18._transform._pos.y += 25.0f;
        _printDebugLine("Lua Scripts:", "%d", LuaManager::getScriptCount());
        _printDebugLine("Lua Script Instances:", "%d", LuaManager::getScriptInstances());
        _printDebugLine("Lua MBytes:", "%f", LuaManager::getKbytesUsed()/1024.0f);
         if(getPartySys()->getPartyLeader()) {
            _printDebugLine("player.resting:", "%d",
                            getPartySys()->getPartyLeader()->getCollidable()->getRestingOnBody());
            _printDebugLine("player.contacts:", "%d",
                            getPartySys()->getPartyLeader()->getCollidable()->getContactCount());
        }
        break;
    default: break;
    }
    pragmata18.endBatchRender();

}

void GuiSystem::_renderCombatText(void) {
    gyVidRectTexOrient(GY_RECT_TEX_ORIENT_NORMAL);
    gyVidRectPlaneType(GY_RECT_PLANE_BOTTOM);
    for(auto it = _combatText.begin(); it < _combatText.end(); ++it) {
        pragmata22._transform._pos.x = (*it).pos.x;
        pragmata22._transform._pos.y = (*it).pos.y;
        pragmata22._transform._pos.z = 50.0f;

        pragmata22.setColor((*it).color);
        pragmata22.setAlpha((*it).color.a);

        pragmata22.drawString(Font::C_ALIGN, (*it).text);
    }
}

void GuiSystem::render(void) {
    gyVidAlphaLevel(GY_ALPHA_FULL);
    gyVidBlendMode(GY_BLEND_ONE_MINUS_SRC_ALPHA);
    gyVidDepthTest(false);

    _renderCombatText();
    _renderExamineEntity();
    for(auto it = _combatStatIndicator.begin(); it < _combatStatIndicator.end(); ++it) {
        if((*it).getEntity()) (*it).render();
    }

    //update VictoryText
    GYColor4 textColor = { 255, 255, 255, 255 };
    for(auto it = _victoryText.begin(); it < _victoryText.end(); ++it) {
       char buffer[ELYSIAN_GUI_SYSTEM_VICTORY_TEXT_MAX_CHARS];
        const float percentage =  (*it).timer / VictoryText::stateTime[(*it).state];
        pragmata18._transform._pos.x = (*it).pos.x;
        pragmata18._transform._pos.y = (*it).pos.y;
        pragmata18._transform._pos.z = 50.0f;

        strcpy(buffer, (*it).string);

        float timePerChar = VictoryText::stateTime[(*it).state];
        unsigned totalChars = strlen(buffer);
        timePerChar /= (float)totalChars;
        unsigned chars = (*it).timer/timePerChar;

        switch((*it).state) {
        case VictoryText::STATE_FADE:
            chars = totalChars - (*it).timer/timePerChar;
            //textColor.a = (float)255.0f * (1.0f - percentage);
            pragmata18._transform._pos.y += percentage*32.0f;
            for(unsigned i = 0; i < chars; ++i) {
                buffer[i] = buffer[totalChars-chars+i];
            }
            buffer[chars] = 0;
            break;
        default: break;
        case VictoryText::STATE_BOUNCE:
            chars = (*it).timer/timePerChar;
            buffer[chars] = 0;
            //textColor.a = (float)255.0f * (percentage);
            pragmata18._transform._pos.y += (1.0f-percentage)*32.0f;
            break;

        }

        pragmata18.setColor(textColor);
        pragmata18.drawString(Font::C_ALIGN, buffer);
    }
}

void GuiSystem::addCombatText(const char*const text, const GYColor4 color, const GYVector2& pos, const GYVector2& vel) {

    CombatText* combat = &_combatText.claimFreeEntry();

    strcpy(combat->text, text);
    combat->color = color;
    combat->color.a = 255;
    combat->pos = pos;
    combat->vel = vel;
    combat->timer = 0.0f;

}

void GuiSystem::_renderExamineEntity(void) {
    if(!_examineEntity) return;
    const InstanceTransform* xform = &_examineEntity->_transform;

    const GYColor4 color = { 255, 255, 255, 255 };
    pragmata18._transform._pos.x = xform->_pos.x;
    pragmata18._transform._pos.y = xform->_pos.y + 0.5f * xform->_size.y;
    pragmata18._transform._pos.z = xform->_pos.z + xform->_size.y + ELYSIAN_GUI_SYSTEM_EXAMINE_TEXT_LOG_OFFSET;

    pragmata18.setColor(color);

    gyVidRectPlaneType(GY_RECT_PLANE_BOTTOM);
    gyVidRectTexOrient(GY_RECT_TEX_ORIENT_NORMAL);
    pragmata18.drawString(Font::C_ALIGN, _examineEntity->getDispName());

}


}
