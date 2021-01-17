#pragma include "LightStructs.glsl"

#define NUM_POINT_LIGHT 8

struct Lights {

	AmbientLight ambientlight;
	DirectionalLight directional;

	PointLight pointsLight[NUM_POINT_LIGHT];
	unsigned int num_points;

};