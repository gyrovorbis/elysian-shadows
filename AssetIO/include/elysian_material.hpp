#ifndef ELYSIAN_MATERIAL_HPP
#define ELYSIAN_MATERIAL_HPP

#include <gyro_shader_api.h>

namespace elysian {

	class Material: public GYMaterial {
	public:
		Material(void);

		void setAmbientColor(const GYVector4& color);
		void setDiffuseColor(const GYVector4& color);
		void setSpecularColor(const GYVector4& color);
		void setEmissiveColor(const GYVector4& color);
		void setShininess(const float val);

	};

	inline Material::Material(void):
		GYMaterial{
			{ 1.0f, 1.0f, 1.0f, 1.0f },
			{ 1.0f, 1.0f, 1.0f, 1.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f },
			{ 1.0f, 1.0f, 1.0f, 1.0f },
			1.0f
		}
	{ }

	inline void Material::setAmbientColor(const GYVector4& color) { ambientColor = color; }
	inline void Material::setDiffuseColor(const GYVector4& color) { diffuseColor = color; }
	inline void Material::setSpecularColor(const GYVector4& color) { specularColor = color; }
	inline void Material::setEmissiveColor(const GYVector4& color) { emissiveColor = color; }
	inline void Material::setShininess(const float val) { shininess = val; }

}


#endif // ELYSIAN_MATERIAL_HPP
