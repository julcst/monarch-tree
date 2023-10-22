#version 410 core
layout (location = 0) in vec3 position;

uniform float uAspectRatio; // Window aspect ratio
uniform mat3 uCameraRotation; // Camera rotation matrix (3x3)
uniform float uFocalLength; // Focal length of the camera

out vec3 viewDir;

void main() {
    gl_Position = vec4(position, 1.0);
    vec2 uv = vec2(position.x * uAspectRatio, position.y);
    viewDir = uCameraRotation * vec3(uv, uFocalLength);
}