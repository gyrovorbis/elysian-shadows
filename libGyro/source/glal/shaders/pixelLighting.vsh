const int	cMaxLights				= 4;
const float cfZero					= 0.0;
const float cfOne					= 1.0;
const int	ciZero					= 0;
const float cMaxSpotCutoffAngle		= 180.0;
const vec3	cViewVector				= vec3(0.0, 0.0, 1.0);

struct GYLightSource {
	vec4	position;					//Position for point light, normalized direciton for directional light
	vec4	ambientColor;
	vec4	diffuseColor;
	vec4	specularColor;
	vec3	direction;
	vec3	attenuationFactors;
	float	spotExponent;
	float	spotCutoffAngle;
	bool	computeAttenuation;
};

struct GYMaterial {
	float	shininess;
	vec4	ambientColor;
	vec4	diffuseColor;
	vec4	specularColor;
	vec4	emissiveColor;
};

uniform vec3			uCamWorldPos;
uniform GYLightSource	uLights[cMaxLights];
//uniform int			uLightCount;  //can't loop dynamically or break
uniform GYMaterial		uMaterial;
uniform mat4			uProjViewMat;
uniform vec4			uAmbience;
uniform vec3			uNormal;
uniform vec3			uTangent;
uniform bool			uLightsEnabled[cMaxLights];
uniform samplerCube		uShadowCubeMaps[cMaxLights];
uniform bool			uTextureEnabled;
uniform bool			uLightingEnabled;
uniform bool			uNormalMapEnabled;
uniform bool			uSpecularMapEnabled;
uniform sampler2D		uNormalMap;
uniform sampler2D		uSpecularMap;
uniform sampler2D		uShadowMaps[cMaxLights];

uniform mat4			uLightProjViewMats[cMaxLights];

attribute vec4			aPosition;
attribute vec4			aColor;
attribute vec2			aTexCoord;
//attribute vec3			aNormal;
//attribute vec3			aTangent;

varying vec4			vColor;
varying vec4			vWorldPos;
//varying vec4			vDiffuses[cMaxLights];
//varying vec4			vAmbients[cMaxLights];
//varying vec4			vShadowPos[cMaxLights];
varying vec3			vNormal;
varying vec2			vTexCoord;


void main(void) {
	gl_Position = uProjViewMat * aPosition;
	vColor = aColor;
	vWorldPos = aPosition;
	vNormal = uNormal;
	vTexCoord = aTexCoord;
}
