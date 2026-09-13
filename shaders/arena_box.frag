#version 410 core

in float arenaDepth;

out vec4 fragColor;

void main()
{
    float depthFactor = (arenaDepth + 50.0) / 100.0;

    vec3 frontColor = vec3(0.0, 1.0, 1.0);
    vec3 backColor = vec3(0.0, 0.25, 0.45);

    vec3 finalColor = mix(frontColor, backColor, depthFactor);

    fragColor = vec4(finalColor, 1.0);
}