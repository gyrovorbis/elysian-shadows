#ifndef ELYSIAN_LIGHT_SOURCE_HPP
#define ELYSIAN_LIGHT_SOURCE_HPP

#define ELYSIAN_LIGHT_SOURCE_ATTENUATION_EPISOLON	0.1f

#include <gyro_shader_api.h>
#include <gyro_video_api.h>
#include "elysian_component.hpp"
#ifdef ELYSIAN_ENGINE
#	include "elysian_camera_system.hpp"
#endif

namespace elysian {

	class LightAttrib: private GYLightProperties {
		friend class LightSystem;
		friend class LightSource;

	public:
		LightAttrib(void):
			GYLightProperties(defaultProperties) {}

		static GYLightProperties defaultProperties;

		void setAmbientColor(const GYVector4& color);
		void setDiffuseColor(const GYVector4& color);
		void setSpecularColor(const GYVector4& color);
		void setAttenuationFactors(const GYVector3& vec);
		void setSpotExponent(const float exp);
		void setComputeDistanceAttenuation(const bool val);

		float getRadius(void) const;
	};

	class LightSource: public Component, private GYLightSource {
		friend class LightSystem;
		friend class Engine;

#ifdef ELYSIAN_ENGINE
		CameraState _camState;
#endif
		bool _shadowCaster = false;

	public:

		LightSource(Entity& entity);
		virtual ~LightSource(void) override;

		//reimplemented from component
		virtual bool canAddTo(Entity& entity) override;
		virtual bool load(FILE* file) override;
		virtual bool save(FILE* file) override;
		virtual bool addToLevel(void) override;
		virtual bool removeFromLevel(void) override;
		virtual TYPE getType(void) const override;

		void setLightType(const GY_LIGHT_TYPE _type);
		GY_LIGHT_TYPE getLightType(void) const;
		void setDirection(const GYVector3& dir);
		void setSpotCutoffAngle(const float angle);
		void setAttrib(const LightAttrib* const attrib);
		const GYVector4& getPosition(void) const;
		void setShadowCaster(const bool val);
		bool isShadowCaster(void) const;
		void updateBoundingVolume(const CameraState*const cam, const GY_CUBE_MAP_FACE face);

		LightAttrib& getAttrib(void);
#ifdef ELYSIAN_ENGINE
		CameraState* getCameraState(void);
#endif

		void update(void);

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


#ifdef ELYSIAN_ENGINE
	inline CameraState* LightSource::getCameraState(void) {
		return &_camState;
	}

#endif

}


#endif // ELYSIAN_LIGHT_SOURCE_HPP
