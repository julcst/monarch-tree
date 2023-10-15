#pragma once

#include "glm/glm.hpp"

class SDF {
public:
    virtual float calcSignedDistance(glm::vec3 position) = 0;
    glm::vec3 calcGradient(glm::vec3 position);
    glm::vec3 calcNormal(glm::vec3 position);
};