#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat4 uNormalMatrix;

out vec3 worldPosition;
out vec3 worldNormal;

void main()
{
    vec4 worldPos =
        uModel * vec4(position, 1.0);

    worldPosition =
        worldPos.xyz;

    worldNormal =
        normalize(
            mat3(uNormalMatrix) * normal
        );

    gl_Position =
        uProjection *
        uView *
        worldPos;
}