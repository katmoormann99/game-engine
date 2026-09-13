#version 410 core

in vec3 textureDirection;
uniform samplerCube uSkybox;
out vec4 fragColor;

void main()
{
    fragColor = texture(uSkybox, textureDirection);
}