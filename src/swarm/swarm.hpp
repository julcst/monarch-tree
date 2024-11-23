#pragma once

#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

struct SwarmConfig {
    unsigned int nBoids = 1024;
    float viewRange = 3.0f;
    float viewAngle = 1.0f;
    float separationRange = 1.0f;
    float speed = 10.0f;
    float separationFactor = 0.5f;
    float alignmentFactor = 0.5f;
    float cohesionFactor = 0.5f;
    float randomFactor = 0.25f;
    float forwardFactor = 2.0f;
    float fleeFactor = 10.0f;
    glm::vec4 spawn = glm::vec4(0.0f, 0.5f, 0.0f, 2.0f);
    glm::vec4 bound = glm::vec4(0.0f, 10.0f, 0.0f, 10.0f);
};

struct Boid {
    glm::vec3 position;
    float phase;
    glm::vec3 velocity;
    float excitement;
    Boid(glm::vec3 position, glm::vec3 velocity, float excitement);
};

class SpatialHash {
    private:
        std::unordered_map<glm::ivec3, std::vector<Boid>> map;
    public:
        float radius;
        void add(Boid b);
        void iterateNeighbors(const Boid b, const std::function<void(const Boid)>& callback);
        void reset(float radius);
        SpatialHash(float radius);
};

struct Swarm {
  public:
    SwarmConfig config;
    std::vector<Boid> boids;

    Swarm() = default;
    Swarm(SwarmConfig config);
    void update(float dt, glm::vec3 ro = glm::vec3(0.0f), glm::vec3 rd = glm::vec3(0.0f), bool flee = false);
    void reset();
};