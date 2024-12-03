#pragma once

#include "glm/glm.hpp"

class SDF {
public:
    virtual float calcSignedDistance(const glm::vec3& position) const = 0;
    glm::vec3 calcGradient(const glm::vec3& position) const;
    glm::vec3 calcNormal(const glm::vec3& position) const;
    glm::vec3 calcScaledNormal(const glm::vec3& position) const;
};