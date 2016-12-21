#ifndef ELYSIAN_QUAD_SPRITE_ATTRIB_HPP
#define ELYSIAN_QUAD_SPRITE_ATTRIB_HPP

#include <primitive/elysian_textured_attrib.hpp>
#include <crtp/elysian_sprite_attrib.hpp>

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

            submitVerts();

        }

        void setVertexColor4(const unsigned short index, const Color4& color) {
            GY_ASSERT(index < 4);
            if(index > 0) _vertColor[index-1] = *(const GYColor4*)color;
            else setColor4(color);
        }

        void setVertexColor(const unsigned short index, const GYColor4 color) {
            GY_ASSERT(index < 4);
            if(index > 0) _vertColor[index-1] = color;
            else setColor(color);
        }

        void setVertexColor(const unsigned short index, const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a) {
            GY_ASSERT(index < 4);
            if(index > 0) {
                _vertColor[index-1].r = r; _vertColor[index-1].g = g; _vertColor[index-1].b = b; _vertColor[index-1].a = a;
            }
            else setColor(r, g, b, a);
        }

        Color4 getVertexColor4(const unsigned short index) {
            GY_ASSERT(index < 4);
            return (index > 0)? Color4(&_vertColor[index-1]) : getColor4();
        }

        void getVertexColor(const unsigned short index, unsigned char& r, unsigned char& g, unsigned char& b, unsigned char& a) const {
            GY_ASSERT(index < 4);
            if(index > 0) {
                r = _vertColor[index-1].r; g = _vertColor[index-1].g; b = _vertColor[index-1].b; a = _vertColor[index-1].a;
            }

            else getColor(r, g, b, a);
        }

        void setVerticesAlpha(const unsigned char a1, const unsigned char a2, const unsigned char a3, const unsigned char a4) {
            _vertColor[0].a = a2; _vertColor[1].a = a3; _vertColor[2].a = a4; _color.a = a1;
        }

        void setVertexAlpha(const unsigned short index, const float alpha) {
            GY_ASSERT(index < 4);
            if(index > 0) {
                _vertColor[index-1].a = alpha;
            } else {
                _color.a = alpha;
            }
        }
        unsigned char getVertexAlpha(const unsigned short index) const {
            GY_ASSERT(index < 4);
            return (index > 0)? _vertColor[index-1].a : _color.a;
        }


        virtual void submitVerts(GYVector4 *verts = nullptr) override {
            //We got some serious fucking design issues if this shit ain't true.
            GY_ASSERT((char *)&_vertColor == ((char*)&_color+4));
            gyVidTexBind(_tex);
            gyVidAlphaLevel(GY_ALPHA_FULL);
            gyVidQuadListBegin(&_color);
            gyVidColorArray(&_color);
            gyVidQuad((verts)? verts :_verts, _frame);
            gyVidQuadListEnd();
        }

        PrimAttrib::TYPE getType(void) const override { return PrimAttrib::QUAD_SPRITE; }

    };
}

#endif
