#ifndef ELYSIAN_RENDERABLE_HPP
#define ELYSIAN_RENDERABLE_HPP

#include "elysian_component.hpp"
#include "elysian_entity.hpp"
#include "elysian_sprite_attrib.hpp"
#include <gyro.h>

namespace elysian {

	class SpriteAttrib;

	/*ONLY SUPPORTS SPRITE PRIMITIVES NOW!
	  The old mechanism for supporting others was totally fucking reested and not worth
	  the effort... we will cross that bridge when (if) we ever get to it... */
	class Renderable: public Component {
	friend class Entity;
	private:
		SpriteAttrib *_sprAttrib;	//Prefab-able attributes
		unsigned _frame;			//for animation
		GYVector2 _offset;			//texture offset from base transform (local coords)

		static GYVector4 _localVerts[4];

		inline Renderable(Entity &parent): Component(parent), _sprAttrib(NULL), _frame(0) { _offset.x = _offset.y = 0; }
	public:
        virtual bool canAddTo(Entity& /*entity*/) { return true; } //no prereqs
		virtual bool load(std::ifstream& stream);
		virtual bool save(std::ofstream& stream);
		virtual bool addToLevel(void);
		virtual bool removeFromLevel(void);
		virtual Component::TYPE getType(void) const { return Component::RENDERABLE; }

		/*Split into phases JUST IN CASE we wanted to optimize for 
		header submission/state changes when renderables share attributes*/
		inline void beginRender(void) {
			gyVidTexBind(_sprAttrib->_tex);
			gyVidAlphaLevel((_sprAttrib->_color.a == 255)? GY_ALPHA_PUNCHTHRU : GY_ALPHA_FULL);
			gyVidQuadListBegin(&_sprAttrib->_color);
		}

		inline void render(void) { 
			if(_sprAttrib->_tex) {
				GYVector2 *size = &_entity->transform._size;
				GYTexCoordFrame *fr = &_sprAttrib->_tex->_coord->texCoord[_frame];
				float locX = ((float) fr->spriteWidth / size->x)/2.0f;
				float locY = ((float) fr->spriteHeight / size->y)/2.0f;
				_localVerts[0].x = -locX + _offset.x;
				_localVerts[0].y = -locY + _offset.y;
				_localVerts[1].x = locX + _offset.x;
				_localVerts[1].y = -locY + _offset.y;
				_localVerts[2].x = -locX + _offset.x;
				_localVerts[2].y = locY + _offset.y;
				_localVerts[3].x = locX + _offset.x;
				_localVerts[3].y = locY + _offset.y;
				gyVidQuad(_localVerts, _frame);
			}
			else {
				gyVidRect(0);
			}
		}

		inline void endRender(void) {
			gyVidQuadListEnd();
		}

		inline unsigned getFrame(void) const { return _frame; }
		inline void setFrame(const unsigned fr) { _frame = fr; }
		inline void setOffset(const GYVector2 &offset) { _offset = offset; }
		inline void getOffset(GYVector2 &offset) const { offset = _offset; } 

		inline SpriteAttrib *getSpriteAttrib(void) const { return _sprAttrib; }
		inline void setSpriteAttrib(SpriteAttrib *const attrib) { _sprAttrib = attrib; if(attrib) _frame = attrib->_frame;}
		
	};

}

#endif
