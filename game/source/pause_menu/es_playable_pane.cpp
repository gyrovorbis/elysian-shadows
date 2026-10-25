/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <game/pause_menu/es_playable_pane.hpp>
#include <ESGamma/gui/elysian_layout.hpp>
#include <ESGamma/gui/elysian_label_widget.hpp>
#include <AssetIO/crtp/elysian_entity.hpp>
#include <AssetIO/crtp/elysian_lua_behavior.hpp>
#include <AssetIO/crtp/elysian_renderable.hpp>
#include <AssetIO/proxy/elysian_entity_proxy.hpp>
#include <AssetIO/crtp/elysian_combat.hpp>
#include <ESGamma/system/elysian_party_system.hpp>
#include <ESGamma/crtp/elysian_gui_system.hpp>

namespace ElysianShadows {

class HpMpGaugeWidget: public EntityWidget {
private:
    Entity* _fg;
protected:

    virtual void render(void) const override {
        EntityWidget::render();
        _fg->render();
    }

public:
    HpMpGaugeWidget(Entity* entity, bool hpGauge):
        EntityWidget(entity),
        _fg(new elysian::Entity(entity))
    {
        GY_ASSERT(_fg->getRenderable());
        auto quadAttrib = make_shared<elysian::QuadSpriteAttrib>();
        if(!hpGauge) {
            quadAttrib->setVertexColor(0, { 0, 0, 255, 255 });
            quadAttrib->setVertexColor(2, { 0, 0, 255, 255 });
            quadAttrib->setVertexColor(1, { 0, 100, 255, 255 });
            quadAttrib->setVertexColor(3, { 0, 100, 255, 255 });
        } else {
            quadAttrib->setVertexColor(0, { 0, 255, 0, 255 });
            quadAttrib->setVertexColor(2, { 0, 255, 0, 255 });
            quadAttrib->setVertexColor(1, { 50, 255, 200, 255 });
            quadAttrib->setVertexColor(3, { 50, 255, 200, 255 });
        }
        _fg->getRenderable()->setSpriteAttrib(quadAttrib);
        setSizePolicy(Widget::FIXED, Widget::FIXED);
    }

