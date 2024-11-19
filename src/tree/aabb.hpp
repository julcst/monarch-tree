/*
 * Julian Stamm 2023
 * Axis aligned bounding box
 */
#pragma once

#include <glm/glm.hpp>

class AABB {
public:
    glm::vec3 min;
    glm::vec3 max;
    AABB();
    AABB(const glm::vec3& origin);
    AABB(const glm::vec4& sphere);
    void add(const glm::vec3& point);
    void add(const glm::vec4& sphere);
    void add(const AABB& other);
};