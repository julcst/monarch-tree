#version 410 core
in vec3 viewDir;
out vec4 fragColor;

///////////////////// Uniforms /////////////////////
uniform uint uFrames; // Frame counter
uniform vec3 uCameraPosition; // Camera position

#include "scene.glsl"

void main() {
    vec3 rayDirection = normalize(viewDir);
    Ray ray = Ray(uCameraPosition, rayDirection, 1.0 / rayDirection);

    vec4 hit = iScene(ray);
    
    if (isHit(hit)) {
        float d = hit.x;
        vec3 n = hit.yzw;

        fragColor = vec4(n * 0.5 + 0.5, 1.0);
    } else {
        fragColor = vec4(rayDirection * 0.5 + 0.5, 1.0);
    }
}