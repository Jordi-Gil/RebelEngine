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

#define NUM_POINT_LIGHT 8

struct Lights {

	AmbientLight ambientlight;
	DirectionalLight directionalLight;

	PointLight pointsLight[NUM_POINT_LIGHT];
	unsigned int num_points;

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

	vec3 L = normalize(_light.direction - fragPosition);
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

vec3 Calculate_PointLight_Part(unsigned int i){

	return vec3(0.0);

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
		float d = distance(lights.pointsLight[i].position, fragPosition);

		Lo += Calculate_PointLight_Part(i);
	}

	return vec4(Lo, 1);

}

void main() {
   outColor = pow(PBR_Phong(), vec4(1/2.2));
   //outColor = texture(specular_material.diffuse_map, uvF);
}