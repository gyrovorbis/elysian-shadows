#ifndef ELYSIAN_COMBAT_HPP
#define ELYSIAN_COMBAT_HPP

#include "elysian_component.hpp"
#ifdef ELYSIAN_ENGINE
#	include <oolua.h>
#endif

namespace elysian {

	class CombatAttrib {
	private:
		uint32_t _typeFlags			= ~0x0;
		uint32_t _typeAgainstFlags	= ~0x0;
	public:
		uint32_t getTypeFlags(void) const;
		uint32_t getTypeAgainstFlags(void) const;
		void setTypeFlags(const uint32_t flags);
		void setTypeAgainstFlags(const uint32_t flags);
	};

	class Combat: public Component {
	public:
		enum DMG_TYPE { DMG_NONE, DMG_SOURCE, DMG_TARGET };

	private:
#ifdef ELYSIAN_ENGINE
		OOLUA::Lua_table_ref	_selfTable;
		OOLUA::Lua_table_ref	_actionTable;
#endif
		CombatAttrib*			_attrib		= nullptr;
		DMG_TYPE				_dmgType	= DMG_NONE;
	public:

				Combat(Entity& entity);
		virtual ~Combat(void) override = default;

		//reimplemented from component
		virtual bool canAddTo(Entity& entity) override;
		virtual bool load(std::ifstream& stream) override;
		virtual bool save(std::ofstream& stream) override;
		virtual bool addToLevel(void) override;
		virtual bool removeFromLevel(void) override;
		virtual TYPE getType(void) const override;

#ifdef ELYSIAN_ENGINE
		void					setSelfTable(OOLUA::Lua_table_ref self);
		OOLUA::Lua_table_ref&	getSelfTable(void);
		void					setActionTable(OOLUA::Lua_table_ref self);
		OOLUA::Lua_table_ref&	getActionTable(void);
#endif

		DMG_TYPE		getDmgType(void) const;
		void			setDmgType(const DMG_TYPE type);
		CombatAttrib*	getAttrib(void) const;
		void			setAttrib(CombatAttrib* attrib);

	};


	// ======== INLINEZ ========
	inline uint32_t CombatAttrib::getTypeFlags(void) const { return _typeFlags; }
	inline uint32_t CombatAttrib::getTypeAgainstFlags(void) const { return _typeAgainstFlags; }
	inline void CombatAttrib::setTypeFlags(const uint32_t flags) { _typeFlags = flags; }
	inline void CombatAttrib::setTypeAgainstFlags(const uint32_t flags) { _typeAgainstFlags = flags; }

#ifdef ELYSIAN_ENGINE
	inline void	Combat::setSelfTable(OOLUA::Lua_table_ref self) { _selfTable = self; }
	inline OOLUA::Lua_table_ref& Combat::getSelfTable(void) { return _selfTable; }
	inline void	Combat::setActionTable(OOLUA::Lua_table_ref self) { _actionTable = self; }
	inline OOLUA::Lua_table_ref& Combat::getActionTable(void) { return _actionTable; }
#endif

	inline auto	Combat::getDmgType(void) const -> DMG_TYPE { return _dmgType; }
	inline void Combat::setDmgType(const DMG_TYPE type) { _dmgType = type; }
	inline CombatAttrib* Combat::getAttrib(void) const { return _attrib; }
	inline void Combat::setAttrib(CombatAttrib* attrib) { _attrib = attrib; }

}


#endif // ELYSIAN_COMBAT_HPP
