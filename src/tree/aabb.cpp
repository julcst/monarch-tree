#include "aabb.hpp"

#include <glm/glm.hpp>
#include <limits>

using namespace glm;

AABB::AABB() : min(vec3(std::numeric_limits<float>::infinity())), max(vec3(-std::numeric_limits<float>::infinity())) {}

AABB::AABB(const vec3& origin) : min(origin), max(origin) {}

AABB::AABB(const vec4& sphere) : min(vec3(sphere) - vec3(sphere.w)), max(vec3(sphere) + vec3(sphere.w)) {}

void AABB::add(const vec3& point) {
    min = glm::min(min, point);
    max = glm::max(max, point);
}

void AABB::add(const vec4& sphere) {
    min = glm::min(min, vec3(sphere) - vec3(sphere.w));
    max = glm::max(max, vec3(sphere) + vec3(sphere.w));
}

void AABB::add(const AABB& other) {
    min = glm::min(min, other.min);
    max = glm::max(max, other.max);
}