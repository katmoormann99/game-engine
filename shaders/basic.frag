#version 410 core

in vec3 worldPosition;
in vec3 worldNormal;

uniform vec3 uMaterialColor;

uniform vec3 uLightPosition;
uniform vec3 uLightColor;
uniform float uLightIntensity;

uniform vec3 uLightDirection;
uniform float uInnerCutoff;
uniform float uOuterCutoff;

out vec4 fragColor;

void main()
{
    vec3 N = normalize(worldNormal);
    vec3 L = normalize(uLightPosition - worldPosition);

    float diffuse = max(dot(N, L), 0.0);

    vec3 lightToFragment = normalize(worldPosition - uLightPosition);

    float theta = dot(lightToFragment, normalize(uLightDirection));

    float spot =
        smoothstep(
            uOuterCutoff,
            uInnerCutoff,
            theta
        );

    // Always-visible baseline room illumination.
    vec3 ambient = uMaterialColor * 1.80;

    // Directional Lambert illumination throughout the room.
    vec3 diffuseLight = uMaterialColor * uLightColor * diffuse * 0.50;

    // Additional concentrated flashlight beam.
    vec3 flashlight = uMaterialColor * uLightColor * diffuse * spot * uLightIntensity;

    vec3 finalColor = ambient + diffuseLight + flashlight;
    
    fragColor = vec4(finalColor, 1.0);
}