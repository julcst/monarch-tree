#pragma once

#include <vector>
#include <random>

#include <glm/glm.hpp>

struct SwarmConfig {
    unsigned int nBoids = 1024;
    float viewRange = 3.0f;
    float viewAngle = 1.0f;
    float separationRange = 1.0f;
    float speed = 10.0f;
    float separationFactor = 0.5f;
    float alignmentFactor = 0.5f;
    float cohesionFactor = 0.5f;
    float randomFactor = 0.1f;
    float forwardFactor = 2.0f;
    float fleeFactor = 10.0f;
    float excitementVariance = 0.4f;
    float excitementFactor = 1.0f;
    float excitementDecay = 0.1f;
    float returnTime = 0.1f;
    glm::vec4 spawn = glm::vec4(0.0f, 5.0f, 0.0f, 2.0f);
    glm::vec4 bound = glm::vec4(0.0f, 10.0f, 0.0f, 10.0f);
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
    std::default_random_engine rng;

    Swarm() = default;
    Swarm(SwarmConfig config);
    void update(float dt, glm::vec3 ro = glm::vec3(0.0f), glm::vec3 rd = glm::vec3(0.0f), bool flee = false);
    void reset();
};