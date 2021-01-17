#version 460

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 uvF;

out vec4 outColor;

struct BaseLight {

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Attenuation {

	float constant;
	float linear;
	float quadratic;

};

struct AmbientLight{
	vec3 ambient;
};

struct DirectionalLight{

	float intensity;
	vec3 direction;
	BaseLight baseLight;
};

struct PointLight{

	vec3 position;
	float intensity; 
	Attenuation att;
	BaseLight baseLight;
};

struct SpotLight{

	float intensity;
	float innerAngle;
	float outterAngle;
	vec3 position;
	vec3 aim;
	Attenuation att;
	BaseLight baseLight;

};

#define NUM_POINT_LIGHTS 8
#define NUM_SPOT_LIGHTS 8

struct Lights {

	AmbientLight ambientlight;
	DirectionalLight directionalLight;

	PointLight pointLights[NUM_POINT_LIGHTS];
	unsigned int num_points;

	SpotLight spotLights[NUM_SPOT_LIGHTS];
	unsigned int num_spots;

};

struct StandardSpecular {

	sampler2D diffuse_map;
	vec3 diffuse_color;

	sampler2D specular_map;
	vec3 specular_color;

	sampler2D normal_map;

	unsigned int has_diffuse_map;
	unsigned int has_specular_map;
	unsigned int has_normal_map;

};

uniform StandardSpecular specular_material;
uniform Lights lights;

uniform vec3 cameraPosition;

//uniform float 
float Kd = 0.5;
float Ks = 1;

//uniform float n; //Shininess
float n = 80;

float refractIdx = 2.2;

vec3 matDiffuse, matSpecular, matNormal;

vec3 toLinear(vec3 map){
	return pow(map,vec3(2.2));
}

//  [(n1 - n2) / (n1 + n2)]^2
// assuming n1 the vacuum
float Schlick0(){
	return	pow((1-refractIdx)/(1+refractIdx), 2.0);
}

float Schlick(float angle){
	return Schlick0() + (1 - Schlick0()) * pow((1-cos(angle)),5);
}

vec3 Calculate_DirectionalLight_Part(DirectionalLight _light, vec3 N, vec3 V){

	//Ambient Light
	vec3 Cd = lights.ambientlight.ambient;
	vec3 Li = _light.baseLight.diffuse * _light.intensity;

	vec3 diffuse = Cd * matDiffuse * Kd;

	vec3 L = normalize(-_light.direction);
	vec3 R = normalize(L - (2 * dot(L,N) * N));

	float VdotR = max(0.0, dot(V, R));
	float NdotL = max(0.0, dot(N, -L));

	vec3 specular = matSpecular * Ks;
	vec3 fresnel0 = specular * Schlick0();
	vec3 fresnelT = specular * Schlick(NdotL);

	//Lo = (Cd * (1-Rf(0)) + n+2/2 * Rf(theta) * (V dot R)^n ) * Li * (N dot L)
	//Lo = [                  p1   * Rf(theta) *       VdotR   * Li *   NdotL
	
	vec3 BRDF = (
			diffuse * (1 - fresnel0) + 
			(n+2)/2 * fresnelT * pow(VdotR, n));

	vec3 Lo = BRDF * Li * NdotL;

	return Lo;

}

vec3 Calculate_PointLight_Part(PointLight _light, vec3 N, vec3 V){

	float dist = length(fragPosition - _light.position);

	float Kc = _light.att.constant;
	float Kl = _light.att.linear;
	float Kq = _light.att.quadratic;

	float Fatt = 1/(Kc + (Kl * dist) + (Kq * dist * dist));

	vec3 Cd = lights.ambientlight.ambient;
	vec3 Li = _light.baseLight.diffuse * _light.intensity * Fatt;

	vec3 diffuse = Cd * matDiffuse * Kd;

	vec3 L = normalize(fragPosition - _light.position);
	vec3 R = normalize(L - (2 * dot(L,N) * N));

	float VdotR = max(0.0, dot(V, R));
	float NdotL = max(0.0, dot(N, -L));

	vec3 specular = matSpecular * Ks;
	vec3 fresnel0 = specular * Schlick0();
	vec3 fresnelT = specular * Schlick(NdotL);

	//Lo = (Cd * (1-Rf(0)) + n+2/2 * Rf(theta) * (V dot R)^n ) * Li * (N dot L)
	//Lo = [                  p1   * Rf(theta) *       VdotR   * Li *   NdotL
	
	vec3 BRDF = (
			diffuse * (1 - fresnel0) + 
			(n+2)/2 * fresnelT * pow(VdotR, n));

	vec3 Lo = BRDF * Li * NdotL;

	return Lo;

}

vec3 Calculate_SpotLight_Part(SpotLight _light, vec3 N, vec3 V) {
	
	vec3 D = normalize(fragPosition - _light.position);
	float C = dot(_light.aim, D);

	float dist = length(fragPosition - _light.position);

	float Kc = _light.att.constant;
	float Kl = _light.att.linear;
	float Kq = _light.att.quadratic;

	float Fatt = 1/(Kc + (Kl * dist) + (Kq * dist * dist));
	float Catt = 1;

	float cOutter = cos(_light.outterAngle);
	float cInner = cos(_light.innerAngle);

	if(C > cInner) Catt = 1;
	else if( cInner > C &&  C > cOutter ) {
		Catt = (C - cOutter) / (cInner - cOutter);
	}

	vec3 Cd = lights.ambientlight.ambient;
	vec3 Li = _light.baseLight.diffuse * _light.intensity * Fatt * Catt;

	vec3 diffuse = Cd * matDiffuse * Kd;

	vec3 L = normalize(fragPosition - _light.position);
	vec3 R = normalize(L - (2 * dot(L,N) * N));

	float VdotR = max(0.0, dot(V, R));
	float NdotL = max(0.0, dot(N, -L));

	vec3 specular = matSpecular * Ks;
	vec3 fresnel0 = specular * Schlick0();
	vec3 fresnelT = specular * Schlick(NdotL);

	//Lo = (Cd * (1-Rf(0)) + n+2/2 * Rf(theta) * (V dot R)^n ) * Li * (N dot L)
	//Lo = [                  p1   * Rf(theta) *       VdotR   * Li *   NdotL
	
	vec3 BRDF = (
			diffuse * (1 - fresnel0) + 
			(n+2)/2 * fresnelT * pow(VdotR, n));

	vec3 Lo = BRDF * Li * NdotL;

	return Lo;
}

vec4 PBR_Phong(){

	if(specular_material.has_diffuse_map == 1) matDiffuse = toLinear(texture(specular_material.diffuse_map, uvF).rgb);
	else matDiffuse = specular_material.diffuse_color;
	
	if(specular_material.has_specular_map == 1) matSpecular = toLinear(texture(specular_material.specular_map, uvF).rgb);
	else matSpecular = specular_material.specular_color;
	
	if(specular_material.has_normal_map == 1) matNormal = toLinear(texture(specular_material.normal_map, uvF).rgb);
	else matNormal = fragNormal;

	vec3 N = normalize(fragNormal);
	vec3 V = normalize(cameraPosition - fragPosition);
	vec3 Lo = vec3(0.0);

	Lo += Calculate_DirectionalLight_Part(lights.directionalLight, N, V);
	
	for(unsigned int i = 0; i < lights.num_points; i++){
		Lo += Calculate_PointLight_Part(lights.pointLights[i], N, V);
	}

	for(unsigned int i = 0; i < lights.num_spots; i++){
		Lo += Calculate_SpotLight_Part(lights.spotLights[i], N, V);
	}

	return vec4(Lo, 1);

}

void main() {
   outColor = pow(PBR_Phong(), vec4(1/2.2));
}