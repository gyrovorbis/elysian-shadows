#ifndef ELYSIAN_LIGHT_SOURCE_HPP
#define ELYSIAN_LIGHT_SOURCE_HPP

#define ELYSIAN_LIGHT_SOURCE_ATTENUATION_EPISOLON	0.1f

#include <gyro_shader_api.h>
#include <gyro_video_api.h>
#include "elysian_component.hpp"
#include "elysian_camera_system.hpp"

namespace elysian {

	class LightAttrib: private GYLightProperties {
		friend class LightSystem;
		friend class LightSource;

	public:
		LightAttrib(void):
			GYLightProperties(defaultProperties) {}

		static GYLightProperties defaultProperties;

        void    setAmbientColor(const GYVector4& color);
        void    setDiffuseColor(const GYVector4& color);
        void    setSpecularColor(const GYVector4& color);
        void    setAttenuationFactors(const GYVector3& vec);
        void    setSpotExponent(const float exp);
        void    setComputeDistanceAttenuation(const bool val);

        void    getAmbientColor(GYVector4& color) const;
        void    getDiffuseColor(GYVector4& color) const;
        void    getSpecularColor(GYVector4& color) const;
        void    getAttenuationFactors(GYVector3& vec) const;
        float   getSpotExponent(void) const;

        float   getRadius(void) const;

        void    linearInterpolate(const LightAttrib& startKeyFrame, const LightAttrib& endKeyFrame,
                               const float interval, const float elapsed);
	};


	class LightSource: public Component, private GYLightSource {
		friend class LightSystem;
		friend class Engine;

        CameraState         _camState;
        GYVector3           _relPos = { 0.0f, 0.0f, 0.0f };
        bool                _shadowCaster = false;
	public:

                            LightSource(Entity& entity);
        virtual             ~LightSource(void) override;

		//reimplemented from component
        virtual bool        canAddTo(Entity& entity) override;
        virtual bool        load(FILE* file) override;
        virtual bool        save(FILE* file) override;
        virtual bool        addToLevel(void) override;
        virtual bool        removeFromLevel(void) override;
        virtual TYPE        getType(void) const override;

        void                setLightType(const GY_LIGHT_TYPE _type);
        void                setDirection(const GYVector3& dir);
        void                setSpotCutoffAngle(const float angle);
        void                setAttrib(const LightAttrib* const attrib);
        void                setShadowCaster(const bool val);
        void                setRelativePos(const float x, const float y, const float z);

        GY_LIGHT_TYPE       getLightType(void) const;
        bool                isShadowCaster(void) const;
        const GYVector4&    getPosition(void) const;
        LightAttrib&        getAttrib(void);
        void                getDirection(GYVector3& dir) const;
        float               getSpotCutoffAngle(void) const;

        void                updateBoundingVolume(const CameraState*const cam, const GY_CUBE_MAP_FACE face);
        CameraState*        getCameraState(void);
        void                update(void);

	};

	inline void LightAttrib::setAmbientColor(const GYVector4& color) {
		ambientColor = color;
	}
	inline void LightAttrib::setDiffuseColor(const GYVector4& color) {
		diffuseColor = color;
	}
	inline void LightAttrib::setSpecularColor(const GYVector4& color) {
		specularColor = color;
	}

	inline void LightAttrib::setSpotExponent(const float exp) {
		spotExponent = exp;
	}

	inline float LightAttrib::getRadius(void) const {
		return _radius;
	}

    inline void LightAttrib::getAmbientColor(GYVector4& color) const { color = ambientColor; }
    inline void LightAttrib::getDiffuseColor(GYVector4& color) const { color = diffuseColor; }
    inline void LightAttrib::getSpecularColor(GYVector4& color) const { color = specularColor; }
    inline void LightAttrib::getAttenuationFactors(GYVector3& vec) const { vec = attenuationFactors; }
    inline float LightAttrib::getSpotExponent(void) const { return spotExponent; }

	inline bool LightSource::canAddTo(Entity &/*entity*/) { return true; }
	inline auto LightSource::getType(void) const -> TYPE { return LIGHT_SOURCE; }
	inline LightSource::LightSource(Entity& entity):
		Component(entity),
		GYLightSource({
			{ 0.0f, 0.0f, 0.0f, 0.0f }, //pos
			{ 0.0f, -0.0f, 0.00f },		//dir
			360.0f,						//cutoff
			GY_LIGHT_POINT,				//type
			&LightAttrib::defaultProperties,
			&_camState,
            nullptr
	}) { gyVec3Normalize(&direction);  }
	inline void LightSource::setAttrib(const LightAttrib* const attrib) { properties = (GYLightProperties*)attrib; }
	inline void LightSource::setDirection(const GYVector3& dir) { gyVec3Normalize(&direction); direction = dir; }
	inline void LightSource::setSpotCutoffAngle(const float angle) { spotCutoffAngle = angle; }
	inline auto LightSource::getLightType(void) const -> GY_LIGHT_TYPE { return type; }
	inline const GYVector4& LightSource::getPosition(void) const { return position; }
	inline LightAttrib& LightSource::getAttrib(void) { return *static_cast<LightAttrib*>(properties); }
	inline void LightSource::setShadowCaster(const bool val) { _shadowCaster = val; }
	inline bool LightSource::isShadowCaster(void) const { return _shadowCaster; }
    inline void LightSource::setRelativePos(const float x, const float y, const float z) { _relPos.x = x; _relPos.y = y; _relPos.z = z; }
    inline void LightSource::getDirection(GYVector3& dir) const { dir = direction; }
    inline float LightSource::getSpotCutoffAngle(void) const { return spotCutoffAngle; }

	inline CameraState* LightSource::getCameraState(void) {
		return &_camState;
	}

    inline void LightAttrib::linearInterpolate(const LightAttrib& startKeyFrame, const LightAttrib& endKeyFrame,
                           const float interval, const float elapsed)
    {
        //So fucking type unsafe, but so fucking awesome...
        gyLinearInterp((float*)&ambientColor.x, 16,
                       (float*)&startKeyFrame.ambientColor.x, (float*)&endKeyFrame.ambientColor.x,
                       interval, elapsed);
    }

}


#endif // ELYSIAN_LIGHT_SOURCE_HPP
