#include <AssetIO/crtp/elysian_examinable.hpp>
#include <AssetIO/crtp/elysian_entity.hpp>
#include <AssetIO/utility/elysian_generic.hpp>
#include <AssetIO/crtp/elysian_dialog.hpp>
#include <AssetIO/crtp/elysian_area.hpp>
#include <AssetIO/crtp/elysian_dialog.hpp>

namespace elysian {

    ELYSIAN_LUA_PROXY_FIELDS_EXPORT(ExaminableBase)

    void ExaminableBase::setDialog(std::shared_ptr<Dialog> dialog) {
        _dialog = dialog? std::make_shared<Dialog>(*dialog.get()) : nullptr;
        if(_dialog) {
            _dialog->setTriggerEntity(_entity);
        }
    }

    ExaminableBase& ExaminableBase::operator=(ExaminableBase& rhs) {
        _dialog = std::make_shared<Dialog>();
        if(rhs._dialog) *_dialog.get() = *rhs._dialog.get();
        else _dialog = nullptr;
        _enabled    = rhs._enabled;
        _maxDist    = rhs._maxDist;
        _dir        = rhs._dir;
        _hasDesc    = rhs._hasDesc;
        return *this;
    }

}
