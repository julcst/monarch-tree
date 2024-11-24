#version 330 core

in vec3 normal;
out vec4 fragColor;

uniform vec3 uLightDir = normalize(vec3(1.0, 1.0, 1.0));
uniform vec3 uTopColor = vec3(1.0, 0.4, 0.0);
uniform vec3 uBottomColor = vec3(0.7, 0.4, 0.0);
uniform vec3 uTransmission = vec3(0.5);

void main() {
    vec3 N = normalize(normal) * (gl_FrontFacing ? 1.0 : -1.0);
    float cosTheta = dot(N, uLightDir);
    bool isTransmission = cosTheta < 0.0;
    vec3 color;
    if (isTransmission) {
        color = uTopColor * uBottomColor * uTransmission;
    } else {
        color = gl_FrontFacing ? uTopColor : uBottomColor;
    }
    fragColor = vec4(color * abs(cosTheta), 1.0);
}