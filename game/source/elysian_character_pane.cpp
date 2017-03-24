#include <elysian_character_pane.hpp>
#include <gui/elysian_layout.hpp>
#include <gui/elysian_label_widget.hpp>
#include <crtp/elysian_entity.hpp>
#include <crtp/elysian_lua_behavior.hpp>
#include <crtp/elysian_renderable.hpp>
#include <proxy/elysian_entity_proxy.hpp>
#include <crtp/elysian_combat.hpp>

namespace game {

CharacterPane::CharacterPane(void) {
    _portrait       = new EntityWidget(LuaManager::getPrefab("menu_partyMemberPane_portrait").getValue<Entity*>());

    _levelText      = new LabelWidget();
    _nameText       = new LabelWidget();
    _hpText         = new LabelWidget();
    _hpMaxText      = new LabelWidget();
    _hpBg           = new EntityWidget(LuaManager::getPrefab("menu_partyMemberPane_hpBg").getValue<Entity*>());
    _hpFg           = new EntityWidget(LuaManager::getPrefab("menu_partyMemberPane_hpFg").getValue<Entity*>());

    _vLayoutLeft    = new VerticalLayout(std::vector<Widget*> {
        _levelText,
        _nameText,
        _hpText,
        _hpBg
    });

    _groupLeft      = new GroupWidget();
    _groupLeft->setLayout(_vLayoutLeft);
    _groupLeft->setSize(X, 30);

    _echoLevelText  = new LabelWidget();
    _echoIcon       = new EntityWidget(LuaManager::getPrefab("menu_partyMemberPane_echoIcon").getValue<Entity*>());
    _echoNameText   = new LabelWidget();
    _mpText         = new LabelWidget();
    _mpMaxText      = new LabelWidget();
    _mpBg           = new EntityWidget(LuaManager::getPrefab("menu_partyMemberPane_hpBg").getValue<Entity*>());
    _mpFg           = new EntityWidget(LuaManager::getPrefab("menu_partyMemberPane_mpFg").getValue<Entity*>());

    _hLayoutEcho    = new HorizontalLayout(std::vector<Widget*> {
        _echoIcon,
        _echoNameText
    });

    _groupEcho = new GroupWidget();
    _groupEcho->setLayout(_hLayoutEcho);

    _vLayoutRight   = new VerticalLayout(std::vector<Widget*> {
        _echoLevelText,
        _groupEcho,
        _mpText,
        _hpBg
    });

    _groupRight     = new GroupWidget();
    _groupRight->setLayout(_vLayoutRight);
    _groupRight->setSize(X, 30);

    HorizontalLayout* mainLayout = new HorizontalLayout(std::vector<Widget*> {
        _portrait,
        _groupLeft,
        _groupRight
    });

    Entity* entity = new Entity;
    entity->getTransform().setSize(300, 200, 400);
    entity->getTransform().setPos(400, 400, 51);
    Renderable* renderable = entity->addRenderable();
    SpriteAttrib* attrib = new SpriteAttrib;
    attrib->setColor(0, 0, 200, 100);
    renderable->setSpriteAttrib(attrib);
    setEntity(entity);

    setLayout(mainLayout);
}


void CharacterPane::render(void) {
    char buff[50];
    elysian::EntityWidget::render();
    if(_playable) {
        CombatStatus& stats = _playable->getCombat()->getStatus();

        sprintf(buff, "Lv. %d", _combatTable["stats"]["level"].getVariant().getValue<int>());
        _levelText->setText(buff);

        sprintf(buff, "Hp %d/%d", stats.getHp(), stats.getMaxHp());
        _hpText->setText(buff);

        _hpFg->getEntity()->getTransform().setScaleX((float)stats.getHp()/(float)stats.getMaxHp());

        sprintf(buff, "Lv. %d", _combatTable["echo"]["level"].getVariant().getValue<int>());
        _echoLevelText->setText(buff);

        _echoNameText->setText(_combatTable["echo"]["attrib"]["name"].getVariant().getValue<const char*>());

        sprintf(buff, "Mp %d/%d", stats.getMp(), stats.getMaxMp());
        _mpText->setText(buff);

        _mpFg->getEntity()->getTransform().setScaleX((float)stats.getMp()/(float)stats.getMaxMp());

    }
}

void CharacterPane::setCharacter(Entity* entity) {
    if(_playable == entity) return;

    _playable = entity;

    if(_playable) {
        _nameText->enable();
        _nameText->setText(_playable->getName());
        _combatTable = entity->getLuaBehavior(ELYSIAN_CHARACTER_PANE_COMBAT_BEHAV)->getSelfTable();
        _portrait->enable();
        _portrait->getEntity()->getRenderable()->getSpriteAttrib()->setTexPath(_combatTable["playable"]["portrait"].getVariant().getValue<const char*>());
    } else {
        _combatTable.setNil();
        _nameText->disable();
        _portrait->disable();

    }

}

}
