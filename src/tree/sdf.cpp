#include "sdf.hpp"

#include "glm/glm.hpp"
#include "util.hpp"

using namespace glm;

const float EPS = 1e-6f;
const float INV4EPS = 1.0f / (4.0f * EPS);
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
vec3 SDF::calcGradient(const vec3& position) const {
    return (SAMPLES[0] * calcSignedDistance(position + EPS * SAMPLES[0])
          + SAMPLES[1] * calcSignedDistance(position + EPS * SAMPLES[1])
          + SAMPLES[2] * calcSignedDistance(position + EPS * SAMPLES[2])
          + SAMPLES[3] * calcSignedDistance(position + EPS * SAMPLES[3])) * INV4EPS;
}

vec3 SDF::calcNormal(const vec3& position) const {
    return normalizeNoNaN(
          SAMPLES[0] * calcSignedDistance(position + EPS * SAMPLES[0])
        + SAMPLES[1] * calcSignedDistance(position + EPS * SAMPLES[1])
        + SAMPLES[2] * calcSignedDistance(position + EPS * SAMPLES[2])
        + SAMPLES[3] * calcSignedDistance(position + EPS * SAMPLES[3])
    );
}

vec3 SDF::calcScaledNormal(const vec3& position) const {
    float d0 = calcSignedDistance(position + EPS * SAMPLES[0]);
    float d1 = calcSignedDistance(position + EPS * SAMPLES[1]);
    float d2 = calcSignedDistance(position + EPS * SAMPLES[2]);
    float d3 = calcSignedDistance(position + EPS * SAMPLES[3]);
    float d = (d0 + d1 + d2 + d3) * 0.25f;

    return normalizeNoNaN(SAMPLES[0] * d0 + SAMPLES[1] * d1 + SAMPLES[2] * d2 + SAMPLES[3] * d3) * d;
}