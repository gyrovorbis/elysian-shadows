#ifndef ELYSIAN_RENDERABLE_HPP
#define ELYSIAN_RENDERABLE_HPP

#include "elysian_component.hpp"
#include "elysian_entity.hpp"
#include "elysian_quad_sprite_attrib.hpp"
#include "elysian_material.hpp"
#include <gyro_video_api.h>

namespace elysian {

	class Renderable: public Component {
	friend class Entity;
	private:
		Material*		_material		= nullptr;
		GYTexture*		_normalMap		= nullptr;
		GYTexture*		_specularMap	= nullptr;
		SpriteAttrib*	_spriteAttrib	= nullptr;	//Prefab-able attributes
		int				_frame			= 0;			//for animation
		GYVector2		_offset			= {0.0f, 0.0f};	//texture offset from base transform (local coords)

		inline Renderable(Entity &parent): Component(parent) {}
	public:
		static const GYVector4 flatRectVerts[4];
		static const GYVector4 depthRectVerts[4];

		virtual bool canAddTo(Entity& /*entity*/) override { return true; } //no prereqs
		virtual bool load(std::ifstream& stream) override;
		virtual bool save(std::ofstream& stream) override;
		virtual bool addToLevel(void) override;
		virtual bool removeFromLevel(void) override;
		virtual Component::TYPE getType(void) const override { return Component::RENDERABLE; }

		/*Split into phases JUST IN CASE we wanted to optimize for 
		header submission/state changes when renderables share attributes*/
		inline void beginRender(void) {}

		inline void render(void) { 
			GYVector4 localVerts[4] = {
				{-0.5f, -0.5f, 1.0f, 1.0f},
				{0.5f, -0.5f, 1.0f, 1.0f},
				{-0.5f, 0.5f, 0.0f, 1.0f},
				{0.5f, 0.5f, 0.0f, 1.0f}
			};

			gyVidTexBind(_spriteAttrib->_tex);
			gyVidNormalMapBind(_normalMap);
			gyVidSpecularMapBind(_specularMap);
			gyVidMaterialBind(_material);

			//Why aren't we just pushing another matrix to handle this?
			if(_spriteAttrib->_tex) {
				const GYVector2 *size = (GYVector2*)&_entity->transform._size;
				const GYTexCoordFrame *fr = &_spriteAttrib->_tex->_coord->texCoord[_frame];
				const float locX = ((float) fr->spriteWidth / size->x)/2.0f;
				const float locY = ((float) fr->spriteHeight / size->y)/2.0f;
				localVerts[0].x = -locX + _offset.x;
				localVerts[0].y = -locY + _offset.y;
				localVerts[0].z = locY*2.0f + _offset.y;
				localVerts[1].x = locX + _offset.x;
				localVerts[1].y = -locY + _offset.y;
				localVerts[1].z = locY*2.0f + _offset.y;
				localVerts[2].x = -locX + _offset.x;
				localVerts[2].y = locY + _offset.y;
			//	localVerts[0].z = -locY + _offset.y;
				localVerts[3].x = locX + _offset.x;
				localVerts[3].y = locY + _offset.y;
			//	localVerts[0].z = -locY + _offset.y;
			}

			//branch assumes shit will usually be depth buffered
			if(getEntity()->getDepthType() != DEPTH_BUFFERED) {
				localVerts[0].z = localVerts[1].z = 0.0f;
				gyVidRectNormalType(GY_RECT_NORMAL_FLAT);
			} else {
				gyVidRectNormalType(GY_RECT_NORMAL_DEPTH);
			}

			//Submit vertices
			switch(_spriteAttrib->getType()) {
			case PrimAttrib::QUAD_SPRITE:
				gyVidAlphaLevel(GY_ALPHA_FULL);
				gyVidColorArray(&_spriteAttrib->_color);
				if(_spriteAttrib->_tex) {
					if(_spriteAttrib->_frame != -1)
						gyVidTexCoordArray(gyVidTexCoordArrayFromFrame(&_spriteAttrib->_tex->_coord->texCoord[_frame],
																   GY_RECT_TEX_ORIENT_NORMAL));
					else
						gyVidTexCoordArray(_spriteAttrib->_texCoords);

				}
				gyVidTriangleStrip(localVerts, 0, 4);
				break;
			case PrimAttrib::SPRITE:
				gyVidAlphaLevel((_spriteAttrib->_color.a == 255)? GY_ALPHA_PUNCHTHRU : GY_ALPHA_FULL);
				gyVidQuadListBegin(&_spriteAttrib->_color);
				if(_spriteAttrib->_frame == -1) gyVidTexCoordArray(_spriteAttrib->_texCoords);
				gyVidQuad(localVerts, _frame);
				gyVidQuadListEnd();
				break;
			default: break;//the fuck is wrong with you, son?
			}

		}

		inline void endRender(void) {
			//gyVidQuadListEnd();
		}

		inline unsigned getFrame(void) const { return _frame; }
		inline void setFrame(const unsigned fr) { _frame = fr; }
		inline void setOffset(const GYVector2 &offset) { _offset = offset; }
		inline void getOffset(GYVector2 &offset) const { offset = _offset; } 

		inline SpriteAttrib *getSpriteAttrib(void) const { return _spriteAttrib; }
		inline QuadSpriteAttrib* getQuadSpriteAttrib(void) const { return static_cast<QuadSpriteAttrib*>(_spriteAttrib); }
		inline void setSpriteAttrib(SpriteAttrib *const attrib) { _spriteAttrib = attrib; if(attrib) _frame = attrib->_frame;}

		inline void setMaterial(Material*const material) {
			_material = material;
		}

		inline Material* getMaterial(void) const {
			return _material;
		}

		inline void loadNormalMap(const char*const path, const unsigned width, const unsigned height) {
			if(_normalMap) gyVidTexDestroy(_normalMap);
			_normalMap = gyVidTexCreate(path, width, height);
		}

		inline void loadSpecularMap(const char*const path, const unsigned width, const unsigned height) {
			if(_specularMap) gyVidTexDestroy(_specularMap);
			_specularMap = gyVidTexCreate(path, width, height);
		}
		
	};

}

#endif
