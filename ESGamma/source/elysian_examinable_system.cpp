#include "elysian_examinable_system.hpp"
#include "elysian_dialog_system.hpp"
#include "elysian_party_system.hpp"
#include "elysian_area.hpp"
#include "elysian_game_input.hpp"
#include "elysian_examinable.hpp"
#include "elysian_character.hpp"
#include "elysian_behavior_system.hpp"
#include "elysian_asset_typedefs.hpp"
#include "elysian_entity.hpp"
#include "elysian_gui_system.hpp"

namespace elysian {

void ExaminableSystem::processInput(GameInput& input) {
    Entity*const partyLeader = getPartySys()->getCharacterPartyMember(0u);
    Entity* curExamine = nullptr;
    float minDist = -1.0f;

	if(!partyLeader) return;
	if(getDialogSys()->isOpen()) return;

    //if(input.getTapped(GameInput::BUT_J)) {

		//check for examine
        for(auto it: getCurArea()->examinables) {
			GYVector2 distVec;

            gyVec2Sub(&distVec, (GYVector2*)&partyLeader->_transform._pos, (GYVector2*)&it->getEntity()->_transform._pos);

			//We are officially examining this motherfucker
            //if(partyLeader->getCharacter()->isFacing(*it->getEntity())) {

                const float dist = gyVec2Magnitude(&distVec);
                if(dist <= it->getMaxDist()) {
                    if(minDist > 0.0f) {
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
            if(partyLeader->getCharacter()->isFacing(curExamine) && input.getTapped(GameInput::BUT_J)) {
                getBehavSys()->getBehavSys()->onExamine(curExamine, partyLeader);
                if(curExamine->getExaminable()->getDialog()) getDialogSys()->startDialog(curExamine->getExaminable()->getDialog());
                curExamine = nullptr;
            }
        }

        getGuiSys()->setExamineEntity(curExamine);
}

}
