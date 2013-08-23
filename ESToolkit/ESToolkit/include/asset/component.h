#ifndef COMPONENT_H
#define COMPONENT_H

#include <QWidget>
#include "elysian_component.hpp"

class Component: public QWidget {
    Q_OBJECT
public:
	virtual elysian::Component::TYPE getType() const = 0;

	Component() { }
	virtual ~Component() { } //No memory leaks, fuckmite
};


#endif // COMPONENT_H
