/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_MESH_ATTRIB_HPP
#define ELYSIAN_MESH_ATTRIB_HPP

#include <AssetIO/crtp/elysian_blend_mode.hpp>
#include <


struct GYTexture;
struct GYVertex;

namespace elysian {

    class MeshAttrib {
        BlendMode   _blendMode;
        GYTexture*  _diffuseMap     = nullptr;
        GYTexture*  _normalMap      = nullptr;
        GYTexture*  _emissiveMap    = nullptr;

    public:

        BlendMode   getBlendMode(void) const;
        GYTexture*  getDiffuseMap(void) const;
        GYTexture*  getNormalMap(void) const;
        GYTexture*  getEmissiveMap(void) const;

    };


    class Mesh: public RenderbleObject {
        GYVertex*       _worldVerts     = nullptr;
        MeshAttrib*     _attrib         = nullptr;
        size_t          _vertCount      = 0;

                        Mesh(void);

        virtual void    render(Renderer& renderer) override;
    };

    class InstanceMesh: public Mesh {
        GYVector4*          _localVerts  = nullptr;
        InstanceTransform*  _transform   = nullptr;

        virtual void    updateVerts(void) override;

    };

#define ELYSIAN_SPRITE_VERTEX_COUNT 4

    class Sprite: public Mesh {
        static constexpr const Vector4  _localPositions[ELYSIAN_SPRITE_VERTEX_COUNT];

        Vector4             _worldVerts[ELYSIAN_SPRITE_VERTEX_COUNT];
    };




}


#endif // ELYSIAN_MESH_ATTRIB_HPP
