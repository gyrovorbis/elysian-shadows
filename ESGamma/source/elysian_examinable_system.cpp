#include "elysian_examinable_system.hpp"
#include "elysian_dialog_system.hpp"
#include "elysian_party_system.hpp"
#include "elysian_area.hpp"
#include "elysian_game_input.hpp"
#include "elysian_examinable.hpp"
#include "elysian_character.hpp"
#include "elysian_behavior_system.hpp"
#include "elysian_asset_typedefs.hpp"
#include ELYSIAN_ASSET_HEADER("elysian_entity.hpp", "entity.h")

namespace elysian {

void ExaminableSystem::processInput(GameInput& input) {
	Entity* partyLeader = getPartySys()->getCharacterPartyMember(0u);

	if(!partyLeader) return;
	if(getDialogSys()->isOpen()) return;

	if(input.getTapped(GameInput::BUT_J)) {

		//check for examine
		for(auto it: getCurArea()->examinables) {
			GYVector2 distVec;

            gyVec2Sub(&distVec, (GYVector2*)&partyLeader->_transform._pos, (GYVector2*)&it->getEntity()->_transform._pos);

			//We are officially examining this motherfucker
			if(partyLeader->getCharacter()->isFacing(*it->getEntity())) {

				if(gyVec2Magnitude(&distVec) <= it->getMaxDist()) {

					getBehavSys()->getBehavSys()->onExamine(it->getEntity(), partyLeader);

					if(it->getDialog()) getDialogSys()->startDialog(it->getDialog());

				}


			}
		}

	}


}


}
