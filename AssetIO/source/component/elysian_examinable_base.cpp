#include <crtp/elysian_examinable.hpp>
#include <crtp/elysian_entity.hpp>
#include <utility/elysian_generic.hpp>
#include <level/elysian_dialog.hpp>
#include <crtp/elysian_area.hpp>

namespace elysian {

    ELYSIAN_LUA_PROXY_FIELDS_EXPORT(ExaminableBase)

    void ExaminableBase::setDialog(Dialog* const dialog) {
		_dialog = dialog;
		_dialog->setTriggerEntity(_entity);
	}

    ExaminableBase::ExaminableBase(const ExaminableBase& other) {
        *this = other;
    }

}
