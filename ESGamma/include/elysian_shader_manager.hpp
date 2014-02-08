#ifndef ELYSIAN_SHADER_MANAGER_HPP
#define ELYSIAN_SHADER_MANAGER_HPP

#include <map>
#include <gyro_shader_api.h>
#include "elysian_stl_algorithm.hpp"

namespace elysian {

	class ShaderProgram {
		std::map<char const *, int, ltstr>	_uniforms;
		std::map<char const *, int, ltstr>	_attributes;
		GYShaderProgram*					_gyShaderProgram = nullptr;

		bool _queryAttribute(const char*const name);
		bool _queryUniform(const char*const name);

	public:

		virtual void submit(void) = 0;
		virtual void becomeCurrent(void) = 0;

	};


	template <typename T>
	class ShaderManager {



	};

}



#endif // ELYSIAN_SHADER_MANAGER_HPP
