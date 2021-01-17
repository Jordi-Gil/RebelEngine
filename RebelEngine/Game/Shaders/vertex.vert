#version 460

layout(location=0) in vec3 position;
layout(location = 2) in vec2 vertex_uv0;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 uv0;

void main() {

	gl_Position = projection*view*model*vec4(position, 1.0);
	uv0 = vertex_uv0;

}