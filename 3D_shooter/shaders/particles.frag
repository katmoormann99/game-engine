#version 410 core

in vec2 localPosition;

uniform vec3 uParticleColor;
uniform float uParticleAlpha;

out vec4 fragColor;

void main()
{
    // Distance from the center of the billboard.
    float radius =
        length(localPosition);

    // Throw away pixels outside the circle.
    if (radius >= 1.0)
    {
        discard;
    }

    // Soft fade from the center toward the edge.
    float radialAlpha =
        1.0 -
        smoothstep(
            0.2,
            1.0,
            radius
        );

    float alpha =
        radialAlpha *
        uParticleAlpha;

    if (alpha <= 0.01)
    {
        discard;
    }

    fragColor =
        vec4(
            uParticleColor,
            alpha
        );
}