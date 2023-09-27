/*
 * A tree generator using particle systems
 * Copyright © Julian Stamm 2023
 */
#pragma once

#include "framework/aabb.hpp"

#include <glm/glm.hpp>
#include <vector>

namespace Tree {

    /* 
     * Configuration structure for tree generation
     */
    struct Config {
        unsigned int nBranches;
        float radius;
        float segmentLength;
        float forwardFactor = 1.0f;
        float upFactor = 1.0f;
        float randomFactor = 1.0f;
        float spreadFactor = 1.0f;
        glm::vec3 root = glm::vec3(0.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        unsigned int branching = 2;
    };

    struct Branch {
        glm::vec4 start;
        glm::vec4 end;
    };

    /*
     * Contains a list of spheres and a bounding box 
     * The spheres are vec4 where xyz is the center and w is the radius, two consecutive spheres define a capsule which represents a branch
     * The AABB can be used to optimize rendering
     */
    struct Model {
        std::vector<Branch> branches;
        AABB aabb;
    };

    struct Particle {
        glm::vec3 position;
        glm::vec3 momentum;
        float radius;
        float age;
    };

    class Generator {
    public:
        Config config;
        Generator(Config config, unsigned int seed = 42);
        Model generate(bool fixSeed = false);
    private:
        unsigned int seed;
    };
    
}