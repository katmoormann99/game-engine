#version 410 core

layout(location = 0) in vec3 position;

uniform mat4 uView;
uniform mat4 uProjection;

out float arenaDepth;

void main()
{
    arenaDepth = position.y;
    gl_Position = uProjection * uView * vec4(position, 1.0);
}