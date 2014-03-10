const int	cMaxLights = 4;
const int	GY_LIGHT_DIRECTIONAL = 0;
const int	GY_LIGHT_POINT = 1;
const int	GY_LIGHT_SPOT = 2;
const float cfZero = 0.0;
const int	ciZero = 0;
const vec3	cViewVector = vec3(0.0, 0.0, 1.0);

struct GYLightSource {
	int		type;
	vec4	position;
	vec4	color;
	float	ambientCoeff;
	float	diffuseCoeff;
	float	specularCoeff;
};

struct GYMaterial {
	float	shininess;
	float	ambientCoeff;
	float	diffuseCoeff;
	float	specularCoeff;
};

uniform mat4			uProjMat;
uniform GYLightSource	uLights[cMaxLights];
//uniform int			uLightCount;  //can't loop dynamically or break
uniform GYMaterial		uMaterial;
uniform vec4			uAmbience;

attribute vec4			aPosition;
attribute vec4			aColor;
attribute vec2			aTexCoord;
attribute vec3			aNormal;

varying vec4			vColor;
varying vec2			vTexCoord;

void main(void)
{
	vec4	globalAmbience;
	vec3	lightDir, halfVector;
	float	diffuse, ambient, specular;
	float	NdotL, NdotHV;
	int i;

	vColor = vec4(cfZero, cfZero, cfZero, cfZero);
	globalAmbience = uAmbience * uMaterial.ambientCoeff;

	for(i = ciZero; i < cMaxLights; ++i) {

		/* now normalize the light's direction. Note that according to the
		OpenGL specification, the light is stored in eye space. Also since
		we're talking about a directional light, the position field is actually
		direction */
		lightDir = normalize(vec3(uLights[i].position));

		/* compute the cos of the angle between the normal and lights direction.
		The light is directional so the direction is constant for every vertex.
		Since these two are normalized the cosine is the dot product. We also
		need to clamp the result to the [0,1] range. */

		NdotL = max(dot(aNormal, lightDir), cfZero);

		/* Compute the diffuse, ambient and globalAmbient terms */
		diffuse = uMaterial.diffuseCoeff * uLights[i].diffuseCoeff;
		ambient = uMaterial.ambientCoeff * uLights[i].ambientCoeff;

		/* compute the specular term if NdotL is  larger than zero */
		if (NdotL > cfZero) {
			halfVector = normalize(cViewVector - lightDir);
			NdotHV = max(dot(aNormal, halfVector), cfZero);

			//accumulate all of this then multiply by material coefficients ONCE at the end!!!
			specular = uMaterial.specularCoeff * uLights[i].specularCoeff * pow(NdotHV, uMaterial.shininess);
		}

		vColor += uLights[i].color * (diffuse + ambient + specular);
	}

	vColor += (aColor * globalAmbience);
	vTexCoord = aTexCoord;
	gl_Position = uProjMat * aPosition;

}
