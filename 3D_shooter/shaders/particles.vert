#version 410 core

layout(location = 0) in vec3 position;

uniform mat4 uView;
uniform mat4 uProjection;

uniform vec3 uParticlePosition;
uniform float uParticleSize;

out vec2 localPosition;

void main()
{
    // Transform the particle center from world space into view space.
    vec4 centerView =
        uView * vec4(uParticlePosition, 1.0);

    // The unit square ranges from -0.5 to +0.5.
    // Apply the particle size while we're in view space.
    vec2 offset =
        position.xy * uParticleSize;

    // Build the billboard vertex around the particle center.
    //
    // Because this offset is added in view space,
    // X corresponds to camera-right and Y to camera-up.
    // That makes the quad always face the camera.
    vec4 vertexView =
        centerView +
        vec4(offset.x, offset.y, 0.0, 0.0);

    gl_Position =
        uProjection * vertexView;

    // Convert the square coordinates from:
    //
    // [-0.5, +0.5]
    //
    // to:
    //
    // [-1.0, +1.0]
    //
    // for calculating the circular particle shape.
    localPosition =
        position.xy * 2.0;
}