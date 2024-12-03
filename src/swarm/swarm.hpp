#pragma once

#include <vector>
#include <random>

#include <glm/glm.hpp>

#include "tree/model.hpp"

struct SwarmConfig {
    unsigned int nBoids = 2048;
    float viewRange = 3.0f;
    float viewAngle = 1.0f;
    float separationRange = 1.0f;
    size_t neighborLimit = 128;
    float speed = 5.0f;
    float separationFactor = 0.5f;
    float alignmentFactor = 0.5f;
    float cohesionFactor = 0.5f;
    float randomFactor = 0.1f;
    float forwardFactor = 2.0f;
    float fleeFactor = 20.0f;
    float maxMoveRadius = 50.0f;
    float maxMoveTime = 10.0f;
    float excitementVariance = 1.0f;
    float excitementMean = 0.75f;
    float excitementSpeedBoost = 3.0f;
    float homingSpeed = 0.01f;
    float homingPressure = 0.1f;
};

struct Boid {
    glm::vec3 position;
    float phase;
    glm::vec3 velocity;
    float excitement;
};

struct Swarm {
  public:
    SwarmConfig config;
    std::vector<Boid> boids;
    std::vector<glm::vec3> homes;
    std::default_random_engine rng;

    Swarm() = default;
    Swarm(SwarmConfig config);
    void spawn(const Tree::Model& tree, bool replaceHomes = false);
    void update(float dt);
    void flee(glm::vec3 ro, glm::vec3 rd);
    void reset();
};