#ifndef ELYSIAN_EXAMINABLE_HPP
#define ELYSIAN_EXAMINABLE_HPP

#include "elysian_component.hpp"

namespace elysian {

	class Dialog;
	class Entity;

	class Examinable: public Component {
	private:
		Dialog*			_dialog				= nullptr;
		bool			_isDialogActive		= false;
		bool			_enabled			= false;
		float			_maxDist			= 48.0f;
	public:
						Examinable(Entity& parent);

		//overridden from Component
		virtual bool	canAddTo(Entity& entity) override;
		virtual bool	load(std::ifstream& stream) override;
		virtual bool	save(std::ofstream& stream) override;
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

	};


	inline Examinable::Examinable(Entity& parent): Component(parent) {}
	inline Dialog* Examinable::getDialog(void) const { return _dialog; }
	inline bool Examinable::isDialogActive(void) const { return _isDialogActive; }
	inline void Examinable::setEnabled(const bool en) { _enabled = en; }
	inline bool Examinable::isEnabled(void) const { return _enabled; }
	inline void Examinable::setMaxDist(const float dist) { _maxDist = dist; }
	inline float Examinable::getMaxDist(void) const { return _maxDist; }

}

#endif
