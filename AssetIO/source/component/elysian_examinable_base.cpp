#include <crtp/elysian_examinable.hpp>
#include <crtp/elysian_entity.hpp>
#include <utility/elysian_generic.hpp>
#include <crtp/elysian_dialog.hpp>
#include <crtp/elysian_area.hpp>
#include <crtp/elysian_dialog.hpp>

namespace elysian {

    ELYSIAN_LUA_PROXY_FIELDS_EXPORT(ExaminableBase)

    void ExaminableBase::setDialog(std::shared_ptr<Dialog> dialog) {
		_dialog = dialog;
        if(_dialog) {
            _dialog->setTriggerEntity(_entity);
        }
    }

    ExaminableBase& ExaminableBase::operator=(ExaminableBase& rhs) {
        _dialog = std::make_shared<Dialog>();
        if(rhs._dialog) *_dialog.get() = *rhs._dialog.get();
        _enabled    = rhs._enabled;
        _maxDist    = rhs._maxDist;
        _dir        = rhs._dir;
        _hasDesc    = rhs._hasDesc;
        return *this;
    }

}
