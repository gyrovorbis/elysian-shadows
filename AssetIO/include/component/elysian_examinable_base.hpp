#ifndef ELYSIAN_EXAMINABLE_BASE_HPP
#define ELYSIAN_EXAMINABLE_BASE_HPP

#include <elysian_component.hpp>
#include <elysian_direction.hpp>
#include <elysian_lua_table_proxy.hpp>

#define ELYSIAN_EXAMINABLE_DESC_LENGTH    50

ELYSIAN_ASSET_TYPEDEF(Entity)

namespace elysian {

	class Dialog;

    class ExaminableBase:
        public Component,
        public LuaTableProxy<ExaminableBase, Examinable>
    {
	private:
		Dialog*			_dialog				= nullptr;
		bool			_isDialogActive		= false;
		bool			_enabled			= false;
		float			_maxDist			= 48.0f;
		Direction::DIR4 _dir				= Direction::DIR4_ANY;
        bool            _hasDesc            = true;

	public:
        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
        ELYSIAN_LUA_PROXY_FIELDS_END()

                        ExaminableBase(Entity& parent);

		//overridden from Component
        static constexpr const Component::TYPE componentType = Component::EXAMINABLE;

		void			setDialog(Dialog* const dialog);
		Dialog*			getDialog(void) const;
		void			setMaxDist(const float dist);
		float			getMaxDist(void) const;
		bool			isDialogActive(void) const;
		void			setEnabled(const bool en);
		bool			isEnabled(void) const;
		void			setRelativeDir(const Direction::DIR4 dir);
		Direction::DIR4	getRelativeDir(void);
        bool            hasDescription(void) const;
        void            setHasDescription(const bool val);
	};






	// ====== INLINEZ =======


    inline ExaminableBase::ExaminableBase(Entity& parent): Component(parent) {}
    inline bool ExaminableBase::hasDescription(void) const { return _hasDesc; }
    inline void ExaminableBase::setHasDescription(const bool val) { _hasDesc = val; }
    inline Dialog* ExaminableBase::getDialog(void) const { return _dialog; }
    inline bool ExaminableBase::isDialogActive(void) const { return _isDialogActive; }
    inline void ExaminableBase::setEnabled(const bool en) { _enabled = en; }
    inline bool ExaminableBase::isEnabled(void) const { return _enabled; }
    inline void ExaminableBase::setMaxDist(const float dist) { _maxDist = dist; }
    inline float ExaminableBase::getMaxDist(void) const { return _maxDist; }
    inline void ExaminableBase::setRelativeDir(const Direction::DIR4 dir) { _dir = dir; }
    inline auto ExaminableBase::getRelativeDir(void) -> Direction::DIR4 { return _dir; }

}

#endif
