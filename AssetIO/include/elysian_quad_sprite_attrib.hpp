#ifndef ELYSIAN_QUAD_SPRITE_ATTRIB_HPP
#define ELYSIAN_QUAD_SPRITE_ATTRIB_HPP

#include "elysian_textured_attrib.hpp"
#include "elysian_sprite_attrib.hpp"

namespace elysian {

	class QuadSpriteAttrib: public SpriteAttrib {
	public:
		GYColor4 _vertColor[3] = {
			{255, 255, 255, 255},
			{255, 255, 255, 255},
			{255, 255, 255, 255}
		};//_color inherited from Rect is an implied _vertColor[4];
		const static GYVector4 _verts[4];

		virtual void submit(void) override {
#ifdef ELYSIAN_ENGINE
			submitVerts();
#endif
		}

		void setVertexColor(const unsigned short index, const GYColor4 color) {
			if(index > 0) _vertColor[index-1] = color;
			else setColor(color);
		}

		void setVertexColor(const unsigned short index, const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a) {
			if(index > 0) {
				_vertColor[index-1].r = r; _vertColor[index-1].g = g; _vertColor[index-1].b = b; _vertColor[index-1].a = a;
			}
			else setColor(r, g, b, a);
		}

		void getVertexColor(const unsigned short index, unsigned char& r, unsigned char& g, unsigned char& b, unsigned char& a) const {
			if(index > 0) {
				r = _vertColor[index-1].r; g = _vertColor[index-1].g; b = _vertColor[index-1].b; a = _vertColor[index-1].a;
			}

			else getColor(r, g, b, a);
		}

		void setVerticesAlpha(const unsigned char a1, const unsigned char a2, const unsigned char a3, const unsigned char a4) {
			_vertColor[0].a = a2; _vertColor[1].a = a3; _vertColor[2].a = a4; _color.a = a1;
		}


		virtual void submitVerts(GYVector4 *verts = nullptr) {
			//We got some serious fucking design issues if this shit ain't true.
			assert((char *)&_vertColor == ((char*)&_color+4));
			gyVidTexBind(_tex);
			gyVidAlphaLevel(GY_ALPHA_FULL);
			gyVidColorArray(&_color);
			gyVidTriangleStrip((verts)? verts :_verts, 0, 4);
		}

		PrimAttrib::TYPE getType(void) const { return PrimAttrib::QUAD_SPRITE; }

		virtual bool load(FILE* file) {
			return false;
		}

		virtual bool save(FILE* file) {
			return false;
		}
	};
}

#endif
