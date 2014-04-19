const int	cMaxLights				= 4;
const float cfZero					= 0.0;
const float cfOne					= 1.0;
const int	ciZero					= 0;
const float cMaxSpotCutoffAngle		= 180.0;
const vec3	cViewVector				= vec3(0.0, 0.0, 1.0);
const vec3	cFlatNormal				= vec3(0.0, 0.0, 1.0);

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
uniform bool			uLightsEnabled[cMaxLights];
uniform samplerCube		uShadowCubeMaps[cMaxLights];
uniform bool			uLightingEnabled;
uniform bool			uTextureEnabled;
uniform bool			uNormalMapEnabled;
uniform bool			uSpecularMapEnabled;
uniform sampler2D		uNormalMap;
uniform sampler2D		uSpecularMap;
uniform vec3			uNormal;
uniform vec3			uTangent;
uniform sampler2D		uShadowMaps[cMaxLights];

uniform mat4			uLightProjViewMats[cMaxLights];
uniform mat4			uShadowCubeMapProjMat;

uniform sampler2D uTexture;

varying vec4			vColor;
varying vec4			vWorldPos;
//varying vec4			vDiffuses[cMaxLights];
//varying vec4			vambientGlobal;
//varying vec4			vAmbients[cMaxLights];
//varying vec4			vShadowPos[cMaxLights];
varying vec3			vNormal;
varying vec2			vTexCoord;

//Old varyings
vec4			diffuses[cMaxLights];
vec4			ambients[cMaxLights];
//vec4			shadowPos[cMaxLights];

//cached pixel lighting values.
vec3	normal;
float	shininess;
vec4	specularity;
vec3	viewDir;

/*
void calculateLighting(void) {
	int i;

	//Accumulate colors from each active light source
	for(i = ciZero; i < cMaxLights; ++i) {
		if(uLightsEnabled[i]) {
			shadowPos[i] = uLightProjViewMats[i] * vWorldPos;
			ambients[i] = uLights[i].ambientColor * uMaterial.ambientColor;
			diffuses[i] = uLights[i].diffuseColor * uMaterial.diffuseColor;
		}
	}

	//vertexColor.a = uMaterial.diffuseColor.a;
	//return vertexColor;
}*/

vec4 lightingEquation(const int i) {
	vec4 shadowPos = uLightProjViewMats[i] * vWorldPos;
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

			//WRONG, THIS WILL AFFECT AMBIENCE!!!! ...wut?
			attFactor *= spotFactor;

			if(texture2D(uShadowMaps[i], (shadowPos.xy)/shadowPos.w).z < ((shadowPos.z-0.005)/shadowPos.w)) {
				shadowFactor = 0.5;
			}
		} else { //point light cube shadows

			vec4 absPos = abs(shadowPos);
			float frus_z = -max(absPos.x, max(absPos.y, absPos.z));
			vec4 clip = uShadowCubeMapProjMat * vec4(0.0, 0.0, frus_z, 1.0);
			float depth = (clip.z/clip.w) * 0.5 + 0.5;

			if(textureCube(uShadowCubeMaps[i], shadowPos.xyz).r < depth) {
				shadowFactor = 0.5;
			}
/*
			float shadowDepth = textureCube(uShadowCubeMaps[i], vShadowPos[i].xyz).r;
			if(shadowDepth < VectorToDepthValue(vShadowPos[i].xyz)) {
				shadowFactor = 0.1;
			}*/

			//shadowFactor = texture(uShadowCubeMaps[i], vShadowPos[i]);

			//vec4 clip = uShadowCubeMapProjMat * vec4(0.0, 0.0, frus_z, 1.0);

		}
	} else {								//Directional Light
		//VPpli = -uLights[i].position.xyz;	//MUST BE PRENORMALIZED
		VPpli = -uLights[i].direction.xyz;

		if(texture2D(uShadowMaps[i], shadowPos.xy).z < shadowPos.z) {
			shadowFactor = 0.5;
		}
	}

	//Process lighting equation
	if(attFactor > cfZero) {
		//Calculate and apply diffuse lighting
		nDotL = max(cfZero, dot(normal, VPpli));
		computedColor += (nDotL * uLights[i].diffuseColor * uMaterial.diffuseColor);

		//Calculate and apply specular lighting
		halfVec = normalize(VPpli + viewDir);
		nDotH = dot(normal, halfVec);
		if(nDotH > cfZero) {
			computedColor += (pow(nDotH, shininess) *
								specularity *
								uLights[i].specularColor);
		}

		//computedColor *= shadowFactor;

	}

	computedColor *= attFactor;
	computedColor *= shadowFactor;

	//Calculate and apply ambient lighting
	computedColor += uLights[i].ambientColor * uMaterial.ambientColor;

	return computedColor;

}


void main(void) {
	vec4 color = vColor * uAmbience;
	vec4 texColor = vec4(cfOne);
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

	viewDir = normalize(uCamWorldPos - vWorldPos.xyz);
	specularity = uMaterial.specularColor;
	shininess = uMaterial.shininess;
	if(uSpecularMapEnabled) {
		vec4 specMapVal = texture2D(uSpecularMap, vTexCoord);
		specularity.rgba *= specMapVal.rgba; //dunno if a is supposed to be factored in here.
		//.shininess += specularity.a;
	}

	for(i = 0; i < cMaxLights; ++i) {
		if(uLightsEnabled[i]) {
			color += lightingEquation(i);
		}
	}

	if(uTextureEnabled) {
		texColor = texture2D(uTexture, vTexCoord);
	}

	vec4 finalColor = texColor * color;
	if(finalColor.a == 0.0) discard;

	//if(!uNormalMapEnabled)
	gl_FragColor =  finalColor;
	//else
	//gl_FragColor = texture2D(uNormalMap, vTexCoord) * color;


	//gl_FragColor = /*texture2D(uTexture, vTexCoord) **/ vColor;
}
