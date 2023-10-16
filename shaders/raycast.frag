#version 410 core
out vec4 fragColor;

///////////////////// Uniforms /////////////////////
uniform vec2 uRes; // Window resolution
uniform float uT; // Time since start in seconds
uniform uint uFrames; // Frame counter
uniform vec3 uCameraPosition; // Camera position
uniform mat3 uCameraRotation; // Camera rotation matrix (3x3)
uniform float uFocalLength; // Focal length of the camera
uniform vec3 uAABBCenter;
uniform vec3 uAABBSize;

#include "scene.glsl"

void main() {
    vec2 uv = (2.0 * gl_FragCoord.xy - uRes.xy) / uRes.y;

    vec3 rayOrigin = uCameraPosition;
    vec3 rayDirection = uCameraRotation * normalize(vec3(uv, uFocalLength));

    vec4 hit = iScene(rayOrigin, rayDirection);

    if (isHit(hit)) {
        float d = hit.x;
        vec3 n = hit.yzw;

        fragColor = vec4(n, 1.0);
    } else {
        fragColor = vec4(abs(rayDirection.zyx), 1.0);
    }
}