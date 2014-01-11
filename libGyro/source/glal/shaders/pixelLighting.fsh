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
uniform bool			uLightsEnabled[cMaxLights];
uniform bool			uLightingEnabled;
uniform bool			uNormalMapEnabled;
uniform sampler2D		uNormalMap;
uniform vec3			uNormal;
uniform vec3			uTangent;
uniform sampler2D		uShadowMaps[cMaxLights];

uniform sampler2D uTexture;

varying vec4			vColor;
varying vec4			vWorldPos;
varying vec4			vDiffuses[cMaxLights];
//varying vec4			vambientGlobal;
varying vec4			vAmbients[cMaxLights];
varying vec4			vShadowPos[cMaxLights];
varying vec3			vhalfVector;
varying vec3			vNormal;
varying vec2			vTexCoord;

vec3 normal;

vec4 lightingEquation(const int i) {
	vec4 computedColor = vec4(cfZero, cfZero, cfZero, cfZero);
	vec3 halfVec;
	vec3 VPpli;
	float nDotL, nDotH;
	float attFactor = cfOne;
	float shadowFactor = cfOne;

	if(uLights[i].position.w != cfZero) {		//Point Light
		vec3 attDist;
		float spotFactor;

		//Direction of light
		VPpli = uLights[i].position.xyz - vWorldPos.xyz;
		//if() {
			attDist.x = cfOne;
			attDist.z = dot(VPpli, VPpli);
			attDist.y = sqrt(attDist.z);
			attFactor = cfOne / dot(attDist, uLights[i].attenuationFactors);
		//}
		VPpli = normalize(VPpli);

		//Spot light
		if(uLights[i].spotCutoffAngle < cMaxSpotCutoffAngle) {
			spotFactor = dot(-VPpli, uLights[i].direction);

			if(spotFactor >= cos((radians(uLights[i].spotCutoffAngle))))
				spotFactor = pow(spotFactor, uLights[i].spotExponent);
			else
				spotFactor = cfZero;

			//WRONG, THIS WILL AFFECT AMBIENCE!!!!
			attFactor *= spotFactor;

			if(texture2D(uShadowMaps[i], (vShadowPos[i].xy)/vShadowPos[i].w).z < ((vShadowPos[i].z-0.005)/vShadowPos[i].w)) {
				shadowFactor = 0.5;
			}
		}
	} else {								//Directional Light
		//VPpli = -uLights[i].position.xyz;	//MUST BE PRENORMALIZED
		VPpli = -uLights[i].direction.xyz;

		if(texture2D(uShadowMaps[i], vShadowPos[i].xy).z < vShadowPos[i].z) {
			shadowFactor = 0.5;
		}
	}

	//Process lighting equation
	if(attFactor > cfZero) {
		//Calculate and apply diffuse lighting
		nDotL = max(cfZero, dot(normal, VPpli));
		computedColor += (nDotL * vDiffuses[i]);

		//Calculate and apply specular lighting
		halfVec = normalize(VPpli + vec3(cfZero, cfZero, cfOne));
		nDotH = dot(normal, halfVec);
		if(nDotH > cfZero) {
			computedColor += (pow(nDotH, uMaterial.shininess) *
								uMaterial.specularColor *
								uLights[i].specularColor);
		}

		//computedColor *= shadowFactor;

	}

	computedColor *= attFactor;

	//Calculate and apply ambient lighting
	computedColor += vAmbients[i];


	computedColor *= shadowFactor;

	return computedColor;

}


void main(void) {
	vec4 color = vColor + uAmbience;
	int i;

	if(uNormalMapEnabled) {

		mat3 TBNMat = mat3(uTangent, normalize(cross(uTangent, uNormal)), uNormal);

		normal = texture2D(uNormalMap, vTexCoord).xyz;
		//normal = TBNMat * normal;

		//normal.x = -normal.x;
		//normal.z = -normal.z;
		//normal.y = -normal.y;
		//normal = 2.0*(normal-0.5);
		//normal.z = -normal.z;
		//float temp = normal.y;
		//normal.y = -normal.z;
		//normal.z = temp;
		normal = normalize(normal*2.0-1.0);
		//normal.y = -normal.y;
		normal = TBNMat * normal;
		normal.x = -normal.x;
		//normal.z = -normal.z;
		//normal = normalize(normal);
		//normal = normalize(normal*2.0)-1.0;

	} else {
		//normal = vec3(0.0, 0.0, 1.0);
		normal = uNormal;
	}

	for(i = 0; i < cMaxLights; ++i) {
		if(uLightsEnabled[i]) {
			color += lightingEquation(i);
		}
	}

	//if(!uNormalMapEnabled)
	gl_FragColor = texture2D(uTexture, vTexCoord) * color;
	//else
	//gl_FragColor = texture2D(uNormalMap, vTexCoord) * color;


	//gl_FragColor = /*texture2D(uTexture, vTexCoord) **/ vColor;
}
