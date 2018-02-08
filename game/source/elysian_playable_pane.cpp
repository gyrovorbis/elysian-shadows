#include <elysian_playable_pane.hpp>
#include <gui/elysian_layout.hpp>
#include <gui/elysian_label_widget.hpp>
#include <crtp/elysian_entity.hpp>
#include <crtp/elysian_lua_behavior.hpp>
#include <crtp/elysian_renderable.hpp>
#include <proxy/elysian_entity_proxy.hpp>
#include <crtp/elysian_combat.hpp>
#include <system/elysian_party_system.hpp>

namespace game {

PlayablePane::PlayablePane(void) {
    auto newFromPrefab = [](const char* name) {
        LuaVariant pref = LuaManager::getPrefab(name);
        Entity* en1 = pref.getValue<Entity*>();
        GY_ASSERT(!pref.isNil());
        Entity* entity = new elysian::Entity(pref.getValue<Entity*>());//(*pref["new"])(pref).getValue<Entity*>();
        GY_ASSERT(entity);
        GY_ASSERT(en1 != entity);
        dbgprintf("NEW ENTITY: %x", entity);
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
    _hpText         = new LabelWidget();
    _hpMaxText      = new LabelWidget();
    _hpBg           = new EntityWidget(newFromPrefab(ELYSIAN_PLAYABLE_PANE_HP_BG_PREFAB));
    _hpFg           = new EntityWidget(newFromPrefab(ELYSIAN_PLAYABLE_PANE_HP_FG_PREFAB));

    _vLayoutLeft    = new VerticalLayout(std::vector<Widget*> {
        _levelText,
        _nameText,
        _hpText,
        _hpBg
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
    _mpBg           = new EntityWidget(newFromPrefab(ELYSIAN_PLAYABLE_PANE_HP_BG_PREFAB));
    _mpFg           = new EntityWidget(newFromPrefab(ELYSIAN_PLAYABLE_PANE_MP_FG_PREFAB));

    _echoIcon->setSizePolicy(Widget::FIXED, Widget::FIXED);

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
        _mpFg
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

        sprintf(buff, "Hp %d/%d", stats.getHp(), stats.getMaxHp());
        _hpText->setText(buff);

        if(stats.getHp() <= 0) {
            _portrait->getEntity()->getRenderable()->getSpriteAttrib()->setColor(255, 0, 0, 255);
        } else {
            _portrait->getEntity()->getRenderable()->getSpriteAttrib()->setColor(255, 255, 255, 255);
        }

        _hpFg->getEntity()->getTransform().setScaleX((float)stats.getHp()/(float)stats.getMaxHp());

        sprintf(buff, "Lv. %d", _combatTable["echo"]["level"].getVariant().getValue<int>());
        _echoLevelText->setText(buff);

        _echoNameText->setText(_combatTable["echo"]["attrib"]["name"].getVariant().getValue<const char*>());

        sprintf(buff, "Mp %d/%d", stats.getMp(), stats.getMaxMp());
        _mpText->setText(buff);

        _mpFg->getEntity()->getTransform().setScaleX((float)stats.getMp()/(float)stats.getMaxMp());

        if(!System::getPartySys()->isPartyMemberActive(System::getPartySys()->getPartySlot(_playable))) {
            _layout->insertWidget(0, _indentWidget);
            _portrait->getEntity()->getRenderable()->getSpriteAttrib()->setAlpha(100);
        } else {
            _layout->insertWidget(3, _indentWidget);
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
    } else {
        _combatTable.setNil();
        _nameText->disable();
        _portrait->disable();

    }

}

}
