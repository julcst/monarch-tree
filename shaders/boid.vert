#version 330 core

layout (location = 0) in vec3 _position;

uniform mat4 uWorldToClip;

out vec3 normal;

struct Boid {
    vec3 position;
    float phase;
    vec3 velocity;
    float excitement;
};

layout(std140) uniform Swarm {
    Boid[1024] uBoids;
};

mat3 buildOrthonormalBasis(vec3 forward, vec3 up) {
    vec3 localUp = normalize(up - forward * dot(forward, up));
    vec3 right = cross(localUp, forward);
    return mat3(right, localUp, forward);
}

void main() {
    Boid boid = uBoids[gl_InstanceID];

    vec3 forward = normalize(boid.velocity);
    vec3 up = vec3(0.0, 1.0, 0.0);
    mat3 rotation = buildOrthonormalBasis(forward, up);

    normal = rotation[1];
    gl_Position = uWorldToClip * vec4(rotation * _position + boid.position, 1.0);
}