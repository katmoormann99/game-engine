#version 410 core

layout(location = 0) in vec2 aPosition;

uniform vec2 uCrosshairPosition;

void main()
{
    gl_Position = vec4(aPosition + uCrosshairPosition, 0.0, 1.0);
}