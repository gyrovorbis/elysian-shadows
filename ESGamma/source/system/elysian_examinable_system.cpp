#include "elysian_examinable_system.hpp"
#include "elysian_dialog_system.hpp"
#include "elysian_party_system.hpp"
#include <crtp/elysian_area.hpp>
#include <crtp/elysian_examinable.hpp>
#include <crtp/elysian_character.hpp>
#include "elysian_behavior_system.hpp"
#include <crtp/elysian_entity.hpp>
#include "elysian_gui_system_base.hpp"
#include <crtp/elysian_input_system.hpp>

namespace elysian {

void ExaminableSystem::processInput(void) {

    Entity*const partyLeader = getPartySys()->getCharacterPartyMember(0u);
    Entity* curExamine = nullptr;
    float minDist;

	if(!partyLeader) return;
	if(getDialogSys()->isOpen()) return;

    //if(input.getTapped(GameInput::BUT_J)) {

		//check for examine
        for(auto it: getCurrentArea()->examinables) {

            const float dist = gyVec3Dist(&partyLeader->_transform._pos, &it->getEntity()->_transform._pos);

			//We are officially examining this motherfucker
            //if(partyLeader->getCharacter()->isFacing(*it->getEntity())) {

                if(dist <= it->getMaxDist()) {
                    if(curExamine) {
                        if(dist < minDist) {
                            minDist = dist;
                            curExamine = it->getEntity();
                        }
                    } else {
                        minDist = dist;
                        curExamine = it->getEntity();
                    }
               }
            //}
        }

        if(curExamine) {
            if(/*partyLeader->getCharacter()->isFacing(curExamine) &&*/ getInputSys()->getController(0)->isTapped(GY_CONTROLLER_BUT_CIRCLE)) {
                getBehavSys()->getBehavSys()->onExamine(curExamine, partyLeader);
                if(curExamine->getExaminable()->getDialog()) getDialogSys()->startDialog(curExamine->getExaminable()->getDialog());
                curExamine = nullptr;
            }
        }

        getGuiSys()->setExamineEntity(curExamine);

}

}
