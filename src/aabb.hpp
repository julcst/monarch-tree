/*
 * Julian Stamm 2023
 * Axis aligned bounding box
 */
#pragma once

#include <glm/glm.hpp>

struct AABB {
    glm::vec3 center;
    glm::vec3 size;
};

class AABBBuilder {
public:
    glm::vec3 min;
    glm::vec3 max;
    AABBBuilder();
    AABBBuilder(glm::vec3 origin);
    AABBBuilder(glm::vec4 sphere);
    void add(glm::vec3 point);
    void add(glm::vec4 sphere);
    void add(AABB other);
    void add(AABBBuilder other);
    AABB build();
};