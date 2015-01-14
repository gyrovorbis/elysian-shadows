#ifndef ELYSIAN_MATERIAL_HPP
#define ELYSIAN_MATERIAL_HPP

#include <gyro_shader_api.h>

namespace elysian {

	class Material: public GYMaterial {
	public:
                Material(void);

        void    setAmbientColor(const GYVector4& color);
        void    setDiffuseColor(const GYVector4& color);
        void    setSpecularColor(const GYVector4& color);
        void    setEmissiveColor(const GYVector4& color);
        void    setShininess(const float val);

        void    getAmbientColor(GYVector4& color) const;
        void    getDiffuseColor(GYVector4& color) const;
        void    getSpecularColor(GYVector4& color) const;
        void    getEmissiveColor(GYVector4& color) const;
        float   getShininess(void) const;

	};

	inline Material::Material(void):
		GYMaterial{
			{ 1.0f, 1.0f, 1.0f, 1.0f },
			{ 1.0f, 1.0f, 1.0f, 1.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f },
			1.0f
		}
	{ }

	inline void Material::setAmbientColor(const GYVector4& color) { ambientColor = color; }
	inline void Material::setDiffuseColor(const GYVector4& color) { diffuseColor = color; }
	inline void Material::setSpecularColor(const GYVector4& color) { specularColor = color; }
	inline void Material::setEmissiveColor(const GYVector4& color) { emissiveColor = color; }
	inline void Material::setShininess(const float val) { shininess = val; }

    inline void Material::getAmbientColor(GYVector4& color) const { color = ambientColor; }
    inline void Material::getDiffuseColor(GYVector4& color) const { color = diffuseColor; }
    inline void Material::getSpecularColor(GYVector4& color) const { color = specularColor; }
    inline void Material::getEmissiveColor(GYVector4& color) const { color = emissiveColor; }
    inline float Material::getShininess(void) const { return shininess; }

}


#endif // ELYSIAN_MATERIAL_HPP
