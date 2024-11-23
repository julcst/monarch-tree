#pragma once

#include <glm/glm.hpp>

using namespace glm;

inline vec3 normalizeNoNaN(const vec3& v) {
    float len2 = dot(v, v);
    return std::isnormal(len2) ? v / std::sqrtf(len2) : vec3(0.0f);
}