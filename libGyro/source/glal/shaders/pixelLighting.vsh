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

uniform GYLightSource	uLights[cMaxLights];
//uniform int			uLightCount;  //can't loop dynamically or break
uniform GYMaterial		uMaterial;
uniform mat4			uProjViewMat;
uniform vec4			uAmbience;
uniform vec3			uNormal;
uniform vec3			uTangent;
uniform bool			uLightsEnabled[cMaxLights];
uniform samplerCube		uShadowCubeMaps[cMaxLights];
uniform bool			uLightingEnabled;
uniform bool			uNormalMapEnabled;
uniform sampler2D		uNormalMap;
uniform sampler2D		uShadowMaps;

uniform mat4			uLightProjViewMats[cMaxLights];

attribute vec4			aPosition;
attribute vec4			aColor;
attribute vec2			aTexCoord;
//attribute vec3			aNormal;
//attribute vec3			aTangent;

varying vec4			vColor;
varying vec4			vWorldPos;
varying vec4			vDiffuses[cMaxLights];
//varying vec4			vambientGlobal;
varying vec4			vAmbients[cMaxLights];
varying vec4			vShadowPos[cMaxLights];
varying vec3			vhalfVector;
varying vec3			vNormal;
varying vec2			vTexCoord;


/*
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
*/

void calculateLighting(void) {
	int i;

	//Accumulate colors from each active light source
	for(i = ciZero; i < cMaxLights; ++i) {
		if(uLightsEnabled[i]) {
			vShadowPos[i] = uLightProjViewMats[i] * aPosition;
			vAmbients[i] = uLights[i].ambientColor * uMaterial.ambientColor;
			vDiffuses[i] = uLights[i].diffuseColor * uMaterial.diffuseColor;
		}
	}

	//vertexColor.a = uMaterial.diffuseColor.a;
	//return vertexColor;
}

void main(void) {
	gl_Position = uProjViewMat * aPosition;
	gl_Position.xyz /= gl_Position.w;
	vColor = aColor;
	vWorldPos = aPosition;
	vNormal = uNormal;
	vTexCoord = aTexCoord;
	if(uLightingEnabled) calculateLighting();
}
