#version 460

uniform mat4 matGeo;
uniform mat4 matView;
uniform mat4 matProj;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;


out vec2 uvF; 
out vec3 fragPosition;
out vec3 fragNormal;


void main() {

   	uvF = uv;
   	fragNormal = transpose(inverse(mat3(matGeo)))*normal;
   	fragPosition = (matGeo * vec4(position, 1.0)).xyz;
  	gl_Position =  matProj * matView * matGeo * vec4(position, 1.0);
}
