#ifndef ELYSIAN_RECT_ATTRIB_HPP
#define ELYSIAN_RECT_ATTRIB_HPP
#if 0
#include "elysian_prim_attrib.hpp"

#include <gyro.h>

namespace elysian {

	//Thin wrapper for untextured LibGyro rects
	class RectAttrib: public PrimAttrib {
	protected:
		GYColor4 _color;
	public:
		//Not super efficient, but since each rect needs its own color, header must be resubmitted...
		virtual void submit(void) override {
#ifdef ELYSIAN_ENGINE
			submitVerts();
#endif
		}

		virtual void submitVerts(GYVector4* verts = nullptr) {
			if(_color.a == 255) gyVidAlphaLevel(GY_ALPHA_NONE);
			else gyVidAlphaLevel(GY_ALPHA_FULL);

			gyVidTexBind(NULL);
			gyVidQuadListBegin(&_color);
			if(verts) gyVidQuad(verts, 0);
			else gyVidRect(0);
			gyVidQuadListEnd();
		}

		RectAttrib(void) { 
			_color.a = _color.r = _color.g = _color.b = 255;
		}

		void setColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a) { _color.r = r; _color.g = g; _color.b = b; _color.a = a; }
		void setColor(const GYColor4& color) { _color = color; }

		void getColor(unsigned char& r, unsigned char& g, unsigned char& b, unsigned char& a) const { r = _color.r; g = _color.g; b = _color.b; a = _color.a; }
		void getColor(GYColor4& color) const { color = _color; }

		void setAlpha(const unsigned char a) { _color.a = a; }
		unsigned char getAlpha(void) const { return _color.a; }

		PrimAttrib::TYPE getType(void) const { return PrimAttrib::RECT; }

		bool load(std::ifstream& stream) {
			dbgprintf(LOG_VERBOSE, "RectPrim::Load(ifstream&)");
			dbgPush();
			stream >> _color.r;
			stream >> _color.g;
			stream >> _color.b;
			stream >> _color.a;
			dbgprintf(LOG_VERBOSE, "_color = <%u, %u, %u, %u>", _color.r, _color.g, _color.b, _color.a);
			dbgPop();
			return true;
		}

		bool save(std::ofstream& stream) {
			dbgprintf(LOG_VERBOSE, "RectPrim::Save(ofstream&)");
			dbgPush();
			stream << _color.r << std::endl;
			stream << _color.g << std::endl;
			stream << _color.b << std::endl;
			stream << _color.a << std::endl;
			dbgPop();
			return true;
		}
	};

}
#endif
#endif
