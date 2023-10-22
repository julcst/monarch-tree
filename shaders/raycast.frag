#version 410 core
in vec3 viewDir;
out vec4 fragColor;

///////////////////// Uniforms /////////////////////
uniform uint uFrames; // Frame counter
uniform vec3 uCameraPosition; // Camera position
uniform vec3 uAABBCenter;
uniform vec3 uAABBSize;

#include "scene.glsl"

void main() {
    vec3 rayDirection = normalize(viewDir);

    vec4 hit = iScene(uCameraPosition, rayDirection);
    
    if (isHit(hit)) {
        float d = hit.x;
        vec3 n = hit.yzw;

        fragColor = vec4(n, 1.0);
    } else {
        fragColor = vec4(abs(rayDirection.zyx), 1.0);
    }
}