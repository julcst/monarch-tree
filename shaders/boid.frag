#version 330 core

in vec3 normal;
out vec4 fragColor;

const vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));

void main() {
    vec3 N = normalize(normal) * (gl_FrontFacing ? 1.0 : -1.0);
    fragColor = vec4(N * 0.5 + 0.5, 1.0);
}