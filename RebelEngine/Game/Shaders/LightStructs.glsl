
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