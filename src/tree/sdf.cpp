#include "sdf.hpp"

#include "glm/glm.hpp"

using namespace glm;

const float EPS = 0.01f;
const vec3 SAMPLES[] = {
    vec3( 1.0, -1.0, -1.0),
    vec3(-1.0, -1.0,  1.0),
    vec3(-1.0,  1.0, -1.0),
    vec3( 1.0,  1.0,  1.0),
};

/*
 * Calculate gradient using tetrahedron method as pointed out by Inigo Quilez
 * (https://iquilezles.org/articles/normalsSDF/) to reduce the number of evaluations
 */
vec3 SDF::calcGradient(const vec3 position) {
    return SAMPLES[0] * calcSignedDistance(position + EPS * SAMPLES[0])
         + SAMPLES[1] * calcSignedDistance(position + EPS * SAMPLES[1])
         + SAMPLES[2] * calcSignedDistance(position + EPS * SAMPLES[2])
         + SAMPLES[3] * calcSignedDistance(position + EPS * SAMPLES[3]);
}

vec3 SDF::calcNormal(const vec3 position) {
    return normalize(calcGradient(position));
}