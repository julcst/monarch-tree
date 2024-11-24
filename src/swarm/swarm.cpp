#include "swarm.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>
#include <glm/gtx/hash.hpp>

#include <iostream>
#include <unordered_map>
#include <vector>

#include "util.hpp"

/* A simple spatial hash */
SpatialHash::SpatialHash(float radius) : radius(radius), map() {}

void SpatialHash::add(const Boid b) {
    const glm::ivec3 cell = glm::ivec3(b.position / radius);
    map[cell].push_back(b);
}

// TODO: Use generator
inline void SpatialHash::iterateNeighbors(const Boid b, const std::function<void(const Boid)>& callback) {
    const glm::ivec3 gridPosition = glm::floor(b.position / radius);
    for (int x = -1; x <= 1; ++x)
    for (int y = -1; y <= 1; ++y)
    for (int z = -1; z <= 1; ++z) {
        const glm::ivec3 neighborCell = gridPosition + glm::ivec3(x, y, z);
        for (const Boid& neighbor : map[neighborCell]) callback(neighbor);
    }
}

void SpatialHash::reset(float r) { radius = r; map.clear(); }

/* Simple boid flocking implementation.
 * Based on the amazing paper "Steering Behaviors For Autonomous Characters" by Craig W. Reynolds */
void Swarm::update(float dt, glm::vec3 ro, glm::vec3 rd, bool flee) {
    if (config.nBoids < boids.size()) {
        boids.erase(boids.begin() + config.nBoids, boids.end());
    } else if (config.nBoids > boids.size()) {
        boids.reserve(config.nBoids);
        for (unsigned int i = boids.size(); i < config.nBoids; i++) {
            boids.emplace_back(glm::ballRand(config.spawn.w) + glm::vec3(config.spawn), glm::linearRand(0.0f, two_pi<float>()), glm::sphericalRand(1.0f), 0.0f);
        }
    }

    // Hash all boids
    SpatialHash hash(config.viewRange);
    for (Boid b : boids) hash.add(b);

    float sqrange = config.viewRange * config.viewRange; // Squaring for faster comparisons
    float viewangle = glm::cos(config.viewAngle); // Cosine of view angle
    float seprange = config.separationRange * config.separationRange; // Squaring for faster comparisons

    for (unsigned int i = 0; i < boids.size(); i++) {
        glm::vec3 separation(0.f), alignment(0.f), cohesion(0.f);
        float nSeparation = 0.f, nCohesion = 0.f;

        hash.iterateNeighbors(boids[i], [b = boids[i], sqrange, viewangle, seprange, &separation, &nSeparation, &alignment, &cohesion, &nCohesion] (Boid other) {
            float sqdist = glm::distance2(b.position, other.position); // Squared distance between boid and neighbor
            if (sqdist > sqrange) return; // Neighbor is out of visual range
            if (sqdist < 0.0001f) return; // Neighbor is self (or very close)
            glm::vec3 dir = (other.position - b.position) / glm::sqrt(sqdist); // Direction from boid to neighbor
            if (sqdist < seprange) { // Neighbor is too close => separate
                separation -= dir;
                nSeparation++;
            } else if (glm::dot(b.velocity, dir) > viewangle) { // Neighbor is in view => align
                alignment += other.velocity;
                cohesion += dir;
                nCohesion++;
            }
        });

        // Randomization
        glm::vec3 force = (glm::ballRand(0.5f) + boids[i].velocity * 0.5f) * config.randomFactor; // Sphere in front of boid

        // Separation
        if (nSeparation > 0) {
            separation /= nSeparation;
            force += separation * config.separationFactor;
        }

        // Cohesion
        if (nCohesion > 0) {
            alignment /= nCohesion;
            cohesion /= nCohesion;
            force += alignment * config.alignmentFactor;
            force += cohesion * config.cohesionFactor;
        }

        // Bounding sphere
        glm::vec3 bounddir = glm::vec3(config.bound) - boids[i].position;
        float distbound = glm::length(bounddir) - config.bound.w;
        if (distbound > 0.0f) force += bounddir * distbound * 0.001f;

        // Flee from mouse
        // For the flee direction find the closest point on the line
        if (flee) {
            glm::vec3 closest = glm::dot(boids[i].position - ro, rd) * rd + ro;
            glm::vec3 fleedir = normalizeNoNaN(boids[i].position - closest);
            glm::vec3 fleeing = fleedir - boids[i].velocity; // Steer away from the the line
            force += fleeing * config.fleeFactor;
            boids[i].excitement = glm::gaussRand(1.0f, config.excitementVariance);
        }

        // Attraction to spawn
        float distspawn = glm::length(glm::vec3(config.spawn) - boids[i].position) - config.spawn.w;
        if (boids[i].excitement <= config.returnTime) force += (glm::vec3(config.spawn) + glm::ballRand(config.spawn.w) - boids[i].position);

        // Move
        boids[i].velocity = normalizeNoNaN(boids[i].velocity * config.forwardFactor + force);
        boids[i].phase += glm::length(boids[i].velocity) * dt * config.speed;
        boids[i].phase = mod(boids[i].phase, two_pi<float>());
        if (distspawn > 0.f || boids[i].excitement > 0.f) boids[i].position = boids[i].position + boids[i].velocity * dt * config.speed * (1.f + boids[i].excitement * config.excitementFactor);
        boids[i].excitement = std::max(0.f, boids[i].excitement - dt * config.excitementDecay);
    }
}

void Swarm::reset() { boids.clear(); }

Swarm::Swarm(SwarmConfig config) : config(config) {}