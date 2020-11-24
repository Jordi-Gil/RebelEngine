#version 460

out vec4 color;

in vec2 uv0;

uniform sampler2D mytexture;
uniform bool textureEnabled;


void main() {

	
	if(textureEnabled) color = texture2D(mytexture, uv0);
	else color = vec4(1.0, 0.0, 0.0, 1.0);
}
