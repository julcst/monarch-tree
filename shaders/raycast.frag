#version 410 core
in vec3 viewDir;
out vec4 fragColor;

///////////////////// Uniforms /////////////////////
uniform vec3 uCameraPosition; // Camera position
uniform mat4 uWorldToClip; // World to clip space matrix
uniform vec3 uLightDir = normalize(vec3(1.0, 1.0, 1.0));
uniform vec3 uColor = vec3(0.7, 0.45, 0.3);
uniform vec3 uSkyColor = vec3(0.1, 0.3, 0.6);

#include "scene.glsl"

vec3 proceduralSky(vec3 rayDir) {
    return exp(-max(rayDir.y, 0.05) / uSkyColor);
}

vec3 proceduralSun(vec3 rayDir) {
    return pow(max(0.0, dot(rayDir, uLightDir)), 1024) * vec3(1.0);
}

void main() {
    vec3 rayOrigin = uCameraPosition;
    vec3 rayDirection = normalize(viewDir);
    Ray ray = Ray(rayOrigin, rayDirection, 1.0 / rayDirection);

    vec4 hit = iScene(ray);
    
    if (isHit(hit)) {
        float d = hit.x;
        vec3 n = hit.yzw;

        float cosTheta = max(dot(n, uLightDir), 0.0);
        fragColor = vec4(cosTheta * uColor + uSkyColor * 0.1, 1.0);

        vec3 worldPos = rayOrigin + rayDirection * d;
        vec4 clipPos = uWorldToClip * vec4(worldPos, 1.0);
        gl_FragDepth = (clipPos.z / clipPos.w) * 0.5 + 0.5;
    } else {
        float angle = ray.direction.y;
        vec3 sky = proceduralSky(ray.direction) + proceduralSun(ray.direction);
        fragColor = vec4(sky, 1.0);
        gl_FragDepth = 1.0;
    }
}