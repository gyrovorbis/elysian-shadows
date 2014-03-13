#include "elysian_examinable.hpp"
#include "elysian_entity.hpp"
#include "elysian_area.hpp"
#include "elysian_generic.hpp"
#include "elysian_dialog.hpp"

namespace elysian {

	void Examinable::setDialog(Dialog* const dialog) {
		_dialog = dialog;
		_dialog->setTriggerEntity(_entity);
	}

	//overridden from Component
	bool Examinable::canAddTo(Entity& entity) {
		return true;
	}

	bool Examinable::load(std::ifstream& /*stream*/) {
		return true;
	}

	bool Examinable::save(std::ofstream& /*stream*/) {
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
