/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <libGyro/gyro_audio_api.h>

#include <AssetIO/crtp/elysian_area.hpp>
#include <AssetIO/crtp/elysian_examinable.hpp>
#include <AssetIO/crtp/elysian_character.hpp>
#include <AssetIO/crtp/elysian_entity.hpp>

#include <ESGamma/system/elysian_examinable_system.hpp>
#include <ESGamma/system/elysian_dialog_system.hpp>
#include <ESGamma/system/elysian_party_system.hpp>
#include <ESGamma/system/elysian_behavior_system.hpp>
#include <ESGamma/system/elysian_gui_system_base.hpp>
#include <ESGamma/crtp/elysian_input_system.hpp>
#include <ESGamma/crtp/elysian_camera_system.hpp>
#include <ESGamma/system/elysian_audio_system.hpp>


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
                if(curExamine->getExaminable()->getDialog()) {
                   // auto keyfr = getCameraSys()->getKeyFrame();
                   // keyfr.setScale(3.0f, 3.0f);
                   // getCameraSys()->moveToKeyFrame(frame, 0.2);
                    getDialogSys()->startDialog(curExamine->getExaminable()->getDialog());
                }
                curExamine = nullptr;
            }
        }

        if(_curEntity != curExamine) {
            if(curExamine) getAudioSys()->playSfxChannel(-1, *_examineSfx);
            getGuiSys()->setExamineEntity(curExamine);
            _curEntity = curExamine;
        }
}

bool ExaminableSystem::initialize(void) {
    _examineSfx = gyAudWavCreate(ELYSIAN_EXAMINABLE_SYSTEM_EXAMINE_SFX);
    return (_examineSfx);
}

void ExaminableSystem::uninitialize(void) {
    gyAudWavDestroy(_examineSfx);
}


}
