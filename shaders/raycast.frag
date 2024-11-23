#version 410 core
in vec3 viewDir;
out vec4 fragColor;

///////////////////// Uniforms /////////////////////
uniform vec3 uCameraPosition; // Camera position
uniform mat4 uWorldToClip; // World to clip space matrix

#include "scene.glsl"

void main() {
    vec3 rayOrigin = uCameraPosition;
    vec3 rayDirection = normalize(viewDir);
    Ray ray = Ray(rayOrigin, rayDirection, 1.0 / rayDirection);

    vec4 hit = iScene(ray);
    
    if (isHit(hit)) {
        float d = hit.x;
        vec3 n = hit.yzw;

        fragColor = vec4(n * 0.5 + 0.5, 1.0);

        vec3 worldPos = rayOrigin + rayDirection * d;
        vec4 clipPos = uWorldToClip * vec4(worldPos, 1.0);
        gl_FragDepth = (clipPos.z / clipPos.w) * 0.5 + 0.5;
    } else {
        fragColor = vec4(rayDirection * 0.5 + 0.5, 1.0);
        gl_FragDepth = 1.0;
    }
}