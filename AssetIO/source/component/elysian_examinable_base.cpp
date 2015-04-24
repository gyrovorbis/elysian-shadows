#include "elysian_examinable.hpp"
#include "elysian_entity.hpp"
#include "elysian_generic.hpp"
#include "elysian_dialog.hpp"
#include "elysian_asset_typedefs.hpp"
#include "elysian_area.hpp"

namespace elysian {

    ELYSIAN_LUA_PROXY_FIELDS_EXPORT(ExaminableBase)

    void ExaminableBase::setDialog(Dialog* const dialog) {
		_dialog = dialog;
		_dialog->setTriggerEntity(_entity);
	}

}
