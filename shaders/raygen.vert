#version 330 core

layout (location = 0) in vec3 _position;

out vec3 viewDir;

uniform mat4 uClipToWorld;
uniform vec3 uCameraPosition;

/**
 * Calculates the ray direction for every vertex, which is then hardware interpolated to calculate the per pixel ray direction
 */
void main() {
    /* Just passing the vertex position */
    gl_Position = vec4(_position, 1.0);

    /* To minimize shader workload we utilize hardware interpolation to generate the ray direction in world space per fragment. */
    vec4 clipPos = vec4(_position.x, _position.y, -1.0, 1.0);
    vec4 worldPos = uClipToWorld * clipPos;
    viewDir = worldPos.xyz / worldPos.w - uCameraPosition;
}