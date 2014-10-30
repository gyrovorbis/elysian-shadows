#include "elysian_examinable.hpp"
#include "elysian_entity.hpp"
#include "elysian_generic.hpp"
#include "elysian_dialog.hpp"
#include "elysian_asset_typedefs.hpp"
#include ELYSIAN_ASSET_HEADER("elysian_area.hpp", "area.h")

namespace elysian {

	void Examinable::setDialog(Dialog* const dialog) {
		_dialog = dialog;
		_dialog->setTriggerEntity(_entity);
	}

	//overridden from Component
	bool Examinable::canAddTo(Entity& /*entity*/) {
		return true;
	}

	bool Examinable::load(FILE* /*file*/) {
		return true;
	}

	bool Examinable::save(FILE* /*file*/) {
		return true;
	}

	bool Examinable::addToLevel(void) {
		_curArea->examinables.push_back(this);
		return true;
	}
	
	bool Examinable::removeFromLevel(void) {
        containerEraseOnMatch(this, _curArea->examinables);
		return true;
	}

	Examinable::TYPE Examinable::getType(void) const {
		return EXAMINABLE;
	}

}
