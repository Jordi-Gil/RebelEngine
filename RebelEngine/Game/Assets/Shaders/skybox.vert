#version 460 core

layout (location = 0) in vec3 position;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = vec3(-position.x, position.yz);
    vec4 pos = projection * vec4(mat3(view) * position, 1.0);
    gl_Position = pos.xyww;
}  