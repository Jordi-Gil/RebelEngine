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