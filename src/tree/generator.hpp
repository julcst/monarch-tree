/*
 * A tree generator using particle systems
 * Copyright © Julian Stamm 2023
 */
#pragma once

#include "model.hpp"

#include <glm/glm.hpp>

namespace Tree {

    /* 
     * Configuration structure for tree generation
     */
    struct Config {
        unsigned int nBranches;
        float radius;
        float segmentLength;
        float forwardFactor = 0.5f;
        float upFactor = 0.5f;
        float randomFactor = 0.5f;
        float spreadFactor = 0.5f;
        glm::vec3 root = glm::vec3(0.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        unsigned int branching = 2;
        bool fixedSeed = true;
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
        Generator(const Config& config, unsigned int seed = 42);
        Model generate();
        void shuffle();
    private:
        unsigned int seed;
    };
}