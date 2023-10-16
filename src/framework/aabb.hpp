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
    AABBBuilder(const glm::vec3& origin);
    AABBBuilder(const glm::vec4& sphere);
    void add(const glm::vec3& point);
    void add(const glm::vec4& sphere);
    void add(const AABB& other);
    void add(const AABBBuilder& other);
    AABB build() const;
};