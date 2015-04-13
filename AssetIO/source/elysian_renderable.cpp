#include "elysian_renderable.hpp"
#include "elysian_generic.hpp"
#include "elysian_sprite_attrib.hpp"
#include "elysian_quad_sprite_attrib.hpp"
#include "elysian_area.hpp"
#include "elysian_material.hpp"
#include "elysian_entity.hpp"

namespace elysian {

ELYSIAN_LUA_PROXY_FIELDS_EXPORT(Renderable)

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

bool Renderable::load(FILE* /*file*/) {
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

bool Renderable::save(FILE* /*file*/) {
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

void Renderable::setSpriteAttrib(SpriteAttrib *const attrib) { _spriteAttrib = attrib; if(attrib && _frame != -1) _frame = attrib->_frame; }

//O(1)
bool Renderable::addToLevel(void) {
    _curArea->renderables.push_back(this);
    return true;
}

//O(n)
bool Renderable::removeFromLevel(void) {
    return containerEraseOnMatch(this, _curArea->renderables);
}


QuadSpriteAttrib* Renderable::getQuadSpriteAttrib(void) const { return static_cast<QuadSpriteAttrib*>(_spriteAttrib); }

void Renderable::render(void) {


 //   gyVidBlendMode(GY_BLEND_ONE_MINUS_SRC_ALPHA);

    gyVidTexBind(_spriteAttrib->_tex);
    gyVidNormalMapBind(_normalMap);
    gyVidSpecularMapBind(_specularMap);
    gyVidMaterialBind(_material);

    gyMatTranslate(_offset.x, _offset.y, 0.0f);

    //branch assumes shit will usually be depth buffered

    gyVidRectPlaneType(_entity->getPlaneType());
    bool alpha = false;

    //Submit vertices
    switch(_spriteAttrib->getType()) {
    case PrimAttrib::QUAD_SPRITE:

        for(unsigned i = 0; i < 4; ++i) {
            if(static_cast<QuadSpriteAttrib*>(_spriteAttrib)->getVertexAlpha(i) != 255) {
                alpha = true;
                break;
            }
        }

         gyVidAlphaLevel((alpha)? GY_ALPHA_PUNCHTHRU : GY_ALPHA_FULL);

        gyVidQuadListBegin(&_spriteAttrib->_color);
     //   gyVidAlphaLevel(GY_ALPHA_FULL);
        gyVidColorArray(&_spriteAttrib->_color);
        if(_spriteAttrib->_tex) {
            if(_frame != -1)
                gyVidTexCoordArray(gyVidTexCoordArrayFromFrame(&_spriteAttrib->_tex->_coord->texCoord[_frame],
                                                           _texOrient));
            else
                gyVidTexCoordArray(_texCoord);

        }
        //gyVidTriangleStrip(localVerts, 0, 4);
        gyVidRect(-1); //HACK AS FUCK PROBABLY BROKE OTHER NON-GLAL PLATFORMS!!!
        gyVidQuadListEnd();
        break;
    case PrimAttrib::SPRITE:
        if(_spriteAttrib->_color.a == 0.0f) return;
        gyVidAlphaLevel((_spriteAttrib->_color.a == 255)? GY_ALPHA_PUNCHTHRU : GY_ALPHA_FULL);
        gyVidQuadListBegin(&_spriteAttrib->_color);

        if(_frame == -1) {
            //for(unsigned i = 0; i < 4; ++i) colorArray[i] = _spriteAttrib->_color;
            gyVidTexCoordArray(_texCoord);
            //gyVidColorArray(colorArray);
        } else {
            gyVidRectTexOrient(_texOrient);
        }

        gyVidRect(_frame);
        gyVidQuadListEnd();
        break;
    default: break;//the fuck is wrong with you, son?
    }

}

}
