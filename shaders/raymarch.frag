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

float uSteps = 100.0;
float uEpsilon = 0.01;

#include "random.glsl"
#include "intersection.glsl"
#include "scene.glsl"

vec2 raymarch(vec3 rayOrigin, vec3 rayDirection, float near, float far) {
    float depth = near;
    // Advance depth until far plane is reached, steps are exhausted or we hit the scene
    for (float steps = 0.0; steps < uSteps; steps++) {
        if (depth > far) return vec2(-1.0, steps); // Break if we exceed far plane
        vec3 p = rayOrigin + depth * rayDirection;
        float delta = sdScene(p); // Get distance to scene
        if (delta < uEpsilon) return vec2(depth, steps); // Test if we hit the scene
        depth += delta; // Otherwise, advance depth
    }
    return vec2(depth, uSteps); // Return our approximation
}

void main() {
    vec2 uv = (2.0 * gl_FragCoord.xy - uRes.xy) / uRes.y;

    vec3 rayDirection = uCameraRotation * normalize(vec3(uv, uFocalLength));
    vec3 rayOrigin = uCameraPosition;

    vec2 nearFar = iAABB(rayOrigin, rayDirection, uAABBCenter, uAABBSize);
    vec2 result = vec2(-1.0, 0.0);

    if (!isinf(nearFar.x)) {
        if (hash3u(uvec3(gl_FragCoord.xy, uFrames)).x % 10 == 0) {
            result = raymarch(rayOrigin, rayDirection, nearFar.x, nearFar.y);
        } else discard;
    }
    fragColor = result.x < 0.0 ? vec4(abs(rayDirection), 1.0) : vec4(vec3(result.y / uSteps), 1.0);
    //fragColor = result.x < 0.0 ? vec4(0.5, 0.7, 1.0, 1.0) : vec4(vec3(result.y / uSteps), 1.0);
    //fragColor = vec4(vec3(hash2u(uvec2(gl_FragCoord.xy)).x % 10 == uFrames % 10), 1.0);
    //fragColor = vec4(vec3((uFrames % 60) / 60.0), 1.0);
    //fragColor = vec4(vec3(result.y / uSteps), 1.0);
    //fragColor = result.x < 0.0 ? vec4(0.5, 0.7, 1.0, 1.0) : vec4(vec3(result.y / uSteps), 1.0);
}