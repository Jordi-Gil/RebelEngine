#version 460

out vec4 color;

in vec2 uv0;

uniform sampler2D mytexture;
uniform bool textureEnabled;


void main() {

	if(textureEnabled) color = texture(mytexture, uv0);
	else color = vec4(0.7, 0.7, 0.7, 1.0);
}
