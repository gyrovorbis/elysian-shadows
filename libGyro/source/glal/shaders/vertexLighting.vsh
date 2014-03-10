const int	cMaxLights				= 4;
const int	GY_LIGHT_DIRECTIONAL	= 0;
const int	GY_LIGHT_POINT			= 1;
const int	GY_LIGHT_SPOT			= 2;
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
	vec3	spotDirection;
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

uniform GYLightSource	uLights[cMaxLights];
//uniform int			uLightCount;  //can't loop dynamically or break
uniform GYMaterial		uMaterial;
uniform mat4			uProjViewMat;
uniform vec4			uCameraPos;
uniform vec4			uAmbience;
uniform bool			uLightsEnabled[cMaxLights];
uniform bool			uLightingEnabled;

attribute vec4			aPosition;
attribute vec4			aColor;
attribute vec2			aTexCoord;
attribute vec3			aNormal;

varying vec4			vColor;
varying vec2			vTexCoord;

vec4 lightingEquation(const int i) {
	vec4 computedColor = vec4(cfZero, cfZero, cfZero, cfZero);
	vec3 halfVec;
	vec3 VPpli;
	float nDotL, nDotH;
	float attFactor = cfOne;

	if(uLights[i].position.w != cfZero) {		//Point Light
		vec3 attDist;
		float spotFactor;

		//Direction of light
		VPpli = uLights[i].position.xyz - aPosition.xyz;
		//if() {
			attDist.x = cfOne;
			attDist.z = dot(VPpli, VPpli);
			attDist.y = sqrt(attDist.z);
			attFactor = cfOne / dot(attDist, uLights[i].attenuationFactors);
		//}
		VPpli = normalize(VPpli);

		//Spot light
		if(uLights[i].spotCutoffAngle < cMaxSpotCutoffAngle) {
			spotFactor = dot(-VPpli, uLights[i].spotDirection);

			if(spotFactor >= cos((radians(uLights[i].spotCutoffAngle))))
				spotFactor = pow(spotFactor, uLights[i].spotExponent);
			else
				spotFactor = cfOne;

			attFactor *= spotFactor;
		}
	} else {								//Directional Light
		VPpli = uLights[i].position.xyz;	//MUST BE PRENORMALIZED
	}

	//Process lighting equation
	if(attFactor > cfZero) {
		//Calculate and apply ambient lighting
		computedColor += uLights[i].ambientColor * uMaterial.ambientColor;

		//Calculate and apply diffuse lighting
		nDotL = max(cfZero, dot(aNormal, VPpli));
		computedColor += (nDotL * uLights[i].diffuseColor * uMaterial.diffuseColor);

		//Calculate and apply specular lighting
		halfVec = normalize(VPpli + vec3(cfZero, cfZero, cfOne));
		nDotH = dot(aNormal, halfVec);
		if(nDotH > cfZero) {
			computedColor += (pow(nDotH, uMaterial.shininess) *
								uMaterial.specularColor *
								uLights[i].specularColor);
		}

		computedColor *= attFactor;
	}

	return computedColor;

}


vec4 lightingColor(void) {
	vec4 vertexColor;
	int i;

	//Calculate and apply global ambient lighting
	vertexColor = uMaterial.emissiveColor + (uMaterial.ambientColor * uAmbience);

	//Accumulate colors from each active light source
	for(i = ciZero; i < cMaxLights; ++i) {
		if(uLightsEnabled[i])
			vertexColor += lightingEquation(i);
	}

	vertexColor.a = uMaterial.diffuseColor.a;
	return vertexColor;
}

void main(void) {
	gl_Position = uProjViewMat * /*uModelViewMat * */ aPosition;
	gl_Position.xyz /= gl_Position.w;
	vColor = aColor;

	if(uLightingEnabled) vColor += lightingColor();

	vTexCoord = aTexCoord;
}
