#include "aabb.hpp"

#include <glm/glm.hpp>
#include <limits>

using namespace glm;

AABBBuilder::AABBBuilder() :
min(std::numeric_limits<float>::max()),
max(-std::numeric_limits<float>::max()) {}

AABBBuilder::AABBBuilder(vec3 origin) :
min(origin), max(origin) {}

AABBBuilder::AABBBuilder(vec4 sphere) :
min(vec3(sphere) - sphere.w),
max(vec3(sphere) - sphere.w) {}

void AABBBuilder::add(vec3 point) {
    min = glm::min(min, point);
    max = glm::max(max, point);
}

void AABBBuilder::add(vec4 sphere) {
    vec3 center = vec3(sphere);
    float radius = sphere.w;
    min = glm::min(min, center - radius);
    max = glm::max(max, center + radius);
}

void AABBBuilder::add(AABB other) {
    min = glm::min(min, other.center - other.size);
    max = glm::max(max, other.center + other.size);
}

void AABBBuilder::add(AABBBuilder other) {
    min = glm::min(min, other.min);
    max = glm::max(max, other.max);
}

AABB AABBBuilder::build() {
    vec3 center = (min + max) / 2.0f;
    return AABB { center, max - center };
}