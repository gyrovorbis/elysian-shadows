#ifndef ELYSIAN_EXAMINABLE_HPP
#define ELYSIAN_EXAMINABLE_HPP

#include "elysian_component.hpp"
#include "elysian_direction.hpp"
#include "elysian_asset_typedefs.hpp"
#include "elysian_lua_table_proxy.hpp"

ELYSIAN_ASSET_TYPEDEF(Entity)

namespace elysian {

	class Dialog;

    class Examinable:
        public Component,
        public LuaTableProxy<Examinable>
    {
	private:
		Dialog*			_dialog				= nullptr;
		bool			_isDialogActive		= false;
		bool			_enabled			= false;
		float			_maxDist			= 48.0f;
		Direction::DIR4 _dir				= Direction::DIR4_ANY;

	public:
        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
        ELYSIAN_LUA_PROXY_FIELDS_END()

						Examinable(Entity& parent);

		//overridden from Component
		virtual bool	canAddTo(Entity& entity) override;
		virtual bool	load(FILE* file) override;
		virtual bool	save(FILE* file) override;
		virtual bool	addToLevel(void) override;
		virtual bool	removeFromLevel(void) override;
		virtual TYPE	getType(void) const override;

		void			setDialog(Dialog* const dialog);
		Dialog*			getDialog(void) const;
		void			setMaxDist(const float dist);
		float			getMaxDist(void) const;
		bool			isDialogActive(void) const;
		void			setEnabled(const bool en);
		bool			isEnabled(void) const;
		void			setRelativeDir(const Direction::DIR4 dir);
		Direction::DIR4	getRelativeDir(void);
	};

	// ====== INLINEZ =======


	inline Examinable::Examinable(Entity& parent): Component(parent) {}
	inline Dialog* Examinable::getDialog(void) const { return _dialog; }
	inline bool Examinable::isDialogActive(void) const { return _isDialogActive; }
	inline void Examinable::setEnabled(const bool en) { _enabled = en; }
	inline bool Examinable::isEnabled(void) const { return _enabled; }
	inline void Examinable::setMaxDist(const float dist) { _maxDist = dist; }
	inline float Examinable::getMaxDist(void) const { return _maxDist; }
	inline void Examinable::setRelativeDir(const Direction::DIR4 dir) { _dir = dir; }
	inline auto Examinable::getRelativeDir(void) -> Direction::DIR4 { return _dir; }

}

#endif
