// This vertex shader needs to remove translation from the camera view
// this is the key to the skybox trick - we want the galaxy to rotate 
// when the camera rotates, but not move closer/father when the camera translates!

#version 410 core

layout(location = 0) in vec3 position;
out vec3 textureDirection;

uniform mat4 uView;
uniform mat4 uProjection;

void main()
{
    textureDirection = vec3(
        position.x,
        position.z,
        position.y
    );

    mat4 rotationOnlyView = mat4(mat3(uView));
    vec4 clipPosition = uProjection * rotationOnlyView * vec4(position, 1.0);

    // Force the skybox depth to the far plane
    gl_Position = clipPosition.xyww;
}