    void updateGauge(float curVal, float maxVal) {
        _fg->getTransform() = getEntity()->getTransform();
        float scale = curVal/maxVal;
        float oldSize = _fg->getTransform().getSizeX();
        float newSize = oldSize*scale;
        _fg->getTransform().setSizeX(newSize);

        _fg->getTransform().translate((oldSize-newSize)*-0.5f, 0.0f, 1.0f);
    }

};


PlayablePane::PlayablePane(void) {
    auto newFromPrefab = [](const char* name) {
        LuaVariant pref = LuaManager::getPrefab(name);
        Entity* en1 = pref.getValue<Entity*>();
        GY_ASSERT(!pref.isNil());
        Entity* entity = new elysian::Entity(pref.getValue<Entity*>());//(*pref["new"])(pref).getValue<Entity*>();
        GY_ASSERT(entity);
        GY_ASSERT(en1 != entity);
        return entity;
    };

    _indentWidget   = new GroupWidget();
    _indentWidget->setSize(Widget::X, ELYSIAN_PLAYABLE_PANE_INDENT_SIZE);
    _indentWidget->setHorizontalSizePolicy(Widget::FIXED);

    _portrait       = new EntityWidget(newFromPrefab(ELYSIAN_PLAYABLE_PANE_PORTRAIT_PREFAB));
    auto portraitSpriteAttrib = std::make_shared<elysian::SpriteAttrib>();
    _portrait->getEntity()->getRenderable()->setSpriteAttrib(portraitSpriteAttrib);
    GYVector3 size = _portrait->getEntity()->getTransform().getSize();
    size.x *= 2.0f;
    size.y *= 2.0f;
    _portrait->getEntity()->getTransform().setSize(size.x, size.y);
    _portrait->setSizePolicy(Widget::FIXED, Widget::FIXED);

    _levelText      = new LabelWidget();
    _nameText       = new LabelWidget();
    //_nameText->setFont(&elysian::System::getGuiSys()->verdana);
    _hpText         = new LabelWidget();
    _hpMaxText      = new LabelWidget();
    _hpGauge        = new HpMpGaugeWidget(newFromPrefab(ELYSIAN_PLAYABLE_PANE_HP_BG_PREFAB), true);
    _mpGauge        = new HpMpGaugeWidget(newFromPrefab(ELYSIAN_PLAYABLE_PANE_HP_BG_PREFAB), false);

    _vLayoutLeft    = new VerticalLayout(std::vector<Widget*> {
        _nameText,
        _levelText,
        _hpText,
        _hpGauge
    });

    Entity* entity2 = new Entity;
   entity2->getTransform().setSize(0, 0, 0);
    Renderable* renderable2 = entity2->addRenderable();
    auto attrib2 = std::make_shared<SpriteAttrib>();
    attrib2->setColor(0, 255, 0, 100);
    renderable2->setSpriteAttrib(attrib2);

    _groupLeft      = new GroupWidget;
    _groupLeft->setLayout(_vLayoutLeft);
   // _groupLeft->setSize(X, 30);

    _echoLevelText  = new LabelWidget();
    _echoIcon       = new EntityWidget(newFromPrefab(ELYSIAN_PLAYABLE_PANE_ECHO_ICON_PREFAB));
    _echoNameText   = new LabelWidget();
    _mpText         = new LabelWidget();
    _mpMaxText      = new LabelWidget();

    _echoIcon->setSizePolicy(Widget::FIXED, Widget::FIXED);

    _hLayoutEcho    = new HorizontalLayout(std::vector<Widget*> {
      //  _echoIcon,
        _echoNameText
    });

    _groupEcho = new GroupWidget();
    _groupEcho->setLayout(_hLayoutEcho);

    _vLayoutRight   = new VerticalLayout(std::vector<Widget*> {
        //_groupEcho,
        _echoNameText,
        _echoLevelText,
        _mpText,
        _mpGauge
    });

    _groupRight     = new GroupWidget;
    _groupRight->setLayout(_vLayoutRight);
   // _groupRight->setSize(X, 30);

    HorizontalLayout* mainLayout = new HorizontalLayout(std::vector<Widget*> {
        _portrait,
        _groupLeft,
        _groupRight
    });

    Entity* entity = new Entity;
    //entity->getTransform().setSize(10, 10, 10);
    //entity->getTransform().setPos(400, 400);
    Renderable* renderable = entity->addRenderable();
    auto attrib = std::make_shared<SpriteAttrib>();
    attrib->setColor(Color4::aqua);
    attrib->setAlpha(0);
    renderable->setSpriteAttrib(attrib);
    setEntity(entity);

    setLayout(mainLayout);
}


void PlayablePane::update(void) {
    char buff[50];
    EntityWidget::update();
    if(_playable) {

        CombatStatus& stats = _playable->getCombat()->getStatus();

        sprintf(buff, "Lv. %d", _combatTable["stats"]["level"].getVariant().getValue<int>());
        _levelText->setText(buff);

        sprintf(buff, "HP %d/%d", stats.getHp(), stats.getMaxHp());
        _hpText->setText(buff);

        if(stats.getHp() <= 0) {
            _portrait->getEntity()->getRenderable()->getSpriteAttrib()->setColor(255, 0, 0, 255);
        } else {
            _portrait->getEntity()->getRenderable()->getSpriteAttrib()->setColor(255, 255, 255, 255);
        }

        _hpGauge->updateGauge(stats.getHp(), stats.getMaxHp());


        sprintf(buff, "Lv. %d", _combatTable["echo"]["level"].getVariant().getValue<int>());
        _echoLevelText->setText(buff);

        _echoNameText->setText(_combatTable["echo"]["attrib"]["name"].getVariant().getValue<const char*>());

        sprintf(buff, "MP %d/%d", stats.getMp(), stats.getMaxMp());
        _mpText->setText(buff);

        _mpGauge->updateGauge(stats.getMp(), stats.getMaxMp());

        if(!System::getPartySys()->isPartyMemberActive(System::getPartySys()->getPartySlot(_playable))) {
            getLayout<HorizontalLayout>()->insertWidget(0, _indentWidget);
            _portrait->getEntity()->getRenderable()->getSpriteAttrib()->setAlpha(100);
        } else {
            getLayout<HorizontalLayout>()->insertWidget(3, _indentWidget);
            _portrait->getEntity()->getRenderable()->getSpriteAttrib()->setAlpha(255);
        }

    }
}

void PlayablePane::setPartyMember(Entity* entity) {
    if(_playable == entity) return;

    _playable = entity;

    if(_playable) {
        _nameText->enable();
        _nameText->setText(_playable->getName());
        _combatTable = _playable->getLuaBehavior(ELYSIAN_PLAYABLE_PANE_COMBAT_BEHAV)->getSelfTable();
        _portrait->enable();
        _portrait->getEntity()->getRenderable()->getSpriteAttrib()->setTexPath(_combatTable["playable"]["portrait"].getVariant().getValue<const char*>());
        _echoIcon->getEntity()->getRenderable()->getSpriteAttrib()->setTexPath("/assets/graphics/items/health_globe.png");
    } else {
        _combatTable.setNil();
        _nameText->disable();
        _portrait->disable();

    }

}

}
