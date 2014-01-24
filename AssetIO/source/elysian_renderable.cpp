#include "elysian_renderable.hpp"
#include "elysian_area.hpp"
#include "elysian_generic.hpp"

namespace elysian {

/*
Primitive *Renderable::setPrimitiveType(const Primitive::TYPE type) { 
	delete _prim;
	switch(type) {
	case Primitive::RECT:
		_prim = new RectPrim;
		break;
	case Primitive::SPRITE:
		_prim = new SpritePrim;
		break;
	default:
		assert(0); //shit ain't implementated.
	}
	return _prim;
}*/

bool Renderable::load(std::ifstream& /*stream*/) {
	/*unsigned primType;
	Primitive *prim;
	bool success = true;

	dbgprintf(LOG_VERBOSE, "Renderable::Load(ifstream&)");
	dbgPush();

	stream >> primType;

	switch((Primitive::TYPE)primType) {
	case Primitive::RECT:
		prim = new RectPrim;
		break;
	case Primitive::SPRITE:
		prim = new SpritePrim;
		break;
	case Primitive::QUAD:
		prim = new QuadPrim;
		break;
	case Primitive::QUAD_SPRITE:
		prim = new QuadSpritePrim;
		break;
	case Primitive::MODEL:
		prim = new ModelPrim;
		break;
	default:
		prim = NULL;
		dbgprintf(LOG_CRITICAL, "Invalid primitive type - %ud", primType);
		break;
	}*/

/*
	Allocate a RectPrim as the default. Safety precaution to prevent shitting its
	pants when it tries to render with no primitive.
*/
	/*if(!prim) { 
		dbgprintf(LOG_VERBOSE, "Defaulting to Primitive::RECT");
		prim = new RectPrim;
		success = false;
	}
	else if(!prim->Load(stream)) { //so now what? Whatever, their constructors can handle defaults.
		success = false;
	}
	_prim = prim;

	dbgPop();
	return success;*/
	return true;
}

bool Renderable::save(std::ofstream& /*stream*/) {
	/*bool success = true;
	dbgprintf(LOG_VERBOSE, "Renderable::Save(ofstream&)");
	dbgPush();
	
	if(!_prim) {
		dbgprintf(LOG_CRITICAL, "Contains no primitive!");
		stream << Primitive::NONE << std::endl; //Write a bullshit primitive, so the next load will fail and allocate the default primitive type.
		success = false;
	}
	else {
		stream << _prim->getType() << std::endl;
		success &= _prim->Save(stream);
	}

	dbgPop();
	return success;*/
	return true;
}

//O(1)
bool Renderable::addToLevel(void) {
	_curArea->renderables.push_back(this);
	return true;
}

//O(n)
bool Renderable::removeFromLevel(void) {
    return containerEraseOnMatch(this, _curArea->renderables);
}

}
