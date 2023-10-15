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

    float d = iScene(rayOrigin, rayDirection);

    fragColor = hit(d) ? vec4(vec3(0.0), 1.0) : vec4(abs(rayDirection), 1.0);
}