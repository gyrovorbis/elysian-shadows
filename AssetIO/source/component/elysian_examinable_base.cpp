#include <crtp/elysian_examinable.hpp>
#include <crtp/elysian_entity.hpp>
#include <utility/elysian_generic.hpp>
#include <crtp/elysian_dialog.hpp>
#include <crtp/elysian_area.hpp>

namespace elysian {

    ELYSIAN_LUA_PROXY_FIELDS_EXPORT(ExaminableBase)

    void ExaminableBase::setDialog(std::shared_ptr<Dialog> dialog) {
		_dialog = dialog;
        if(_dialog) {
            _dialog->setTriggerEntity(_entity);
        }
    }

}
