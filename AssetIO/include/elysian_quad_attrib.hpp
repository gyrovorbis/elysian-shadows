#ifndef ELYSIAN_QUAD_ATTRIB_HPP
#define ELYSIAN_QUAD_ATTRIB_HPP

#if 0

#include "elysian_rect_attrib.hpp"
#include <cassert>

namespace elysian {

	class QuadAttrib: public virtual RectAttrib {
	protected:
		GYColor4 _vertColor[3]; //_color inherited from Rect is an implied _vertColor[4];
		const static GYVector4 _verts[4];
	public:

		QuadAttrib(void) {
			for(unsigned i = 0; i < 3; ++i) _vertColor[i].a = _vertColor[i].r = _vertColor[i].g = _vertColor[i].b = 255;
		}

		void setVertexColor(const unsigned short index, const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a) {
			if(index > 0) {
				_vertColor[index-1].r = r; _vertColor[index-1].g = g; _vertColor[index-1].b = b; _vertColor[index-1].a = a;
			} 
			else RectAttrib::setColor(r, g, b, a);
		}
		/*
		void setColor(const GYColor4& color) {
			_vertColor[0] = color; _vertColor[1] = color;
			_vertColor[2] = color; _color = color;
		}*/
/*
		void setColor(const short index, const GYColor4& color) { 
			if(index > 0) {
				_vertColor[index-1] = color;
			}
			else RectAttrib::setColor(color);
		}
*/
		void getVertexColor(const unsigned short index, unsigned char& r, unsigned char& g, unsigned char& b, unsigned char& a) const {
			if(index > 0) {
				r = _vertColor[index-1].r; g = _vertColor[index-1].g; b = _vertColor[index-1].b; a = _vertColor[index-1].a;
			}

			else RectAttrib::getColor(r, g, b, a);
		}
/*
		void getColor(const short index, GYColor4& color) const { 
			if(index > 0) color = _vertColor[index-1];
			else RectAttrib::getColor(color);
		}
*/
		void setVerticesAlpha(const unsigned char a1, const unsigned char a2, const unsigned char a3, const unsigned char a4) {
			_vertColor[0].a = a2; _vertColor[1].a = a3; _vertColor[2].a = a4; _color.a = a1; 
		}

		virtual void submit(void) override {
#ifdef ELYSIAN_ENGINE			
			submitVerts();
#endif
		}

		virtual void submitVerts(GYVector4 *verts = nullptr) {
			//We got some serious fucking design issues if this shit ain't true.
			assert((char *)&_vertColor == ((char*)&_color+4));
			gyVidTexBind(nullptr);
			gyVidAlphaLevel(GY_ALPHA_FULL);
			gyVidColorArray(&_color);
			gyVidTriangleStrip((verts)? verts :_verts, 0, 4);
		}


		PrimAttrib::TYPE getType(void) const { return PrimAttrib::QUAD; }

        bool load(std::ifstream& /*stream*/) {
			dbgprintf(LOG_VERBOSE, "QuadPrim::Load(ifstream&)");
			dbgPush();
			dbgprintf(LOG_CRITICAL, "Not implemented.");
			dbgPop();
			return false;
		}

        bool save(std::ofstream& /*stream*/) {
			dbgprintf(LOG_VERBOSE, "QuadPrim::Save(ofstream&)");
			dbgPush();
			dbgprintf(LOG_CRITICAL, "Not implemented.");
			dbgPop();
			return false;
		}
	};
}

#endif

#endif
