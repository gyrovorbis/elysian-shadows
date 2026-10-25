/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_EXAMINABLE_BASE_HPP
#define ELYSIAN_EXAMINABLE_BASE_HPP

#include <AssetIO/crtp/elysian_component.hpp>
#include <AssetIO/level/elysian_direction.hpp>
#include <AssetIO/crtp/elysian_lua_table_proxy.hpp>

#define ELYSIAN_EXAMINABLE_DESC_LENGTH    50

ELYSIAN_ASSET_TYPEDEF(Entity)
ELYSIAN_ASSET_TYPEDEF(Dialog)

namespace elysian {

    class ExaminableBase:
        public Component,
        public LuaTableProxy<ExaminableBase, Examinable>
    {
	private:
        std::shared_ptr<Dialog>	_dialog;
        bool                    _isDialogActive		= false;
        bool                    _enabled			= false;
        float                   _maxDist			= 48.0f;
        Direction::DIR4         _dir				= Direction::DIR4_ANY;
        bool                    _hasDesc            = true;

	public:
        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(enabled,        isEnabled,      setEnabled)
            ELYSIAN_LUA_PROXY_FIELD(maxDist,        getMaxDist,     setMaxDist)
            ELYSIAN_LUA_PROXY_FIELD(relativeDir,    getRelativeDir, setRelativeDir)
        //    ELYSIAN_LUA_PROXY_FIELD(description,    hasDescription, setHasDescription)
            ELYSIAN_LUA_PROXY_FIELD(dialog,         getDialog,      setDialog)
        ELYSIAN_LUA_PROXY_FIELDS_END()

                        ExaminableBase(Entity& parent, ExaminableBase*const prefab=nullptr);
                        ExaminableBase(void) = default;
        ExaminableBase& operator=(ExaminableBase& rhs);

		//overridden from Component
        static constexpr const Component::TYPE componentType = Component::EXAMINABLE;

        void			setDialog(std::shared_ptr<Dialog> dialog);
        auto			getDialog(void) const -> std::shared_ptr<Dialog>;
		void			setMaxDist(const float dist);
		float			getMaxDist(void) const;
		bool			isDialogActive(void) const;
		void			setEnabled(const bool en);
		bool			isEnabled(void) const;
		void			setRelativeDir(const Direction::DIR4 dir);
        Direction::DIR4	getRelativeDir(void) const;
        bool            hasDescription(void) const;
        void            setHasDescription(const bool val);
	};






	// ====== INLINEZ =======
    inline ExaminableBase::ExaminableBase(Entity& parent, ExaminableBase*const prefab): Component(parent) {
        if(prefab) *this = *prefab;
    }

    inline bool ExaminableBase::hasDescription(void) const { return _hasDesc; }
    inline void ExaminableBase::setHasDescription(const bool val) { _hasDesc = val; }
    inline std::shared_ptr<Dialog> ExaminableBase::getDialog(void) const { return _dialog; }
    inline bool ExaminableBase::isDialogActive(void) const { return _isDialogActive; }
    inline void ExaminableBase::setEnabled(const bool en) { _enabled = en; }
    inline bool ExaminableBase::isEnabled(void) const { return _enabled; }
    inline void ExaminableBase::setMaxDist(const float dist) { _maxDist = dist; }
    inline float ExaminableBase::getMaxDist(void) const { return _maxDist; }
    inline void ExaminableBase::setRelativeDir(const Direction::DIR4 dir) { _dir = dir; }
    inline auto ExaminableBase::getRelativeDir(void) const -> Direction::DIR4 { return _dir; }

}

#endif
