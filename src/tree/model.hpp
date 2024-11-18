/*
 * A tree generator using particle systems
 * Copyright © Julian Stamm 2023
 */
#pragma once

#include "sdf.hpp"
#include "tree/aabb.hpp"

#include <glm/glm.hpp>
#include <vector>

namespace Tree {

    struct Branch {
        glm::vec4 start;
        glm::vec4 end;
    };

    /*
     * Contains a list of Branches and a bounding box 
     * The branches are stored as two vec4s where xyz is the center and w is the radius of the start-/endsphere,
     * two consecutive spheres define a capsule which represents a branch
     * The AABB can be used to optimize rendering
     */
    class Model : public SDF {
    public:
        void addBranch(const Branch& branch);
        float calcSignedDistance(const glm::vec3& position) const override;
        AABBBuilder aabb;
        std::vector<Branch> branches;
    };

}