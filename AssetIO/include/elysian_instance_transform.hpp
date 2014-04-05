#ifndef ELYSIAN_INSTANCE_TRANSFORM_HPP
#define ELYSIAN_INSTANCE_TRANSFORM_HPP

#include <gyro.h>

#include "elysian_asset.hpp"

namespace elysian {

/*
	Essentially a thin, inlined-to-fuck, OO C++ wrapper around LibGyro's InstanceMat() operation.
*/
	/*class*/ struct InstanceTransform: public GYInstMat, public Asset {
		//This is public for overhead reduction, but the accessors are safer.
		//Lua should treat this as a class
	
	public:

	InstanceTransform(void):
		GYInstMat({
			{ 100.0f, 100.0f, 100.0f },
			{ 100.0f, 100.0f, 100.0f },
			{ 1.0f, 1.0f, 1.0f },
			nullptr,
			0.0f
		}) {}
		void apply(void) const {
#ifdef ELYSIAN_ENGINE
			gyMatInstance(this);
#endif
		}

		void setPos(const float x, const float y, const float z) { _pos.x = x; _pos.y = y; _pos.z = z; }
		void setPos(const float x, const float y) { _pos.x = x; _pos.y = y; }
		void setPos(const GYVector3& pos) { _pos.x = pos.x; _pos.y = pos.y; _pos.z = pos.z; }
		void setPos(const GYVector2& pos) { _pos.x = pos.x; _pos.y = pos.y; }
		void getPos(float& x, float& y) const { x = _pos.x; y = _pos.y; }
		void getPos(float& x, float& y, float& z) const { x = _pos.x; y = _pos.y; z = _pos.z; }
		void getPos(GYVector3& pos) const { pos.x = _pos.x; pos.y = _pos.y; pos.z = _pos.z; }
		void getPos(GYVector2& pos) const { pos.x = _pos.x; pos.y = _pos.y; }
		void setPosZ(const float z) { _pos.z = z; }
		float getPosZ(void) const { return _pos.z; }

		//void setSize(const float x, const float y, const float z) { _size.x = x; _size.y = y; _size.z = z; }
		void setSize(const float x, const float y) { _size.x = x; _size.y = y; }
		//void setSize(const GYVector3& size) { _size = size; }
		void getSize(float& x, float& y) const { x = _size.x; y = _size.y; }
		//void getSize(GYVector3& size) const { size = _size; }
		void getSize(GYVector2& size) const { size.x = _size.x; size.y = _size.y; }

		void setScale(const float x, const float y, const float z) { _scale.x = x; _scale.y = y; _scale.z = z; }
		void setScale(const float x, const float y) { _scale.x = x; _scale.y = y; }
		void setScale(const GYVector3& scale) { _scale = scale; }
		void getScale(float& x, float& y) const { x = _scale.x; y = _scale.y; }
		//void getScale(GYVector3& scale) const { scale = _scale; }
		void getScale(GYVector2& scale) const { scale.x = _scale.x; scale.y = _scale.y; }

		void setOrientZ(const float z) { _orientZ = z; }
		float getOrientZ(void) const { return _orientZ; }

		void rotate(const float z) { _orientZ += z; }

		void translate(const float x, const float y, const float z) { _pos.x += x; _pos.y += y; _pos.z += z; }
		void translate(const float x, const float y) { _pos.x += x; _pos.y += y; }
		void translate(const GYVector3& dist) { _pos.x += dist.x; _pos.y += dist.y; _pos.z += dist.z; }
		void translate(const GYVector2& dist) { _pos.x += dist.x; _pos.y += dist.y; }

        void scale(const float x, const float y, const float /*z*/) { _scale.x *= x; _scale.y *= y; }
		void scale(const float x, const float y) { _scale.x *= x; _scale.y *= y; }
		//void scale(const GYVector2& factor) { gyVec2CompProdAccum(&_scale, &factor); }

		void save(FILE* /*file*/) const { /*save(*this, stream);*/ }
		void load(FILE* /*file*/) { /*load(*this, stream);*/ }

        //static void save(const GYInstMat &mat, std::ofstream &stream) {}
        //static void load(GYInstMat &mat, std::ifstream &stream) {}
	};

}


#endif
